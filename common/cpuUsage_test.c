#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "cpuUsage.h"


int main(int argc, char* argv[]) {

	printf("%s\n", argv[0]);
	pid_t pid = getpid();
	printf("pid: %d\n", pid);

	// Get processor num.
	int processorNum = sysconf(_SC_NPROCESSORS_CONF); // "unistd.h" is required.
	printf("Processors: %d\n", processorNum);
	

	// Test
	ProcStat ps1, ps2;
	ProcPidStat pps1, pps2; 
	int i = 0;
	for (i = 0; i <= 100000; i++) {
		
		srand((unsigned) time(NULL));
		int m = rand() % 100000;
		int n = 1 + rand() % 100000;
		m = m / n;
		
		if (i == 10) {
			getWholeCPUStatus(&ps1);
			getProcessCPUStatus(&pps1, pid);
		}

		if (i == 10000) {
			getWholeCPUStatus(&ps2);	
			getProcessCPUStatus(&pps2, pid);
		}
	}
	float CPUUse = calWholeCPUUse(&ps1, &ps2);
	float processCPUUse = calProcessCPUUse(&ps1, &pps1, &ps2, &pps2);
	printf("CPUUse: %f, processCPUUse: %f\n", CPUUse, processCPUUse);


	return 0;
}
