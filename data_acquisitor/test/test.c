#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <string.h>
#include <pthread.h>
#include <sys/syscall.h>


void* threadCall(void* arg) {
	int* tag = (int*)arg;	
	printf("Arg: %d\n", *tag);

	char fileName[10];
	sprintf(fileName, "%d.data", *tag);
	FILE* fp;
    fp = fopen(fileName, "w");
	if (fp == NULL) {
		perror("fopen failed in threadCall.");
		exit(1);
	}
    fprintf(fp, "Tag:%d\n%d %d %s", *tag, 1, 2, "text");
    fclose(fp);	

	free(tag);
	pthread_exit((void*) 0);
	return ((void*) 0);
}


int main () {

	int i = 0;


	// Test 1.
	struct sockaddr_in servAddr;
	memset(&servAddr, 0, sizeof(servAddr));
    servAddr.sin_family = AF_INET; // Internet address family.
    servAddr.sin_addr.s_addr = inet_addr("192.168.37.222"); // Server IP address.
    servAddr.sin_port = htons(1234); // Server port.

	char fileName[100];
	sprintf(fileName, "%s-%d", inet_ntoa(servAddr.sin_addr), ntohs(servAddr.sin_port));
	printf("%s\n", fileName);

	FILE* fp;
	fp = fopen(fileName, "a+");
	for (i = 0; i < 3; i++) {		
		fprintf(fp, "%d %d %s\n", 1, 2, "text");	
	}
	fclose(fp);

	/*
	// Test thread to deal with file.
	for (i = 0; i < 9; i++) {
		int* tag = (int*) malloc(sizeof(int));
		*tag = i;
		pthread_t tid;
		if (pthread_create(&tid, NULL, threadCall, tag) < 0) {
			perror("pthread_create failed in main.");
			exit(1);
		}

	}

	for (;;) {

	}
	*/

	return 0;
}
