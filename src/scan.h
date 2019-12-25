#ifndef __SCAN_H__
#define __SCAN_H__

#include <stdio.h>
#include <iostream>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

constexpr int ROTATE_CYCLES = 3;
constexpr double SQUARE_TOLERANCE = 0.2;
constexpr double SQUARE_TOLERANCE_MIN = 1 + SQUARE_TOLERANCE;
constexpr double SQUARE_TOLERANCE_MAX = 1 - SQUARE_TOLERANCE;

struct ScanConfig {
	double square_width;
	double square_height;
	double circle_width;
	double circle_height;
};

std::vector<double> scan(std::string imagePath, ScanConfig config, std::vector<double> bubbles);

#endif
