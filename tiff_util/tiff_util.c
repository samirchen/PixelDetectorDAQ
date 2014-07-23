#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include "../common/dieWithError.h"
//#include "../common/cpuUsage.h"

// ================== Macros ====================
// little-endian to big-endian or big-endian to little-endian.
#define switch16(x) ( (short) ( (((short)(x) & (short)0x00ffU) << 8) | (((short)(x) & (short)0xff00U) >> 8) ) )
#define switch32(x) ( (long) ( ((long)(x) & (long)0x000000ffU) << 24  ) | ( ((long)(x) & (long)0x0000ff00U) << 8  ) | (((long)(x) & (long)0x00ff0000U) >> 8) | (((long)(x) & (long)0xff000000U) >> 24) )

// ================== Consts when using ==================
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

	// Info.
	char* artist; // 18 bytes.
	char* datetime; // 32 bytes.
	char* model; // 48 bytes.
	char* software; // 20 bytes.

	// IFD.
	IFD ifd; // 1 IFD.
	


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
void fill(char value, int count, FILE* fp);
void writeStr(char* str, int limitLen, FILE* fp);
void writePixelsDataToTIFF();
void test();

int main() {
	printf("Hello\n");

	char* str = "abc";
	size_t len = strlen(str);
	printf("%zu\n", len);

	//test();
	
	// Set parameters.
	Paras.isLittleEndian = 1;
	Paras.version = 0x2A;
	Paras.firstIFDOffset = 0x82;
	Paras.artist = "artist";
	Paras.datetime = "2014:07:07 22:30:23";
	Paras.model = "model";
	Paras.software = "software";

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
	firstIFD.entities[0] = enSubfileType;
	// 2.
	IFDEntity enImageWidth;
	enImageWidth.tag = ENTagImageWidth;
	enImageWidth.type = ENTypeLong;
	enImageWidth.count = 0x01;
	enImageWidth.valueOrOffset = 0x3D5;
	firstIFD.entities[1] = enImageWidth;
	// 3.
	IFDEntity enImageHeight;
	enImageHeight.tag = ENTagImageHeight;
	enImageHeight.type = ENTypeLong;
	enImageHeight.count = 0x01;
	enImageHeight.valueOrOffset = 0x413;
	firstIFD.entities[2] = enImageHeight;
	// 4.
	IFDEntity enBitsPerSample;
	enBitsPerSample.tag = ENTagBitsPerSample;
	enBitsPerSample.type = ENTypeShort;
	enBitsPerSample.count = 0x01;
	enBitsPerSample.valueOrOffset = 0x20;
	firstIFD.entities[3] = enBitsPerSample;
	// 5.
	IFDEntity enCompression;
	enCompression.tag = ENTagCompression;
	enCompression.type = ENTypeShort;
	enCompression.count = 0x01;
	enCompression.valueOrOffset = 0x01;
	firstIFD.entities[4] = enCompression;
	// 6.
	IFDEntity enPhotometricInterpretation;
	enPhotometricInterpretation.tag = ENTagPhotometricInterpretation;
	enPhotometricInterpretation.type = ENTypeShort;
	enPhotometricInterpretation.count = 0x01;
	enPhotometricInterpretation.valueOrOffset = 0x01;
	firstIFD.entities[5] = enPhotometricInterpretation;
	// 7.
	IFDEntity enImageDescription;
	enImageDescription.tag = ENTagImageDescription;
	enImageDescription.type = ENTypeASCII;
	enImageDescription.count = 0x1E8;
	enImageDescription.valueOrOffset = 0x190;
	firstIFD.entities[6] = enImageDescription;
	// 8.
	IFDEntity enModel;
	enModel.tag = ENTagModel;
	enModel.type = ENTypeASCII;
	enModel.count = 0x30;
	enModel.valueOrOffset = 0x3E;
	firstIFD.entities[7] = enModel;
	// 9.
	IFDEntity enStripOffsets;
	enStripOffsets.tag = ENTagStripOffsets;
	enStripOffsets.type = ENTypeLong;
	enStripOffsets.count = 0x01;
	enStripOffsets.valueOrOffset = 0x800;
	firstIFD.entities[8] = enStripOffsets;
	// 10.
	IFDEntity enRowsPerStrip;
	enRowsPerStrip.tag = ENTagRowsPerStrip;
	enRowsPerStrip.type = ENTypeLong;
	enRowsPerStrip.count = 0x01;
	enRowsPerStrip.valueOrOffset = 0x413;
	firstIFD.entities[9] = enRowsPerStrip;
	// 11.
	IFDEntity enStripByteCounts;
	enStripByteCounts.tag = ENTagStripByteCounts;
	enStripByteCounts.type = ENTypeLong;
	enStripByteCounts.count = 0x01;
	enStripByteCounts.valueOrOffset = 0x3E733C;
	firstIFD.entities[10] = enStripByteCounts;
	// 12.
	IFDEntity enXResolution;
	enXResolution.tag = ENTagXResolution;
	enXResolution.type = ENTypeRational;
	enXResolution.count = 0x01;
	enXResolution.valueOrOffset = 0x154;
	firstIFD.entities[11] = enXResolution;
	// 13.
	IFDEntity enYResolution;
	enYResolution.tag = ENTagYResolution;
	enYResolution.type = ENTypeRational;
	enYResolution.count = 0x01;
	enYResolution.valueOrOffset = 0x15C;
	firstIFD.entities[12] = enYResolution;
	// 14.
	IFDEntity enSoftware;
	enSoftware.tag = ENTagSoftware;
	enSoftware.type = ENTypeASCII;
	enSoftware.count = 0x14;
	enSoftware.valueOrOffset = 0x6E;
	firstIFD.entities[13] = enSoftware;
	// 15.
	IFDEntity enDateTime;
	enDateTime.tag = ENTagDateTime;
	enDateTime.type = ENTypeASCII;
	enDateTime.count = 0x20;
	enDateTime.valueOrOffset = 0x1E;
	firstIFD.entities[14] = enDateTime;
	// 16.
	IFDEntity enArtist;
	enArtist.tag = ENTagArtist;
	enArtist.type = ENTypeASCII;
	enArtist.count = 0x12;
	enArtist.valueOrOffset = 0x0C;
	firstIFD.entities[15] = enArtist;
	// 17.
	IFDEntity enSampleFormat;
	enSampleFormat.tag = ENTagSampleFormat;
	enSampleFormat.type = ENTypeShort;
	enSampleFormat.count = 0x01;
	enSampleFormat.valueOrOffset = 0x020002;
	firstIFD.entities[16] = enSampleFormat;
	// Next IFD offset or 0 if none.
	firstIFD.nextIFDOffset = 0x0;
	Paras.ifd = firstIFD;

	// Write tiff.
	writePixelsDataToTIFF();

	// Clear.
	free(firstIFD.entities);

	return 0;
}

void test() {

	const char* fileName = "out.txt";
	FILE* fp = fopen(fileName, "wb");

	int i = switch32(0x12345678);
	char* str = "test";
	size_t len = strlen(str);

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
	fwrite(&byteOrder, LByteOrder, 1, fp);

	// Version. 2-3, 2 bytes.
	fwrite(&Paras.version, LVersion, 1, fp);

	// First IFD offset. 4-7, 4 bytes.
	fwrite(&Paras.firstIFDOffset, LFirstIFDOffset, 1, fp);  

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
	fwrite(&Paras.ifd.numOfDirEntities, LNumOfDirEntities, 1, fp);
	int i = 0;
	for (i = 0; i < Paras.ifd.numOfDirEntities; ++i) {
		IFDEntity en = Paras.ifd.entities[i];
		fwrite(&en.tag, LDirEntityTag, 1, fp);
		fwrite(&en.type, LDirEntityType, 1, fp);
		fwrite(&en.count, LDirEntityCount, 1, fp);
		fwrite(&en.valueOrOffset, LDirEntityValueOrOffset, 1, fp);
	}
	fwrite(&Paras.ifd.nextIFDOffset, LNextIFDOffset, 1, fp);

	// XResolution. 340-347, 8 bytes.
	// ...

	// YResolution. 348-355, 8 bytes.
	// ...

	// Fill 0. 356-399, 44 bytes.
	fill(0x00, 44, fp);

	// ImageDescription.
	// ...
	
	// Fill 0. 400-887, 488 bytes.
	fill(0x00, 488, fp);

	// Image Data.
	// ...

	fclose(fp);

}



