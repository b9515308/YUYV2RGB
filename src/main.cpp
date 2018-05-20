#include<stdio.h>
#include <stdlib.h>
#include <math.h>

/* strchr example */
#include <stdio.h>
#include <string.h>

int main(int argc, char *argv[])
{
	char *file = argv[1];
	char *res = argv[2];
	FILE *fp;
	long f_size;
	void *yuv422_data;
	int width;
	int height;


	width = atoi(strtok(res, "x"));
	height = atoi(strtok(NULL, "x"));

	printf("%s,%d x %d \n", file, width,height);

	fp = fopen(file, "rb");
	if (fp==NULL) {
		fputs ("File error",stderr);
		exit (1);
	 }
	 fseek(fp, 0, SEEK_END);
	 f_size = ftell(fp);
	 yuv422_data = malloc(sizeof(char) * f_size);
	 fread(yuv422_data, 1, f_size, fp);

	 return 0;

}
