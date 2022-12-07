#include <stdio.h> 
#include <stdlib.h> 
#include <string.h>
#include <limits.h>		/* USHRT_MAX 상수를 위해서 사용한다. */
#include <unistd.h>
#include <math.h>

#include "bmpHeader.h"

/* 이미지 데이터의 경계 검사를 위한 매크로 */
#define LIMIT_UBYTE(n) ((n)>UCHAR_MAX)?UCHAR_MAX:((n)<0)?0:(n)

typedef unsigned char ubyte;
inline ubyte clip(int value, int min, int max);

int main(int argc, char** argv) 
{
    FILE* fp; 
    BITMAPFILEHEADER bmpHeader;             /* BMP FILE INFO */
    BITMAPINFOHEADER bmpInfoHeader;     /* BMP IMAGE INFO */
    RGBQUAD *palrgb;
    ubyte *inimg, *outimg, *paddingImg, *grayImg;
    int paddingSize;
    int x, y, z, imageSize;
    int grayVal;

    if(argc != 3) {
        fprintf(stderr, "usage : %s input.bmp output.bmp\n", argv[0]);
        return -1;
    }
    
    /***** read bmp *****/ 
    if((fp=fopen(argv[1], "rb")) == NULL) { 
        fprintf(stderr, "Error : Failed to open file...₩n"); 
        return -1;
    }

    /* BITMAPFILEHEADER 구조체의 데이터 */
    fread(&bmpHeader, sizeof(BITMAPFILEHEADER), 1, fp);

    /* BITMAPINFOHEADER 구조체의 데이터 */
    fread(&bmpInfoHeader, sizeof(BITMAPINFOHEADER), 1, fp);

    /* 트루 컬러를 지원하면 변환할 수 없다. */
    if(bmpInfoHeader.biBitCount != 24) {
        perror("This image file doesn't supports 24bit color\n");
        fclose(fp);
        return -1;
    }
    
    int elemSize = bmpInfoHeader.biBitCount/8;
    int size = bmpInfoHeader.biWidth*elemSize;
    int paddingRow = (bmpInfoHeader.biWidth+2)*elemSize;
    imageSize = size * bmpInfoHeader.biHeight; 

    /* 이미지의 해상도(넓이 × 깊이) */
    printf("Resolution : %d x %d\n", bmpInfoHeader.biWidth, bmpInfoHeader.biHeight);
    printf("Bit Count : %d\n", bmpInfoHeader.biBitCount);     /* 픽셀당 비트 수(색상) */
    printf("Image Size : %d\n", imageSize);

    inimg = (ubyte*)malloc(sizeof(ubyte)*imageSize); 
    grayImg = (ubyte*)malloc(sizeof(ubyte)*imageSize);
    outimg = (ubyte*)malloc(sizeof(ubyte)*imageSize);

    paddingSize = (bmpInfoHeader.biWidth+2)*(bmpInfoHeader.biHeight+2)*elemSize;
    paddingImg = (ubyte*)malloc(sizeof(ubyte)*paddingSize);

    fseek(fp, bmpHeader.bf0ffBits, SEEK_SET);
    fread(inimg, sizeof(ubyte), imageSize, fp); 
    
    fclose(fp);

	for(y = 1; y < bmpInfoHeader.biHeight + 1; y++) {
		for(x = elemSize; x < (bmpInfoHeader.biWidth + 1) * elemSize; x+=elemSize) {
			grayVal = (299*inimg[x+y*size+2]+587*inimg[x+y*size+1]+114*inimg[x+y*size])/1000;
            for(z = 0; z < elemSize; z++) {
				grayImg[(x-elemSize)+(y-1)*size+z] = clip(grayVal, 0, 255);
            }
        }
    }


    // define the kernel
    int kernelX[3][3] = { {-1, 0, 1},
                           {-1, 0, 1},
                           {-1, 0, 1} };

    int kernelY[3][3] = { {1, 1, 1},
                           {0, 0, 0},
                           {-1, -1, -1} };

    memset(paddingImg, 0, sizeof(ubyte)*paddingSize);
    memset(outimg, 0, sizeof(ubyte)*imageSize);

	for(y = 1; y < bmpInfoHeader.biHeight + 1; y++) {
		for(x = elemSize; x < (bmpInfoHeader.biWidth + 1)*elemSize; 
				x+=elemSize) {
			paddingImg[x+y*paddingRow+0] = grayImg[x-elemSize+(y-1)*size+0];
		    paddingImg[x+y*paddingRow+1] = grayImg[x-elemSize+(y-1)*size+1];
		    paddingImg[x+y*paddingRow+2] = grayImg[x-elemSize+(y-1)*size+2];
	    }
    }
    for(x = elemSize; x < (bmpInfoHeader.biWidth + 1)*elemSize; x+=elemSize) {
	    paddingImg[x+0] = grayImg[x-elemSize+0];
	    paddingImg[x+1] = grayImg[x-elemSize+1];
	    paddingImg[x+2] = grayImg[x-elemSize+2];

	    paddingImg[x+(bmpInfoHeader.biHeight+1)*paddingRow+0] = grayImg[x-elemSize+(bmpInfoHeader.biHeight-1)*size+0];
	    paddingImg[x+(bmpInfoHeader.biHeight+1)*paddingRow+1] = grayImg[x-elemSize+(bmpInfoHeader.biHeight-1)*size+1];
	    paddingImg[x+(bmpInfoHeader.biHeight+1)*paddingRow+2] = grayImg[x-elemSize+(bmpInfoHeader.biHeight-1)*size+2];
    }
    for(y = 1; y < bmpInfoHeader.biHeight + 1; y++) {
	    paddingImg[y*paddingRow+0] = grayImg[(y-1)*size+0];
	    paddingImg[y*paddingRow+1] = grayImg[(y-1)*size+1];
	    paddingImg[y*paddingRow+2] = grayImg[(y-1)*size+2];

	    paddingImg[(bmpInfoHeader.biWidth+1)*elemSize+y*paddingRow+0] = grayImg[(bmpInfoHeader.biWidth-1)*elemSize+(y-1)*size+0];
	    paddingImg[(bmpInfoHeader.biWidth+1)*elemSize+y*paddingRow+1] = grayImg[(bmpInfoHeader.biWidth-1)*elemSize+(y-1)*size+1];
	    paddingImg[(bmpInfoHeader.biWidth+1)*elemSize+y*paddingRow+2] = grayImg[(bmpInfoHeader.biWidth-1)*elemSize+(y-1)*size+2];
    }

    int sobelX = 0;
    int sobelY = 0;
    float sobel = 0.0;
    for(y = 1; y < bmpInfoHeader.biHeight + 1; y++) { 
        for(x = elemSize; x < (bmpInfoHeader.biWidth + 1) * elemSize; 
				x+=elemSize) {
            for(z = 0; z < elemSize; z++) {
                for(int i = -1; i < 2; i++) {
                    for(int j = -1; j < 2; j++) {
                        sobelX += kernelX[i+1][j+1]*paddingImg[(x+i*elemSize)+(y+j)*paddingRow+z];
                        sobelY += kernelY[i+1][j+1]*paddingImg[(x+i*elemSize)+(y+j)*paddingRow+z];
                    }
                }
				sobel = sqrt(sobelX*sobelX+sobelY*sobelY);
				sobelX = 0; sobelY = 0;
                outimg[(x-elemSize)+(y-1)*size+z] = clip(sobel, 0, 255);
            }
        }
    }         
     
    /***** write bmp *****/ 
    if((fp=fopen(argv[2], "wb"))==NULL) { 
        fprintf(stderr, "Error : Failed to open file...₩n"); 
        return -1;
    }

    /* BITMAPFILEHEADER 구조체의 데이터 */
    fwrite(&bmpHeader, sizeof(BITMAPFILEHEADER), 1, fp);

    /* BITMAPINFOHEADER 구조체의 데이터 */
    fwrite(&bmpInfoHeader, sizeof(BITMAPINFOHEADER), 1, fp);

    //fwrite(inimg, sizeof(ubyte), imageSize, fp); 
    fwrite(outimg, sizeof(ubyte), imageSize, fp);

    fclose(fp); 
    
    free(inimg); 
    free(outimg);
    free(paddingImg);
    free(grayImg);
    
    return 0;
}

ubyte clip(int value, int min, int max) {
	return (value > max ? max : value < min ? min : value);
}
