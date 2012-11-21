/**
 * Funzioni varie utili nel programma
 * 
 * \file utility.c
 * \date 30 November 2011
 * \author Davide Gessa (45712)
 */
#include <time.h>
#include "utility.h"


/**
 * Calcolo la differenza in msec tra due timeval struct
 * 
 * \param t1 tempo iniziale
 * \param t2 tempo finale
 * \return differenza in msecondi
 */
long timevaldiff(struct timeval *t1, struct timeval *t2)
{
	long msec;
	msec = (t2->tv_sec - t1->tv_sec) * 1000;
	msec += (t2->tv_usec - t1->tv_usec) / 1000;
	return msec;
}



/** 
 * Renderizza un array di stringhe ad una posizione prefissata 
 *
 * \param x	Posizione x
 * \param y Posizione y
 * \param data Array di stringhe
 * \param size Numero di righe 
 */
void render_string_array(int x, int y, const char *data[], int size)
{
	int i;
	
	for(i = 0; i < size; i++)
		mvaddstr(y + i, x, data[i]);	
}

/** 
 * Pulisce una parte quadrata dello schermo, di dimensione size 
 * 
 * \param x Posizione x
 * \param y Posizione y
 * \param size Dimensione oggetto
 */
void clear_quad(int x, int y, int size)
{
	int i;
	int j;
	
	for(i = 0; i < size; i++)
		for(j = 0; j < size; j++)
			mvaddch(y+i, x+j, ' ');
}
