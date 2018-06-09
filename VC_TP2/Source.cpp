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
	IplImage *gray = NULL, *grayAux = NULL;
	IplImage *grayAux1 = NULL, *grayAux2 = NULL;
	IplImage *grayBin = NULL, *BinBlob = NULL, *grayBinAux = NULL, *bin = NULL;
	IplImage *frameHSVmp = NULL, *frameHSVmd = NULL, *frameHSVme = NULL, *frameHSVmeNeg = NULL;
	IplImage *dilate1 = NULL;
	OVC *blobsMP, *blobsMD, *blobsME;
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

		if (frameHSVmp == NULL) frameHSVmp = cvCreateImage(cvGetSize(frameAux), 8, 1);
		if (dilate1 == NULL) dilate1 = cvCreateImage(cvGetSize(frameAux), 8, 1);
		if (frameHSVmd == NULL) frameHSVmd = cvCreateImage(cvGetSize(frameAux), 8, 1);
		if (frameHSVme == NULL) frameHSVme = cvCreateImage(cvGetSize(frameAux), 8, 1);
		if (frameHSVmeNeg == NULL) frameHSVmeNeg = cvCreateImage(cvGetSize(frameAux), 8, 1);
		

		

		vc_bgr_to_rgb(frame, frameAux);

		//Procura por moedas de valor mais baixo (1 - 5 cent) AKA moedas pretas
		vc_rgb_to_hsv_mp(frameAux, frameHSVmp);
		cvDilate(frameHSVmp, dilate1, NULL, 10);
		cvErode(dilate1, frameHSVmp, NULL, 10);

		//Procura por moedas douradas (10 - 50 cent)
		vc_rgb_to_hsv_md(frameAux, frameHSVmd);

		//Procura por moedas de euro (1 - 2 €)
		vc_rgb_to_hsv_me(frameAux, frameHSVme);

		//Inverte para detetar melhor as moedas
		vc_gray_negative(frameHSVme, frameHSVmeNeg);

		///*cvDilate(gray, grayAux, NULL, 15);
		//cvErode(grayAux, grayAux1, NULL, 5);*/
		//vc_gray_to_binary(gray, grayBin, 65);
		//vc_binary_open(frameHSVmp, grayBinAux, 1, 1);
		/*cvDilate(grayBin, grayBinAux, NULL, 15);
		cvErode(grayBinAux, grayBin, NULL, 5);*/

		//Identificar blobs de cada tipo de moeda
		//Moedas pretas
		blobsMP = vc_binary_blob_labellingOpencv(frameHSVmp, BinBlob, &nblobs);

		vc_binary_blob_info(BinBlob, blobsMP, nblobs);

		if (blobsMP != NULL)
		{
			//printf("\nNumber of labels: %d\n", nblobs);
			for (i = 0; i < nblobs; i++)
			{
				if (blobsMP[i].area < 100)
				{
					//Percorre para fora dos limites da área em questão e elimina as blobs pequenas dos dados brancos
					for (int y = blobsMP[i].y; y < blobsMP[i].y + blobsMP[i].height; y++)
					{
						for (int x = blobsMP[i].x; x < blobsMP[i].x + blobsMP[i].width; x++)
						{
							//Muda as cores das blobs pequenas para preto, eliminando-as
							long int pos = y * frameHSVmp->nChannels + x * frameHSVmp->nChannels;
							frameHSVmp->imageData[pos] = 0;
						}
					}
				}
			}

			free(blobsMP);
		}

		/* Exibe a frame */
		cvShowImage("VC - TP2", frame);

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