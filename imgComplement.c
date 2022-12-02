#include <stdio.h> 
#include <stdlib.h> 
#include <string.h>

#include "bmpHeader.h"

#define ubyte unsigned char

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

	unsigned int width, height, imagesize, sizeScale;
	unsigned short bits;

	char input[128], output[128];
	
	int i, j, size; 
	
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
	
	//imagesize = bmpInfoHeader.SizeImage;
	imagesize = width * height;
	if(!imagesize) 
		imagesize=height*size;
	
	inimg=(ubyte*)malloc(sizeof(ubyte)*imagesize); 
	outimg=(ubyte*)malloc(sizeof(ubyte)*imagesize); 
	fread(inimg, sizeof(ubyte), imagesize, fp); 
	
	fclose(fp);
	
	for(i=0; i<height*3; i+=3) { 
		for(j=0; j<width*3; j+=3) {
			unsigned char b = inimg[j+(i*width+0)]; 
			unsigned char g = inimg[j+(i*width+1)]; 
			unsigned char r = inimg[j+(i*width+2)]; 
			
			b = ~b;
			g = ~g;
			r = ~r; 
			
			outimg[j+width*i+0]= clip(b, 0, 255);
			outimg[j+width*i+1]= clip(g, 0, 255);
			outimg[j+width*i+2]= clip(r, 0, 255);
		};
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
