#ifndef IMAGE_UTIL_H
#define IMAGE_UTIL_H

// ================== Pixel Point ==================
typedef struct cxPixelPoint {
	int x;
	int y;
	long value;
} CXPixelPoint;

// API.
void calculateCenterPoint(int width, const int height, const long* pixelData, CXPixelPoint* center);
void calculateIQData(const int width, const int height, const long* pixelData, const CXPixelPoint* center, int iq2map);

#endif // IMAGE_UTIL_H