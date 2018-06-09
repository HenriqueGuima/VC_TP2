#pragma once
#define _CRT_SECURE_NO_WARNINGS
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

int vc_bgr_to_hsv(IplImage *srcdst);
int vc_rgb_to_hsv_mp(IplImage *src, IplImage *dst);
int vc_rgb_to_hsv_md(IplImage *src, IplImage *dst);
int vc_rgb_to_hsv_me(IplImage *src, IplImage *dst);
int vc_binary_dilate(IplImage *src, IplImage *dst, int kernel);
int vc_binary_open(IplImage *src, IplImage *dst, int sizeerode, int sizedilate);
int vc_gray_gaussian_filter(IplImage *src, IplImage *dst, float n);
int vc_gray_to_binary(IplImage *src, IplImage *dst, int threshold);;
int vc_rgb_to_gray(IplImage *src, IplImage *dst);
int vc_gray_edge_sobel(IplImage *src, IplImage *dst, float th);
int vc_binary_blob_info(IplImage *src, OVC *blobs, int nblobs);
OVC* vc_binary_blob_labellingOpencv(IplImage *src, IplImage *dst, int *nlabels);
int vc_draw_boundingbox(IplImage *src, OVC blob);
int vc_bgr_to_rgb(IplImage *src, IplImage *dst);
int vc_rgb_to_hsv(IplImage *srcdst);
int vc_hsv_to_binary_in_range(IplImage *src, IplImage *dst, unsigned short Cor);
int vc_gray_negative(IplImage *src, IplImage *dst);