#include "Header.h"
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <malloc.h>
#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <opencv\cv.h>
#include <opencv\cxcore.h>
#include <opencv\highgui.h>

//RGB para HSV
int vc_rgb_to_hsv_imgimg(IplImage *src, IplImage *dst) {
	unsigned char *datasrc = (unsigned char *)src->imageData;
	unsigned char *datadst = (unsigned char *)dst->imageData;
	int bytesperline_src = src->width * src->nChannels;
	int bytesperline_dst = dst->width * dst->nChannels;
	int channels_src = src->nChannels;
	int channels_dst = dst->nChannels;
	int width_src = src->width;
	int width_dst = dst->width;
	int height_src = src->height;
	int height_dst = dst->height;
	long int pos_src, pos_dst;
	int x, y;
	float r, g, b, sat, val, hue;
	float rgb_max, rgb_min;

	//Verificar erros
	if ((src->width <= 0) || (src->height <= 0) || (src->imageData == NULL))
	{
		return 0;
	}

	if ((src->width != dst->width) || (src->height != dst->height))
	{
		return 0;
	}

	if (src->nChannels != 3 || dst->nChannels != 3)
	{
		return 0;
	}

	for (y = 0; y < height_src; y++)
	{
		for (x = 0; x < width_src; x++)
		{
			pos_src = y * bytesperline_src + x * channels_src;
			pos_dst = y * bytesperline_dst + x * channels_dst;

			r = (float)datasrc[pos_src];
			g = (float)datasrc[pos_src + 1];
			b = (float)datasrc[pos_src + 2];

			rgb_max = (r > g ? (r > b ? r : b) : (g > b ? g : b));
			rgb_min = (r < g ? (r < b ? r : b) : (g < b ? g : b));

			val = rgb_max;

			if (val == 0.0 || rgb_max == rgb_min)
			{
				hue = 0.0;
				sat = 0.0;
			}
			else
			{
				sat = (rgb_max - rgb_min) / (val);

				if ((rgb_max == r) && (g >= b))
				{
					hue = 60.0f * (g - b) / (rgb_max == rgb_min ? 1 : (rgb_max - rgb_min));
				}
				else if ((rgb_max == r) && (b > g))
				{
					hue = 360.0f + 60.0f * (g - b) / (rgb_max == rgb_min ? 1 : (rgb_max - rgb_min));
				}
				else if (rgb_max == g)
				{
					hue = 120 + 60 * (b - r) / (rgb_max == rgb_min ? 1 : (rgb_max - rgb_min));
				}
				else
				{
					hue = 240.0f + 60.0f * (r - g) / (rgb_max == rgb_min ? 1 : (rgb_max - rgb_min));
				}
			}


			datadst[pos_dst] = (unsigned char)(hue*255/360);
			datadst[pos_dst + 1] = (unsigned char)(sat * 255);
			datadst[pos_dst + 2] = (unsigned char)(val);

			datadst[pos_dst + 2 ] = datadst[pos_dst + 2 ] * 0.5;

			//if (datadst[pos_dst+2] > 190 && datadst[pos_dst+2] < 300 && sat > 1 && sat < 20)
			//{
			//	datadst[pos_dst + 2] = 255;
			//}
			//else
			//{
			//	/*datadst[pos_dst] = 255;
			//	datadst[pos_dst + 1] = 255;
			//	datadst[pos_dst + 2] = 255;*/
			//}

			//Procura a cor e torna-a branca 
			if (hue > 200 && hue < 290 && sat > 0.1 && val > 90)
			{
				datadst[pos_dst] = 255;
				datadst[pos_dst + 1] = 255;
				datadst[pos_dst + 2] = 255;
			}
			else //O resto fica a preto
			{
				datadst[pos_dst] = 0;
				datadst[pos_dst + 1] = 0;
				datadst[pos_dst + 2] = 0;
			}
		}
	}
	return 1;
}

int vc_binary_dilate(IplImage *src, IplImage *dst, int kernel)
{

	unsigned char *datasrc = (unsigned char *)src->imageData;
	unsigned char *datadst = (unsigned char *)dst->imageData;
	int width = src->width;
	int height = src->height;
	int bytesperline = src->nChannels;
	int channels = src->nChannels;
	int x, y, xx, yy, max, min;
	long int pos, pos_k;
	int offset = kernel / 2;
	int aux;

	memset(datadst, 0, width*height);

	for (y = offset; y < height - offset; y++)
	{
		for (x = offset; x < width - offset; x++)
		{
			pos = y * bytesperline + x * channels;
			aux = 0;

			for (yy = -offset; yy <= offset; yy++)
			{
				for (xx = -offset; xx <= offset; xx++)
				{
					pos_k = (y + yy) * width + (x + xx) * channels; //Posição do vizinho

					if (datasrc[pos_k] == 255) aux = 255;
				}
			}
			datadst[pos] = aux;
		}
	}

	return 1;
}

int vc_binary_erode(IplImage *src, IplImage *dst, int kernel)
{
	unsigned char *datasrc = (unsigned char *)src->imageData;
	unsigned char *datadst = (unsigned char *)dst->imageData;
	int width = src->width;
	int height = src->height;
	int bytesperline = src->nChannels;
	int channels = src->nChannels;
	int x, y;
	int xx, yy;
	int xxyymax = (kernel - 1) / 2;
	int xxyymin = -xxyymax;
	long int pos, posk;
	unsigned char threshold;
	int aux = 255;

	// Verificação de erros
	if ((src->width <= 0) || (src->height <= 0) || (src->imageData == NULL)) return 0;
	if ((src->width != dst->width) || (src->height != dst->height) || (src->nChannels != dst->nChannels)) return 0;
	if (channels != 1) return 0;

	memset(datadst, 0, width*height);

	for (y = 0; y<height; y++)
	{
		for (x = 0; x<width; x++)
		{
			pos = y * bytesperline + x * channels;
			aux = 255;

			// NxM Vizinhos
			for (yy = xxyymin; yy <= xxyymax; yy++)
			{
				for (xx = xxyymin; xx <= xxyymax; xx++)
				{
					if ((y + yy >= 0) && (y + yy < height) && (x + xx >= 0) && (x + xx < width))
					{
						posk = (y + yy) * bytesperline + (x + xx) * channels;

						if (datasrc[posk] == 0) aux = 0;
					}
				}
			}

			datadst[pos] = aux;
		}
	}

	return 1;
}

int vc_binary_open(IplImage *src, IplImage *dst, int sizeerode, int sizedilate)
{
	int ret = 1;
	
	IplImage *aux = cvCreateImage(cvGetSize(src), src->depth, src->nChannels);

	ret &= vc_binary_erode(src, aux, sizeerode);
	ret &= vc_binary_dilate(aux, dst, sizedilate);
	return ret;
}

//Função utilizada para aplicar um filtro sobre uma imagem gray e, desta forma, remover todo o seu ruído
int vc_gray_gaussian_filter(IplImage *src, IplImage *dst, float n)
{
	unsigned char *datasrc = (unsigned char *)src->imageData;
	unsigned char *datadst = (unsigned char *)dst->imageData;
	int width = src->width;
	int height = src->height;

	int bytesperline = src->nChannels;
	int channels = src->nChannels;

	int x, y, i, j;
	int pos, pos_m;
	int total = 0;
	int mask = 5;

	// mascara gaussian
	int gaussian[5][5] =
	{//	{ min, max}
	{ 1, 4,  7,  4,  1 },
	{ 4, 16, 26, 16, 4 },
	{ 7 ,26, 41, 26, 7 },
	{ 4, 16, 26, 16, 4 },
	{ 1, 4,  7,  4,  1 },
	};

	if ((src->width <= 0) || (src->height <= 0) || (src->imageData == NULL))
	{
		return 0;
	}

	if ((src->width != dst->width) || (src->height != dst->height))
	{
		return 0;
	}

	if (src->nChannels != 3 || dst->nChannels != 3)
	{
		return 0;
	}

	for (y = 2; y < height - 2; y++)
	{
		for (x = 2; x < width - 2; x++)
		{
			pos = y * bytesperline + x * channels;

			for (i = 0; i < mask; i++) {
				for (j = 0; j < mask; j++) {
					// mask -2 para que a imagem analizada seja de pos-2 a pos+2
					pos_m = (y + i - 2)*bytesperline + (x + j - 2) * channels;
					total += gaussian[i][j] * datasrc[pos_m];
				}
			}
			// pintar a branco os limites
			if (y == 0 || y == 1 || x == 0 || x == 1) datadst[pos] = (unsigned char)255;
			else datadst[pos] = (unsigned char)((int)(total / 273));
			total = 0;
			//if (datadst[pos] != 255) printf("datadst[pos] = %uc", datadst[pos]);
		}
	}

	return 0;
}

int vc_gray_to_binary(IplImage *src, IplImage *dst, int threshold) {
	unsigned char *datasrc = (unsigned char *)src->imageData;
	int bytesperline_src = src->width * src->nChannels;
	int channels_src = src->nChannels;
	unsigned char *datadst = (unsigned char *)dst->imageData;
	int bytesperline_dst = dst->width * dst->nChannels;
	int channels_dst = dst->nChannels;
	int width = src->width;
	int height = src->height;
	int x, y;
	long int pos_src, pos_dst;

	if ((src->width <= 0) || (src->height <= 0) || (src->imageData == NULL))
	{
		return 0;
	}

	if ((src->width != dst->width) || (src->height != dst->height))
	{
		return 0;
	}

	if ((src->nChannels != 1) || (dst->nChannels!= 1))
	{
		return 0;
	}

	for (y = 0; y < height; y++)
	{
		for (x = 0; x < width; x++)
		{
			pos_src = y * bytesperline_src + x * channels_src;
			pos_dst = y * bytesperline_dst + x * channels_dst;

			if (datasrc[pos_src]>threshold)
			{
				datadst[pos_src] = 255;
			}
			else
			{
				datadst[pos_src] = 0;
			}
		}
	}
}

int vc_rgb_to_gray(IplImage *src, IplImage *dst) {
	unsigned char *datasrc = (unsigned char *)src->imageData;
	unsigned char *datadst = (unsigned char *)dst->imageData;
	int bytesperline_src = src->width * src->nChannels;
	int bytesperline_dst = dst->width * dst->nChannels;
	int channels_src = src->nChannels;
	int channels_dst = dst->nChannels;
	int width_src = src->width;
	int width_dst = dst->width;
	int height_src = src->height;
	int height_dst = dst->height;
	int x, y;
	long int pos_src, pos_dst;
	float rf, gf, bf;

	if ((src->width <= 0) || (src->height <= 0) || (src->imageData == NULL))
	{
		return 0;
	}

	if ((src->width != dst->width) || (src->height != dst->height))
	{
		return 0;
	}

	if ((src->nChannels != 3) || (dst->nChannels != 1))
	{
		return 0;
	}

	for (y = 0; y < height_src; y++)
	{
		for (x = 0; x < width_src; x++)
		{
			pos_src = y * bytesperline_src + x * channels_src;
			pos_dst = y * bytesperline_dst + x * channels_dst;

			rf = (float)datasrc[pos_src];
			gf = (float)datasrc[pos_src + 1];
			bf = (float)datasrc[pos_src + 2];

			datadst[pos_dst] = (unsigned char)((rf*0.299) + (gf*0.587) + (bf*0.114));
		}
	}
	return 1;
}

int vc_gray_edge_sobel(IplImage *src, IplImage *dst, float th) // th = [0.001, 1.000]
{
	unsigned char *datasrc = (unsigned char *)src->imageData;
	unsigned char *datadst = (unsigned char *)dst->imageData;
	int width = src->width;
	int height = src->height;
	int bytesperline = src->nChannels;
	int channels = src->nChannels;
	int x, y;
	long int posX, posA, posB, posC, posD, posE, posF, posG, posH;
	int i, size;
	int histmax, histthreshold;
	int sumx, sumy;
	int hist[256] = { 0 };

	// Verificacao de erros
	if ((src->width <= 0) || (src->height <= 0) || (src->imageData == NULL)) return 0;
	if ((src->width != dst->width) || (src->height != dst->height) || (src->nChannels != dst->nChannels)) return 0;
	if (channels != 1) return 0;

	size = width * height;

	for (y = 1; y<height - 1; y++)
	{
		for (x = 1; x<width - 1; x++)
		{
			posA = (y - 1) * bytesperline + (x - 1) * channels;
			posB = (y - 1) * bytesperline + x * channels;
			posC = (y - 1) * bytesperline + (x + 1) * channels;
			posD = y * bytesperline + (x - 1) * channels;
			posX = y * bytesperline + x * channels;
			posE = y * bytesperline + (x + 1) * channels;
			posF = (y + 1) * bytesperline + (x - 1) * channels;
			posG = (y + 1) * bytesperline + x * channels;
			posH = (y + 1) * bytesperline + (x + 1) * channels;

			sumx = datasrc[posA] * -1;
			sumx += datasrc[posD] * -2;
			sumx += datasrc[posF] * -1;

			sumx += datasrc[posC] * +1;
			sumx += datasrc[posE] * +2;
			sumx += datasrc[posH] * +1;
			sumx = sumx / 4; // 4 = 1 + 2 + 1

			sumy = datasrc[posA] * -1;
			sumy += datasrc[posB] * -2;
			sumy += datasrc[posC] * -1;

			sumy += datasrc[posF] * +1;
			sumy += datasrc[posG] * +2;
			sumy += datasrc[posH] * +1;
			sumy = sumy / 4; // 4 = 1 + 2 + 1

			datadst[posX] = (unsigned char)sqrt((double)(sumx*sumx + sumy * sumy));
			//datadst[posX] = (unsigned char) (sqrt((double) (sumx*sumx + sumy*sumy)) / 4.0);
		}
	}

	// Compute a grey level histogram
	for (y = 0; y<height; y++)
	{
		for (x = 0; x<width; x++)
		{
			hist[datadst[y * bytesperline + x * channels]]++;
		}
	}

	// Threshold at the middle of the occupied levels
	histmax = 0;
	for (i = 0; i <= 255; i++)
	{
		histmax += hist[i];

		// th = Sobel Threshold
		if (histmax >= (((float)size) * th)) break;
	}
	histthreshold = i;

	// Apply the threshold
	for (y = 0; y<height; y++)
	{
		for (x = 0; x<width; x++)
		{
			posX = y * bytesperline + x * channels;

			if (datadst[posX] >= histthreshold) datadst[posX] = 255;
			else datadst[posX] = 0;
		}
	}

	return 1;
}