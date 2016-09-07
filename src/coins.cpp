#include "coins.h"
#include "rotationPoints.h"
#include <map>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <opencv/highgui.h>
#include <string>
#include <opencv/cv.h>

using namespace cv;
using namespace std;

bool chercher(unsigned int elem, vector<unsigned int> tableau)
/// Cherche un élément entier dans un tableau d'entiers
{
    for (unsigned int i=0;i<tableau.size();i++)
    {
        if (tableau[i]==elem)
        {
            return true;
        }
    }
    return false;
}

bool chercher(Point elem, vector<Point> tableau)
/// Cherche un point dans un tableau de points
{
    for (unsigned int i=0;i<tableau.size();i++)
    {
        if (tableau[i]==elem)
        {
            return true;
        }
    }
    return false;
}

void trouverCoins(IplImage* const& img, vector<vector<Point> >const& contours,vector<vector<Point> >& coins)
{
    vector<vector<vector<Point> > > allCoins;
    /// allCoins regroupe tous résultat obtenus par les méthodes qui suivent (sa taille est égale au nombre de méthodes)

  /// Detecter coins image 45°
    vector<vector <Point> >coins2;
    detecterCoins2(img,contours,coins2);
    allCoins.push_back(coins2);

  /// Detecter coins image droite
    vector<vector <Point> >coins1;
    detecterCoins(img,contours,coins1);
    allCoins.push_back(coins1);

  /// Détecter coins image avec rotation
    for(double angle(10);angle<=70;angle+=20)
    {
        vector<vector<Point> > coinsRot;
        detecterCoinsRot(img,contours,angle,coinsRot);

        allCoins.push_back(coinsRot);
    }

  /// Synthèse des résultats obtenus avec les différentes méthodes. Ces résultats sobt stockés dans le tableau coins donné en argument.
    for (unsigned int k=0;k<allCoins.size();k++)
    {
        for(unsigned int i=0;i<allCoins[k].size();i++)
        {
            for(unsigned int l(0);l<allCoins[k][i].size();l++)
            /// On regarde tous les points trouvés : méthode n°k, contour n°i, point n°l
            {
                int nb(1);
                int x = allCoins[k][i][l].x;
                int y = allCoins[k][i][l].y;
                for(unsigned int n=k+1;n<allCoins.size();n++)
                /*** On cherche si ce point n°l (ou un point proche) a été trouvé pour le même contour n°i
                par une autre méthode non encore explorée (méthode numéro n avec n>k)*/
                {
                    int id=0;
                    for (int xi(x-3);xi<=x+3;xi++)
                    {
                        for (int yi(y-3);yi<=y+3;yi++)
                        {
                            if (xi>=0 and yi>=0 and chercher(Point(xi,yi),allCoins[n][i]))
                            { id = 1; }
                             /// id devient égal à 1 si la méthode numéro n a trouvé pour le contour n°i
                             /// au moins un coin situé dans un carré de 7px de côté centré sur le point étudié (le point n°l)
                        }
                    }
                    if(id==1)
                    {
                        nb++; /// nb représente le nombre de méthode qui ont aussi trouvé le point l (ou un point proche) comme coins du contour i
                    }
                }
                /// Le morceau de code qui suit permet d'éviter de sélectionner deux fois un même point (ou des points proches)
                int ID(1);
                if (coins.size()>i)
                {
                    for (int xi(x-5);xi<=x+5;xi++)
                    {
                        for (int yi(y-5);yi<=y+5;yi++)
                        {
                            if (xi>=0 and yi>=0 and chercher(Point(xi,yi),coins[i]))
                            { ID = 0; }
                            /// ID = 1 si le coin considéré n'est pas encore dans le tableau coins associé au contour voulu
                            /// ID = 0 si ce coin est déjà dans le tableau coin ou si il y a déjà un point proche
                                /// (c'est à dire un point situé dans un carré centré de côté 11px)
                        }
                    }
                }
                if(nb>=4 and ID==1)
                /*** Le critère pour ajouter un coin est qu'il ait été trouvé par au moins 4 méthodes différentes parmi les 6
                et qu'il ne soit pas déjà dans le tableau coins*/
                {
                    if (coins.size()>i)
                    {
                        coins[i].push_back(allCoins[k][i][l]);
                    }
                    else
                    {
                        coins.push_back(vector<Point>(1,allCoins[k][i][l]));
                    }
                }
            }
            if(coins.size()<=i)
            {
                coins.push_back(vector<Point>(1,Point(1,1)));
            }
        }
    }
}

void detecterCoins(IplImage* const& img,  vector<vector<Point> > const& contours, vector<vector<Point> >& coins)
{
    for(unsigned int i = 0; i< contours.size(); i++ )
    {
        int xmin=img->width;
        int ymin = img->height;
        int xmax(0);
        int ymax(0);
        /// On pourra détecter 8 points remarquables au maximum, initialisés comme suit
        Point coingaucheH=contours[i][0];
        Point coingaucheB=contours[i][0];
        Point coindroiteB=contours[i][0];
        Point coindroiteH=contours[i][0];
        Point coinbasD=contours[i][0];
        Point coinbasG=contours[i][0];
        Point coinhautD=contours[i][0];
        Point coinhautG=contours[i][0];

        for (unsigned int j = 0; j<contours[i].size();j++)
        {
            if (contours[i][j].x < xmin) /// On cherche le point du contour qui a le plus petit x
            {
                coingaucheH=contours[i][j];
                coingaucheB=contours[i][j];
                xmin = contours[i][j].x;
            }
            else if (contours[i][j].x == xmin) /// Si il y a plusieurs points avec le plus petit x, on en choisit 2 en particulier
            {
                if(contours[i][j].y>coingaucheB.y)
                {
                    coingaucheB = contours[i][j];  /// Celui avec le plus grand y parmi ceux avec le plus petit x
                }
                else if (contours[i][j].y < coingaucheH.y)
                {
                    coingaucheH = contours[i][j];  /// Celui avec le plus petit y parmi ceux avec le plus petit x
                }
            }
            if(contours[i][j].x > xmax) /// On cherche le point du contour qui a le plus grand x
            {
                coindroiteH=contours[i][j];
                coindroiteB=contours[i][j];
                xmax = contours[i][j].x;
            }
            else if (contours[i][j].x == xmax) /// Si il y a plusieurs points avec le plus grand x, on en choisit 2 en particulier
            {
                if(contours[i][j].y>coindroiteB.y)
                {
                    coindroiteB = contours[i][j]; /// Celui avec le plus grand y parmi ceux avec le plus grand x
                }
                else if (contours[i][j].y < coindroiteH.y)
                {
                    coindroiteH = contours[i][j];  /// Celui avec le plus petit y parmi ceux avec le plus grand x
                }
            }
            if(contours[i][j].y < ymin) /// On cherche le point du contour qui a le plus petit y
            {
                coinhautD=contours[i][j];
                coinhautG=contours[i][j];
                ymin = contours[i][j].y;
            }
            else if (contours[i][j].y == ymin)  /// Si il y a plusieurs points avec le plus petit y, on en choisit 2 en particulier
            {
                if(contours[i][j].x > coinhautD.x)
                {
                    coinhautD = contours[i][j];  /// Celui avec le plus grand x parmi ceux avec le plus petit y
                }
                else if (contours[i][j].x < coinhautG.x)
                {
                    coinhautG = contours[i][j];  /// Celui avec le plus petit x parmi ceux avec le plus petit y
                }
            }
            if(contours[i][j].y > ymax) /// On cherche le point du contour qui a le plus grand y
            {
                coinbasD=contours[i][j];
                coinbasG=contours[i][j];
                ymax = contours[i][j].y;
            }
            else if (contours[i][j].y == ymax)  /// Si il y a plusieurs points avec le plus grand y, on en choisit 2 en particulier
            {
                if(contours[i][j].x>coinbasD.x)
                {
                    coinbasD = contours[i][j];  /// Celui avec le plus grand x parmi ceux avec le plus grand y
                }
                else if (contours[i][j].x < coinbasG.x)
                {
                    coinbasG = contours[i][j];   /// Celui avec le plus petit x parmi ceux avec le plus grand y
                }
            }
        }
        vector<Point> coinsInter;
        /// On ajoute tous les points trouvés en veillant à ne pas ajouter 2 fois le même
        if (not chercher(coingaucheH,coinsInter))
        {
            coinsInter.push_back(coingaucheH);
        }
        if (not chercher(coingaucheB,coinsInter))
        {
            coinsInter.push_back(coingaucheB);
        }
        if (not chercher(coindroiteH,coinsInter))
        {
            coinsInter.push_back(coindroiteH);
        }
        if (not chercher(coindroiteB,coinsInter))
        {
            coinsInter.push_back(coindroiteB);
        }
        if (not chercher(coinhautD,coinsInter))
        {
            coinsInter.push_back(coinhautD);
        }
        if (not chercher(coinhautG,coinsInter))
        {
            coinsInter.push_back(coinhautG);
        }
        if (not chercher(coinbasD,coinsInter))
        {
            coinsInter.push_back(coinbasD);
        }
        if (not chercher(coinbasG,coinsInter))
        {
            coinsInter.push_back(coinbasG);
        }
        coins.push_back(coinsInter);
    }
}

void detecterCoins2(IplImage* const& img, vector<vector<Point> > contours, vector<vector<Point> >& coins)
{
    int largeur = img->width;
    int hauteur = img->height;

  /// On effectue une rotation de 45° de tous les points du contour considéré.
    for(unsigned int i = 0; i< contours.size(); i++ )
    {
        for (unsigned int j = 0; j<contours[i].size();j++)
        {
            int x = contours[i][j].x;
            int y = contours[i][j].y;

            int x1 = x+y;
            int y1 = y-x+largeur;

            contours[i][j]=Point(x1,y1);
        }
    }
    IplImage* img2= cvCreateImage(Size(largeur + hauteur, largeur + hauteur),IPL_DEPTH_8U,3);

  /// On applique la fonction de détection des coins sur le contour après rotation (modification des coordonnées de chaque point)
    detecterCoins(img2, contours, coins);

  /// On applique la rotation inverse (-45°) pour les coins trouvés et on obtient le tableau de coins voulu
    for(unsigned int k(0);k<coins.size();k++)
    {
        for(unsigned int l(0);l<coins[k].size();l++)
        {
            int x1 = coins[k][l].x;
            int y1 = coins[k][l].y;

            int x = (x1-y1+largeur)/2;
            int y = (x1+y1-largeur)/2;

            coins[k][l] = Point(x,y);
        }
    }
}

void detecterCoinsRot(IplImage* const& img, vector<vector<Point> > const& contours,double angle,vector<vector<Point> >&coinsRot)
{
    /// Rotation du contour de Canny
    vector<vector<Point> > contoursRot;
    rotationContours(contours,contoursRot,angle,img);

    /// detection des coins dans le contour avec rotation
    IplImage* fondRot= cvCreateImage(Size((img->width)*2,(img->height)*2),IPL_DEPTH_8U,3);
    detecterCoins(fondRot,contoursRot,coinsRot);

    /// Rotation inverse des coins trouvés de cette façon. On obtient le tableau de coins voulu.
    rotationCoinsInverse(coinsRot, angle,img);

}

void dessinerCoins(IplImage*& img, vector<vector<Point> > const& coins)
/*** Cette fonction prend en argument une image qu'elle va modifier en marquant d'une croix jaune (5px)
l'emplacement de chaque point du tableau coins également passé en argument.
Pour cela, la fonction change simplement la couleur des pixels correspondant dans l'image */
{
    for (unsigned int k=0;k<coins.size();k++)
    {
        for(unsigned int l(0);l<coins[k].size();l++)
        {
            CvScalar s=cvGet2D(img,coins[k][l].y,coins[k][l].x);
            s.val[0] = 0;
            s.val[1] = 255;
            s.val[2] = 255;
            cvSet2D(img,coins[k][l].y,coins[k][l].x,s);

            CvScalar s1=cvGet2D(img,(coins[k][l].y + 1),coins[k][l].x);
            s1.val[0] = 0;
            s1.val[1] = 255;
            s1.val[2] = 255;
            cvSet2D(img,coins[k][l].y+1,coins[k][l].x,s1);

            CvScalar s2=cvGet2D(img,coins[k][l].y,coins[k][l].x+1);
            s2.val[0] = 0;
            s2.val[1] = 255;
            s2.val[2] = 255;
            cvSet2D(img,coins[k][l].y,coins[k][l].x+1,s2);

            CvScalar s3=cvGet2D(img,(coins[k][l].y-1),coins[k][l].x);
            s3.val[0] = 0;
            s3.val[1] = 255;
            s3.val[2] = 255;
            cvSet2D(img,coins[k][l].y-1,coins[k][l].x,s3);

            CvScalar s4=cvGet2D(img,coins[k][l].y,coins[k][l].x-1);
            s4.val[0] = 0;
            s4.val[1] = 255;
            s4.val[2] = 255;
            cvSet2D(img,coins[k][l].y,coins[k][l].x-1,s4);
        }

    }
}

void dessinerContours(IplImage*& img, vector<vector<Point> > const& contours)
/*** Cette fonction prend en argument une image (toute noire généralement) qu'elle va modifier en marquant d'une couleur précise
l'emplacement de chaque point du tableau contours également passé en argument. La couleur diffère suivant le contour auquel appartient le point.
Pour cela, la fonction change simplement la couleur des pixels correspondant dans l'image */
{
    RNG rng(12345);
    for (unsigned int i=0;i<contours.size();i++)
    {
        int b = rng.uniform(0, 255);
        int v = rng.uniform(0, 255);
        int r = rng.uniform(0, 255);
        for(unsigned int j=0;j<contours[i].size();j++)
        {
            CvScalar s=cvGet2D(img,contours[i][j].y,contours[i][j].x);
            s.val[0] = b;
            s.val[1] = v;
            s.val[2] = r;
            cvSet2D(img,contours[i][j].y,contours[i][j].x,s);
        }

    }

}
