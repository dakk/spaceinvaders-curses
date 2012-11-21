/**
 * Bomba aliena
 * 
 * \file bomb.c
 * \date 24 November 2011
 * \author Davide Gessa (45712)
 */
#include "bomb.h"


/**
 * Funzione di gestione della bomba lanciata da un alieno
 */
void *bomb_task(void *args)
{
	object_data_t bomb;
	
	
	// Riempio la struttura con le info della bomba
	bomb = *((object_data_t *) (args));
	bomb.type = OT_BOMB;
	bomb.size = 1;
	
	
	// Invia al controllo la posizione iniziale
	queue_add(bomb);
	
	// Indica se la bomba e' in vita
	int alive = 1;
		
	// Ciclo finche la bomba non esce dallo schermo e finche non riceve un segnale di quit
	while((!(bomb.x < 0 || bomb.y < 0 || bomb.x > SCREEN_WIDTH || bomb.y > SCREEN_HEIGHT)) && alive)
	{
		// Leggo lo stato delle collisioni
		object_type_t coll_type = get_collision_state(bomb.id);
		
		if(coll_type == OT_DELETE)
			alive = 0;
			
		// La faccio scendere di una posizione			
		bomb.y += 1;
	
		// Invia al controllo la posizione attuale
		queue_add(bomb);
		
		// Attende un tempo prestabilito
		usleep(SLEEP_BOMB_UTIME);
	}
	
	// Diciamo al controllo di cancellare la bomba
	bomb.type = OT_DELETE;
	queue_add(bomb);
	
	
	// Termina il thread
	pthread_exit(NULL);
}
