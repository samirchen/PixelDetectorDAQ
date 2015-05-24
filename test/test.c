#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define MAXSIZE 100

int mypush(int i, int* stack, int* top);
int mypop(int* stack, int* top);
int mysize(int* stack, int* top);

int main() {
    /*
	printf("Hello!\n");

	const char* fileName = "out.txt";
    FILE* fp = fopen(fileName, "wb");

    int i = 0x4949;
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
    */


    int stack[MAXSIZE] = {0};
    int top = 0;

    int i = 0;
    for (i = 0; i < MAXSIZE+1; i++) {
        mypush(i, stack, &top);
        mysize(stack, &top);
    }

    for (i = 0; i < MAXSIZE+1; i++) {
        mypop(stack, &top);
        mysize(stack, &top);
    }



	return 0;
}




int mypush(int i, int* stack, int* top) {
    if ((*top) >= MAXSIZE) {
        printf("Stack is full.\n");
        return 0;
    }

    
    stack[*top] = i;
    (*top)++;

    printf("Push %d into stack.\n", i);
    return 1;
}

int mypop(int* stack, int* top) {
    if ((*top) <= 0) {
        printf("Stack is empty.\n");
        return -1;
    }

    (*top)--;
    printf("Pop %d out from stack.\n", stack[*top]);
    return stack[*top];
}

int mysize(int* stack, int* top) {
    printf("Stack size is %d.\n", (*top));
    return (*top);
}


