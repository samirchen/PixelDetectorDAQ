#include <stdio.h>
#include <stdlib.h>
#include "image_util.h"



void calculateCenterPoint(const int width, const int height, const long* pixelData, CXPixelPoint* point) {

	point->x = 10;
	point->y = 10;
	point->value = 8;

	// Step 1: Find the point with the max count.
	int maxIndex = 0;

	// Step 2: Find the darkest point around the brightest point as center.


}

void calculateIQData(const int width, const int height, const long* pixelData, const CXPixelPoint* center, int iq2map) {

}


