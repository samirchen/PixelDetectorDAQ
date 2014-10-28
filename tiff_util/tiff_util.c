#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tiff_util.h"
//#include "../common/cpuUsage.h"


TiffParas Paras;


/* ######################## Method Declare ######################## */
/*
// ================= Out of this file. ================
// In "cpuUsage.c".
void getWholeCPUStatus(ProcStat* ps);
float calWholeCPUUse(ProcStat* ps1, ProcStat* ps2);
void getProcessCPUStatus(ProcPidStat* pps, pid_t pid);
float calProcessCPUUse(ProcStat* ps1, ProcPidStat* pps1, ProcStat* ps2, ProcPidStat* pps2);

// Thread "/proc/<pid>/task/<tid>" has the same data structure as process, ProcPidStat.
void getThreadCPUStatus(ProcPidStat* pps, pid_t pid, pid_t tid);
float calThreadCPUUse(ProcStat* ps1, ProcPidStat* pps1, ProcStat* ps2, ProcPidStat* pps2);
*/


// ================= In this file. ================
// Utility.
int checkIsLittleEndian();
void fill(char value, int count, FILE* fp);
void writeStr(char* str, int limitLen, FILE* fp);
void writeInteger(long i, int len, FILE* fp);
// Write Tiff.
//void prepareAndWrite(TiffParas* paras, long* pixelData, const char* fileName); // API in .h.
void writePixelsDataToTIFF(const long* pixelData, const char* fileName);
// Read Tiff.
//void readTIFFParas(TiffParas* paras, const char* fileName);
//void readTIFFPixelsData(const TiffParas* paras, long* pixelData, const char* fileName);


/*
int main() {
		
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
	}
		printf("%ld ", rData[i]);
	printf("\n");
	free(rParas);
	free(rData);

	return 0;
}
*/



int checkIsLittleEndian() {
	union checkData {
		int i;
		char c;
	} d;
	d.i = 1;
	if (d.c == 1) {
		printf("System is Little Endian.\n");
		return 1; // Little Endian.
	}
	else {
		printf("System is Big Endian.\n");
		return 0; // Big Endian.
	}
}

void fill(char value, int count, FILE* fp) {
	int i = 0;
	for (i = 0; i < count; i++) {
		fwrite(&value, 1, 1, fp);
	}
}

void writeStr(char* str, int limitLen, FILE* fp) {
	size_t strLen = strlen(str);
	if (strLen < limitLen) {
		fwrite(str, 1, strLen, fp);
		fill(0x00, limitLen-strLen, fp);
	}
	else {
		fwrite(str, 1, limitLen, fp);
	}
}

void writeInteger(long i, int len, FILE* fp) {
	if (len < 1 || len > 4) {
		perror("Not compatible integer.");
		exit(1);
	}
	else {

		if (!Paras.isLittleEndian) {
			if (len == 1) {
			
			}
			else if (len == 2) {
				i = switch16(i);
			}
			else if (len == 3) {
				i = switch24(i);
			}
			else if (len == 4) {
				i = switch32(i);
			}
		}

		fwrite(&i, len, 1, fp);
	}
	
	
}


void prepareAndWrite(TiffParas* paras, long* pixelData, const char* fileName) {

	if (paras == NULL) {
		printf("Set default tiff Paras.\n");
		// Set default parameters.
		Paras.isLittleEndian = 1;
		Paras.version = 0x2A;
		Paras.firstIFDOffset = 0x82;

		Paras.width = 10;
		Paras.height = 10;
		Paras.bitsPerSample = 32;
		Paras.stripOffset = 2048;
		Paras.artist = "artist";
		Paras.datetime = "2014:07:07 22:30:23";
		Paras.model = "model";
		Paras.software = "software";
		Paras.xResolutionA = 1043;
		Paras.xResolutionB = 7;
		Paras.yResolutionA = 1043;
		Paras.yResolutionB = 7;
		Paras.imageDescription = "image description";
	}
	else {
		// Set parameters.
		Paras.isLittleEndian = paras->isLittleEndian;
		Paras.version = paras->version;
		Paras.firstIFDOffset = paras->firstIFDOffset;

		Paras.width = paras->width;
		Paras.height = paras->height;
		Paras.bitsPerSample = paras->bitsPerSample;
		Paras.stripOffset = paras->stripOffset;
		Paras.artist = paras->artist;
		Paras.datetime = paras->datetime;
		Paras.model = paras->model;
		Paras.software = paras->software;
		Paras.xResolutionA = paras->xResolutionA;
		Paras.xResolutionB = paras->xResolutionB;
		Paras.yResolutionA = paras->yResolutionA;
		Paras.yResolutionB = paras->yResolutionB;
		Paras.imageDescription = paras->imageDescription;
	}
	

	//// IFD.
	TiffIFD firstIFD;
	firstIFD.numOfDirEntities = 0x11;
	firstIFD.entities = malloc(firstIFD.numOfDirEntities*sizeof(TiffIFDEntity)); // Cannot replace "sizeof(TiffIFDEntity)" with "LDirEntity".
	// Set IFD.
	// 1.
	TiffIFDEntity enSubfileType;
	enSubfileType.tag = ENTagSubfileType;
	enSubfileType.type = ENTypeLong;
	enSubfileType.count = 0x01;
	enSubfileType.valueOrOffset = 0x00;
	firstIFD.entities[ENIndexSubfileType] = enSubfileType;
	// 2.
	TiffIFDEntity enImageWidth;
	enImageWidth.tag = ENTagImageWidth;
	enImageWidth.type = ENTypeLong;
	enImageWidth.count = 0x01;
	enImageWidth.valueOrOffset = Paras.width;
	firstIFD.entities[ENIndexImageWidth] = enImageWidth;
	// 3.
	TiffIFDEntity enImageHeight;
	enImageHeight.tag = ENTagImageHeight;
	enImageHeight.type = ENTypeLong;
	enImageHeight.count = 0x01;
	enImageHeight.valueOrOffset = Paras.height;
	firstIFD.entities[ENIndexImageHeight] = enImageHeight;
	// 4.
	TiffIFDEntity enBitsPerSample;
	enBitsPerSample.tag = ENTagBitsPerSample;
	enBitsPerSample.type = ENTypeShort;
	enBitsPerSample.count = 0x01;
	enBitsPerSample.valueOrOffset = Paras.bitsPerSample;
	firstIFD.entities[ENIndexBitsPerSample] = enBitsPerSample;
	// 5.
	TiffIFDEntity enCompression;
	enCompression.tag = ENTagCompression;
	enCompression.type = ENTypeShort;
	enCompression.count = 0x01;
	enCompression.valueOrOffset = 0x01;
	firstIFD.entities[ENIndexCompression] = enCompression;
	// 6.
	TiffIFDEntity enPhotometricInterpretation;
	enPhotometricInterpretation.tag = ENTagPhotometricInterpretation;
	enPhotometricInterpretation.type = ENTypeShort;
	enPhotometricInterpretation.count = 0x01;
	enPhotometricInterpretation.valueOrOffset = 0x01;
	firstIFD.entities[ENIndexPhotometricInterpretation] = enPhotometricInterpretation;
	// 7.
	TiffIFDEntity enImageDescription;
	enImageDescription.tag = ENTagImageDescription;
	enImageDescription.type = ENTypeASCII;
	enImageDescription.count = 0x1E8;
	enImageDescription.valueOrOffset = 0x190;
	firstIFD.entities[ENIndexImageDescription] = enImageDescription;
	// 8.
	TiffIFDEntity enModel;
	enModel.tag = ENTagModel;
	enModel.type = ENTypeASCII;
	enModel.count = 0x30;
	enModel.valueOrOffset = 0x3E;
	firstIFD.entities[ENIndexModel] = enModel;
	// 9.
	TiffIFDEntity enStripOffsets;
	enStripOffsets.tag = ENTagStripOffsets;
	enStripOffsets.type = ENTypeLong;
	enStripOffsets.count = 0x01;
	enStripOffsets.valueOrOffset = Paras.stripOffset;
	firstIFD.entities[ENIndexStripOffsets] = enStripOffsets;
	// 10.
	TiffIFDEntity enRowsPerStrip;
	enRowsPerStrip.tag = ENTagRowsPerStrip;
	enRowsPerStrip.type = ENTypeLong;
	enRowsPerStrip.count = 0x01;
	enRowsPerStrip.valueOrOffset = 0x413;
	firstIFD.entities[ENIndexRowsPerStrip] = enRowsPerStrip;
	// 11.
	TiffIFDEntity enStripByteCounts;
	enStripByteCounts.tag = ENTagStripByteCounts;
	enStripByteCounts.type = ENTypeLong;
	enStripByteCounts.count = 0x01;
	enStripByteCounts.valueOrOffset = Paras.bitsPerSample/8 * Paras.width * Paras.height;
	firstIFD.entities[ENIndexStripByteCounts] = enStripByteCounts;
	// 12.
	TiffIFDEntity enXResolution;
	enXResolution.tag = ENTagXResolution;
	enXResolution.type = ENTypeRational;
	enXResolution.count = 0x01;
	enXResolution.valueOrOffset = 0x154;
	firstIFD.entities[ENIndexXResolution] = enXResolution;
	// 13.
	TiffIFDEntity enYResolution;
	enYResolution.tag = ENTagYResolution;
	enYResolution.type = ENTypeRational;
	enYResolution.count = 0x01;
	enYResolution.valueOrOffset = 0x15C;
	firstIFD.entities[ENIndexYResolution] = enYResolution;
	// 14.
	TiffIFDEntity enSoftware;
	enSoftware.tag = ENTagSoftware;
	enSoftware.type = ENTypeASCII;
	enSoftware.count = 0x14;
	enSoftware.valueOrOffset = 0x6E;
	firstIFD.entities[ENIndexSoftware] = enSoftware;
	// 15.
	TiffIFDEntity enDateTime;
	enDateTime.tag = ENTagDateTime;
	enDateTime.type = ENTypeASCII;
	enDateTime.count = 0x20;
	enDateTime.valueOrOffset = 0x1E;
	firstIFD.entities[ENIndexDateTime] = enDateTime;
	// 16.
	TiffIFDEntity enArtist;
	enArtist.tag = ENTagArtist;
	enArtist.type = ENTypeASCII;
	enArtist.count = 0x12;
	enArtist.valueOrOffset = 0x0C;
	firstIFD.entities[ENIndexArtist] = enArtist;
	// 17.
	TiffIFDEntity enSampleFormat;
	enSampleFormat.tag = ENTagSampleFormat;
	enSampleFormat.type = ENTypeShort;
	enSampleFormat.count = 0x01;
	enSampleFormat.valueOrOffset = 0x020002;
	firstIFD.entities[ENIndexSampleFormat] = enSampleFormat;
	// Next IFD offset or 0 if none.
	firstIFD.nextIFDOffset = 0x0;
	Paras.ifd = firstIFD;

	// Write tiff.
	writePixelsDataToTIFF(pixelData, fileName);

	// Clear.
	free(firstIFD.entities);

}

void writePixelsDataToTIFF(const long* pixelData, const char* fileName) {

	//const char* fileName = "sample.tif";
	FILE* fp = fopen(fileName, "wb");
	if (!fp) {
		perror("Error: Failed to open tiff file when write.");
		exit(1);
	}

	// ========= Header ===========
	// Byte order. 0-1, 2 bytes.
	int byteOrder = 0x4949;
	if (Paras.isLittleEndian) {
		byteOrder = 0x4949;
	}
	else {
		byteOrder = 0x4D4D;
	}
	//fwrite(&byteOrder, LByteOrder, 1, fp);
	writeInteger(byteOrder, LByteOrder, fp);

	// Version. 2-3, 2 bytes.
	//fwrite(&Paras.version, LVersion, 1, fp);
	writeInteger(Paras.version, LVersion, fp);

	// First IFD offset. 4-7, 4 bytes.
	//fwrite(&Paras.firstIFDOffset, LFirstIFDOffset, 1, fp);  
	writeInteger(Paras.firstIFDOffset, LFirstIFDOffset, fp);

	// ======= Info ========
	// Fill 0. 8-11, 4 bytes.
	fill(0x00, 4, fp);

	// Artist. 12-29, 18 bytes.
	writeStr(Paras.artist, LArtist, fp);

	// DateTime. 30-61, 32 bytes.
	writeStr(Paras.datetime, LDateTime, fp);

	// Model. 62-109, 48 bytes.
	writeStr(Paras.model, LModel, fp);

	// Software. 110-129, 20 bytes.
	writeStr(Paras.software, LSoftware, fp);
	

	// ====== IFD ======
	// IFD. 130-339, 210 bytes.
	writeInteger(Paras.ifd.numOfDirEntities, LNumOfDirEntities, fp);
	int i = 0;
	for (i = 0; i < Paras.ifd.numOfDirEntities; ++i) {
		TiffIFDEntity en = Paras.ifd.entities[i];
		writeInteger(en.tag, LDirEntityTag, fp);
		writeInteger(en.type, LDirEntityType, fp);
		writeInteger(en.count, LDirEntityCount, fp);
		writeInteger(en.valueOrOffset, LDirEntityValueOrOffset, fp);
	}
	writeInteger(Paras.ifd.nextIFDOffset, LNextIFDOffset, fp);

	// XResolution. 340-347, 8 bytes.
	writeInteger(Paras.xResolutionA, 4, fp);
	writeInteger(Paras.xResolutionB, 4, fp);
	// YResolution. 340-347, 8 bytes.
	writeInteger(Paras.yResolutionA, 4, fp);
	writeInteger(Paras.yResolutionB, 4, fp);

	// Fill 0. 356-399, 44 bytes.
	fill(0x00, 44, fp);

	// ImageDescription. 400-887, 488 bytes.
	writeStr(Paras.imageDescription, LImageDescription, fp);
	
	// Fill 0. 888-2047, 1160 byts.
	fill(0x00, 1160, fp);

	// Image Data.
	for (i = 0; i < Paras.width*Paras.height; i++) {
		writeInteger(pixelData[i], Paras.bitsPerSample/8, fp);
	}

	fclose(fp);

}




void readTIFFParas(TiffParas* paras, const char* fileName) {
	//const char* fileName = "x.tif";
	FILE* fp = fopen(fileName, "rb");
	if (!fp) {
		perror("Error: Failed to open tiff file when read paras.");
		exit(1);
	}

	// Byte order. 0-1, 2 bytes.
	//int byteOrder = 0; // Important to init this when it's int, or some bits are random.
	short byteOrder = 0; 
	fread(&byteOrder, LByteOrder, 1, fp);
	printf("ByteOrder: 0x%x\n", byteOrder);
	if (byteOrder == 0x4949) {
		paras->isLittleEndian = 1;
	}
	else if (byteOrder == 0x4D4D) {
		paras->isLittleEndian = 0;
	}
	else {
		perror("Error: Not compatible byte order.");
		exit(1);
	}

	// First IFD offset. 4-7, 4 bytes.
	fseek(fp, LByteOrder+LVersion, SEEK_SET);
	fread(&paras->firstIFDOffset, LFirstIFDOffset, 1, fp);
	printf("First IFD Offset: %ld\n", paras->firstIFDOffset);

	// IFD info.
	// Number of entities. 2 bytes.
	fseek(fp, paras->firstIFDOffset, SEEK_SET);
	fread(&paras->ifd.numOfDirEntities, LNumOfDirEntities, 1, fp);
	printf("Number of IFD entities: %d\n", paras->ifd.numOfDirEntities);
	// Get image width, image height, bits per sample and strip offset info from IFD.
	int i = 0;
	for (i = 0; i < paras->ifd.numOfDirEntities; i++) {
		int tag = 0;
		fread(&tag, LDirEntityTag, 1, fp);

		long value = 0;
		if (tag == ENTagImageWidth) {
			fseek(fp, LDirEntityType+LDirEntityCount, SEEK_CUR);
			fread(&value, LDirEntityValueOrOffset, 1, fp);
			paras->width = value;
		}
		else if (tag == ENTagImageHeight) {
			fseek(fp, LDirEntityType+LDirEntityCount, SEEK_CUR);
			fread(&value, LDirEntityValueOrOffset, 1, fp);
			paras->height = value;
		}
		else if (tag == ENTagBitsPerSample) {
			fseek(fp, LDirEntityType+LDirEntityCount, SEEK_CUR);
			fread(&value, LDirEntityValueOrOffset, 1, fp);
			paras->bitsPerSample = value;
		}
		else if (tag == ENTagStripOffsets) {
			fseek(fp, LDirEntityType+LDirEntityCount, SEEK_CUR);
			fread(&value, LDirEntityValueOrOffset, 1, fp);
			paras->stripOffset = value;
		}
		else {
			fseek(fp, LDirEntityType+LDirEntityCount+LDirEntityValueOrOffset, SEEK_CUR);
		}
	}
	

	fclose(fp);
}

void readTIFFPixelsData(const TiffParas* paras, long* pixelData, const char* fileName) {

	//const char* fileName = "x.tif";
	FILE* fp = fopen(fileName, "rb");
	if (!fp) {
		perror("Error: Failed to open tiff file when read data.");
		exit(1);
	}

	// Get pixels data.
	fseek(fp, paras->stripOffset, SEEK_SET);
	long size = paras->width*paras->height;
	int i = 0;
	for (i = 0; i < size; i++) {
		long value = 0;
		int typeLen = paras->bitsPerSample/8;
		fread(&value, typeLen, 1, fp);
		if (typeLen == 2) {
			pixelData[i] = (short)value;
		}
		else if (typeLen == 4) {
			pixelData[i] = (int)value;
		}
		else { // long
			pixelData[i] = value;
		}
	}
	
	fclose(fp);

}

