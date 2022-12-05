#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bmpHeader.h"

#define BYTE	unsigned char
#define BASE    16

#define widthbytes(bits)   (((bits)+31)/32*4)

int main(int argc, char** argv)
{
	FILE *fp;
	RGBQUAD palrgb[255];

	BITMAPFILEHEADER bmpHeader;
	BITMAPINFOHEADER bmpInfoHeader;

	unsigned int imagesize;

	unsigned int bits, width, height;

	char input[128], output[128];

	float r, g, b, gray1, gray2;
 
	int i, j, size, index, value;
	unsigned long histogram[256];
	unsigned char *inimg;
	unsigned char *outimg;
 
	strcpy(input, argv[1]);
	strcpy(output, argv[2]);
 
	if((fp = fopen(input, "rb")) == NULL) {
		fprintf(stderr, "Error : Failed to open file...\n");
		exit(EXIT_FAILURE);
	}

	fread(&bmpHeader, sizeof(BITMAPFILEHEADER), 1, fp);
	fread(&bmpInfoHeader, sizeof(BITMAPINFOHEADER), 1, fp);

	bits = bmpInfoHeader.biBitCount;
	width = bmpInfoHeader.biWidth;
	height = bmpInfoHeader.biHeight;

	size = widthbytes(bits * width);

	imagesize = bmpInfoHeader.SizeImage;
	if(!imagesize) imagesize = height * size;
	inimg = (BYTE*)malloc(sizeof(BYTE)*imagesize);
	outimg = (BYTE*)malloc(sizeof(BYTE)*imagesize);
	fread(inimg, sizeof(BYTE), imagesize, fp);
	fclose(fp);
 
    printf("Image width : %d, height : %d(%d)\n", width, height, width*height);
	
	for(i = 0; i < 256; i++) histogram[i] = 0;
 
	for(i = 0; i < height; i++) {
		index = (height-i-1) * size; 
		for(j = 0 ; j < width; j++) { 
			r = (float)inimg[index+3*j+2];
			g = (float)inimg[index+3*j+1];
			b = (float)inimg[index+3*j+0];
			gray1 = (r*0.3F)+(g*0.59F)+(b*0.11F);


			r = (float)inimg[index+3*j+2-3];
			g = (float)inimg[index+3*j+1-3];
			b = (float)inimg[index+3*j+0-3];
			gray2 = (r*0.3F)+(g*0.59F)+(b*0.11F);

			value = (int)(gray2 - gray1);
			outimg[index+3*j] = (value > BASE)?255:0;
			outimg[index+3*j+1] = (value > BASE)?255:0;
			outimg[index+3*j+2] = (value > BASE)?255:0;
		};
	};

	/*
    for(i = 0; i < 256; i++) {
		printf("%-3d  ", i);
		if(!(int)(histogram[i]/height) && (histogram[i]%height))
		   printf("*");
		else for(j = 0; j < (int)(histogram[i]/height); j++)
		   printf("*");
		printf("  %d\n", histogram[i]);
    };
    */
	
	bmpHeader.bf0ffBits += 256*sizeof(RGBQUAD); 

	if((fp = fopen(output, "wb")) == NULL) {
		fprintf(stderr, "Error : Failed to open file...\n");
		exit(EXIT_FAILURE);
	}
 
	
	fwrite(&bmpHeader, sizeof(BITMAPFILEHEADER), 1, fp);
	fwrite(&bmpInfoHeader, sizeof(BITMAPINFOHEADER), 1, fp);

	fwrite(palrgb, sizeof(unsigned int), 256, fp);
	fwrite(outimg, sizeof(unsigned char), imagesize, fp);

	free(inimg);
	free(outimg);

	fclose(fp);

        return 0;
}
