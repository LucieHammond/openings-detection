#ifndef FILTRE_H_INCLUDED
#define FILTRE_H_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include <opencv/highgui.h>
#include <string>
#include <opencv/cv.h>
#include <vector>

float dist(cv::Point const& p, cv::Point const& q);

void filtreTaille(std::vector<std::vector<cv::Point> >& contours, float rapport);

#endif // FILTRE_H_INCLUDED
