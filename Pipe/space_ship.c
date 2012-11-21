/**
 * Astronave
 * 
 * \file space_ship.c
 * \date 24 November 2011
 * \author Davide Gessa (45712)
 */
#include "space_ship.h"
#include "utility.h"
#include <time.h>
#include <stdio.h>


// Variabile globale che inidica se la navicella deve ciclare
int space_ship_alive = 1;


/** Handler della navicella per il segnale SIGQUIT */ 
void space_ship_end_handler(int signum)
{
	if(signum == SIGQUIT)
		space_ship_alive = 0;	
}


/**
 * Funzione di gestione della spaceship guidata dall'utente
 * 
 * \param pipe_out Pipe nella quale scrivere le informazioni della navicella
 */
void space_ship_task(int pipe_out)
{
	char ch;
	object_data_t spaceship;
	
	// Variabili per salvare la distanza temporale tra uno sparo e l'altro
	struct timeval t1;
	struct timeval t2;
	long time_dif;
	
	// Inizializzo t1 con l'istante attuale
	gettimeofday(&t1, NULL);
	
	// Registro il segnale SIGQUIT con l'handler apposito
	signal(SIGQUIT, space_ship_end_handler);	
	
	// Inizializzo la struttura da inviare al controllo
	spaceship.type = OT_SPACESHIP;
	spaceship.x = SPACE_SHIP_START_X;
	spaceship.y = SPACE_SHIP_START_Y;
	spaceship.size = SPACE_SHIP_SIZE;
	spaceship.life = SPACE_SHIP_LIFES;
	spaceship.pid = getpid();
	spaceship.pipes[0] = spaceship.pipes[1] = 0;
	 
	// Invia al controllo la posizione iniziale
	write(pipe_out, &spaceship, sizeof(object_data_t));
	
	
	// Cicla finche la navicella e' in vita
	while(space_ship_alive)
	{
		// Ricevo in input un azione del giocare
		switch(ch = getchar())
		{
			// Spacebar, sparo
			case KEYCODE_SPACE:
			{
				// Calcolo la differenza tra il momento attuale e quello
				// dello sparo precedente
				gettimeofday(&t2, NULL);
				time_dif = timevaldiff(&t1, &t2);
				
				// Se la differenza e' maggiore di un certo tot definito
				// dalla costante SPACE_SHIP_SHOOT_DISTANCE, allora possiamo
				// effettuare lo sparo				
				if(time_dif >= SPACE_SHIP_SHOOT_DISTANCE)
				{
					// Aggiorno il timer di partenza
					t1 = t2;
					
					// Genero il primo missile, quello che va a sinistra
					pid_t missile1 = fork();
					
					switch(missile1)
					{
						case -1:
							perror("Creazione processo missile non riuscita.");
							break;
							
						// Figlio, avvia la funzione di gestione missile
						case 0:
							missile_task(pipe_out, MD_LEFT, spaceship.x + spaceship.size / 2, spaceship.y - 1);
							exit(0);
					}
					
					
					// Genero il secondo missile, quello che va' a destra
					pid_t missile2 = fork();
					switch(missile2)
					{
						case -1:
							perror("Creazione processo missile non riuscita.");
							break;
							
						// Figlio, avvia la funzione di gestione missile
						case 0:
							missile_task(pipe_out, MD_RIGHT, spaceship.x + spaceship.size / 2, spaceship.y - 1);
							exit(0);
					}
				}
				break;
			}
			
			
			// Freccia sinistra, se possibile mi sposto
			case KEYCODE_LEFT:
			{
				if((spaceship.x - 1) >= 0)
					spaceship.x -= 1;
				
				break;
			}

			
			// Freccia destra, se possibile mi sposto
			case KEYCODE_RIGHT:
			{
				if((spaceship.x + 1) < (SCREEN_WIDTH - spaceship.size))
					spaceship.x += 1;
				
				break;
			}
		}
		
		// Invia al controllo la posizione attuale
		write(pipe_out, &spaceship, sizeof(object_data_t));
	}
	
	// Siamo fuori dal ciclo, chiudo la pipe ed esco
	close(pipe_out);
	exit(0);
}
