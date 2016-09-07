#ifndef COINS_H_INCLUDED
#define COINS_H_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include <opencv/highgui.h>
#include <string>
#include <opencv/cv.h>
#include <vector>

bool chercher(cv::Point elem, std::vector<cv::Point> tableau);

void trouverCoins(IplImage* const& img, std::vector<std::vector<cv::Point> >const& contours,std::vector<std::vector<cv::Point> >& coins);

void detecterCoins(IplImage* const& img, std::vector<std::vector<cv::Point> >const& contours, std::vector<std::vector<cv::Point> >& coins);

void detecterCoins2(IplImage* const& img, std::vector<std::vector<cv::Point> > contours, std::vector<std::vector<cv::Point> >& coins);

void detecterCoinsRot(IplImage* const& img, std::vector<std::vector<cv::Point> >const& contours,double angle,std::vector<std::vector<cv::Point> >&coinsRot);

void dessinerCoins(IplImage*& img, std::vector<std::vector<cv::Point> > const& coins);

void dessinerContours(IplImage*& img, std::vector<std::vector<cv::Point> > const& contours);



#endif // COINS_H_INCLUDED
