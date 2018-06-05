#include "include/com.h"
#include <string.h>

/* Ouverture et configuration du port COM */
int OpenPortHandle (int* comPortHandle, char* comPortID) {
	char comPortName[100]; 
	struct termios io; 
	
	// Ouverture du port COM
	sprintf (comPortName, "/dev/tty%s", comPortID); 
	printf ("Ouverture du port %s...\n", comPortName); 
	*comPortHandle = open (comPortName, O_RDWR | O_NOCTTY | O_NDELAY); 
	if ((*comPortHandle < 0) || !isatty(*comPortHandle)) {
		printf ("Echec de l'ouverture du port %s.\n", comPortName); 
		return -1;  
	}
	printf ("Port %s ouvert avec succes.\n", comPortName); 
	
	// Configuration du port COM
	printf ("Configuration du port %s...\n", comPortName); 
	if (tcgetattr (*comPortHandle, &io) != 0) {
		printf ("Erreur lors de la lecture de la configuration du port %s.\n", comPortName); 
		return -1; 
	}
	io.c_iflag &= ~(IGNBRK | BRKINT | ICRNL | INLCR | PARMRK | INPCK | ISTRIP | IXON);
	io.c_oflag = 0; 
	io.c_lflag &= ~(ECHO | ECHONL | ICANON | IEXTEN | ISIG);
	io.c_cflag &= ~(CSIZE | PARENB);	
	io.c_cflag |= (CS8 | CREAD); 
	io.c_cc[VMIN] = 1; 
	io.c_cc[VTIME] = 0; 
	if ((cfsetispeed (&io, B115200) < 0) || (cfsetospeed (&io, B115200) < 0) || (tcsetattr (*comPortHandle, TCSAFLUSH, &io) <= 0)) {
		printf ("Erreur lors de la configuration du port %s.\n", comPortName); 
		return -1; 
	}

	// Initialisation du port COM
	printf ("Configuration du port %s...\n", comPortName); 
	printf ("Port %s configure avec succes.\n", comPortName); 
	tcflush (*comPortHandle, TCIOFLUSH); 
	printf ("Port %s initialise avec succes.\n", comPortName); 
	return 0; 
}

/* Fermeture du port COM */
int ClosePortHandle (int* comPortHandle) {
	// Fermeture du port
	printf ("Fermeture du port.\n"); 
	if (close (*comPortHandle) < 0) {
		printf ("Echec de la fermeture du port.\n"); 
		return -1;  
	}
	printf ("Port ferme avec succes.\n"); 
	*comPortHandle = 0; 
	return 0; 
}

/* Lecture depuis le port COM */ 
int ReadPort (int comPortHandle, char* buffer, int bufferSize) {
	int bytesRead = read (comPortHandle, buffer, bufferSize); 
	buffer[bytesRead] = '\0'; 
	printf ("%d caracteres reçus avec succes: \"%s\".\n", bytesRead, buffer); 
	return 0; 
}

/* Ecriture dans un port COM */ 
int WritePort (int comPortHandle, char* buffer) {
	int bytesWritten = write (comPortHandle, buffer, strlen (buffer)); 
	printf ("%d caracteres envoyes avec succes: \"%s\".\n", bytesWritten, buffer); 
	return 0; 
}
