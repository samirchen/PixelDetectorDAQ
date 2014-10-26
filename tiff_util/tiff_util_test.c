#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tiff_util.h"
#include "image_util.h"

void test();

int main(int argc, char const *argv[]) {
	
	//test();
	
	// Write Data to TIFF.
	//// Tiff paras.
	TiffParas* paras = (TiffParas*) malloc(sizeof(TiffParas));
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
	long* data = (long*) malloc(sizeof(long)*paras->width*paras->height);
	int i = 0;
	for (i = 0; i < paras->width*paras->height; ++i) {
		data[i] = 100+i;
	}
	//// Prepare and write.
	prepareAndWrite(paras, data, "sample.tif");
	//// Clean.
	free(data);
	free(paras);


	// Read Data from TIFF.
	TiffParas* rParas = (TiffParas*) malloc(sizeof(TiffParas));
	readTIFFParas(rParas, "sample.tif");
	printf("Image Width: %ld\n", rParas->width);
	printf("Image Height: %ld\n", rParas->height);
	printf("Bits Per Sample: %d\n", rParas->bitsPerSample);
	printf("Strip Offset: %ld\n", rParas->stripOffset);
	long size = rParas->width*rParas->height;
	long* rData = (long*) malloc(sizeof(long)*size);
	readTIFFPixelsData(rParas, rData, "sample.tif");
	long printLimit = size > 1000 ? 1000 : size;
	for (i = 0; i < printLimit; i++) {
		printf("%ld ", rData[i]);
	}
	printf("\n");
	


	// Calculate Center Point.
	CXPixelPoint p;
	calculateCenterPoint(rParas->width, rParas->height, rData, &p);
	printf("Center:(%d, %d)=%ld\n", p.x, p.y, p.value);


	free(rParas);
	free(rData);

	return 0;
}

void test() {

	int check = 0x1;
	if (*(char*)&check == 0x1) {
		printf("System is Little Endian\n");
	}
	else {
		printf("System is Big Endian\n");
	}

	printf("Hello\n");
	char* str = "abc";
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