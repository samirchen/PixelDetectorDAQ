#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <map>
#include "image_util.h"

extern "C" {
	#include "tiff_util.h"
}

using namespace std;


/* ######################## Method Declare ######################## */
// ================= Out of this file. ================
/*
// In "image_util.c".
void calculateCenterPoint(int width, const int height, const long* pixelData, CXPixelPoint* center);
void calculateIQData(const int width, const int height, const long* pixelData, const CXPixelPoint* center, int iq2map);
// In "tiff_util.c"
// Write TIFF.
void prepareAndWrite(TiffParas* paras, long* pixelData, const char* fileName);
// Read TIFF.
void readTIFFParas(TiffParas* paras, const char* fileName);
void readTIFFPixelsData(const TiffParas* paras, long* pixelData, const char* fileName);
*/

// ================= In this file. ====================
// Method Declare.
void test();



int main(int argc, char const *argv[]) {
	
	//test();
	//return 0;
	
	int i = 0;

	/*
	// Write Data to TIFF.
	//// Tiff paras.
	TiffParas* paras = (TiffParas*) malloc(sizeof(TiffParas)); // Use pointer.
	memset(paras, 0, sizeof(TiffParas)); // Must not forget to bezero paras.
	paras->isLittleEndian = 1;
	paras->version = 0x2A;
	paras->firstIFDOffset = 0x82;
	paras->width = 10;
	paras->height = 10;
	paras->bitsPerSample = 32;
	paras->stripOffset = 2048;
	paras->artist = "artist";
	paras->datetime = "2014:07:07 22:30:23";
	paras->model = "model";
	paras->software = "software";
	paras->xResolutionA = 1043;
	paras->xResolutionB = 7;
	paras->yResolutionA = 1043;
	paras->yResolutionB = 7;
	paras->imageDescription = "image description";
	//// Tiff pixel data.
	int size = paras->width*paras->height;
	long* data = (long*) malloc(sizeof(long)*size);
	memset(data, 0, sizeof(long)*size);
	for (i = 0; i < size; ++i) {
		data[i] = 100+i;
	}
	//// Prepare and write.
	prepareAndWrite(paras, data, "sample.tif");
	//// Clean.
	free(data);
	data = NULL;
	free(paras);
	paras = NULL;
	*/

	// Read Data from TIFF.
	TiffParas rParas; // Not use pointer.
	memset(&rParas, 0, sizeof(TiffParas)); // Must not forget to bezero paras.
	readTIFFParas(&rParas, "csclp5.tif");
	printf("Image Width: %ld\n", rParas.width);
	printf("Image Height: %ld\n", rParas.height);
	printf("Bits Per Sample: %d\n", rParas.bitsPerSample);
	printf("Strip Offset: %ld\n", rParas.stripOffset);
	long rSize = rParas.width*rParas.height;
	long* rData = (long*) malloc(sizeof(long)*rSize);
	memset(rData, 0, sizeof(long)*rSize);
	readTIFFPixelsData(&rParas, rData, "csclp5.tif");
	long printLimit = rSize > 1000 ? 1000 : rSize;
	for (i = 0; i < printLimit; i++) {
		printf("%ld ", rData[i]);
	}
	printf("\n");
	


	// Calculate Center Point.
	CXPixelPoint centerPoint;
	calculateCenterPoint(rParas.width, rParas.height, rData, &centerPoint);
	printf("Center:(%d, %d)=%ld\n", centerPoint.x, centerPoint.y, centerPoint.value);

	free(rData);
	rData = NULL;

	return 0;
}

void test() {

	long l = 4294967296;
	printf("%lu %ld\n", sizeof(short), l);

	int check = 0x1;
	if (*(char*)&check == 0x1) {
		printf("System is Little Endian\n");
	}
	else {
		printf("System is Big Endian\n");
	}

	printf("Hello\n");
	const char* str = "abc";
	size_t len = strlen(str);
	printf("%zu\n", len);

	const char* fileName = "out.txt";
	FILE* fp = fopen(fileName, "wb");

	int i = switch32(0x12345678);
	str = "test";
	len = strlen(str);

	fwrite(&i, sizeof(int), 1, fp);
	fwrite(str, sizeof(char), len, fp);
	fclose(fp);

	fp = fopen(fileName, "rb");
	int k = 0;
	char buf[1024];
	bzero(buf, 1024);

	fread(&k, sizeof(int), 1, fp);
	printf("%d\n", k);
	fread(buf, sizeof(char), 1024, fp);
	printf("%s\n", buf);
	fclose(fp);

}
