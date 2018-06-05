#include "include/evalbot.h"

EVALBOT ev; 

EVALBOT* EvInit (char* comPortID) {
	OpenPortHandle (&ev.portCom, comPortID); 
	ev.posBack.x		= 0; 
	ev.posBack.y		= 0; 
	ev.posCenter.x		= 0; 
	ev.posCenter.y		= 0; 
	return &ev; 
}

void EvDelete () {
	ClosePortHandle (&ev.portCom); 
}

void EvCommand (char action) {
	char str[2] = { action, '\0' }; 
	WritePort (ev.portCom, str); 
	ev.action = action; 
}

void EvClearInstructions () {
	ev.instrCount = 0; 
	ev.instrIndex = 0; 
	EvCommand (EV_ACTION_IDLE); 
}

void EvAddInstruction (float x, float y, char action) {
	ev.instructions[ev.instrCount].destination.x	= x; 
	ev.instructions[ev.instrCount].destination.y	= y; 
	ev.instructions[ev.instrCount].action		= action; 
	ev.instrCount++; 
}

void EvUpdate (CVMODULE* cv) {
	int i; 
	Shape shape; 
	// Analyse des formes pour calcul de la position de l'Evalbot: 
	for (i = 0 ; i < cv->shapeCount ; i++) {
		shape = cv->shapes[i]; 
		if ((shape.color == COLOR_BLUE) && (shape.area > 600)) {
			ev.posCenter.x = shape.center.x; 
			ev.posCenter.y = shape.center.y; 
			break; 
		}
	}
	
	// Analyse des formes pour calcul de la direction de l'Evalbot: 
	for (i = 0 ; i < cv->shapeCount ; i++) {
		shape = cv->shapes[i]; 
		if ((shape.color != COLOR_BLUE) && (distanceBetween (ev.posCenter, shape.center) < 20)) {
			ev.posBack.x = shape.center.x; 
			ev.posBack.y = shape.center.y; 
			break; 
		}
	}
	
	// Affichage de la position
	printf ("Evalbot en position (%.1f, %.1f)...\n", ev.posCenter.x, ev.posCenter.y); 
	
	// Instruction en cours
	if (ev.instrIndex < ev.instrCount) {
		Instruction instruction = ev.instructions[ev.instrIndex]; 
		float distance		= distanceBetween (ev.posCenter, instruction.destination); 
		float angleDest		= angleBetween (ev.posCenter, instruction.destination); 
		float angleEv		= angleBetween (ev.posBack, ev.posCenter); 
		float angleDiff		= angleDest - angleEv; 
		float angleDiffMax	= 4; 
		if (ev.action == EV_ACTION_FORWARD || ev.action == EV_ACTION_BACKWARD)
			angleDiffMax	= 12; 
		printf ("Destination en position (%.1f, %.1f)... (distance de %.1f)\n", instruction.destination.x, instruction.destination.y, distance); 
		printf ("Evalbot en direction %.1f...\nDestination en direction %.1f... (difference de %.1f, maximum de %.1f).\n\n", angleEv, angleDest, angleDiff, angleDiffMax); 
		
		// L'Evalbot est arrivé à la destination de l'instruction actuelle
		if (distance < EV_INSTR_MARGIN) {
			// On passe à l'instruction suivante
			ev.instrIndex++; 
			EvUpdate (cv); 
			
		// L'Evalbot doit continuer à se diriger vers sa destination actuelle
		} else {
			// L'angle est inversé si on doit se déplacer en marche arrière
			if (instruction.action == EV_ACTION_BACKWARD)
				angleDiff += 180; 
			// On assure que l'angle est compris entre -180° et +180°
			while (angleDiff < -180)
				angleDiff += 360; 
			while (angleDiff > 180)
				angleDiff -= 360; 
			
			// L'Evalbot est dans la bonne direction
			if (abs (angleDiff) < angleDiffMax) {
				// L'Evalbot doit avancer vers sa destination
				EvCommand (instruction.action); 
				
			// L'Evalbot doit tourner vers sa destination
			} else if (abs (angleDiff) > 30) {
				if (angleDiff > 0) {
					EvCommand (EV_ACTION_RIGHT_FAST); 
				} else {
					EvCommand (EV_ACTION_LEFT_FAST); 
				}
			} else if (angleDiff > 0) {
				EvCommand (EV_ACTION_RIGHT); 
			} else {
				EvCommand (EV_ACTION_LEFT); 
			}
		}
		
	// Plus d'instruction à effectuer, plus besoin d'avancer
	} else {
		printf ("En attente de nouvelles instructions...\n"); 
		EvClearInstructions (); 
		
		// Analyse des formes pour trouver un objet à déplacer
		for (i = 0 ; i < cv->shapeCount ; i++) {
			shape = cv->shapes[i]; 
			// Vérification des critères: Couleur, taille modérée, dans la zone d'intérêt, pas l'arrière de l'Evalbot
			if (((shape.color == COLOR_BLUE) || (shape.color == COLOR_GREEN) || (shape.color == COLOR_RED)) && (shape.area > 30) && (shape.area < 300) && (shape.center.y > cv->ROITopLeft.y) && (shape.center.y < cv->ROIBottomRight.y) && (shape.center.x > cv->ROITopLeft.x) && (shape.center.x < cv->ROIBottomRight.x) && !((shape.center.x == ev.posBack.x) && (shape.center.y == ev.posBack.y))) {
				printf ("Objet detecte en position (%.1f, %.1f)...\n", shape.center.x, shape.center.y); 
				// Calcul de la trajectoire vers l'objet
				float direction1 = angleBetween (ev.posCenter, shape.center) * PI / 180; 
				Point point1 = { shape.center.x + 10 * cos (direction1), shape.center.y + 10 * sin (direction1) }; 
				EvAddInstruction (shape.center.x, shape.center.y, EV_ACTION_FORWARD); 
				EvAddInstruction (point1.x, point1.y, EV_ACTION_FORWARD); 
				// Calcul de la trajectoire vers le point de dépôt
				float direction2 = angleBetween (point1, cv->ROIDestPoint) * PI / 180; 
				Point point2 = { cv->ROIDestPoint.x - 10 * cos (direction2), cv->ROIDestPoint.y - 10 * sin (direction2) }; 
				EvAddInstruction (point2.x, point2.y, EV_ACTION_FORWARD); 
				// Léger recul permettant de ne pas déplacer l'objet actuel en allant chercher le suivant
				Point point3 = { cv->ROIDestPoint.x - 30 * cos (direction2), cv->ROIDestPoint.y - 30 * sin (direction2) };
				EvAddInstruction (point3.x, point3.y, EV_ACTION_BACKWARD); 
				break; 
			}
		}
	}
}
