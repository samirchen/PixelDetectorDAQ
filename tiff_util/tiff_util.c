#include <stdio.h>
#include <stdlib.h>
#include "../common/dieWithError.h"
#include "../common/cpuUsage.h"
#include <string.h>

/* ######################## Method Declare ######################## */
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



int main() {
	printf("Hello\n");

	const char* fileName = "out.txt";
	FILE* fp = fopen(fileName, "wb");

	int i = 10;
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


	return 0;
}
