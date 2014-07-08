#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include "../common/dieWithError.h"
//#include "../common/cpuUsage.h"

// ================== Macros ====================
// little-endian to big-endian or big-endian to little-endian.
#define switch16(x) ((short) ( (((short)(x) & (short)0x00ffU) << 8) | (((short)(x) & (short)0xff00U) >> 8) ) )
#define switch32(x) ( (long) ( ((long)(x) & (long)0x000000ffU) << 24  ) | ( ((long)(x) & (long)0x0000ff00U) << 8  ) | (((long)(x) & (long)0x00ff0000U) >> 8) | (((long)(x) & (long)0xff000000U) >> 24)  )

// ================== Consts when using ==================
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
	// TO BE CONTINUE... Set IFD.
	firstIFD.nextIFDOffset = 0x0;
	

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
	


	fclose(fp);

}



