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

void fixInvalidPixel(const int width, const int height, long* pixelData) {
	printf("Begin to fix bad pixels.\n");
	int i = 0;

	// data[height][width] Style. 
	// long** data = (long**) malloc(sizeof(long*) * height);
	// for (i = 0; i < height; ++i) {
	// 	data[i] = (long*) malloc(sizeof(long) * width);
	// }


	

	int badCount = 0;
	int size = width * height;
	for (i = 0; i < size; i++) {
		int x = i % width;
		int y = i / width;

		CXPixelValueType type = getPixelValueType(pixelData[i]);
		if (type == CXPixelValueTypeBad || type == CXPixelValueTypeGap) {
			badCount++;
			printf("Get bad pixel %d and its neighbors:\n", badCount);

			long sum = 0;
			int validNeighborsCount = 0;

			// Warning:
			// offset 这里需要改进，需要讨论在当前 offset 下没完成取均值的情况下 offset 递增到下一个值：√2。
			// 这里讨论 offset^2 比较好，都是平方和正整数：1，2，5，8，10...
			int offset = 1; 

			int startX = (x - offset) < 0 ? 0 : (x - offset);
			int startY = (y - offset) < 0 ? 0 : (y - offset);
			int endX = (x + offset) > (width - 1) ? (width - 1) : (x + offset);
			int endY = (y + offset) > (height - 1) ? (height - 1) : (y + offset);

			int m = 0;
			int n = 0;
			for (n = startY; n <= endY; ++n) {
				for (m = startX; m <= endX; ++m) {
					int index = n * width + m;
					
					long value = pixelData[index];
					printf("%ld ", value);

					if (getPixelValueType(value) == CXPixelValueTypeValid) {
						sum += value;
						validNeighborsCount++;
					}
				}
				printf("\n");
			}
			pixelData[i] = sum / validNeighborsCount;
		}
		else {
		}
	}

}
