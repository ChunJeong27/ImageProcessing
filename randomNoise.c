#include <stdio.h> 
#include <stdlib.h> 
#include <string.h>
#include <time.h>

#include "bmpHeader.h"

#define BYTE unsigned char

#define widthbytes(bits) (((bits)+31)/32*4)

inline unsigned char clip(int value, int min, int max);
unsigned char clip(int value, int min, int max)
{
    return(value > max? max : value < min? min : value);
}

int main(int argc, char** argv) {
	FILE* fp; 
	RGBQUAD palrgb[256];
	
	BITMAPFILEHEADER bmpHeader;
	BITMAPINFOHEADER bmpInfoHeader;

	char input[128], output[128];
	
	int i, j, size; 
	int bits, width, height, imagesize;
	int r,g,b, noise;

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

	size=widthbytes(bits * width); 
	
	imagesize = bmpInfoHeader.SizeImage;
	if(!imagesize) 
		imagesize=height*size;
	
	inimg=(BYTE*)malloc(sizeof(BYTE)*imagesize); 
	outimg=(BYTE*)malloc(sizeof(BYTE)*imagesize); 
	fread(inimg, sizeof(BYTE), imagesize, fp); 
	
	fclose(fp);

	srand((unsigned int)time(NULL));
	
	for(i=0; i<height*3; i+=3) {
		for(j=0; j<width*3; j+=3) {
			outimg[j+width*i+0] = inimg[j+width*i+0];
			outimg[j+width*i+1] = inimg[j+width*i+1];
			outimg[j+width*i+2] = inimg[j+width*i+2];
		}
	}

	for(int k = 0; k<atoi(argv[3]); k++) {
		i = rand() % height;
		j = rand() % width;

		int b = inimg[j*3+(i*width*3+0)]; 
		int g = inimg[j*3+(i*width*3+1)]; 
		int r = inimg[j*3+(i*width*3+2)]; 

		int noise = rand() & 0xff;
		
		b = noise;
		g = noise;
		r = noise;
			
		outimg[j*3+width*i*3+0]= clip(b, 0, 255);
		outimg[j*3+width*i*3+1]= clip(g, 0, 255);
		outimg[j*3+width*i*3+2]= clip(r, 0, 255);
	 };	   
	 
	size=widthbytes(bits*width); 
	imagesize=height*size; 
	//offset+=256*sizeof(RGBQUAD);
	
	if((fp=fopen(output, "wb"))==NULL) { 
		fprintf(stderr, "Error : Failed to open file...₩n"); 
		return -1;
	}
	
	fwrite(&bmpHeader, sizeof(BITMAPFILEHEADER), 1, fp);
	
	fwrite(&bmpInfoHeader, sizeof(BITMAPINFOHEADER), 1, fp);

	//fwrite(palrgb, sizeof(unsigned int), 256, fp); 
	
	fwrite(outimg, sizeof(unsigned char), imagesize, fp);
	
	free(inimg); 
	free(outimg);
	
	fclose(fp); 
	
	return 0;
}
