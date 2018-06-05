#include "include/util.h"

float angleBetween (Point pos1, Point pos2) {
	float dx = pos2.x - pos1.x; 
	float dy = pos2.y - pos1.y; 
	return atan2 (dy, dx) / PI * 180.f; 
}

float distanceBetween (Point pos1, Point pos2) {
	float dx = pos2.x - pos1.x; 
	float dy = pos2.y - pos1.y; 
	return sqrt (dx * dx + dy * dy); 
}
