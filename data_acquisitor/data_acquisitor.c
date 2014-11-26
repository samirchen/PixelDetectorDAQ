#include <stdlib.h> 
#include <stdio.h> // for printf() and fprintf().
#include <sys/socket.h> // for sockaddr_in and inet_nota().
#include <sys/time.h>
#include <arpa/inet.h>
#include <string.h> // for memset().
#include <pthread.h> // for pthread_create(). And use -pthread on Ubuntu, -lpthread on SLC.
#include <sys/syscall.h>
#include <sys/types.h>
#include "../common/cpuUsage.h"
#include "../tiff_util/tiff_util.h"


/* ######################## Local Global Data Structure ######################## */
// ================== Const =================
#define MAXPENDING 5
#define DATASIZE (400)
//#define RCVBUFSIZE (400*400)
//#define SERVPORT 5555

// ================== Paramerters when using ==================
struct DAQPARAS {
	// Server.
	unsigned short serverPort;

} DAQParas;


// ================== Global ==================
int Data_Buffer[DATASIZE];



// [ ConnectionSockPool
// Connection socket pool, main thread accept client connection socket and put it in pool, receive thread of server or receive-send thread of L2 client get client socket from pool.
// Deprecated.
typedef struct connectionSockPool {
    int pool[MAXPENDING];
    int poolTop;
    pthread_mutex_t poolLock;
} ConnectionSockPool;

ConnectionSockPool* connectionSockPoolAlloc() {
    ConnectionSockPool* cspool;
    if ((cspool = (ConnectionSockPool*) malloc(sizeof(ConnectionSockPool))) != NULL) {
        cspool->poolTop = 0;
        if (pthread_mutex_init(&cspool->poolLock, NULL) != 0) {
            free(cspool);
            return NULL;
        }
    }

    return cspool;
}

void connectionSockPoolRelease(ConnectionSockPool* cspool) {
    pthread_mutex_lock(&cspool->poolLock);
    if (cspool->poolTop == 0) {
        pthread_mutex_unlock(&cspool->poolLock);
        pthread_mutex_destroy(&cspool->poolLock);
        free(cspool);
    }
    else {
        pthread_mutex_unlock(&cspool->poolLock);
    }
}

// ]

// [ Connection 
// Connection is a structure contains: socket fd, server address, client address.
// Receive thread accept one Connection then deal with it.
typedef struct connection {
	int socketfd;
	struct sockaddr_in serverAddress;
	struct sockaddr_in clientAddress;
} Connection;
// ]



/* ######################## Method Declare ######################## */
// ================= Out of this file. ================
/*
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
void printUsage();

// Could accept multiple connections, but only able to deal with them one by one. 
void recvMultiConnsDataOneByOne();

// Get thread id.
pid_t gettid();
unsigned long long int threadRTag = 0; // Receive thread tag.
pthread_mutex_t threadRTagLock; // Receive thread tag lock.
// Thread to receive data.
void* threadReceive(void* arg); // accept a ConnecitonSockPool* . Deprecated.
void* threadReceiveConnection(void* arg); // accept a Connection* .

// Could accept multiple connections, and deal with them in different threads.
void recvMultiConnsDataWithMultiThreads();




/* ######################## Method Implement ######################## */
int main(int argc, char* argv[]) {

	// Default parameter values.
	DAQParas.serverPort = 5555;
	// Get parameters from inputing.
	int i = 1;
	for (i = 1; i < argc; i++) {
		if (strcmp(argv[i], "-p") == 0) {
			i++;
			DAQParas.serverPort = atoi(argv[i]);
		}
		else if (strcmp(argv[i], "--help") == 0) {
			printUsage();
			return 0;
		}
		else {

		}
	}

	//recvMultiConnsDataOneByOne();	
	recvMultiConnsDataWithMultiThreads();

	return 0;
}

void printUsage() {
	printf("Usage:\n");
    printf("    exe.o -p serverPort");
}

// Get thread id.
pid_t gettid() {
    return syscall(SYS_gettid); // Return the <tid> in "/proc/<pid>/task/<tid>"
}
// Thread to receive data.
void* threadReceive(void* arg) {
	printf("threadReceive\n");
    ConnectionSockPool* cspool = (ConnectionSockPool*) arg;
    int connectionSock = -1;
    while (1) {
        connectionSock = cspool->pool[cspool->poolTop-1];
        if (connectionSock != -1) {
            pthread_mutex_lock(&cspool->poolLock);
            cspool->poolTop--;
            pthread_mutex_unlock(&cspool->poolLock);
            break;
        }
    }

    // Get process id and thread id.
    pid_t pid = getpid();
    //pthread_t tid = pthread_self(); // Get tid, different from "syscall(SYS_gettid)".
    pid_t tid = gettid();
    printf("thread connectionSock: %d, pid: %u, tid: %u\n\n", connectionSock, (unsigned int) pid, (unsigned int) tid);

    int buffer[DATASIZE];
    bzero(buffer, DATASIZE*sizeof(int));

    int recvMsgSize;
    unsigned long long int totalRecvMsgSize = 0;

	// CPU calculating.
    ProcStat ps1, ps2;
    ProcPidStat pps1, pps2;
    getWholeCPUStatus(&ps1);
    getThreadCPUStatus(&pps1, pid, tid); // accurate?

    // Time calculating.
    struct timeval t1, t2;
    gettimeofday(&t1, NULL);
    double timeSpan = 0.0;

    while (1) {
        if ((recvMsgSize = recv(connectionSock, buffer, DATASIZE*sizeof(int), 0)) < 0) {
            perror("threadReceive recv() failed");
			exit(1);
        }
        else if (recvMsgSize > 0) {
            totalRecvMsgSize += recvMsgSize;
            //printf("thread %u recvMsgSize: %d\n", (unsigned int) tid, recvMsgSize);
            //printf("thread %u totalRecvMsgSize: %lld\n", (unsigned int) tid, totalRecvMsgSize);
			/*int i = 0;
            for (i = 0; i < DATASIZE; i++) {
                printf("%d ", ntohs(buffer[i]));
            }
            printf("\n");*/
        }
        else {
            break;
        }
    }
   
    getWholeCPUStatus(&ps2);
    getThreadCPUStatus(&pps2, pid, tid);
    float CPUUse = calWholeCPUUse(&ps1, &ps2);
    float threadCPUUse = calProcessCPUUse(&ps1, &pps1, &ps2, &pps2);

    gettimeofday(&t2, NULL);
    timeSpan = (double) (t2.tv_sec-t1.tv_sec) + (double) t2.tv_usec*1e-6 - (double) t1.tv_usec*1e-6;
    double recvSpeed = ((double) totalRecvMsgSize * 8) / (timeSpan * 1000 *1000);

	pthread_mutex_lock(&threadRTagLock);
    threadRTag++;
    printf("Tag: %llu\n", threadRTag);
    pthread_mutex_unlock(&threadRTagLock);
    printf("thread %d-%d CPUUse: %f, threadCPUUse: %f\n", pid, tid, CPUUse, threadCPUUse);
    printf("thread %d-%d totalRecvMsgSize: %llu Bytes\n", pid, tid, totalRecvMsgSize);
    printf("thread %d-%d time span: %lf\n", pid, tid, timeSpan);
    printf("thread %d-%d receive speed: %lf Mb/s\n\n", pid, tid, recvSpeed);

    close(connectionSock);
    pthread_exit((void*) 0);

	return ((void*) 0);
}

// Thread to deal with one Connection.
void* threadReceiveConnection(void* arg) {
	printf("threadReceiveConnection\n");

	Connection* conn = (Connection*) arg;
	int connectionSock = conn->socketfd;
	struct sockaddr_in serverAddress = conn->serverAddress;
	struct sockaddr_in clientAddress = conn->clientAddress;

    int Tiff_Width = 981;
    int Tiff_Height = 1043;
    int size = Tiff_Width*Tiff_Height;
    long* data = (long*) malloc(sizeof(long)*size);
    memset(data, 0, sizeof(long)*size);
    int dataIndex = 0;


    int buffer[DATASIZE];
    //bzero(buffer, DATASIZE*sizeof(int));
    memset(buffer, 0, DATASIZE*sizeof(int));

    int recvMsgSize;
    unsigned long long int totalRecvMsgSize = 0;

	// Get process id and thread id.
    pid_t pid = getpid();
    //pthread_t tid = pthread_self(); // Get tid, different from "syscall(SYS_gettid)".
    pid_t tid = gettid();
    printf("thread connectionSock: %d, pid: %u, tid: %u\n\n", connectionSock, (unsigned int) pid, (unsigned int) tid);

/*
    // CPU calculating.
    ProcStat ps1, ps2;
    ProcPidStat pps1, pps2;
    getWholeCPUStatus(&ps1);
    getThreadCPUStatus(&pps1, pid, tid); // accurate?
*/

    // Time calculating.
    struct timeval t1, t2;
    gettimeofday(&t1, NULL);
    double timeSpan = 0.0;


    FILE* fp;
    char fileName[100];
    sprintf(fileName, "%s-%d-%d.data", inet_ntoa(clientAddress.sin_addr), ntohs(clientAddress.sin_port), connectionSock);
    printf("File Name: %s\n", fileName);
    fp = fopen(fileName, "a+");
    if (fp == NULL) {
        perror("threadReceiveConnection fopen() failed");   
        exit(1);
    }
    
    while (1) {
        if ((recvMsgSize = recv(connectionSock, buffer, DATASIZE*sizeof(int), 0)) < 0) {
            fclose(fp);
            perror("threadReceiveConnection recv() failed");
			exit(1);
        }
        else if (recvMsgSize > 0) {
            totalRecvMsgSize += recvMsgSize;
            //printf("thread %u recvMsgSize: %d\n", (unsigned int) tid, recvMsgSize);
            //printf("thread %u totalRecvMsgSize: %lld\n", (unsigned int) tid, totalRecvMsgSize);
            int i = 0;
            //for (i = 0; i < recvMsgSize/sizeof(int); i++) {
            //    printf("%d ", ntohs(buffer[i]));
            //}
            //printf("\n");

            //printf("Receive: %d\n", recvMsgSize);
            
            
            // Write data to file(named by client IP-Port).
            for (i = 0; i < recvMsgSize/sizeof(int); i++,dataIndex++) {
                fprintf(fp, "%d ", ntohs(buffer[i]));
                data[dataIndex] = ntohs((long) buffer[i]);
            }
			fprintf(fp, "\n");
            

			/*
			// Test write file.
			char fileName[20];
			sprintf(fileName, "%s-%d-%d.data", inet_ntoa(clientAddress.sin_addr), ntohs(clientAddress.sin_port), connectionSock);
			FILE* fp;
			fp = fopen(fileName, "w");
			if (fp == NULL) {
				perror("threadReceiveConnection fopen() failed");
				exit(1);
			}
			fprintf(fp, "Socket:%d\n%d %d %s", connectionSock, 1, 2, "text");
			fclose(fp);
			*/

        }
        else {
            printf("Received message size is 0, over!\n");

            
            // Write the data to tiff.
            //// Tiff paras.
            TiffParas* paras = (TiffParas*) malloc(sizeof(TiffParas)); // Use pointer.
            memset(paras, 0, sizeof(TiffParas)); // Must not forget to bezero paras.
            paras->isLittleEndian = 1;
            paras->version = 0x2A;
            paras->firstIFDOffset = 0x82;
            paras->width = Tiff_Width;
            paras->height = Tiff_Height;
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
            
            //// Print tiff pixel data.
            int i = 0;
            printf("Data Head:\n");
            for (i = 0; i < 1000; i++) {
                printf("%ld ", data[i]);
            }
            printf("\n");
            printf("Data Tail:\n");
            for (i = size-1000; i < size; i++) {
                printf("%ld ", data[i]);
            }
            printf("\n");
            
            //// Prepare and write.
            char tiffFileName[100];
            sprintf(tiffFileName, "%s-%d-%d.tif", inet_ntoa(clientAddress.sin_addr), ntohs(clientAddress.sin_port), connectionSock);
            prepareAndWrite(paras, data, tiffFileName);
            //// Clean.
            free(paras);
            paras = NULL;

            break;
        }
    }
    fclose(fp);

    free(data);
    data = NULL;

/*
	// Calculate CPU and time consume.
    getWholeCPUStatus(&ps2);
    getThreadCPUStatus(&pps2, pid, tid);
    float CPUUse = calWholeCPUUse(&ps1, &ps2);
    float threadCPUUse = calProcessCPUUse(&ps1, &pps1, &ps2, &pps2);
    printf("thread %d-%d CPUUse: %f, threadCPUUse: %f\n", pid, tid, CPUUse, threadCPUUse);
*/


    gettimeofday(&t2, NULL);
    timeSpan = (double) (t2.tv_sec-t1.tv_sec) + (double) t2.tv_usec*1e-6 - (double) t1.tv_usec*1e-6;
    double recvSpeed = ((double) totalRecvMsgSize * 8) / (timeSpan * 1000 *1000);

    pthread_mutex_lock(&threadRTagLock);
    threadRTag++;
    printf("Tag: %llu\n", threadRTag);
    pthread_mutex_unlock(&threadRTagLock);
    printf("thread %d-%d time span: %lf\n", pid, tid, timeSpan);
    printf("thread %d-%d totalRecvMsgSize: %llu Bytes\n", pid, tid, totalRecvMsgSize);
    printf("thread %d-%d receive speed: %lf Mb/s\n\n", pid, tid, recvSpeed);


    close(connectionSock);
	free(conn);
    pthread_exit((void*) 0);

    return ((void*) 0);

}

// Could accept multiple connections, and deal with them in different threads.
void recvMultiConnsDataWithMultiThreads() {
	printf("recvMultiConnsDataWithMultiThreads\n");
    unsigned short servPort = DAQParas.serverPort;//SERVPORT;

	int listenSock, connectionSock; // Listen on listenSock, new connection on connectionSock.
    struct sockaddr_in servAddr; // Server address info.
    struct sockaddr_in clntAddr; // connector's address info.
    socklen_t clntLen;
    int on = 1;

    pthread_mutex_init(&threadRTagLock, NULL);

    // socket: get listen socket.
    if ((listenSock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("recvMultiConnsDataWithMultiThreads socket() failed");
		exit(1);
    }

	// setsockopt: set options of listen socket.
    if (setsockopt(listenSock, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(int)) < 0) {
        perror("recvMultiConnsDataWithMultiThreads setsockopt() failed");
		exit(1);
    }

    // Construct local address structure.
	memset(&servAddr, 0, sizeof(servAddr));
    servAddr.sin_family = AF_INET; // Host byte order.
    servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servAddr.sin_port = htons(servPort);

	// bind: bind listen socket to server address.
    if (bind(listenSock, (struct sockaddr*) &servAddr, sizeof(servAddr)) < 0) {
        perror("recvMultiConnsDataWithMultiThreads bind() failed");
		exit(1);
    }

	// listen: listen on listen socket.
    if (listen(listenSock, MAXPENDING) < 0) {
        perror("recvMultiConnsDataWithMultiThreads listen() failed");
		exit(1);
    }
	printf("Server <IP:Port>: <%s:%d>\n\n", inet_ntoa(servAddr.sin_addr), ntohs(servAddr.sin_port));

    fd_set fds; // file descriptor set: Hold sockets. socket is also regarded as a kind of file descriptor, we could read(recv) and write(send) data on it.
    int maxsock = listenSock; // Init, set listen socket as max.
    struct timeval timeout;

    clntLen = sizeof(clntAddr);


	while (1) {
        FD_ZERO(&fds);
        FD_SET(listenSock, &fds); // Set listen socket fd to fd set.
		// We can also set file related fd to fd set here.
        timeout.tv_sec = 30;
        timeout.tv_usec = 0;
        int readyFdCount = 0;
		// select: returns the ready fd count when it works. Returns 0 if timeout, -1 if error.
        if ((readyFdCount = select(maxsock+1, &fds, NULL, NULL, &timeout)) < 0) {
            perror("recvMultiConnsDataWithMultiThreads select() failed");
			exit(1);
        }
        else if (readyFdCount == 0) {
            printf("timeout\n");
            break;
        }
		// readyFdCount > 0, that means listen socket has new comming connection, continue...
		if (FD_ISSET(listenSock, &fds)) {
			// accept: accept and construct the new connection
			if ((connectionSock = accept(listenSock, (struct sockaddr*) &clntAddr, &clntLen)) < 0) {
				perror("recvMultiConnsDataWithMultiThreads accept() failed");
				exit(1);
			}
			printf("New connection on socket: %d\n", connectionSock);
			printf("New connection from <IP:Port>: <%s:%d>\n", inet_ntoa(clntAddr.sin_addr), ntohs(clntAddr.sin_port));

			// [
			// Get client IP:Port and server IP:Port.
			struct sockaddr_in c, s;
			//char cIP[20];
			//char sIP[20];
			socklen_t cLen = sizeof(c);
			socklen_t sLen = sizeof(s);
			getsockname(connectionSock, (struct sockaddr*) &s, &sLen);
			getpeername(connectionSock, (struct sockaddr*) &c, &cLen);
			//inet_ntop(AF_INET, &c.sin_addr, cIP, sizeof(cIP));
			//inet_ntop(AF_INET, &s.sin_addr, sIP, sizeof(sIP));
			//printf("Client: %s:%d\nServer: %s:%d\n", cIP, ntohs(c.sin_port), sIP, ntohs(s.sin_port));
			printf("Client: %s:%d\nServer: %s:%d\n", inet_ntoa(c.sin_addr), ntohs(c.sin_port), inet_ntoa(s.sin_addr), ntohs(s.sin_port));
			// ]

			// Dispatch a thread to deal with this conneciton.
			Connection* conn = (Connection*) malloc(sizeof(Connection));
			conn->socketfd = connectionSock;
			conn->serverAddress = s;
			conn->clientAddress = c;
			pthread_t ntid;
			if (pthread_create(&ntid, NULL, threadReceiveConnection, conn) < 0) { // threadReceiveConnection: thread method; conn: argument for thread method.
				perror("recvMultiConnsDataWithMultiThreads pthread_create() failed");
				exit(1);
			}
			//pthread_join(ntid, NULL);

		}
    }
   
    close(listenSock);

    exit(0);
}

// Could accept multiple connections, but only able to deal with them one by one. 
void recvMultiConnsDataOneByOne() {
	printf("data_acquisitor recvMultiConnsDataOneByOne() is called.");

	int listenSock;
	int connectionSock;
	struct sockaddr_in servAddr; // Local address.
	struct sockaddr_in clntAddr; // Client address.
	unsigned int clntLen; // Length of client address data structure.

	unsigned short servPort = DAQParas.serverPort;//SERVPORT;
	printf("server port: %d\n", servPort);

	// Create socket for incoming connections.
	if ((listenSock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
		perror("server socket() failed.");		
		exit(1);
	}

	// Construct local address structure.
	memset(&servAddr, 0, sizeof(servAddr)); // Zero out structure.
	servAddr.sin_family = AF_INET; // Internet address family.
	servAddr.sin_addr.s_addr = htonl(INADDR_ANY); // Any incoming interface.
	servAddr.sin_port = htons(servPort); // Local port. Network byte order.

	// Bind to the local address.
	if (bind(listenSock, (struct sockaddr*) &servAddr, sizeof(servAddr)) < 0) {
		perror("server bind() failed.");
		exit(1);
	}

	// Mark the socket so it will listen for incoming connections.
	if (listen(listenSock, MAXPENDING) < 0) {
		perror("server listen() failed.");
		exit(1);
	}

	fd_set fds;
	int maxsock = listenSock;
	struct timeval timeout;
	clntLen = sizeof(clntAddr);

	while (1) {
		FD_ZERO(&fds);
		FD_SET(listenSock, &fds);
		timeout.tv_sec = 30;
		timeout.tv_usec = 0;
		int readyFdCount = 0;
		if ((readyFdCount = select(maxsock+1, &fds, NULL, NULL, &timeout)) < 0) {
			perror("server select() failed.");
			exit(1);
		}
		else if (readyFdCount == 0) {
			printf("timeout\n");
			break;
		}

		// Process will go on until server accepts a new connection.
		if ((connectionSock = accept(listenSock, (struct sockaddr*) &clntAddr, &clntLen)) < 0) {
			perror("server accept failed.");
			exit(1);
		}
		printf("New connection on socket %d\n", connectionSock);
		printf("New connection from <IP:Port>: <%s:%d>\n", inet_ntoa(clntAddr.sin_addr), ntohs(clntAddr.sin_port));

		if (connectionSock > maxsock) {
			maxsock = connectionSock;
		}
		FD_SET(connectionSock, &fds);


		// When coming a connection, recieve the message and calculate the CPU and speed.
		//char buffer[RCVBUFSIZE];
		//bzero(buffer, RCVBUFSIZE);


		unsigned long long int totalRecvMsgSize = 0;
		int recvMsgSize;

		// CPU calculating.
		ProcStat ps1, ps2;
		ProcPidStat pps1, pps2;
		pid_t pid = getpid();
		getWholeCPUStatus(&ps1);
		getProcessCPUStatus(&pps1, pid);

		// Time calculating.
		struct timeval t1, t2;
		gettimeofday(&t1, NULL);
		double timeSpan = 0.0;

		while (1) {
			//if ((recvMsgSize = recv(connectionSock, buffer, RCVBUFSIZE, 0)) < 0) {
			if ((recvMsgSize = recv(connectionSock, Data_Buffer, DATASIZE*sizeof(int), 0)) < 0) {
				perror("server recv() failed.");
				exit(1);
			}
			else if (recvMsgSize > 0) {
				totalRecvMsgSize += recvMsgSize;
				//printf("recvMsgSize: %d\n", recvMsgSize);
                //printf("totalRecvMsgSize: %lld\n", totalRecvMsgSize);
                //printf("%s\n", buffer);
				/*int i = 0;
				for (i = 0; i < DATASIZE; i++) {
					printf("%d ", ntohs(Data_Buffer[i]));
				}
				printf("\n");*/
			}
			else { // recvMsgSize == 0
				gettimeofday(&t2, NULL);
				timeSpan = (double) (t2.tv_sec-t1.tv_sec) + (double) t2.tv_usec*1e-6 - (double) t1.tv_usec*1e-6;

				getWholeCPUStatus(&ps2);
				getProcessCPUStatus(&pps2, pid);
				float CPUUse = calWholeCPUUse(&ps1, &ps2);
				float processCPUUse = calProcessCPUUse(&ps1, &pps1, &ps2, &pps2);
				printf("CPUUse: %f, processCPUUse: %f\n", CPUUse, processCPUUse);

				double recvSpeed = ((double) totalRecvMsgSize * 8) / (timeSpan * 1000 *1000);
				printf("totalRecvMsgSize: %llu Bytes\n", totalRecvMsgSize);
				printf("time span: %lf s\n", timeSpan);
				printf("receive speed: %lf Mb/s\n\n", recvSpeed);

				FD_CLR(connectionSock, &fds);
				close(connectionSock);

				break;
			}
		}
	}

	close(listenSock);

	exit(0);

}


