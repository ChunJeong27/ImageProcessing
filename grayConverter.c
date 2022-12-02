#include <stdio.h> 
#include <stdlib.h> 
#include <string.h>

#include "bmpHeader.h"

#define ubyte unsigned char

inline unsigned char clip(int value, int min, int max);

int main(int argc, char** argv) {
	FILE* fp; 
	RGBQUAD *palrgb;
	RGBQUAD grayPal[256];
	
	BITMAPFILEHEADER bmpHeader;
	BITMAPINFOHEADER bmpInfoHeader;

	unsigned int width, height, imagesize, sizeScale;
	unsigned short bits;

	char input[128], output[128];
	unsigned char *inimg, *outimg;
	
	/* usage a.out in.bmp out.bmp */
	strcpy(input, argv[1]); 
	strcpy(output, argv[2]);
	
	
	if((fp=fopen(input, "rb")) == NULL) { 
		fprintf(stderr, "Error : Failed to open file...₩n"); 
		return -1;
	}
	
	fread(&bmpHeader, sizeof(BITMAPFILEHEADER), 1, fp);
	fread(&bmpInfoHeader, sizeof(BITMAPINFOHEADER), 1, fp);

	bits = bmpInfoHeader.biBitCount;
	width = bmpInfoHeader.biWidth;
	height = bmpInfoHeader.biHeight;
	imagesize = width * height;

	printf("Bitmap File Size : %d\n", bmpHeader.bfSize);
	printf("Bitmap File Offset : %d\n", bmpHeader.bf0ffBits);
	printf("Bitmap Image Width : %d\n", width);
	printf("Bitmap Image Height : %d\n", height);
	printf("Bitmap Image Bits Count : %d\n", bits);
	printf("SizeImage : %d\n", bmpInfoHeader.SizeImage);
	printf("biClrUsed : %d\n", bmpInfoHeader.biClrUsed);

	if(bmpInfoHeader.biClrUsed == 0 && bmpInfoHeader.biBitCount == 8){
		bmpInfoHeader.biClrUsed = 256;
	}


	//palrgb = (RGBQUAD*)malloc(sizeof(RGBQUAD)*bmpInfoHeader.biClrUsed);
	//fread(palrgb, sizeof(RGBQUAD), bmpInfoHeader.biClrUsed, fp);
	
	printf("Calculation Image Size : %d\n", imagesize);
	sizeScale = 8 / bits;
	//printf("%d\n", sizeScale);
	
	inimg=(ubyte*)malloc(sizeof(ubyte)*imagesize*3); 
	outimg=(ubyte*)malloc(sizeof(ubyte)*imagesize); 
	fread(inimg, sizeof(ubyte), imagesize*3, fp); 
	
	fclose(fp);

	for( int i = 0; i < 256; i++){
		grayPal[i].rgbBlue = i;
		grayPal[i].rgbGreen = i;
		grayPal[i].rgbRed = i;
	}


	for( int i = 0; i < imagesize; i++ ){

		unsigned char y = (299*inimg[i*3+2]+587*inimg[i*3+1]+114*inimg[i*3])/1000;
		outimg[i] = y;

	 }	   
	 
	
	bmpInfoHeader.biBitCount = 8;
	bmpInfoHeader.SizeImage = width * height;
	bmpInfoHeader.biClrUsed = 256;
	bmpHeader.bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD)*256+ bmpInfoHeader.SizeImage;
	bmpHeader.bf0ffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD)*256;
	

	if((fp=fopen(output, "wb"))==NULL) { 
		fprintf(stderr, "Error : Failed to open file...₩n"); 
		return -1;
	}
	
	fwrite(&bmpHeader, sizeof(BITMAPFILEHEADER), 1, fp);
	fwrite(&bmpInfoHeader, sizeof(BITMAPINFOHEADER), 1, fp);

	fwrite(grayPal, sizeof(RGBQUAD), 256, fp); 
	
	fwrite(outimg, sizeof(unsigned char), width * height, fp);
	
	free(inimg); 
	free(outimg);
	
	fclose(fp); 
	
	return 0;
}

unsigned char clip(int value, int min, int max)
{
    return(value > max? max : value < min? min : value);
}
