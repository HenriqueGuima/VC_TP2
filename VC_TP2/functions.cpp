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

OVC* vc_binary_blob_labellingOpencv(IplImage *src, IplImage *dst, int *nlabels) {
	unsigned char *datasrc = (unsigned char *)src->imageData;
	unsigned char *datadst = (unsigned char *)dst->imageData;
	int width = src->width;
	int height = src->height;
	int bytesperline = src->widthStep;
	int channels = src->nChannels;
	int x, y, a, b;
	long int i, size;
	long int posX, posA, posB, posC, posD;
	int labeltable[256] = { 0 };
	int labelarea[256] = { 0 };
	int label = 1; // Etiqueta inicial.
	int num;
	OVC *blobs; // Apontador para lista de blobs (objectos) que ser retornada desta funo.

				// Verificao de erros
	if ((src->width <= 0) || (src->height <= 0) || (src->imageData == NULL)) return 0;
	if ((src->width != dst->width) || (src->height != dst->height) || (src->nChannels != dst->nChannels)) return NULL;
	if (channels != 1) return NULL;

	// Copia dados da imagem binria para imagem grayscale
	memcpy(datadst, datasrc, bytesperline * height);

	// Todos os pixis de plano de fundo devem obrigatriamente ter valor 0
	// Todos os pixis de primeiro plano devem obrigatriamente ter valor 255
	// Sero atribudas etiquetas no intervalo [1,254]
	// Este algoritmo est assim limitado a 254 labels
	for (i = 0, size = bytesperline * height; i < size; i++) {
		if (datadst[i] != 0) datadst[i] = 255;
	}

	// Limpa os rebordos da imagem binria
	for (y = 0; y < height; y++) {
		datadst[y * bytesperline + 0 * channels] = 0;
		datadst[y * bytesperline + (width - 1) * channels] = 0;
	}
	for (x = 0; x < width; x++) {
		datadst[0 * bytesperline + x * channels] = 0;
		datadst[(height - 1) * bytesperline + x * channels] = 0;
	}

	// Efectua a etiquetagem
	for (y = 1; y < height - 1; y++) {
		for (x = 1; x < width - 1; x++) {
			// Kernel:
			// A B C
			// D X

			posA = (y - 1) * bytesperline + (x - 1) * channels; // A
			posB = (y - 1) * bytesperline + x * channels; // B
			posC = (y - 1) * bytesperline + (x + 1) * channels; // C
			posD = y * bytesperline + (x - 1) * channels; // D
			posX = y * bytesperline + x * channels; // X

													// Se o pixel foi marcado
			if (datadst[posX] != 0) {
				if ((datadst[posA] == 0) && (datadst[posB] == 0) && (datadst[posC] == 0) && (datadst[posD] == 0)) {
					datadst[posX] = label;
					labeltable[label] = label;
					label++;
				}
				else {
					num = 255;

					// Se A est marcado, j tem etiqueta (j no  255), e  menor que a etiqueta "num"
					if ((datadst[posA] != 0) && (datadst[posA] != 255) && (datadst[posA] < num)) {
						num = datadst[posA];
					}
					// Se B est marcado, j tem etiqueta (j no  255), e  menor que a etiqueta "num"
					if ((datadst[posB] != 0) && (datadst[posB] != 255) && (datadst[posB] < num)) {
						num = datadst[posB];
					}
					// Se C est marcado, j tem etiqueta (j no 255), e  menor que a etiqueta "num"
					if ((datadst[posC] != 0) && (datadst[posC] != 255) && (datadst[posC] < num)) {
						num = datadst[posC];
					}
					// Se D est marcado, j tem etiqueta (j no  255), e  menor que a etiqueta "num"
					if ((datadst[posD] != 0) && (datadst[posD] != 255) && (datadst[posD] < num)) {
						num = datadst[posD];
					}

					// Actualiza a tabela de etiquetas
					if ((datadst[posA] != 0) && (datadst[posA] != 255)) {
						if (labeltable[datadst[posA]] != labeltable[num]) {
							for (a = 1; a < label; a++) {
								if (labeltable[a] == labeltable[datadst[posA]]) {
									labeltable[a] = labeltable[num];
								}
							}
						}
					}
					if ((datadst[posB] != 0) && (datadst[posB] != 255)) {
						if (labeltable[datadst[posB]] != labeltable[num]) {
							for (a = 1; a < label; a++) {
								if (labeltable[a] == labeltable[datadst[posB]]) {
									labeltable[a] = labeltable[num];
								}
							}
						}
					}
					if ((datadst[posC] != 0) && (datadst[posC] != 255)) {
						if (labeltable[datadst[posC]] != labeltable[num]) {
							for (a = 1; a < label; a++) {
								if (labeltable[a] == labeltable[datadst[posC]]) {
									labeltable[a] = labeltable[num];
								}
							}
						}
					}
					if ((datadst[posD] != 0) && (datadst[posD] != 255)) {
						if (labeltable[datadst[posD]] != labeltable[num]) {
							for (a = 1; a < label; a++) {
								if (labeltable[a] == labeltable[datadst[posD]]) {
									labeltable[a] = labeltable[num];
								}
							}
						}
					}
					labeltable[datadst[posX]] = num;

					// Atribui a etiqueta ao pixel
					datadst[posX] = num;
				}
			}
		}
	}

	// Volta a etiquetar a imagem
	for (y = 1; y < height - 1; y++) {
		for (x = 1; x < width - 1; x++) {
			posX = y * bytesperline + x * channels; // X

			if (datadst[posX] != 0) {
				datadst[posX] = labeltable[datadst[posX]];
			}
		}
	}

	// Contagem do nmero de blobs
	// Passo 1: Eliminar, da tabela, etiquetas repetidas
	for (a = 1; a < label - 1; a++) {
		for (b = a + 1; b < label; b++) {
			if (labeltable[a] == labeltable[b]) labeltable[b] = 0;
		}
	}
	// Passo 2: Conta etiquetas e organiza a tabela de etiquetas, para que no hajam valores vazios (zero) entre etiquetas
	*nlabels = 0;
	for (a = 1; a < label; a++) {
		if (labeltable[a] != 0) {
			labeltable[*nlabels] = labeltable[a]; // Organiza tabela de etiquetas
			(*nlabels)++; // Conta etiquetas
		}
	}

	// Se no h blobs
	if (*nlabels == 0) return NULL;

	// Cria lista de blobs (objectos) e preenche a etiqueta
	blobs = (OVC *)calloc((*nlabels), sizeof(OVC));
	if (blobs != NULL) {
		for (a = 0; a < (*nlabels); a++) blobs[a].label = labeltable[a];
	}
	else return NULL;

	return blobs;
}

int vc_binary_blob_info(IplImage *src, OVC *blobs, int nblobs) {
	unsigned char *data = (unsigned char *)src->imageData;
	int width = src->width;
	int height = src->height;
	int bytesperline = src->widthStep;
	int channels = src->nChannels;
	int x, y, i;
	long int pos;
	int xmin, ymin, xmax, ymax;
	long int sumx, sumy;

	// Verificação de erros
	if ((src->width <= 0) || (src->height <= 0) || (src->imageData == NULL)) return 0;
	if (channels != 1) return 0;

	// Conta área de cada blob
	for (i = 0; i < nblobs; i++) {
		xmin = width - 1;
		ymin = height - 1;
		xmax = 0;
		ymax = 0;

		sumx = 0;
		sumy = 0;

		if (blobs[i].label != 0)
			blobs[i].area = 0;

		for (y = 1; y < height - 1; y++) {
			for (x = 1; x < width - 1; x++) {
				pos = y * bytesperline + x * channels;

				if (data[pos] == blobs[i].label) {
					// Área
					blobs[i].area++;

					// Centro de Gravidade
					sumx += x;
					sumy += y;

					// Bounding Box
					if (xmin > x) xmin = x;
					if (ymin > y) ymin = y;
					if (xmax < x) xmax = x;
					if (ymax < y) ymax = y;

					// Perímetro
					// Se pelo menos um dos quatro vizinhos não pertence ao mesmo label, então é um pixel de contorno
					if ((data[pos - 1] != blobs[i].label) || (data[pos + 1] != blobs[i].label) || (data[pos - bytesperline] != blobs[i].label) || (data[pos + bytesperline] != blobs[i].label)) {
						blobs[i].perimeter++;
					}
				}
			}
		}

		// Bounding Box
		blobs[i].x = xmin;
		blobs[i].y = ymin;
		blobs[i].width = (xmax - xmin) + 1;
		blobs[i].height = (ymax - ymin) + 1;

		// Centro de Gravidade
		//blobs[i].xc = (xmax - xmin) / 2;
		//blobs[i].yc = (ymax - ymin) / 2;
		blobs[i].xc = sumx / MAX(blobs[i].area, 1);
		blobs[i].yc = sumy / MAX(blobs[i].area, 1);


		// Calulamos o diametro através da regra de tres simples
		// alutra mais largura / média ()
		blobs[i].diametro = ((blobs[i].width + blobs[i].height) / 2) * 55 / 280;

		// não usar sff
		//blobs[i].diametro = (blobs[i].perimeter / PI) * 55 / 280;

		// condições de atribuição de calibre às laranjas
		// como ha valores que se sobrepoem, as laranjas são sempre calibradas por baixo
		// se ja uma laranja de calibre 8 que caia nos valores de 7 passa a ser 7
		if (blobs[i].diametro >= 100 && blobs[i].diametro <= 900)
		{
			blobs[i].calibre = 0;
		}
		else if (blobs[i].diametro >= 87 && blobs[i].diametro < 100) {
			blobs[i].calibre = 1;
		}
		else if (blobs[i].diametro >= 84 && blobs[i].diametro < 96) {
			blobs[i].calibre = 2;
		}
		else if (blobs[i].diametro >= 81 && blobs[i].diametro < 92) {
			blobs[i].calibre = 3;
		}
		else if (blobs[i].diametro >= 77 && blobs[i].diametro < 88) {
			blobs[i].calibre = 4;
		}
		else if (blobs[i].diametro >= 73 && blobs[i].diametro < 84) {
			blobs[i].calibre = 5;
		}
		else if (blobs[i].diametro >= 70 && blobs[i].diametro < 80) {
			blobs[i].calibre = 6;
		}
		else if (blobs[i].diametro >= 67 && blobs[i].diametro < 76) {
			blobs[i].calibre = 7;
		}
		else if (blobs[i].diametro >= 64 && blobs[i].diametro < 73) {
			blobs[i].calibre = 8;
		}
		else if (blobs[i].diametro >= 62 && blobs[i].diametro < 70) {
			blobs[i].calibre = 9;
		}
		else if (blobs[i].diametro >= 60 && blobs[i].diametro < 68) {
			blobs[i].calibre = 10;
		}
		else if (blobs[i].diametro >= 58 && blobs[i].diametro < 66) {
			blobs[i].calibre = 11;
		}
		else if (blobs[i].diametro >= 56 && blobs[i].diametro < 63) {
			blobs[i].calibre = 12;
		}
		else if (blobs[i].diametro >= 53 && blobs[i].diametro < 60) {
			blobs[i].calibre = 13;
		}

	}

	return 1;
}
