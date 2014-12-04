#include <iostream>
#include <stdio.h>
#include <limits>
#include <sys/socket.h> // for socket(), bind(), and connect().
#include <arpa/inet.h> // for sockaddr_in and inet_nota().
#include <stdlib.h> // for atoi() and exit().
#include <string.h> // for memset().
#include <unistd.h> // for close().
#include <sys/time.h> // for timeval.
#include <pthread.h> // for pthread_create().
#include <sys/syscall.h> // for SYS_gettid.
#include <map>
#include "../common/cpuUsage.h"
#include "../tiff_util/image_util.h"

extern "C" {
	#include "../tiff_util/tiff_util.h"
}

using namespace std;

/* ######################## Local Global Data Structure ######################## */
// ================= Const =================
#define MAX_VALUE 2097151 // 2^21-1 

//#define SERVPORT 5555
//#define SERVIP "192.168.37.229"
//#define PKGSIZE 32
//#define INTERVAL 10


// ================== Parameters when using ==================
struct GENPARAS {
	// Server.
	char* serverIP;
	unsigned short serverPort;

} GenParas;

// ================== Global ==================
int* Pixel_Matrix;
int Pixel_Count;


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

// ================= In this file. ====================
// Method Declare.
void printUsage();
void genData();
void sendData();

int main(int argc, char* argv[]) {

	// Default parameter values.
	GenParas.serverIP = (char*) "127.0.0.1";
	GenParas.serverPort = 5555;
	// Get paramerters from inputing.
	int i = 1;
	for (i = 1; i < argc; i++) {
		if (strcmp(argv[i], "-s") == 0) {
			i++;
			GenParas.serverIP = argv[i];
		}
		else if (strcmp(argv[i], "-p") == 0) {
			i++;
			GenParas.serverPort = atoi(argv[i]);
		}
		else if (strcmp(argv[i], "--help") == 0) {
			printUsage();
			return 0;
		}
		else {

		}
	}


	//int i = numeric_limits<int>::max();
	//printf("max int: %d\n", i);

	genData();

	sendData();

	free(Pixel_Matrix);
	Pixel_Matrix = NULL;
	Pixel_Count = 0;
	
	return 0;
}

void printUsage() {
	printf("Usage:\n");
	printf("    exe.o -s serverIP -p serverPort");
}

void genData() {

	int i = 0;

	// Get data from tiff file.
	TiffParas rParas; // Not use pointer.
	memset(&rParas, 0, sizeof(TiffParas)); // Must not forget to bezero paras.
	readTIFFParas(&rParas, "../tiff_util/csclp5.tif");
	printf("Image Width: %ld\n", rParas.width);
	printf("Image Height: %ld\n", rParas.height);
	printf("Bits Per Sample: %d\n", rParas.bitsPerSample);
	printf("Strip Offset: %ld\n", rParas.stripOffset);
	long rSize = rParas.width*rParas.height;
	long* rData = (long*) malloc(sizeof(long)*rSize);
	memset(rData, 0, sizeof(long)*rSize);
	readTIFFPixelsData(&rParas, rData, "../tiff_util/csclp5.tif");
	long printLimit = rSize > 1000 ? 1000 : rSize;
	printf("Head:\n");
	for (i = 0; i < printLimit; i++) {
		printf("%d ", ((int) rData[i]));
	}
	printf("\n");

	printf("Tail:\n");
	for (i = rSize-printLimit; i < rSize; i++) {
		printf("%ld ", rData[i]);
	}
	printf("\n");


	//srand((unsigned) time(NULL));
	Pixel_Count = rSize;
	Pixel_Matrix = (int*) malloc(sizeof(int)*Pixel_Count);
	memset(Pixel_Matrix, 0, sizeof(int)*Pixel_Count);
	for (i = 0; i < Pixel_Count; i++) {
		Pixel_Matrix[i] = htonl(rData[i]);//rand() % MAX_VALUE;
	}

	free(rData);
	rData = NULL;
}

void sendData() {
	int sock; // Socket descriptor.
	struct sockaddr_in servAddr; // Server address.
	//char* package;

	unsigned short servPort = GenParas.serverPort;//SERVPORT;
	const char* servIP = GenParas.serverIP;//SERVIP;
	//unsigned int pkgSize = PKGSIZE;;
	//unsigned int interval = INTERVAL;

	unsigned int pixelDataSize = Pixel_Count * sizeof(int);
	

	// Create a reliable, stream socket using TCP.
	if ((sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
		perror("sendData socket() failed.");
		exit(1);
	}

	// Construct the server address structure.
	memset(&servAddr, 0, sizeof(servAddr));
	servAddr.sin_family = AF_INET; // Internet address family.
	servAddr.sin_addr.s_addr = inet_addr(servIP); // Server IP address.
	servAddr.sin_port = htons(servPort); // Server port.

	// Establish the connection to the server.
	if (connect(sock, (struct sockaddr*) &servAddr, sizeof(servAddr)) < 0) {
		perror("sendData connect() failed.");
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

/*
	// Init package data.
	package = (char*) malloc(pkgSize * sizeof(char));
	int i = 0;
	package[0] = 's';
    for (i = 1; i <= pkgSize-3; i++) {
        package[i] = 'd';
    }
    package[pkgSize-2] = 'e';
    //package[pkgSize-1] = '\0';
    package[pkgSize-1] = 'e';
    printf("package size: %d\n", pkgSize);
*/

	// Keep Sending.
	//if (send(sock, package, pkgSize, 0) != pkgSize) {
	//while (1) {
		if (send(sock, (char*) Pixel_Matrix, pixelDataSize, 0) != pixelDataSize) {
			perror("sendData send() send a different number of bytes than expected");
			exit(1);
		}
	//}

/*	// Cycled send and time, cpu measure.
	// CPU calculating.
    ProcStat ps1, ps2;
    ProcPidStat pps1, pps2;
    pid_t pid = getpid();
    getWholeCPUStatus(&ps1);
    getProcessCPUStatus(&pps1, pid);

    // Time calculating.
    struct timeval t1, t2; // "struct" required when use gcc.
    gettimeofday(&t1, NULL);
    double timeSpan = 0.0;
    unsigned int sendTimes = 0;

	while (1) {
		if (send(sock, package, pkgSize, 0) != pkgSize) {
            perror("L1 client send() send a different number of bytes than expected");
			exit(1);
        }
        sendTimes++;

        gettimeofday(&t2, NULL);
        timeSpan = (double) (t2.tv_sec-t1.tv_sec) + (double) t2.tv_usec*1e-6 - (double) t1.tv_usec*1e-6;
        if (timeSpan >= interval) {
            break;
        }
	}

	getWholeCPUStatus(&ps2);
    getProcessCPUStatus(&pps2, pid);
    float CPUUse = calWholeCPUUse(&ps1, &ps2);
    float processCPUUse = calProcessCPUUse(&ps1, &pps1, &ps2, &pps2);
    printf("CPUUse: %f, processCPUUse: %f\n", CPUUse, processCPUUse);

    printf("send times: %d\n", sendTimes);
    unsigned long long int totalSendMsgSize = (unsigned long long int) sendTimes * pkgSize;
    printf("totalSendMsgSize: %lld Bytes\n", totalSendMsgSize);
    printf("time span: %lf s\n", timeSpan);
    double sendSpeed = ((double) sendTimes * pkgSize * 8) / (timeSpan * 1000 * 1000);
    printf("send speed: %lf Mb/s\n\n", sendSpeed);

*/

    close(sock);

    //free(package);

    exit(0);

}


