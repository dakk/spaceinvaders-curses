/**
 * Missile
 * 
 * \file missile.c
 * \date 24 November 2011
 * \author Davide Gessa (45712)
 */
#include "missile.h"


/**
 * Funzione di gestione del missile lanciato dalla navicella
 */
void *missile_task(void *args)
{	
	object_data_t missile;

	// Riempio la struttura con le info del missile
	missile = *((object_data_t *) (args));
	missile.type = OT_MISSILE;
	missile.size = 1;
	
	// Invia al controllo la posizione iniziale
	queue_add(missile);
	
	// Indica se il missile e' in vita	
	int alive = 1;
		
	// Eseguo sino a che l'oggetto non esce dallo schermo, o sino a che non riceve un segnale d'uscita
	while((! (missile.x < 0 || missile.y < 0 || missile.x > SCREEN_WIDTH || missile.y > SCREEN_HEIGHT)) && alive)
	{	
		// Leggo lo stato delle collisioni
		object_type_t coll_type = get_collision_state(missile.id);
		
		if(coll_type == OT_DELETE)
			alive = 0;
		
		// Faccio salire il missile di una posizione y			
		missile.y -= 1;
		
		// A seconda della direzione, mi sposto anche in orizzontale
		switch(missile.dir)
		{
			case LEFT:
				missile.x -= 1;
				break;
				
			case RIGHT:
				missile.x += 1;
				break;
				
		}
	
		// Invia al controllo la posizione attuale
		queue_add(missile);
		
		// Attende un tempo prefissato, prima di fare un altra iterazione
		usleep(SLEEP_UTIME);
	}
	
	// Siamo fuori dal ciclo, diciamo al controllo di cancellare il missile
	missile.type = OT_DELETE;
	queue_add(missile);
	
	// Termino il thread
	pthread_exit(NULL);
}
