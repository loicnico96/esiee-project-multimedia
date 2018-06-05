#include "include/com.h"
#include "include/evalbot.h"
#include "include/images.h"

int main () {
	char comPortID[] = "USB0"; 	// Port COM utilisé
	char c; 
	int i; 

	// Création de l'objet Evalbot
	EVALBOT* ev = EvInit (comPortID); 
	EvClearInstructions (); 
	
	// Création du module graphique
	CVMODULE* cv = CvInit (); 
	if (cv == NULL) {
		printf ("Programme termine.\n"); 
		return EXIT_FAILURE; 
	}
	
	// Appuyer sur X pour arrêter le programme
	while (c != 'x') {
		// Mise à jour
		CvUpdate (ev); 
		c = cvWaitKey (5); 
		// Appuyer sur W pour activer ou désactiver l'affichage des coordonnées des formes détectées
		if (c == 'w')
			CvToggleCoordDisplay(); 
		// Appuyer sur C pour supprimmer le cache d'instructions actuelles de l'Evalbot, forçant l'Evalbot à recalculer sa trajectoire
		if (c == 'c') {
			printf ("Suppression du cache.\n"); 
			EvClearInstructions (); 
		}
		// Appuyer sur S pour mettre le programme en pause, arrêtant également l'Evalbot
		if (c == 's') {
			printf ("Programme en pause. Appuyer sur n'importe quelle touche pour reprendre...\n"); 
			EvCommand (EV_ACTION_IDLE); 
			c = cvWaitKey (0); 
			printf ("Reprise du programme.\n\n"); 
		} 
	} 
	
	// Arrêt de l'Evalbot
	EvCommand (EV_ACTION_IDLE); 
	
	// Destruction des ressources
	CvDelete (); 
	EvDelete (); 
	printf ("Programme termine.\n"); 
	return EXIT_SUCCESS; 
}
