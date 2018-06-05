#include "include/evalbot.h"
#include "include/images.h"
#include "/usr/include/opencv/highgui.h"

CVMODULE cvm; 

CVMODULE* CvInit () {
	printf ("Initialisation du module graphique...\n"); 
	
	// Création de l'espace de stockage
	cvm.shapeCount		= 0; 
	cvm.storage		= cvCreateMemStorage (0); 
	
	// Création et positionnement des fenetres
	cvNamedWindow ("Output", CV_WINDOW_AUTOSIZE); 
	cvNamedWindow ("Input", CV_WINDOW_AUTOSIZE); 
	
	// Positionnement des fenetres
	cvMoveWindow ("Input", 160, 160); 
	cvMoveWindow ("Output", 960, 160); 

	// Initialisation de la souris
	cvm.mouseDown = 0; 
	cvm.mousePosition.x = 0; 
	cvm.mousePosition.y = 0; 
	cvSetMouseCallback ("Input", onMouseCallBack, NULL); 
	cvSetMouseCallback ("Output", onMouseCallBack, NULL); 

	// Test de l'ouverture du flux vidéo
	printf ("Ouverture du flux video.\n"); 
	CvCapture* capture = cvCreateCameraCapture (CV_CAP_ANY); 
	if (capture == NULL) {
		printf ("Echec de l'ouverture du flux video.\n\n"); 
		return NULL; 
	}
	IplImage* image = cvQueryFrame (capture); 
	cvShowImage ("Input", image); 
	cvShowImage ("Output", image); 
	sleep (2); 
	printf ("Flux video ouvert avec succes.\n\n"); 
	cvReleaseCapture (&capture); 

	// Initialisation de la police de texte
	cvInitFont (&cvm.font, CV_FONT_HERSHEY_SIMPLEX, 0.5f, 0.5f, 0, 2, 8); 
	printf ("Module graphique initialisé avec succes.\n\n"); 
	return &cvm; 
}

void CvDelete () {
	printf ("Fermeture du module graphique.\n\n"); 
	// Destruction des ressources
	cvDestroyAllWindows (); 
	cvReleaseMemStorage (&cvm.storage); 
}

void CvUpdate (EVALBOT* ev) {
	int i, k; 
	int x, y; 
	cvm.shapeCount = 0; 
	char textBuf[100]; 
	
	// Ouverture du flux vidéo
	CvCapture* capture = cvCreateCameraCapture (CV_CAP_ANY); 
	if (capture == NULL) {
		printf ("Echec de l'ouverture du flux video.\n"); 
		return; 
	}

	// Récupération de l'image depuis le flux vidéo
	IplImage* imageIn 			= cvQueryFrame (capture); 
	if (imageIn == NULL) {
		printf ("Erreur de lecture du flux video.\n"); 
		return; 
	}

	// Création de l'image de sortie
	IplImage* imageOut			= cvCreateImage (cvSize (imageIn->width, imageIn->height), IPL_DEPTH_8U, 3); 
	cvCopy (imageIn, imageOut, NULL); 

	// Création de l'image tampon en noir et blanc
	IplImage* imageContours			= cvCreateImage (cvSize (imageIn->width, imageIn->height), IPL_DEPTH_8U, 1); 
	cvCvtColor (imageIn, imageContours, CV_BGR2GRAY); 
	cvSmooth (imageContours, imageContours, CV_GAUSSIAN, 3, 3, 0, 0); 
	cvThreshold (imageContours, imageContours, 110, 255, CV_THRESH_BINARY); 

	// Extraction des contours
	CvSeq* contours; 
	cvFindContours (imageContours, cvm.storage, &contours, sizeof (CvContour), CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE, cvPoint (0, 0)); 

	// Analyse de chaque forme
	while (contours) {
		// Extraction d'un polynôme approximatif de la forme
		CvSeq* contour = cvApproxPoly (contours, sizeof (CvContour), cvm.storage, CV_POLY_APPROX_DP, cvContourPerimeter (contours) * 0.02, 0); 
		
		// Elimination des formes trop petites ou trop grandes
		float shapeArea = fabs (cvContourArea (contour, CV_WHOLE_SEQ, 0)); 
		if ((shapeArea > 30) && (shapeArea < 6000)) {
			
			// Calcul du centre de la forme (moyenne des sommets)
			int shapePolyCount = contour->total; 
			float shapeCenterX = 0.f; 
			float shapeCenterY = 0.f; 
			for (i = 0 ; i < shapePolyCount ; i++) {
				CvPoint* pt = (CvPoint*) cvGetSeqElem (contour, i); 
				shapeCenterX += pt->x; 
				shapeCenterY += pt->y; 
			}
			shapeCenterX /= shapePolyCount; 
			shapeCenterY /= shapePolyCount; 
			
			// Extraction de la couleur de la forme
			float r = getColorChannel (imageIn, (int)shapeCenterX, (int)shapeCenterY, 2); 
			float g = getColorChannel (imageIn, (int)shapeCenterX, (int)shapeCenterY, 1) * 1.2; // Le vert est légèrement augmenté car plus difficilement détectable
			float b = getColorChannel (imageIn, (int)shapeCenterX, (int)shapeCenterY, 0); 
			int shapeColor = identifyColor (r, g, b); 
			CvScalar color = getColorFromID (shapeColor); 
		
			// Ajout de la forme au tableau
			cvm.shapes[cvm.shapeCount].center.x	= shapeCenterX; 
			cvm.shapes[cvm.shapeCount].center.y	= shapeCenterY; 
			cvm.shapes[cvm.shapeCount].color	= shapeColor; 
			cvm.shapes[cvm.shapeCount].area	= shapeArea; 
			cvm.shapeCount++; 
		
			// Affichage de la forme sur l'image Output
			for (i = 0 ; i < shapePolyCount ; i++) {
				CvPoint* pt1 = (CvPoint*) cvGetSeqElem (contour, i); 
				CvPoint* pt2 = (CvPoint*) cvGetSeqElem (contour, (i+1) % shapePolyCount); 
				cvLine (imageOut, *pt1, *pt2, color, 2, 8, 0); 
			}
		
			// Affichage du centre de la forme sur l'image Output
			drawCross (imageOut, cvPoint (shapeCenterX, shapeCenterY), CV_RGB (0, 0, 0), 1, 10); 
			
			// Affichage des coordonnées si activé
			if (cvm.coordDisplay) {
				sprintf (textBuf, "(%.f, %.f)", shapeCenterX, shapeArea); 
				cvPutText (imageOut, textBuf, cvPoint (shapeCenterX, shapeCenterY), &cvm.font, CV_RGB (0, 0, 0)); 
			}
		}
		// Prochaine forme
		contours = contours->h_next; 
	}
	
	// Mise à jour de la position et de la destination de l'evalbot
	EvUpdate (&cvm); 
	
	// Affichage de la région d'intérêt et de la zone de dépôt
	cvRectangle (imageOut, cvPoint(cvm.ROITopLeft.x, cvm.ROITopLeft.y), cvPoint(cvm.ROIBottomRight.x, cvm.ROIBottomRight.y), CV_RGB (0, 0, 0), 2, 8, 0); 
	drawCross (imageOut, cvPoint(cvm.ROIDestPoint.x, cvm.ROIDestPoint.y), CV_RGB (0, 0, 0), 2, 10); 

	// Affichage de la trajectoire actuelle de l'Evalbot
	CvScalar colorBlack = CV_RGB(0, 0, 0); 
	if (ev->instrIndex < ev->instrCount) {

		cvLine (imageOut, cvPoint (ev->posCenter.x, ev->posCenter.y), cvPoint (ev->instructions[ev->instrIndex].destination.x, ev->instructions[ev->instrIndex].destination.y), colorBlack, 2, 8, 0); 

		for (i = ev->instrIndex ; i < ev->instrCount-2 ; i++) {

			cvLine (imageOut, cvPoint (ev->instructions[i].destination.x, ev->instructions[i].destination.y), cvPoint (ev->instructions[i+1].destination.x, ev->instructions[i+1].destination.y), colorBlack, 2, 8, 0); 

		}

	}

	// Affichage de la direction de l'Evalbot
	float evAngle = angleBetween (ev->posBack, ev->posCenter) * PI / 180; 
	drawArrow (imageOut, cvPoint (ev->posCenter.x, ev->posCenter.y), cvPoint (ev->posCenter.x + 30 * cos (evAngle), ev->posCenter.y + 20 * sin (evAngle)), CV_RGB (255, 0, 0), 2, 10); 
	
	// Affichage des images
	cvShowImage ("Input", imageIn); 
	cvShowImage ("Output", imageOut); 
	
	// Destruction des ressources
	cvReleaseImage (&imageIn); 
	cvReleaseImage (&imageOut); 
	cvReleaseImage (&imageContours); 
	cvReleaseCapture (&capture); 
}

void CvToggleCoordDisplay() {
	cvm.coordDisplay = 1 - cvm.coordDisplay; 
}

void onMouseCallBack (int mouseEvent, int x, int y, int flags, void* data) {
	cvm.mousePosition.x = x; 
	cvm.mousePosition.y = y; 
	switch (mouseEvent) {
		case 0: // EVENT_MOUSEMOVE
			if (cvm.mouseDown == 1) {
				cvm.ROIBottomRight.x = std::max<float> (cvm.ROITopLeft.x, x); 
				cvm.ROIBottomRight.y = std::max<float> (cvm.ROITopLeft.y, y); 
			}
			break; 
		case 1: // EVENT_LBUTTONDOWN
			cvm.mouseDown = 1; 
			cvm.ROITopLeft.x = x; 
			cvm.ROITopLeft.y = y; 
			cvm.ROIBottomRight.x = x; 
			cvm.ROIBottomRight.y = y; 
			break; 
		case 2: // EVENT_RBUTTONDOWN
			cvm.ROIDestPoint.x = x; 
			cvm.ROIDestPoint.y = y; 
			printf ("Nouveau point de depot enregistre: (%.1f, %.1f).\n\n", cvm.ROIDestPoint.x, cvm.ROIDestPoint.y); 
			EvClearInstructions (); 
			break; 
		case 4: // EVENT_LBUTTONUP
			cvm.mouseDown = 0; 
			printf ("Nouvelle region d'interet enregistree: (%.1f, %.1f) (%.1f, %.1f).\n\n", cvm.ROITopLeft.x, cvm.ROITopLeft.y, cvm.ROIBottomRight.x, cvm.ROIBottomRight.y); 
			EvClearInstructions (); 
	}
}

float getColorChannel (IplImage* image, int x, int y, int channel) {
	return (float)((unsigned char)image->imageData[y * image->widthStep + x * image->nChannels + channel]); 
}

int identifyColor (float r, float g, float b) {
	if ((r/b) > CV_COLOR_THRESHOLD && (r/g) > CV_COLOR_THRESHOLD)
		return COLOR_RED; 
	if ((g/b) > CV_COLOR_THRESHOLD && (g/r) > CV_COLOR_THRESHOLD)
		return COLOR_GREEN; 
	if ((b/g) > CV_COLOR_THRESHOLD && (b/r) > CV_COLOR_THRESHOLD)
		return COLOR_BLUE; 
	if (r+g+b < 128*3)
		return COLOR_BLACK; 
	return COLOR_WHITE; 
}

CvScalar getColorFromID (int colorID) {
	switch (colorID) {
		case COLOR_WHITE:
			return CV_RGB (255, 255, 255); 
		case COLOR_RED: 
			return CV_RGB (255, 0, 0); 
		case COLOR_GREEN: 
			return CV_RGB (0, 255, 0); 
		case COLOR_BLUE: 
			return CV_RGB (0, 0, 255); 
		case COLOR_BLACK:
		default:
			return CV_RGB (0, 0, 0); 
	}
}

void drawCross (CvArr* image, CvPoint p, CvScalar color, int thickness, float magnitude) {
	cvLine (image, cvPoint (p.x + magnitude, p.y + magnitude), cvPoint (p.x - magnitude, p.y - magnitude), color, thickness, 8, 0); 
	cvLine (image, cvPoint (p.x + magnitude, p.y - magnitude), cvPoint (p.x - magnitude, p.y + magnitude), color, thickness, 8, 0); 
}

void drawArrow (CvArr* image, CvPoint p1, CvPoint p2, CvScalar color, int thickness, float magnitude) {
	cvLine (image, p1, p2, color, thickness, 8, 0); 
	float angle = atan2 ((float)(p1.y - p2.y), (float)(p1.x - p2.x)); 
	p1.x = (int)(p2.x + magnitude * cos (angle + PI / 4)); 
	p1.y = (int)(p2.y + magnitude * sin (angle + PI / 4)); 
	cvLine (image, p1, p2, color, thickness, 8, 0); 
	p1.x = (int)(p2.x + magnitude * cos (angle - PI / 4)); 
	p1.y = (int)(p2.y + magnitude * sin (angle - PI / 4)); 
	cvLine (image, p1, p2, color, thickness, 8, 0); 
}
