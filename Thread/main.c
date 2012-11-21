/**
 * Main del software
 * 
 * \file main.c
 * \date 24 November 2011
 * \author Davide Gessa (45712)
 */
#include "space_invaders.h"
 

int main(int argc, char **argv)
{
	int i;	
	
	// Imposto le dimensioni di default dello schermo
	SCREEN_WIDTH = SCREEN_DEFAULT_WIDTH;
	SCREEN_HEIGHT = SCREEN_DEFAULT_HEIGHT;
	
	
	// Inizializza la coda
	queue_init();
		
		
	// Se ci sono due argomenti nella shell, li parso come dimensioni
	// dello schermo (se son validi)
	if(argc == 3)
	{
		int h, w;
		
		w = atoi(argv[1]);
		h = atoi(argv[2]);
		
		if(w < SCREEN_MAX_WIDTH && w > SCREEN_MIN_WIDTH)
			SCREEN_WIDTH = w;
		if(h < SCREEN_MAX_HEIGHT && h > SCREEN_MIN_HEIGHT)
			SCREEN_HEIGHT = h;
	}
		
	
	
	// Inizializzo tutti gli oggetti della scena come non utilizzati
	for(i = 0; i < OBJECTS_MAX; i++)
		objects[i].id = -1;
		
			
	/* Creo il thread della navicella */
	objects[0].type = OT_SPACESHIP;
	objects[0].id = 0;
	
	pthread_create(&objects[0].thread, NULL, &space_ship_task, &objects[0]);	
	
	
	
	
	/* Creo gli alieni */
	for(i = 1; i <= ALIEN_NUM; i++)
	{
		objects[i].type = OT_ALIEN;
		objects[i].id = i;
		objects[i].x = (ALIEN_DATA3_SIZE + 2) * (i + 1);
		objects[i].y = 2;
		objects[i].dir = RIGHT;
		
		pthread_create(&objects[i].thread, NULL, &alien_task, &objects[i]);
	}
	
	
	// Richiama la funzione di gestione del gioco
	control_task();
	
	// Deinizializza la coda
	queue_exit();
	
	return 0;
}
