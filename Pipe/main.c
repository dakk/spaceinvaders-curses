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
	int filedes[2];					// Descrittori della pipe
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
		
	
	// Crea la pipe per ricevere le posizioni degli oggetti
	if(pipe(filedes) == -1) 
	{ 
		perror("Errore nella creazione della pipe.");
		exit(1);
	} 	
	
	
	// Creo la navicella 
	pid_space_ship = fork();
	
	switch(pid_space_ship)
	{
		case -1:
			perror("Impossibile creare il processo Space Ship.");
			exit(1);
		
		// Sono il figlio, chiudo il canale di lettura e chiamo la funzione di gestione
		// della navicella spaziale
		case 0:
			close(filedes[0]);
			space_ship_task(filedes[1]);
			exit(0);
	}
	
	
	
	// Creo gli alieni
	int pipes_coll[2];
	
	for(i = 0; i < ALIEN_NUM; i++)
	{
		// Creo le pipe per inviare le info sulle collisioni
		pipe(pipes_coll);
		
		pid_alien = fork();
	
		switch(pid_alien)
		{
			case -1:
				perror("Impossibile creare il processo Alien.");
				exit(1);
			
			// Sono il figlio, chiudo il canale di lettura della pipe per le posizioni,
			// ed il canale di scrittura della pipe per le collisioni e chiamo la funzione di gestione
			// della navicella aliena
			case 0:
				close(filedes[0]);
				close(pipes_coll[1]);

				alien_task(filedes[1], 1, (ALIEN_DATA3_SIZE + 2) * (i + 1), 2, pipes_coll, RIGHT);
				exit(0);
		}
		
		// Chiudo il lato di lettura della pipe per le collisioni del marziano
		close(pipes_coll[0]);
	}
	
	// Chiudo il canale di lettura ed avvio il processo di controllo
	close(filedes[1]);
	control_task(filedes[0]);
	
	
	return 0;
}
