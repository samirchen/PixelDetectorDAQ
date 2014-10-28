#include <stdio.h>
#include <stdlib.h>
#include <map>
#include "image_util.h"
using namespace std;



void calculateCenterPoint(const int width, const int height, const long* pixelData, CXPixelPoint* center) {

	center->x = -1;
	center->y = -1;
	center->value = -1;


	// Step 1: Find the	point with the max count as center.
	int size = width*height;
	CXPixelPoint maxValuePoint;
	int maxIndex = 0;
	long maxValue = pixelData[maxIndex];
	int i = 0;
	for (i = 1; i < size; i++) {
		if (pixelData[i] > maxValue) {
			maxIndex = i;
			maxValue = pixelData[i];
		}
	}
	maxValuePoint.x = maxIndex%width;
	maxValuePoint.y = maxIndex/width;
	maxValuePoint.value = maxValue;

	center->x = maxValuePoint.x;
	center->y = maxValuePoint.y;
	center->value = maxValuePoint.value;


}

void calculateIQData(const int width, const int height, const long* pixelData, const CXPixelPoint* center, int iq2map) {

}


