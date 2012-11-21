/**
 * Missile
 * 
 * \file missile.h
 * \date 24 November 2011
 * \author Davide Gessa (45712)
 */
#ifndef MISSILE_H
#define MISSILE_H

#include "space_invaders.h"

///> Sprite del missile
#define MISSILE_DATA 	'o'
							
///> Direzione del missile
typedef enum { MD_LEFT = 0, MD_RIGHT = 1 } missile_dir_t;

					
/* Funzioni */											
void missile_task(int pipe_out, direction_t direction, int x, int y);
void missile_end_handler(int signum);

#endif
