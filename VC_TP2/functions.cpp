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


			/*datadst[pos_dst] = (unsigned char)(hue*255/360);
			datadst[pos_dst + 1] = (unsigned char)(sat * 255);
			datadst[pos_dst + 2] = (unsigned char)(val);*/

			////Procura a cor e torna-a branca 
			if (hue > 20 && hue < 120 && sat > 10 && val > 20)
			{
				datadst[pos_dst] = 0;
				datadst[pos_dst + 1] = 0;
				datadst[pos_dst + 2] = 0;
			}
			else //O resto fica a preto
			{
				datadst[pos_dst] =255;
				datadst[pos_dst + 1] = 255;
				datadst[pos_dst + 2] = 255;
			}
		}
	}
	return 1;
}