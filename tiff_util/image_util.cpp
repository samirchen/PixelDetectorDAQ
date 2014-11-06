#include <stdio.h>
#include <stdlib.h>
#include <map>
#include <string>
#include <math.h>
#include "image_util.h"
using namespace std;

long getQ2OfTwoPixelPoints(CXPixelPoint p1, CXPixelPoint p2) {
	long q2 = (long) (pow(p1.x-p2.x, 2) + pow(p1.y-p2.y, 2));

	return q2;
}

CXPixelValueType getPixelValueType(long value) {
	if (value > 0) {
		return CXPixelValueTypeValid;
	}
	else if (value == -1) {
		return CXPixelValueTypeGap;
	}
	else {
		return CXPixelValueTypeBad;
	}
}

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

void calculateIQData(const int width, const int height, const long* pixelData, const CXPixelPoint* center, map<long, CXIQData> & iq2map) {
	// iq2map's key is: Q^2(Q is the distance to center); value is: CXIQData(contain the value count of pixels whoes distance to center is Q).

	if (center == NULL) {
		perror("Error: Center is NULL when calculateIQData.");
		exit(1);
	}

	int size = width*height;
	int i = 0;
	for (i = 0; i < size; i++) {
		CXPixelPoint p(i%width, i/width, pixelData[i]);
		long q2 = (long) getQ2OfTwoPixelPoints(p, *center);
		CXIQData data = iq2map[q2];
		if (data.q2 == -1) { // data.q2 == -1 means no this CXIQData in iq2map.
			data.q2 = q2;
		}

		CXPixelValueType type = getPixelValueType(p.value);
		switch (type) {
	        case CXPixelValueTypeValid:
	            data.normalPixelCount++;
	            data.totalI += p.value;
	            data.averageI = data.totalI/data.normalPixelCount;
	            break;
	            
	        case CXPixelValueTypeGap:
	        	data.gapPixelCount++;
		        break;

		    case CXPixelValueTypeBad:
		    	data.badPixelCount++;
		    	break;

	        default:
	            break;
    	}

    	iq2map[q2] = data;

	}

}

int countInvalidPixels(const int width, const int height, const long* pixelData) {
	int count = 0;
	int size = width*height;
	int i = 0;
	for (i = 0; i < size; i++) {
		CXPixelValueType type = getPixelValueType(pixelData[i]);
		if (type != CXPixelValueTypeValid) {
			count++;
		}
	}

	return count;
}
