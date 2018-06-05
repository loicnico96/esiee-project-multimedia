#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>

#ifndef COM_HEADER_INCLUDED
#define COM_HEADER_INCLUDED

/* Ouverture et configuration du port COM */
int OpenPortHandle (int* comPortHandle, char* comPortID); 

/* Fermeture du port COM */
int ClosePortHandle (int* comPortHandle); 

/* Lecture depuis le port COM */ 
int ReadPort (int comPortHandle, char* buffer, int bufferSize); 

/* Ecriture dans un port COM */ 
int WritePort (int comPortHandle, char* buffer); 

#endif
