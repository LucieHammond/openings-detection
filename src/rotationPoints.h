#ifndef ROTATIONPOINTS_H_INCLUDED
#define ROTATIONPOINTS_H_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include <opencv/highgui.h>
#include <string>
#include <opencv/cv.h>
#include <vector>

void rotationContours(std::vector<std::vector<cv::Point> > const& contours, std::vector<std::vector<cv::Point> >& contoursRot, double angle, IplImage*const& img);

cv::Point rotate2d(const cv::Point& inPoint, const double& angRad);

cv::Point rotatePoint(const cv::Point& inPoint, const cv::Point& center, const double& angRad);

void rotationCoinsInverse(std::vector<std::vector<cv::Point> >& coinsRot, double angle, IplImage*const& img);

#endif // ROTATIONPOINTS_H_INCLUDED
