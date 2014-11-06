#ifndef IMAGE_UTIL_H
#define IMAGE_UTIL_H

// ================== Pixel Point ==================
typedef struct cxPixelPoint {
	int x;
	int y;
	long value;

	cxPixelPoint() {
		x = 0; 
		y = 0; 
		value = 0;
	}
	cxPixelPoint(int tx, int ty, int tvalue) {
		x = tx;
		y = ty;
		value = tvalue;
	}
} CXPixelPoint;

enum CXPixelValueType { 
	CXPixelValueTypeValid = 0,
	CXPixelValueTypeGap = -1,
	CXPixelValueTypeBad = -2
};

// ================== IQ Data ==================
typedef struct cxIQData  {
	long q2; // Q is the distance to the center, it may be not integer, so use Q^2 here.
	double averageI; // Average I count for all NOMAL pixels whose distance to center is Q.
	double totalI; // Average I count for all NOMAL pixels whose distance to center is Q.
	int normalPixelCount;
	int badPixelCount;
	int gapPixelCount;

	cxIQData() {
		q2 = -1;
		averageI = 0; 
		totalI = 0; 
		normalPixelCount = 0; 
		badPixelCount = 0; 
		gapPixelCount = 0;
	}
	cxIQData(long tq2, double taverageI, double ttotalI, int tnormalPixelCount, int tbadPixelCount, int tgapPixelCount) {
		q2 = tq2;
		averageI = taverageI;
		totalI = ttotalI;
		normalPixelCount = tnormalPixelCount;
		badPixelCount = tbadPixelCount;
		gapPixelCount = tgapPixelCount;
	}
} CXIQData;


// API.
void calculateCenterPoint(int width, const int height, const long* pixelData, CXPixelPoint* center);
void calculateIQData(const int width, const int height, const long* pixelData, const CXPixelPoint* center, std::map<long, CXIQData> & iq2map);
int countInvalidPixels(const int width, const int height, const long* pixelData);


#endif // IMAGE_UTIL_H