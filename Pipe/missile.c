/**
 * Missile
 * 
 * \file missile.c
 * \date 24 November 2011
 * \author Davide Gessa (45712)
 */
#include "missile.h"


// Numero di nemici colpiti; se ne colpisce uno, continua ad andare,
// al secondo nemico colpito finisce la sua esistenza. Modifica apportata
// a seguito di una richiesta del docente durante l'orale
int hit;


/** Handler del missile per il segnale SIGQUIT */ 
void missile_end_handler(int signum)
{
	if(signum == SIGQUIT)
	{
		hit++;	
	}
}

/**
 * Funzione di gestione del missile lanciato dalla navicella
 * 
 * \param pipe_out Pipe nella quale scrivere le info dell'oggetto
 * \param direction Direzione del missile (destra/sinistra)
 * \param x Posizione iniziale x
 * \param y Posizione iniziale y
 */
void missile_task(int pipe_out, direction_t direction, int x, int y)
{	
	object_data_t missile;

	hit = 0;
	
	// Registro l'handler per il segnale di QUIT
	signal (SIGQUIT, missile_end_handler);	
	
	// Riempio la struttura con le info del missile
	missile.type = OT_MISSILE;
	missile.x = x;
	missile.y = y;
	missile.pid = getpid();
	missile.size = 1;
	missile.pipes[0] = missile.pipes[1] = 0;
	
	// Invia al controllo la posizione iniziale
	write(pipe_out, &missile, sizeof(object_data_t));
		
		
	// Eseguo sino a che l'oggetto non esce dallo schermo, o sino a che non riceve un segnale d'uscita
	while((! (missile.x < 0 || missile.y < 0 || missile.x > SCREEN_WIDTH || missile.y > SCREEN_HEIGHT)) && (hit < 2))
	{	
		// Faccio salire il missile di una posizione y			
		missile.y -= 1;
		
		// A seconda della direzione, mi sposto anche in orizzontale
		switch(direction)
		{
			case LEFT:
				missile.x -= 1;
				break;
				
			case RIGHT:
				missile.x += 1;
				break;
				
		}
	
		// Invia al controllo la posizione attuale
		if(hit < 2)
			write(pipe_out, &missile, sizeof(object_data_t));
		
		// Attende un tempo prefissato, prima di fare un altra iterazione
		usleep(SLEEP_UTIME);
	}
	
	// Siamo fuori dal ciclo, diciamo al controllo di cancellare il missile
	missile.type = OT_DELETE;
	write(pipe_out, &missile, sizeof(object_data_t));
	
	// Chiudiamo la pipe ed esco
	close(pipe_out);	
	exit(0);
}
