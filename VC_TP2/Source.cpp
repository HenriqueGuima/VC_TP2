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
	double hScale = 1.0;
	double vScale = 1.0;
	int lineWidth = 1;
	char str[500] = { 0 };
	IplImage *gray = NULL, *grayAux = NULL;
	IplImage *grayAux1 = NULL, *grayAux2 = NULL;
	// Outros
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
		/*IplImage* frameGray = cvCreateImage(cvGetSize(frame), 8, 1);
		IplImage* frameBin = cvCreateImage(cvGetSize(frame), 8, 1);
		IplImage* frameGauss = cvCreateImage(cvGetSize(frame), 8, 1);
		IplImage* frameSobel = cvCreateImage(cvGetSize(frame), 8, 1);*/

		cvCopy(frame, frameAux, NULL); // OR return img_src_cpy;

		vc_rgb_to_hsv_imgimg(frameAux, frame);

		/*vc_rgb_to_hsv_imgimg(frameAux, frameBin);
		vc_rgb_to_gray(frame, frameGray);
		vc_gray_gaussian_filter(frameGray, frameGauss, 5*5);*/
		//vc_gray_edge_sobel(frameGray, frameAux, 5);

		if (gray == NULL) gray = cvCreateImage(cvGetSize(frameAux), 8, 1); // allocate a 1 channel byte image
		if (grayAux == NULL) grayAux = cvCreateImage(cvGetSize(frameAux), 8, 1); // allocate a 1 channel byte image
		if (grayAux1 == NULL) grayAux1 = cvCreateImage(cvGetSize(frameAux), 8, 1); // allocate a 1 channel byte image
		if (grayAux2 == NULL) grayAux2 = cvCreateImage(cvGetSize(frameAux), 8, 1); // allocate a 1 channel byte image

		cvDilate(gray, grayAux, NULL, 15);
		cvErode(grayAux, grayAux1, NULL, 5);


		//blobs
		blobs = vc_binary_blob_labellingOpencv(grayAux1, grayAux2, &nblobs);

		vc_binary_blob_info(grayAux2, blobs, nblobs);

		if (blobs != NULL)
		{
			//printf("\nNumber of labels: %d\n", nblobs);
			for (i = 0; i < nblobs; i++)
			{
				int raio = (blobs[i].perimeter / (7));
				if (blobs[i].area > 50000 && blobs[i].yc - raio > 10 && blobs[i].yc + raio < frame->height) {
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
					cvCircle(frame, center, 3, CV_RGB(0, 0, 255), -1, 8, 0);
					cvCircle(frame, center, raio, CV_RGB(0, 255, 0), 3, 8, 0);
				}
			}

			free(blobs);
		}
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

		// Fa�a o seu c�digo aqui...

		/* Exibe a frame */
		//cvShowImage("VC - TP2 - aux", frameAux);
		cvShowImage("VC - TP2", frame);
		//cvShowImage("VC - TP2", frameGray);
		//cvShowImage("VC - TP2", frameSobel);

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