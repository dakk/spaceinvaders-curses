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


/**
 * Funzione di gestione della spaceship guidata dall'utente
 */
void *space_ship_task(void *args)
{
	
	int alive = 1;
	char ch;
	object_data_t spaceship;
	
	// Variabili per salvare la distanza temporale tra uno sparo e l'altro
	struct timeval t1;
	struct timeval t2;
	long time_dif;
	
	// Inizializzo t1 con l'istante attuale
	gettimeofday(&t1, NULL);
	
	
	// Inizializzo la struttura da inviare al controllo, utilizzo
	// come base la struttura passata come argomento
	spaceship = *((object_data_t *) (args));
	spaceship.type = OT_SPACESHIP;
	spaceship.x = SPACE_SHIP_START_X;
	spaceship.y = SPACE_SHIP_START_Y;
	spaceship.size = SPACE_SHIP_SIZE;
	spaceship.life = SPACE_SHIP_LIFES;
	 
	// Inserisco nella coda di update i dati iniziali
	queue_add(spaceship);
	
	
	// Cicla finche la navicella e' in vita
	while(alive)
	{
		// Leggo lo stato delle collisioni
		object_type_t coll_type = get_collision_state(spaceship.id);
		
		if(coll_type == OT_DELETE)
		{
			alive = 0;
			continue;
		}


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
					int index;
					
					// Aggiorno il timer di partenza
					t1 = t2;
					
					// Genero il primo missile, quello che va a sinistra
					// (trovo una posizione libera nel buffer condiviso, ed assegno i dati,
					// dopodiche' avvio il thread)
					index = get_free_object_index();
					if(index >= 0)
					{
						objects[index].dir = LEFT;
						objects[index].x = spaceship.x + spaceship.size / 2;
						objects[index].y = spaceship.y - 1;
						pthread_create(&objects[index].thread, NULL, &missile_task, &objects[index]);	
					}

				
					// Genero il secondo missile, quello che va' a destra
					// (trovo una posizione libera nel buffer condiviso, ed assegno i dati,
					// dopodiche' avvio il thread)
					index = get_free_object_index();
					if(index >= 0)
					{
						objects[index].dir = RIGHT;
						objects[index].x = spaceship.x + spaceship.size / 2;
						objects[index].y = spaceship.y - 1;
						pthread_create(&objects[index].thread, NULL, &missile_task, &objects[index]);	
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
		
		// Inserisco la posizione attuale nella coda di update
		queue_add(spaceship);
	}
	
	// Termino il thread
	pthread_exit(NULL);
}
