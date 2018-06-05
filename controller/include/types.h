#ifndef types_HEADER_INCLUDED
#define types_HEADER_INCLUDED

#include "util.h"
#include "opencv/cv.h"
#include "opencv/highgui.h"

#define EV_INSTR_MAX		100
#define CV_MAX_SHAPES		10000

typedef struct {
	Point			destination; 
	char			action; 
} Instruction; 



typedef struct {
	// Position de l'Evalbot
	Point			posCenter; 
	// Position de l'arrière de l'Evalbot (utilisée pour calculer la direction de l'Evalbot)
	Point			posBack; 

	// Instructions de l'Evalbot
	Instruction		instructions [EV_INSTR_MAX]; 
	int			instrCount; 
	int			instrIndex; 
	char			action; 

	// Port COM relié à l'Evalbot
	int			portCom; 
} EVALBOT; 



typedef struct {
	Point			center; 
	int			color; 
	float			area; 
} Shape; 



typedef struct {
	// Espace de stockage
	CvMemStorage*		storage; 

	// Police de texte
	CvFont			font; 

	// Tableau des formes detectées
	Shape			shapes [CV_MAX_SHAPES]; 
	int			shapeCount; 

	// Zone d'intérêt de l'image: Coins du rectangle où se trouvent les objets à collecter
	Point			ROITopLeft; 
	Point			ROIBottomRight; 
	// Zone d'intérêt de l'image: Point de dépôt des objets collectés
	Point			ROIDestPoint; 

	// Etat de la souris
	Point			mousePosition; 
	int			mouseDown; 

	// Affichage des coordonnées activé?
	int			coordDisplay; 
} CVMODULE; 

#endif
