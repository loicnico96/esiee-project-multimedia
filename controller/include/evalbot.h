#ifndef EVALBOT_HEADER_INCLUDED
#define EVALBOT_HEADER_INCLUDED

#include "com.h"
#include "images.h"
#include "util.h"
#include "types.h"

#define EV_ACTION_IDLE			'0'
#define EV_ACTION_FORWARD		'8'
#define EV_ACTION_BACKWARD		'2'
#define EV_ACTION_LEFT			'4'
#define EV_ACTION_RIGHT			'6'
#define EV_ACTION_LEFT_FAST		'7'
#define EV_ACTION_RIGHT_FAST	'9'

#define EV_INSTR_MARGIN			3
#define EV_INSTR_X_MIN			60
#define EV_INSTR_Y_MIN			60

EVALBOT* EvInit (char* comPortID);

void EvDelete ();

void EvCommand (char action);

void EvClearInstructions ();

void EvAddInstruction (float x, float y, char action);

void EvUpdate (CVMODULE* cv);

#endif
