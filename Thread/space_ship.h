/**
 * Astronave
 * 
 * \file space_ship.h
 * \date 24 November 2011
 * \author Davide Gessa (45712)
 */
#ifndef SPACE_SHIP_H
#define SPACE_SHIP_H

#include "space_invaders.h"

#define KEYCODE_SPACE	' '			///< Space Bar
#define KEYCODE_LEFT	68 			///< Freccia sinsitra
#define KEYCODE_RIGHT	67 			///< Freccia destra


/** Posizione iniziale (x,y) e dimensione della navicella */
#define SPACE_SHIP_START_X	(SCREEN_WIDTH / 2)
#define SPACE_SHIP_START_Y	(SCREEN_HEIGHT - 5)
#define SPACE_SHIP_SIZE		5


///> Numero di volte che puo' esser colpita la navicella prima di morire
#define SPACE_SHIP_LIFES	1


///> Tempo di attesa tra uno sparo e l'altro
#define SPACE_SHIP_SHOOT_DISTANCE		900


///> Sprite della navicella
static const char *SPACE_SHIP_DATA[] = { "  |", " / \\", "/_|_\\", " ", " " };
							
							
/* Funzioni */							
void *space_ship_task(void *args);


#endif
