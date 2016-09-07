#ifndef DISTANCE_H_INCLUDED
#define DISTANCE_H_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include <opencv/highgui.h>
#include <string>
#include <opencv/cv.h>
#include <vector>

void distance(std::vector<std::vector<cv::Point2f> > &coinsOuvrant);
float madistance(cv::Point2f& p, cv::Point2f& q);
void echanger(cv::Point2f& x, cv::Point2f& y);
void ordonnerCoinsMur(std::vector<cv::Point2f>& coins);
void ordonnerCoinsOuvrants(std::vector<std::vector<cv::Point2f> >& coins);
bool intersection(cv::Point2f o1, cv::Point2f p1, cv::Point2f o2, cv::Point2f p2, cv::Point2f &r);
void position(std::vector<cv::Point2f> ouvrant, std::vector<cv::Point2f> mur, float dist[12]);
void dimRelatives(float dist[12], float dimRel[2], float posRel[4]);

#endif // DISTANCE_H_INCLUDED
