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
	pid_t pid_space_ship;			// Pid navicella
	pid_t pid_alien;				// Pid alieno
	
	
	// Imposto le dimensioni di default dello schermo
	SCREEN_WIDTH = SCREEN_DEFAULT_WIDTH;
	SCREEN_HEIGHT = SCREEN_DEFAULT_HEIGHT;
	
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
		
		
	// Creo la navicella 
	pid_space_ship = fork();
	
	switch(pid_space_ship)
	{
		case -1:
			perror("Impossibile creare il processo Space Ship.");
			exit(1);
		
		// Sono il figlio, chiamo la funzione di gestione della navicella spaziale
		case 0:
			space_ship_task();
			exit(0);
	}
	
	
	
	// Creo gli alieni	
	for(i = 0; i < ALIEN_NUM; i++)
	{		
		pid_alien = fork();
	
		switch(pid_alien)
		{
			case -1:
				perror("Impossibile creare il processo Alien.");
				exit(1);
			
			// Sono il figlio, chiamo la funzione di gestione della navicella aliena
			case 0:
				alien_task(i, 1, (ALIEN_DATA3_SIZE + 2) * (i + 1), 2, RIGHT);
				exit(0);
		}
	}
	
	// Avvio il processo di controllo
	control_task();
	
	
	return 0;
}
