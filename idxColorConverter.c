#include <stdio.h> 
#include <stdlib.h> 
#include <string.h>

#include "bmpHeader.h"

#define ubyte unsigned char

inline unsigned char clip(int value, int min, int max);

int main(int argc, char** argv) {
	FILE* fp; 
	RGBQUAD* palrgb;
	
	BITMAPFILEHEADER bmpHeader;
	BITMAPINFOHEADER bmpInfoHeader;

	unsigned int width, height, imagesize, sizeScale;
	unsigned short bits;

	char input[128], output[128];
	
	int i, j; 
	
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
	putchar('Y');
	putchar('\n');
	}


	palrgb = (RGBQUAD*)malloc(sizeof(RGBQUAD)*bmpInfoHeader.biClrUsed);
	fread(palrgb, sizeof(RGBQUAD), bmpInfoHeader.biClrUsed, fp);
	
	printf("Calculation Image Size : %d\n", imagesize);
	sizeScale = 8 / bits;
	//printf("%d\n", sizeScale);
	
	inimg=(ubyte*)malloc(sizeof(ubyte)*imagesize); 
	outimg=(ubyte*)malloc(sizeof(ubyte)*imagesize*sizeScale*3); 
	fread(inimg, sizeof(ubyte), imagesize, fp); 
	
	fclose(fp);

	unsigned char mask = 0;
	
	if( bits == 1){
		mask = 0b10000000;
	} else if( bits == 4 ){
		mask = 0b11110000;
	} else if( bits == 8 ){
		mask = 0b11111111;
	}

	int a = 0;
	unsigned char  v;

	for( i = 0; i < imagesize*3; i+=3 ){
			v = inimg[a] & mask;

			if(bits == 1){
				v = v >> (7-((i/3)%8));
				//printf("%d ", v);
			} else if(bits == 4){
				if(mask == 0b11110000){
					v = v >> 4;
					//printf("%d ", v);
				}
			}

			outimg[i]= palrgb[v].rgbBlue;
			outimg[i+1]= palrgb[v].rgbGreen;
			outimg[i+2]= palrgb[v].rgbRed;



			if( mask == 1 ){
				mask = 0b10000000;
				a++;
			} else if (mask == 15) {
				mask = 0b11110000;
				a++;
			} else if ( mask == 255 ) {
				a++;
			} else {
				mask = mask >> bits;
				//printf("%d ", mask);

			}

	 }	   
	 
	
	bmpInfoHeader.biBitCount = 24;
	bmpInfoHeader.SizeImage = imagesize*sizeScale*3;
	bmpInfoHeader.biClrUsed = 0;
	bmpHeader.bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + bmpInfoHeader.SizeImage;
	bmpHeader.bf0ffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
	

	if((fp=fopen(output, "wb"))==NULL) { 
		fprintf(stderr, "Error : Failed to open file...₩n"); 
		return -1;
	}
	
	fwrite(&bmpHeader, sizeof(BITMAPFILEHEADER), 1, fp);
	fwrite(&bmpInfoHeader, sizeof(BITMAPINFOHEADER), 1, fp);

	//fwrite(palrgb, sizeof(unsigned int), 256, fp); 
	
	fwrite(outimg, sizeof(unsigned char), imagesize*sizeScale*3, fp);
	
	free(inimg); 
	free(outimg);
	
	fclose(fp); 
	
	return 0;
}

unsigned char clip(int value, int min, int max)
{
    return(value > max? max : value < min? min : value);
}
