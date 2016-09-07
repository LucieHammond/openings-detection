#include "filtre.h"
#include <map>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <opencv/highgui.h>
#include <string>
#include <opencv/cv.h>

using namespace cv;
using namespace std;

float dist(Point const& p, Point const& q)
///Calcule la distance entre le point p et le point q
{
    Point diff = p - q;
    return sqrt(diff.x*diff.x + diff.y*diff.y);
}

void filtreTaille(vector<vector<Point> >& contours, float rapport)
/*** Cet algorithme fait le tri parmi les contours contenus dans le tableau "contours"
pour ne garder que les plus grands en taille. Il procède en plusieurs étapes :
- Pour chaque contour, il calcule la distance maximale entre deux points de ce contour
- Il compare les distances max trouvés pour les différents contours. La plus grande est la distance maximale globale
- Il supprime les contours dont la distance max est inférieure à une certaine fraction de la distance maximale globale.
(la fraction voulue est donnée par l'argument "rapport"*/
{
    float distMax[contours.size()];
    float distMaxTot(0);
    for (unsigned int i=0;i<contours.size();i++)
    {
        distMax[i]=0;
        for(unsigned int j=0;j<contours[i].size();j++)
        {
            for(unsigned int k=0;k<contours[i].size();k++)
            {
                float d = dist(contours[i][j],contours[i][k]);
                if (d>distMax[i])
                { distMax[i] = d; }
            }
        }
        if(distMax[i]>distMaxTot)
        { distMaxTot = distMax[i]; }
    }
    unsigned int taille(contours.size()-1);
    for (int i=taille;i>=0;i--)
    {
        if (distMax[i] < distMaxTot*rapport)
        {
            contours.erase(contours.begin() + i);
        }
    }
}



