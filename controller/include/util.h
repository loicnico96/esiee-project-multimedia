#include <math.h>

#ifndef UTIL_HEADER_INCLUDED
#define UTIL_HEADER_INCLUDED

#define PI				3.141592653

typedef struct {
	float	x; 
	float	y; 
} Point; 

float angleBetween (Point pos1, Point pos2); 

float distanceBetween (Point pos1, Point pos2); 

#endif
