#include "coins.h"
#include "filtre.h"
#include "traitement.h"

#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <opencv/highgui.h>
#include <string>
#include <opencv/cv.h>

using namespace cv;
using namespace std;

Mat src; Mat src_gray;
int argC;
char** argV;
int thresh = 90;
int max_thresh = 255;
RNG rng(12345);

/// Function header
void thresh_callback(int, void*);

void normalise(Mat input, Mat &output)
/*** Cet algorithme réalise une normalisation d'histogramme.
L'image input est celle dont on veut normaliser l'histogramme, le résultat est enregistré dans l'image output*/
{
    assert(input.type()==CV_8U);
    output=input.clone();
	int temp;
	int max=0;
	int min=255;
    int somme=0;
    for(int i=0; i<input.rows; i++)
    {
        for(int j=0; j<input.cols; j++)
        {
			temp=input.at<uchar>(i,j);
            somme+=temp;
			if(temp<min) { min=temp;}
			if(temp>max) { max=temp;}
        }
    }
    int moyenne=(int)(somme/(input.rows*input.cols));
    cout<<moyenne<<endl;
    for(int i=0; i<input.rows; i++)
    {
        for(int j=0; j<input.cols; j++)
        {
            double tmp=(double)input.at<uchar>(i,j)/(double)moyenne*125.;
            if(tmp>254.)
            {
                tmp=254.;
            }
            output.at<uchar>(i,j)=(uchar)(tmp);
        }
    }
}

/** @function main */
int main( int argc, char** argv )
{

  /// Télécharger l'image source
    src = imread( argv[1], 1 );

  /// Convertir l'image source en niveaux de gris
    cvtColor( src, src_gray, CV_BGR2GRAY );

  /// Appliquer un filtre de floutage sur l'image obtenue
    blur( src_gray, src_gray, Size(3,3) );

  /// Effectuer une normalisation d'histogramme
    normalise(src_gray,src_gray);

  /// Convertir l'image source du format Mat (src) au format IplImage* (src2)
    IplImage* src2 = NULL;
    src2 = cvCreateImage(cvSize(src.cols,src.rows),8,3);
    IplImage ipltemp=src;
    cvCopy(&ipltemp,src2);

  /// Créer une fenêtre "Source" qui affiche l'image source sans modifications
    char* source_window = "Source";
    namedWindow( source_window, CV_WINDOW_AUTOSIZE );
    cvShowImage( source_window, src2);

  /// Appeler la fonction tresh_callback (ci-dessous)
    thresh_callback( 0, 0 );

    cvReleaseImage(&src2);
    waitKey(0);
    return(0);
}

/** @function thresh_callback */
void thresh_callback(int, void* )
{
    Mat canny_output;
    vector<vector<Point> > contoursCanny;
    vector<Vec4i> hierarchy;

  /// Détecter les contours en utilisant l'algorithme de Canny
    Canny( src_gray, canny_output, thresh, thresh*3, 3 );

  /// Trouver les points des contours à partir du résultat de Canny : fonction "findContours"
    findContours( canny_output, contoursCanny, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0) );
    vector<vector<Point> > contours;
    optcontours(contoursCanny,contours);

  /// Filtrer le tableau des contours sur un critère de taille
    filtreTaille(contours,1/3);

  /// Créer une image noire de même taille que la source en format Mat puis la convertir en IplImage*
    Mat drawing = Mat::zeros( canny_output.size(), CV_8UC3 );
    IplImage* dessinbase = NULL;
    dessinbase = cvCreateImage(cvSize(drawing.cols,drawing.rows),8,3);
    IplImage ipltemp2=drawing;
    cvCopy(&ipltemp2,dessinbase);

  /// Dessiner tels quels les contours trouvés par la fonction "findContours" sur l'image noire précédemment créée
    dessinerContours(dessinbase, contours);

  /// Afficher les contours tracés dans une fenêtre
    namedWindow( "Contours", CV_WINDOW_AUTOSIZE );
    cvShowImage( "Contours", dessinbase);

  /// Dessiner les contours reliés par des lignes continues grâce à la fonction "drawContours"
    for( int i = 0; i< contours.size(); i++ )
    {
        Scalar color = Scalar( rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255) );
        drawContours( drawing, contours, i, color, 1, 8, hierarchy, 0, Point() );
    }

  /// Convertir Mat src en IplImage src2
    IplImage* src2 = cvCreateImage(cvSize(src.cols,src.rows),8,3);
    IplImage ipltemp=src;
    cvCopy(&ipltemp,src2);

  /// Trouver les coins
    vector<vector<Point> >coins;
    trouverCoins(src2,contours,coins);

  /// Afficher dans la console le tableau des coins trouvés.
    for(int i(0);i<coins.size();i++)
    {
        cout<<coins[i]<<endl;
    }

  /// Convertir l'image représentant les contours reliés du format Mat (drawing) au format IplImage* (dessinbase2)
    IplImage* dessinbase2 = NULL;
    dessinbase2 = cvCreateImage(cvSize(drawing.cols,drawing.rows),8,3);
    IplImage ipltemp3=drawing;
    cvCopy(&ipltemp3,dessinbase2);

  /// Dessiner les Coins au dessus des contours reliés
    dessinerCoins(dessinbase2,coins);

  /// Afficher les coins dessinés graphiquement dans une fenêtres
    char* titre="Coins";
    cvNamedWindow(titre, CV_WINDOW_AUTOSIZE );
    cvShowImage(titre, dessinbase2);

  ///Trouver les distances relatives
    traitement(coins,dessinbase2);

    cvWaitKey(0);
    cvDestroyAllWindows();

    cvReleaseImage(&dessinbase);

}

