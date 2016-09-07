#ifndef TRAITEMENT_H_INCLUDED
#define TRAITEMENT_H_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include <opencv/highgui.h>
#include <string>
#include <opencv/cv.h>
#include <vector>

void traitement(std::vector<std::vector<cv::Point> > &coins,IplImage*& img);

void convertir(std::vector<cv::Point> & coins, std::vector<cv::Point2f> & coins2);

void optcontours(std::vector<std::vector<cv::Point> > const& contours, std::vector<std::vector<cv::Point> > &contopt);

bool simil(std::vector<cv::Point> const& tab1, std::vector<cv::Point> const& tab2);

void filtrePoints(std::vector<std::vector<cv::Point> >& coins,IplImage*& img);

void trapezeiso(std::vector<cv::Point> &coins,IplImage*& img);

#endif // TRAITEMENT_H_INCLUDED
