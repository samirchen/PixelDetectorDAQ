#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h> // for socket(), bind(), and connect().
#include <arpa/inet.h> // for sockaddr_in and inet_nota().
#include <unistd.h> // for close().
#include <string.h>
#include <map>
#include <sys/time.h>
#include "image_util.h"

extern "C" {
	#include "tiff_util.h"
}

using namespace std;

#define SERVPORT 5555
#define SERVIP "192.168.37.229"

// ================== Parameters when using ==================
struct DATAPROCESSPARAS {
	// Server.
	char* serverIP;
	unsigned short serverPort;

} DataProcessParas;

/* ######################## Method Declare ######################## */
// ================= In this file. ====================
// Method Declare.
void test();
void printUsage();
void sendIQDataToUIClient();



int main(int argc, char* argv[]) {
	
	//test();
	//return 0;

	int i = 1;


	// Default parameter values.
	DataProcessParas.serverIP = (char*) "127.0.0.1";
	DataProcessParas.serverPort = 5555;
	// Get paramerters from inputing.
	for (i = 1; i < argc; i++) {
		if (strcmp(argv[i], "-s") == 0) {
			i++;
			DataProcessParas.serverIP = argv[i];
		}
		else if (strcmp(argv[i], "-p") == 0) {
			i++;
			DataProcessParas.serverPort = atoi(argv[i]);
		}
		else if (strcmp(argv[i], "--help") == 0) {
			printUsage();
			return 0;
		}
		else {

		}
	}



	
	i = 0;

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
	struct timeval t1, t2;
	gettimeofday(&t1, NULL);
	double timeSpan = 0.0;

	TiffParas rParas; // Not use pointer.
	memset(&rParas, 0, sizeof(TiffParas)); // Must not forget to bezero paras.
	readTIFFParas(&rParas, "csclp5.tif");//"../data_acquisitor/127.0.0.1-49654-4.tif");//
	printf("Image Width: %ld\n", rParas.width);
	printf("Image Height: %ld\n", rParas.height);
	printf("Bits Per Sample: %d\n", rParas.bitsPerSample);
	printf("Strip Offset: %ld\n", rParas.stripOffset);
	long rSize = rParas.width*rParas.height;
	long* rData = (long*) malloc(sizeof(long)*rSize);
	memset(rData, 0, sizeof(long)*rSize);
	readTIFFPixelsData(&rParas, rData, "csclp5.tif");//"../data_acquisitor/127.0.0.1-49654-4.tif");//
	long printLimit = rSize > 1000 ? 1000 : rSize;
	for (i = 0; i < printLimit; i++) {
		printf("%ld ", rData[i]);
	}
	printf("\n");
	


	// Calculate Center Point.
	CXPixelPoint centerPoint;
	calculateCenterPoint(rParas.width, rParas.height, rData, &centerPoint);
	printf("Center:(%d, %d)=%ld\n", centerPoint.x, centerPoint.y, centerPoint.value);

	map<long, CXIQData> iq2map;
	calculateIQData(rParas.width, rParas.height, rData, &centerPoint, iq2map);

	gettimeofday(&t2, NULL);
	timeSpan = (double) (t2.tv_sec-t1.tv_sec) + (double) t2.tv_usec*1e-6 - (double) t1.tv_usec*1e-6;
	printf("Time Span: %lf s\n", timeSpan);

	// Count invalid pixel.
	int invalidPixelCount = countInvalidPixels(rParas.width, rParas.height, rData);
	printf("Invalid Pixels Count: %d\n", invalidPixelCount);

	// Print test message.
	CXIQData data = iq2map[0];
	printf("Data[0]:(%ld, %.2f, %.2f, %d, %d, %d)\n", data.q2, data.averageI, data.totalI, data.normalPixelCount, data.badPixelCount, data.gapPixelCount);

	printf("Map Size: %lu\n", iq2map.size());
	for (map<long, CXIQData>::iterator itr = iq2map.begin(); itr != iq2map.end(); itr++) {
		CXIQData d = itr->second;
		if (d.averageI > 20000) {
			printf("Data:(%ld, %.2f, %.2f, %d, %d, %d)\n", d.q2, d.averageI, d.totalI, d.normalPixelCount, d.badPixelCount, d.gapPixelCount);
		}
	}

	free(rData);
	rData = NULL;


	// Send data.
	sendIQDataToUIClient();


	return 0;
}

void printUsage() {
	printf("Usage:\n");
	printf("    exe.o -s serverIP -p serverPort\n");
}

void sendIQDataToUIClient() {
	int sock; // Socket descriptor.
	struct sockaddr_in servAddr; // Server address.

	unsigned short servPort = DataProcessParas.serverPort;
	const char* servIP = DataProcessParas.serverIP;

	unsigned int dataSize = 100 * sizeof(int);
	int* data = (int*) malloc(sizeof(int)*dataSize);
	memset(data, 0, sizeof(int)*dataSize);
	int i = 0;
	for (i = 0; i < dataSize; i++) {
		data[i] = htonl(i);
	}


	// Create a reliable, stream socket using TCP.
	if ((sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
		perror("sendIQDataToUIClient socket() failed.");
		exit(1);
	}

	// Construct the server address structure.
	memset(&servAddr, 0, sizeof(servAddr));
	servAddr.sin_family = AF_INET; // Internet address family.
	servAddr.sin_addr.s_addr = inet_addr(servIP); // Server IP address.
	servAddr.sin_port = htons(servPort); // Server port.

	// Establish the connection to the server.
	if (connect(sock, (struct sockaddr*) &servAddr, sizeof(servAddr)) < 0) {
		perror("sendIQDataToUIClient connect() failed.");
		exit(1);
	}

	// [
	// Get client IP:Port and server IP:Port.
	struct sockaddr_in c, s;
	//char cIP[20];
	//char sIP[20];
	socklen_t cLen = sizeof(c);
	socklen_t sLen = sizeof(s);
	getsockname(sock, (struct sockaddr*) &c, &cLen);
	getpeername(sock, (struct sockaddr*) &s, &sLen);
	//inet_ntop(AF_INET, &c.sin_addr, cIP, sizeof(cIP));
	//inet_ntop(AF_INET, &s.sin_addr, sIP, sizeof(sIP));
	//printf("Client: %s:%d\nServer: %s:%d\n", cIP, ntohs(c.sin_port), sIP, ntohs(s.sin_port));
	printf("Client: %s:%d\nServer: %s:%d\n", inet_ntoa(c.sin_addr), ntohs(c.sin_port), inet_ntoa(s.sin_addr), ntohs(s.sin_port));
	// ]

	//while (1) {
		if (send(sock, (char*) data, dataSize, 0) != dataSize) {
			perror("sendIQDataToUIClient send() send a different number of bytes than expected");
			exit(1);
		}

	//}

	close(sock);

	free(data);
	data = NULL;

	// exit(0);

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
