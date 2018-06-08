#define _CRT_SECURE_NO_WARNINGS

#include "Header.h"

int main(void)
{
	//Rosas passou aqui!
	// Video
	char videofile[50] = "video1_tp2.mp4";
	//char videofile[50] = "video.avi";
	CvCapture *capture;
	IplImage *frame;
	OVC *blobs;
	int i, nblobs;

	struct
	{
		int width, height;
		int ntotalframes;
		int fps;
		int nframe;
	} video;

	// Texto
	CvFont font, fontbkg;
	double hScale = 0.5;
	double vScale = 0.5;
	int lineWidth = 1;
	char str[500] = { 0 };

	// Outros
	int ORANGE_MIN[3] = { 5, 50, 50 };
	int	ORANGE_MAX[3] = { 15, 255, 255 };
	IplImage *gray = NULL, *grayAux = NULL;
	IplImage *grayAux1 = NULL, *grayAux2 = NULL;
	IplImage *grayBin = NULL, *BinBlob = NULL, *grayBinAux = NULL, *frameHSV = NULL, *bin = NULL;
	/* Leitura de vídeo de um ficheiro */
	capture = cvCaptureFromFile(videofile);
	int key = 0;

	/* Leitura de v�deo de um ficheiro */
	/* NOTA IMPORTANTE:
	O ficheiro video-tp2.avi dever estar localizado no mesmo direct�rio que o ficheiro de codigo fonte.
	*/
	capture = cvCaptureFromFile(videofile);

	/* Verifica se foi possvel abrir o ficheiro de vdeo */
	if (!capture)
	{
		fprintf(stderr, "Erro ao abrir o ficheiro de video!\n");
		return 1;
	}

	/* N�mero total de frames no v�deo */
	video.ntotalframes = (int)cvGetCaptureProperty(capture, CV_CAP_PROP_FRAME_COUNT);
	/* Frame rate do v�deo */
	video.fps = (int)cvGetCaptureProperty(capture, CV_CAP_PROP_FPS);
	/* Resolu��o do v�deo */
	video.width = (int)cvGetCaptureProperty(capture, CV_CAP_PROP_FRAME_WIDTH);
	video.height = (int)cvGetCaptureProperty(capture, CV_CAP_PROP_FRAME_HEIGHT);

	/* Cria uma janela para exibir o v�deo */
	cvNamedWindow("VC - TP2", CV_WINDOW_AUTOSIZE);

	/* Inicializa a fonte */
	cvInitFont(&font, CV_FONT_HERSHEY_SIMPLEX, hScale, vScale, 0, lineWidth);
	cvInitFont(&fontbkg, CV_FONT_HERSHEY_SIMPLEX, hScale, vScale, 0, lineWidth + 1);

	while (key != 'q') {
		/* Leitura de uma frame do v�deo */
		frame = cvQueryFrame(capture);

		/* Verifica se conseguiu ler a frame */
		if (!frame) break;

		/* N�mero da frame a processar */
		video.nframe = (int)cvGetCaptureProperty(capture, CV_CAP_PROP_POS_FRAMES);

		
		IplImage* frameAux = cvCreateImage(cvGetSize(frame), 8, 3); // allocate a 3 channel byte image
		
		/*IplImage* frameBin = cvCreateImage(cvGetSize(frame), 8, 3);
		IplImage* frameBinAux = cvCreateImage(cvGetSize(frame), 8, 1);*/

		cvCopy(frame, frameAux, NULL); // OR return img_src_cpy;
		
		/* Exemplo de inser��o texto na frame */
		sprintf(str, "RESOLUCAO: %dx%d", video.width, video.height);
		cvPutText(frame, str, cvPoint(20, 25), &fontbkg, cvScalar(0, 0, 0));
		cvPutText(frame, str, cvPoint(20, 25), &font, cvScalar(255, 255, 255));
		sprintf(str, "TOTAL DE FRAMES: %d", video.ntotalframes);
		cvPutText(frame, str, cvPoint(20, 50), &fontbkg, cvScalar(0, 0, 0));
		cvPutText(frame, str, cvPoint(20, 50), &font, cvScalar(255, 255, 255));
		sprintf(str, "FRAME RATE: %d", video.fps);
		cvPutText(frame, str, cvPoint(20, 75), &fontbkg, cvScalar(0, 0, 0));
		cvPutText(frame, str, cvPoint(20, 75), &font, cvScalar(255, 255, 255));
		sprintf(str, "N. FRAME: %d", video.nframe);
		cvPutText(frame, str, cvPoint(20, 100), &fontbkg, cvScalar(0, 0, 0));
		cvPutText(frame, str, cvPoint(20, 100), &font, cvScalar(255, 255, 255));

		if (frameHSV == NULL) frameHSV = cvCreateImage(cvGetSize(frameAux), 8, 3);
		//if (bin == NULL) bin = cvCreateImage(cvGetSize(frameHSV), 8, 1);

		if (gray == NULL) gray = cvCreateImage(cvGetSize(frameAux), 8, 1); // allocate a 1 channel byte image
		if (grayAux == NULL) grayAux = cvCreateImage(cvGetSize(frameAux), 8, 1); // allocate a 1 channel byte image
		if (grayAux1 == NULL) grayAux1 = cvCreateImage(cvGetSize(frameAux), 8, 1); // allocate a 1 channel byte image
		if (grayAux2 == NULL) grayAux2 = cvCreateImage(cvGetSize(frameAux), 8, 1); // allocate a 1 channel byte image
		if (grayBin == NULL) grayBin = cvCreateImage(cvGetSize(frameAux), 8, 1);
		if (grayBinAux == NULL) grayBinAux = cvCreateImage(cvGetSize(frameAux), 8, 1);
		if (BinBlob == NULL) BinBlob = cvCreateImage(cvGetSize(frameAux), 8, 1);

		

		vc_bgr_to_rgb(frame, frameAux);
		//vc_rgb_to_hsv(frameAux, frameHSV);
		vc_rgb_to_hsv(frameAux);

		vc_hsv_to_binary_in_range(frameAux, bin , 2);

		//vc_rgb_to_gray(frameAux, gray);
		///*cvDilate(gray, grayAux, NULL, 15);
		//cvErode(grayAux, grayAux1, NULL, 5);*/

		//vc_gray_to_binary(gray, grayBin, 65);

		//vc_binary_open(grayBin, grayBinAux, 15, 5);



		/*cvDilate(grayBin, grayBinAux, NULL, 15);
		cvErode(grayBinAux, grayBin, NULL, 5);*/

		blobs = vc_binary_blob_labellingOpencv(grayBinAux, BinBlob, &nblobs);

		vc_binary_blob_info(BinBlob, blobs, nblobs);

		if (blobs != NULL)
		{
			//printf("\nNumber of labels: %d\n", nblobs);
			for (i = 0; i < nblobs; i++)
			{
				int raio = (blobs[i].perimeter / (7));
				if (blobs[i].area > 50 && blobs[i].yc - raio > 100 && blobs[i].yc + raio < frame->height) 
				{
					CvPoint center = cvPoint(blobs[i].xc, blobs[i].yc);

					sprintf(str, "Area: %d", blobs[i].area);
					cvPutText(frame, str, cvPoint(blobs[i].xc, blobs[i].yc - 60), &fontbkg, cvScalar(0, 0, 0, 0));
					cvPutText(frame, str, cvPoint(blobs[i].xc, blobs[i].yc - 60), &font, cvScalar(255, 255, 255, 0));

					sprintf(str, "Perimetro: %d", blobs[i].perimeter);
					cvPutText(frame, str, cvPoint(blobs[i].xc, blobs[i].yc - 40), &fontbkg, cvScalar(0, 0, 0, 0));
					cvPutText(frame, str, cvPoint(blobs[i].xc, blobs[i].yc - 40), &font, cvScalar(255, 255, 255, 0));

					sprintf(str, "Calibre: %d", blobs[i].calibre);
					cvPutText(frame, str, cvPoint(blobs[i].xc, blobs[i].yc - 20), &fontbkg, cvScalar(0, 0, 0, 0));
					cvPutText(frame, str, cvPoint(blobs[i].xc, blobs[i].yc - 20), &font, cvScalar(255, 255, 255, 0));

					sprintf(str, "Diametro: %f", blobs[i].diametro);
					cvPutText(frame, str, cvPoint(blobs[i].xc, blobs[i].yc - 0), &fontbkg, cvScalar(0, 0, 0, 0));
					cvPutText(frame, str, cvPoint(blobs[i].xc, blobs[i].yc - 0), &font, cvScalar(255, 255, 255, 0));
					//cvCircle(frame, center, 3, CV_RGB(0, 0, 255), -1, 8, 0);
					//cvCircle(frame, center, raio, CV_RGB(0, 255, 0), 3, 8, 0);
				}
				vc_draw_boundingbox(frame, blobs[i]);
			}

			free(blobs);
		}

		//vc_rgb_to_hsv(frameAux, frameBin);
		//vc_binary_dilate(frameBin, frameBinAux, 5);

		/* Exibe a frame */
		cvShowImage("VC - TP2", bin);

		/* Sai da aplica��o, se o utilizador premir a tecla 'q' */
		key = cvWaitKey(1);
	}

	/* Fecha a janela */
	cvDestroyWindow("VC - TP2");
	//cvDestroyWindow("VC - TP2 - aux");

	/* Fecha o ficheiro de v�deo */
	cvReleaseCapture(&capture);

	return 0;
}