#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include "../common/dieWithError.h"
//#include "../common/cpuUsage.h"

// ================== Macros ====================
// little-endian to big-endian or big-endian to little-endian.
#define switch16(x) ( (short) ( (((short)(x) & (short)0x00ffU) << 8) | (((short)(x) & (short)0xff00U) >> 8) ) )
#define switch24(x) ( ( (((x) & 0x0000ffU) << 16) | ((x) & 0x00ff00U ) | (((x) & 0xff0000U) >> 16) ) )
#define switch32(x) ( (long) ( ((long)(x) & (long)0x000000ffU) << 24  ) | ( ((long)(x) & (long)0x0000ff00U) << 8  ) | (((long)(x) & (long)0x00ff0000U) >> 8) | (((long)(x) & (long)0xff000000U) >> 24) )

// ================== Consts when using ==================
// Normal IFD index.
#define ENIndexSubfileType 0
#define ENIndexImageWidth 1
#define ENIndexImageHeight 2
#define ENIndexBitsPerSample 3
#define ENIndexCompression 4
#define ENIndexPhotometricInterpretation 5
#define ENIndexImageDescription 6
#define ENIndexModel 7
#define ENIndexStripOffsets 8
#define ENIndexRowsPerStrip 9
#define ENIndexStripByteCounts 10
#define ENIndexXResolution 11
#define ENIndexYResolution 12
#define ENIndexSoftware 13
#define ENIndexDateTime 14
#define ENIndexArtist 15
#define ENIndexSampleFormat 16

// Normal IFD tags.
#define ENTagSubfileType 0xFE
#define ENTagImageWidth 0x100
#define ENTagImageHeight 0x101
#define ENTagBitsPerSample 0x102
#define ENTagCompression 0x103
#define ENTagPhotometricInterpretation 0x106
#define ENTagImageDescription 0x10E
#define ENTagModel 0x110
#define ENTagStripOffsets 0x111
#define ENTagRowsPerStrip 0x116
#define ENTagStripByteCounts 0x117
#define ENTagXResolution 0x11A
#define ENTagYResolution 0x11B
#define ENTagSoftware 0x131
#define ENTagDateTime 0x132
#define ENTagArtist 0x13B
#define ENTagSampleFormat 0x153

// Normal IFD data type.
#define ENTypeByte 0x01
#define ENTypeASCII 0x02
#define ENTypeShort 0x03
#define ENTypeLong 0x04
#define ENTypeRational 0x05

// Length(Bytes) of each field.
#define LByteOrder 2
#define LVersion 2
#define LFirstIFDOffset 4

#define LArtist 18
#define LDateTime 32
#define LModel 48
#define LSoftware 20
#define LImageDescription 488

#define LNumOfDirEntities 2
#define LDirEntity 12
#define LDirEntityTag 2
#define LDirEntityType 2
#define LDirEntityCount 4
#define LDirEntityValueOrOffset 4
#define LNextIFDOffset 4




// ================== IFD ==================
typedef struct tiffIFDEntity {
	short tag; // 2 bytes.
	short type; // 2 bytes.
	long count; // 4 bytes.
	long valueOrOffset;	// 4 bytes.
} IFDEntity;
typedef struct tiffIFD {
	short numOfDirEntities; // 2 bytes.
	IFDEntity* entities; // Array of entities. 12*numOfDirEntities bytes.
	long nextIFDOffset; // 4 bytes.
} IFD;



// ================== Paramerters when using ==================
struct PARAS {
	// Header.
	char isLittleEndian; // 0-1, 2 bytes. little-endian: 4949.H; big-endian: 4D4D.H.
	short version; // 2-3, 2 bytes.
	long firstIFDOffset; // 4-7, 4 bytes.


	// IFD.
	IFD ifd; // 1 IFD.
	//// Some IFD entities value.
	long width;
	long height;
	short bitsPerSample;
	long stripOffset;
	char* artist; // 18 bytes.
	char* datetime; // 32 bytes.
	char* model; // 48 bytes.
	char* software; // 20 bytes.
	long xResolutionA; // 4 bytes.
	long xResolutionB; // 4 bytes.
	long yResolutionA; // 4 bytes.
	long yResolutionB; // 4 bytes.
	char* imageDescription; // 488 bytes.


} Paras;


/* ######################## Method Declare ######################## */
/*
// ================= Out of this file. ================
// In "dieWithError.c".
void dieWithError(const char *errorMessage);

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
void prepareAndWrite();
void writePixelsDataToTIFF();
void prepareAndRead();
void readTIFFPixelsData();
void test();

int main() {
	
	//test();
	
	//prepareAndWrite();

	prepareAndRead();

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


void prepareAndWrite() {

	// Set parameters.
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

	//// IFD.
	IFD firstIFD;
	firstIFD.numOfDirEntities = 0x11;
	firstIFD.entities = malloc(firstIFD.numOfDirEntities*sizeof(IFDEntity)); // Cannot replace "sizeof(IFDEntity)" with "LDirEntity".
	// Set IFD.
	// 1.
	IFDEntity enSubfileType;
	enSubfileType.tag = ENTagSubfileType;
	enSubfileType.type = ENTypeLong;
	enSubfileType.count = 0x01;
	enSubfileType.valueOrOffset = 0x00;
	firstIFD.entities[ENIndexSubfileType] = enSubfileType;
	// 2.
	IFDEntity enImageWidth;
	enImageWidth.tag = ENTagImageWidth;
	enImageWidth.type = ENTypeLong;
	enImageWidth.count = 0x01;
	enImageWidth.valueOrOffset = Paras.width;
	firstIFD.entities[ENIndexImageWidth] = enImageWidth;
	// 3.
	IFDEntity enImageHeight;
	enImageHeight.tag = ENTagImageHeight;
	enImageHeight.type = ENTypeLong;
	enImageHeight.count = 0x01;
	enImageHeight.valueOrOffset = Paras.height;
	firstIFD.entities[ENIndexImageHeight] = enImageHeight;
	// 4.
	IFDEntity enBitsPerSample;
	enBitsPerSample.tag = ENTagBitsPerSample;
	enBitsPerSample.type = ENTypeShort;
	enBitsPerSample.count = 0x01;
	enBitsPerSample.valueOrOffset = Paras.bitsPerSample;
	firstIFD.entities[ENIndexBitsPerSample] = enBitsPerSample;
	// 5.
	IFDEntity enCompression;
	enCompression.tag = ENTagCompression;
	enCompression.type = ENTypeShort;
	enCompression.count = 0x01;
	enCompression.valueOrOffset = 0x01;
	firstIFD.entities[ENIndexCompression] = enCompression;
	// 6.
	IFDEntity enPhotometricInterpretation;
	enPhotometricInterpretation.tag = ENTagPhotometricInterpretation;
	enPhotometricInterpretation.type = ENTypeShort;
	enPhotometricInterpretation.count = 0x01;
	enPhotometricInterpretation.valueOrOffset = 0x01;
	firstIFD.entities[ENIndexPhotometricInterpretation] = enPhotometricInterpretation;
	// 7.
	IFDEntity enImageDescription;
	enImageDescription.tag = ENTagImageDescription;
	enImageDescription.type = ENTypeASCII;
	enImageDescription.count = 0x1E8;
	enImageDescription.valueOrOffset = 0x190;
	firstIFD.entities[ENIndexImageDescription] = enImageDescription;
	// 8.
	IFDEntity enModel;
	enModel.tag = ENTagModel;
	enModel.type = ENTypeASCII;
	enModel.count = 0x30;
	enModel.valueOrOffset = 0x3E;
	firstIFD.entities[ENIndexModel] = enModel;
	// 9.
	IFDEntity enStripOffsets;
	enStripOffsets.tag = ENTagStripOffsets;
	enStripOffsets.type = ENTypeLong;
	enStripOffsets.count = 0x01;
	enStripOffsets.valueOrOffset = Paras.stripOffset;
	firstIFD.entities[ENIndexStripOffsets] = enStripOffsets;
	// 10.
	IFDEntity enRowsPerStrip;
	enRowsPerStrip.tag = ENTagRowsPerStrip;
	enRowsPerStrip.type = ENTypeLong;
	enRowsPerStrip.count = 0x01;
	enRowsPerStrip.valueOrOffset = 0x413;
	firstIFD.entities[ENIndexRowsPerStrip] = enRowsPerStrip;
	// 11.
	IFDEntity enStripByteCounts;
	enStripByteCounts.tag = ENTagStripByteCounts;
	enStripByteCounts.type = ENTypeLong;
	enStripByteCounts.count = 0x01;
	enStripByteCounts.valueOrOffset = Paras.bitsPerSample/8 * Paras.width * Paras.height;
	firstIFD.entities[ENIndexStripByteCounts] = enStripByteCounts;
	// 12.
	IFDEntity enXResolution;
	enXResolution.tag = ENTagXResolution;
	enXResolution.type = ENTypeRational;
	enXResolution.count = 0x01;
	enXResolution.valueOrOffset = 0x154;
	firstIFD.entities[ENIndexXResolution] = enXResolution;
	// 13.
	IFDEntity enYResolution;
	enYResolution.tag = ENTagYResolution;
	enYResolution.type = ENTypeRational;
	enYResolution.count = 0x01;
	enYResolution.valueOrOffset = 0x15C;
	firstIFD.entities[ENIndexYResolution] = enYResolution;
	// 14.
	IFDEntity enSoftware;
	enSoftware.tag = ENTagSoftware;
	enSoftware.type = ENTypeASCII;
	enSoftware.count = 0x14;
	enSoftware.valueOrOffset = 0x6E;
	firstIFD.entities[ENIndexSoftware] = enSoftware;
	// 15.
	IFDEntity enDateTime;
	enDateTime.tag = ENTagDateTime;
	enDateTime.type = ENTypeASCII;
	enDateTime.count = 0x20;
	enDateTime.valueOrOffset = 0x1E;
	firstIFD.entities[ENIndexDateTime] = enDateTime;
	// 16.
	IFDEntity enArtist;
	enArtist.tag = ENTagArtist;
	enArtist.type = ENTypeASCII;
	enArtist.count = 0x12;
	enArtist.valueOrOffset = 0x0C;
	firstIFD.entities[ENIndexArtist] = enArtist;
	// 17.
	IFDEntity enSampleFormat;
	enSampleFormat.tag = ENTagSampleFormat;
	enSampleFormat.type = ENTypeShort;
	enSampleFormat.count = 0x01;
	enSampleFormat.valueOrOffset = 0x020002;
	firstIFD.entities[ENIndexSampleFormat] = enSampleFormat;
	// Next IFD offset or 0 if none.
	firstIFD.nextIFDOffset = 0x0;
	Paras.ifd = firstIFD;

	// Write tiff.
	writePixelsDataToTIFF();

	// Clear.
	free(firstIFD.entities);

}

void writePixelsDataToTIFF() {

	const char* fileName = "sample.tif";
	FILE* fp = fopen(fileName, "wb");


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
		IFDEntity en = Paras.ifd.entities[i];
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
		writeInteger(i, Paras.bitsPerSample/8, fp);
	}

	fclose(fp);

}

void prepareAndRead() {
	readTIFFPixelsData();
}

void readTIFFPixelsData() {

	const char* fileName = "x.tif";
	FILE* fp = fopen(fileName, "rb");

	// Byte order. 0-1, 2 bytes.
	//int byteOrder = 0; // Important to init this when it's int, or some bits are random.
	short byteOrder = 0; 
	fread(&byteOrder, LByteOrder, 1, fp);
	printf("ByteOrder: 0x%x\n", byteOrder);
	if (byteOrder == 0x4949) {
		Paras.isLittleEndian = 1;
	}
	else if (byteOrder == 0x4D4D) {
		Paras.isLittleEndian = 0;
	}
	else {
		perror("Not compatible byte order.");
		exit(1);
	}

	// First IFD offset. 4-7, 4 bytes.
	fseek(fp, LByteOrder+LVersion, SEEK_SET);
	fread(&Paras.firstIFDOffset, LFirstIFDOffset, 1, fp);
	printf("First IFD Offset: %ld\n", Paras.firstIFDOffset);

	// IFD info.
	// Number of entities. 2 bytes.
	fseek(fp, Paras.firstIFDOffset, SEEK_SET);
	fread(&Paras.ifd.numOfDirEntities, LNumOfDirEntities, 1, fp);
	printf("Number of IFD entities: %d\n", Paras.ifd.numOfDirEntities);
	// Get image width, image height, bits per sample and strip offset info from IFD.
	int i = 0;
	for (i = 0; i < Paras.ifd.numOfDirEntities; i++) {
		int tag = 0;
		fread(&tag, LDirEntityTag, 1, fp);

		long value = 0;
		if (tag == ENTagImageWidth) {
			fseek(fp, LDirEntityType+LDirEntityCount, SEEK_CUR);
			fread(&value, LDirEntityValueOrOffset, 1, fp);
			Paras.width = value;
		}
		else if (tag == ENTagImageHeight) {
			fseek(fp, LDirEntityType+LDirEntityCount, SEEK_CUR);
			fread(&value, LDirEntityValueOrOffset, 1, fp);
			Paras.height = value;
		}
		else if (tag == ENTagBitsPerSample) {
			fseek(fp, LDirEntityType+LDirEntityCount, SEEK_CUR);
			fread(&value, LDirEntityValueOrOffset, 1, fp);
			Paras.bitsPerSample = value;
		}
		else if (tag == ENTagStripOffsets) {
			fseek(fp, LDirEntityType+LDirEntityCount, SEEK_CUR);
			fread(&value, LDirEntityValueOrOffset, 1, fp);
			Paras.stripOffset = value;
		}
		else {
			fseek(fp, LDirEntityType+LDirEntityCount+LDirEntityValueOrOffset, SEEK_CUR);
		}
	}
	printf("Image Width: %ld\n", Paras.width);
	printf("Image Height: %ld\n", Paras.height);
	printf("Bits Per Sample: %d\n", Paras.bitsPerSample);
	printf("Strip Offset: %ld\n", Paras.stripOffset);


	// Get pixels data.
	fseek(fp, Paras.stripOffset, SEEK_SET);
	long* data = malloc(sizeof(long) * Paras.width * Paras.height);
	long size = Paras.width*Paras.height;
	long printLimit = size <= 1000 ? size : 1000;
	for (i = 0; i < printLimit; i++) {
		long value = 0;
		fread(&value, Paras.bitsPerSample/8, 1, fp);
		data[i] = value;
		printf("%ld ", data[i]);
	}
	printf("\n");

	free(data);

	
	fclose(fp);

}

