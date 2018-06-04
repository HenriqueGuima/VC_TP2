#pragma once

#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <malloc.h>
#include <stdlib.h>
#include <math.h>
#include <opencv\cv.h>
#include <opencv\cxcore.h>
#include <opencv\highgui.h>

#define PI 3.14159265358979323846
#define MAX3(a,b,c) (a>b ? (a>c ? a:c):(b>c ?b:c))
#define MIN3(a,b,c) (a<b ? (a<c ? a:c):(b<c ?b:c))

typedef struct {
	int x, y, width, height; // Caixa Delimitadora (Bounding Box)
	int area; // área
	int xc, yc; // Centro-de-massa
	int perimeter; // Perimetro
	int label; // Etiqueta
	unsigned char *mask; // Not Used
	unsigned char *data; // Not Used
	int channels; // Not Used
	int levels; // Not Used
	int calibre; //
	float diametro; //
} OVC;

int vc_rgb_to_hsv_imgimg(IplImage *src, IplImage *dst);
int vc_binary_dilate(IplImage *src, IplImage *dst, int kernel);
int vc_binary_open(IplImage *src, IplImage *dst, int sizeerode, int sizedilate);
int vc_gray_gaussian_filter(IplImage *src, IplImage *dst, float n);
