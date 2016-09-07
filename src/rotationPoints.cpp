#include "rotationPoints.h"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <opencv/highgui.h>
#include <string>
#include <opencv/cv.h>
#include <math.h>

using namespace cv;
using namespace std;

Point rotate2d(const Point& inPoint, const double& angRad)
{
    Point outPoint;
    ///CW rotation
    outPoint.x =  cos(angRad)*inPoint.x - sin(angRad)*inPoint.y;
    outPoint.y = sin(angRad)*inPoint.x + cos(angRad)*inPoint.y;
    return outPoint;
}

Point rotatePoint(const Point& inPoint, const Point& center, const double& angRad)
{
    return rotate2d(inPoint - center, angRad) + center;
}

void rotationContours(vector<vector<Point> > const& contours, vector<vector<Point> >& contoursRot, double angle, IplImage* const& img)
{
    int largeur = img->width;
    int hauteur = img->height;

    int x0(largeur/2);
    int y0(hauteur/2);

    angle = angle*M_PI/180;
    for(unsigned int i = 0; i< contours.size(); i++)
    {
        for (unsigned int j = 0; j<contours[i].size();j++)
        {
            ///Point2f pointPrecis = Point2f(contours[i][j].x,contours[i][j].y);
            Point nouveauPoint = rotatePoint(contours[i][j],Point(x0,y0),angle);

            int x1 = nouveauPoint.x+largeur/2;
            int y1 = nouveauPoint.y+hauteur/2;

            if (contoursRot.size()>i)
            {
                contoursRot[i].push_back(Point(x1,y1));
            }
            else
            {
                contoursRot.push_back(vector<Point>(1,Point(x1,y1)));
            }
        }
    }
}

void rotationCoinsInverse(vector<std::vector<cv::Point> >& coinsRot, double angle, IplImage*const& img)
{
    int largeur = img->width;
    int hauteur = img->height;

    int x0(largeur/2);
    int y0(hauteur/2);

    angle = angle*M_PI/180;
    for(unsigned int k = 0; k< coinsRot.size(); k++)
    {
        for(unsigned int l = 0; l< coinsRot[k].size(); l++)
        {
            int x=coinsRot[k][l].x-largeur/2;
            int y=coinsRot[k][l].y-hauteur/2;
            Point2f nouveauPoint = Point2f(x,y);

            Point2f nouveauPoint2 = rotatePoint(nouveauPoint,Point2f(x0,y0),-angle);
            ///Point nouveauPoint3(nouveauPoint2.x,nouveauPoint2.y);
            coinsRot[k][l] = nouveauPoint2;
        }
    }
}
