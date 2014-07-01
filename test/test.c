#include <stdlib.h>
#include <stdio.h>
#include <string.h>


int main() {
	printf("Hello!\n");

	const char* fileName = "out.txt";
    FILE* fp = fopen(fileName, "wb");

    int i = 0x4949;
    char* str = "test";
    size_t len = strlen(str);

    fwrite(&i, sizeof(int), 1, fp);
    //fwrite(str, sizeof(char), len, fp);
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

	return 0;
}
