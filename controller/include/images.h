#ifndef IMAGES_HEADER_INCLUDED
#define IMAGES_HEADER_INCLUDED

#include "util.h"
#include "types.h"
#include "opencv/cv.h"
#include "opencv/highgui.h"
#include <stdio.h>

#define CV_MIN_CONTOUR_AREA		20
#define CV_COLOR_THRESHOLD		1.05

#define COLOR_WHITE			0xFFFFFF
#define COLOR_RED			0xFF0000
#define COLOR_GREEN			0x00FF00
#define COLOR_BLUE			0x0000FF
#define COLOR_BLACK			0x000000

// Initialisation du module graphique
CVMODULE* CvInit (); 

// Destruction du module graphique
void CvDelete (); 

// Mise à jour du module graphique
void CvUpdate (EVALBOT* ev); 

// Active ou désactive l'affichage des coordonnées des formes détectées
void CvToggleCoordDisplay(); 

// Fonction d'événèment de souris
void onMouseCallBack (int mouseEvent, int x, int y, int flags, void* data); 

// Obtient la valeur d'un pixel donné d'une image pour un canal donné
float getColorChannel (IplImage* image, int x, int y, int channel); 

// Convertit une couleur RGB en code couleur
int identifyColor (float r, float g, float b); 

// Calcule un code couleur en couleur RGB
CvScalar getColorFromID (int colorID); 

// Dessin d'une croix au point p
void drawCross (CvArr* image, CvPoint p, CvScalar color, int thickness, float magnitude); 

// Dessin d'une flèche entre p1 et p2
void drawArrow (CvArr* image, CvPoint p1, CvPoint p2, CvScalar color, int thickness, float magnitude);  

#endif
