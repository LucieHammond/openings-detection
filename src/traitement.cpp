#include "traitement.h"
#include "coins.h"
#include "distance.h"
#include <map>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <opencv/highgui.h>
#include <string>
#include <opencv/cv.h>

using namespace cv;
using namespace std;

void traitement(vector<vector<Point> > &coins, IplImage*& img)
/*** Cette fonction a pour objectif de mettre en forme les résultats de détection des coins des ouvrants pour que
ces derniers puissent être utilisés par l'algorithme des distances relatives appelée à la fin*/
{
  /// On filtre le tableau des coins pour ne garder que les listes exploitables,
  /// c'est-à-dire celles qui comportent 4 coins (ou bien 3 si on peut approximer le quatrième)
    filtrePoints(coins,img);

  /*** Le morceau de code qui suit n'a d'intérêt que pour visualiser le résultat de la méthode du trapèze isocèle
  qui permet de trouver l'emplacement approximatif du 4ième coins pour les contours n'ayant que 3 points détectés.*/
    vector<vector<Point2f> > coins2(coins.size());
    dessinerCoins(img,coins);
    char* titre="Coins après trapèze iso";
    cvNamedWindow(titre, CV_WINDOW_AUTOSIZE );
    cvShowImage(titre, img);

  /// Convertir le tableau des coins sous un format lisible par la fonction distance :
  /// On doit stocker tous les points avec le type Point2f et non plus Point.
    for(int i(0);i<coins.size();i++)
    {
        convertir(coins[i],coins2[i]);
    }
  /// Appel de la fonction distance qui détermine les distances relatives
    distance(coins2);

}

void convertir(vector<Point> & coins, vector<Point2f> & coins2)
/*** Il existe plusieurs façon de  définir et représenter un point avec OpenCV.
Cette fonction permet de faire la conversion entre deux formes particulières : Point et Point2f*/
{
    for(int j(0);j<coins.size();j++)
    {
        int x=coins[j].x;
        int y=coins[j].y;
        Point2f p(x,y);
        coins2.push_back(p);
    }
}

void optcontours(vector<vector<Point> > const& contours, vector<vector<Point> > &contopt)
/*** Cette fonction permet de fusionner les contours similaires. Tous les points des deux contours sont regroupés au sein
d'un même tableau dynamique de points représentant un seul et même contour*/
{
    for (int k(0);k<contours.size();k++)
    {
        int id(0);
        if (contopt.size()==0)
        {
            contopt.push_back(contours[0]);
            id=1;
        }
        for(int j(0);j<contopt.size()and id==0;j++)
        {
            if (simil(contours[k],contopt[j]))
            {
                for(int l(0);l<contours[k].size();l++)
                {
                    contopt[j].push_back(contours[k][l]);
                }
                id=1;
            }
        }
        if (id==0)
        {
            contopt.push_back(contours[k]);
        }

    }
}

bool simil(vector<Point> const& tab1, vector<Point> const&tab2)
/*** fontion auxiliaire utilisée dans "opcontours".
Elle renvoie True si les deux tableaux de points passés en argument sont similaire (beucoup de points proches en commun)
Sinon elle renvoie False */
{
    int nb(0);
    for(int i(0);i<tab1.size();i++)
    {
        int x=tab1[i].x;
        int y=tab1[i].y;

        if (chercher(Point(x,y),tab2) or chercher(Point(x+1,y),tab2) or chercher(Point(x,y+1),tab2) or chercher(Point(x-1,y),tab2) or chercher(Point(x,y-1),tab2))
        {
            nb=nb+1;
        }
        if (nb>=3)
        {return (true);}
    }
    return(false);
}

void filtrePoints(vector<vector<Point> > &coins,IplImage*& img)
{
    for(int i(coins.size()-1);i>=0;i--)
    {
      /// Si le contour ne possèdde que 3 coins, on trouve une approximation du 4ième grâce à la fonction "trapezeiso"
        if(coins[i].size()==3)
        {
            trapezeiso(coins[i],img);
        }
      /// On ne garde que les contours qui ont désormais 4 coins (ceux qui avaient 3 ou 4 coins au départ)
        if(coins[i].size()!=4)
        {
            coins.erase(coins.begin() + i); /// Les autres sont supprimés
        }
    }
}

float distp(Point& p, Point& q)
/// distance entre le point p et le point q
{
    Point diff = p - q;
    return sqrt(diff.x*diff.x + diff.y*diff.y);
}

void trapezeiso(vector<Point> &coins,IplImage*& img)
{
    ///A appliquer quand on a 3 coins seulement
    Point seul;
    Point proche;
    Point autre;

  /*** On attribue à chacun des trois coins un rôle particullier :
  - le point proche est celui qui se trouve à la même hauteur que le point manquant (bas ou haut)
  - le point seul est celui qui se trouve du même côté qe le moint manquant (droite au gauche)
  - le point autre est le troisième (en diagonale par rapport au point manquant)*/

    int xd01 = abs(coins[0].x - coins[1].x);
    int xd02 = abs(coins[0].x - coins[2].x);
    int xd12 = abs(coins[1].x - coins[2].x);
    int d01 = distp(coins[0],coins[1]);
    int d02 = distp(coins[0],coins[2]);
    int d12 = distp(coins[1],coins[2]);
    if(xd01<xd02 and xd01<xd12)
    {
        seul = coins[2];
        if (d02<d12)
        {proche = coins[0];autre = coins[1];}
        else
        {proche = coins[1];autre = coins[0];}
    }
    if(xd02<xd01 and xd02<xd12)
    {
        seul = coins[1];
        if (d01<d12)
        {proche = coins[0];autre = coins[2];}
        else
        {proche = coins[2];autre = coins[0];}
    }
    if(xd12<xd02 and xd12<xd01)
    {
        seul = coins[0];
        if (d02<d01)
        {proche = coins[2];autre = coins[1];}
        else
        {proche = coins[1];autre = coins[2];}
    }
    Point quatre = Point(1,1); /// Initialisation du 4ième point à trouver
    if (autre.y!=proche.y)
    {
        float dist(0);
        for(int y(0);y<=img->height;y++)
        {
            /// équation de la droite parrallèle à ("autre"-"proche") passant par "seul"
            int x = (y-seul.y)*(autre.x-proche.x)/(autre.y-proche.y)+seul.x;

            Point p=Point(x,y);
            if(abs(distp(proche,p)-distp(autre,seul))<1 and distp(seul,p)>dist)
            /// On cherche le point de cette droite tel que la distance entre ce point et "proche" soit la même qu'entre "autre" et "seul".
            {
                quatre=p;
                dist=distp(seul,p);
            }
        }
    }
    else /// cas particulier ou la fenêtre est très aplatie
    {
        float dist(0);
        for(int x(0);x<=img->width;x++)
        {
            Point p=Point(x,proche.y);
            if(distp(proche,p)==distp(autre,seul)and distp(seul,p)>dist)
            {
                quatre=p;
                dist=distp(seul,p);
            }
        }
    }
    coins.push_back(quatre); /// On ajoute le point trouvé au tableau initial de coins
}
