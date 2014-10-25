#ifndef TIFF_UTIL_H
#define TIFF_UTIL_H

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
} TiffIFDEntity;
typedef struct tiffIFD {
	short numOfDirEntities; // 2 bytes.
	TiffIFDEntity* entities; // Array of entities. 12*numOfDirEntities bytes.
	long nextIFDOffset; // 4 bytes.
} TiffIFD;



// ================== Paramerters when using ==================
typedef struct tiffParas {
	// Header.
	char isLittleEndian; // 0-1, 2 bytes. little-endian: 4949.H; big-endian: 4D4D.H.
	short version; // 2-3, 2 bytes.
	long firstIFDOffset; // 4-7, 4 bytes.


	// IFD.
	TiffIFD ifd; // 1 IFD.
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


} TiffParas;


// Write TIFF.
void prepareAndWrite(TiffParas* paras, long* pixelData, const char* fileName);
// Read TIFF.
void readTIFFParas(TiffParas* paras, const char* fileName);
void readTIFFPixelsData(const TiffParas* paras, long* pixelData, const char* fileName);

#endif // TIFF_UTIL_H