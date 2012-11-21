/**
 * Astronave aliena
 * 
 * \file alien.c
 * \date 24 November 2011
 * \author Davide Gessa (45712)
 */
#include "alien.h"
#include "bomb.h"

/**
 * Funzione di gestione di una navicella aliena
 */
void *alien_task(void *args)
{
	object_data_t alien;			
	
	// Numero di iterazioni
	int n;
		
	// Movimenti effettuati nella dir corrente
	int movs = 0;
	
	// Iterazioni trascorse dall'ultima collisione. E' necessaria per fare
	// in modo che uno stesso missile non colpisca due volte lo stesso oggetto
	int coll = 0;
	
	// Imposto i valori di default  
	alien = *((object_data_t *) (args));  
	alien.size = 3;
	alien.life = 2;
	
	// Invia al controllo la posizione iniziale
	queue_add(alien);
	
	// Livello della navicella
	int level = 1;
		
		
	// Finche la vita non raggiunge un numero negativo, ciclo
	while(alien.life != -1)
	{	
		// Leggo lo stato delle collisioni
		object_type_t coll_type = get_collision_state(alien.id);
								
		// Collisione con un missile, decremento le vite
		if((coll_type == OT_MISSILE) && (coll >= 6))
		{
			coll = 0;
			alien.life--;
				
			// Se le vite son finite, aumento il livello
			if(alien.life == 0)
			{					
				level++;
				alien.dir = LEFT;
										
				// L'alieno deve morire, imposto falsa la condizione
				// di ciclo del loop
				if(level > 3)
				{
					alien.life = -1;
				}
					
				// LevelUP!
				else if(level == 2)
				{
					alien.size = 4;
					alien.life = 3;
					alien.x = 2;
					alien.y = 2;
				}
				else if(level == 3)
				{
					alien.size = 5;
					alien.life = 4;
					alien.x = 2;
					alien.y = 2;
				}
			}
		}
		// Collisione con un suo simile, cambio direzione
		else if(coll_type == OT_ALIEN)
		{
			switch(alien.dir)
			{
				case LEFT:
					if(movs > 5)
					{
						alien.dir = RIGHT;
						movs = 0;
						alien.x = (alien.x + 2) < SCREEN_WIDTH ? alien.x + 2 : alien.x;
					}
					break;
						
				case RIGHT:
					if(movs > 5)
					{
						alien.dir = LEFT;
						movs = 0;
						alien.x = alien.x <= 1 ? 0 : (alien.x - 2);
					}
					break;				
			}				
		}
		// E' stata richiesta la cancellazione dell'alieno
		else if(coll_type == OT_DELETE)
		{
			alien.life = -1;
			break;
		}
		
		
		
		// Se il numero di iterazioni e' uguale a quello necessario
		// per sganciare una bomba, la sgancio
		if(n == BOMB_DEPLOY_TIME)
		{
			int index = get_free_object_index();
			if(index >= 0)
			{
				objects[index].x = alien.x + alien.size / 2;
				objects[index].y = alien.y + alien.size + 1;
				pthread_create(&objects[index].thread, NULL, &bomb_task, &objects[index]);	
			}
					
			n = 0;
		}
		
		// Incremento il numero di iterazioni
		n++;
			
		// Aumento il numero di iterazioni trascorse dall'ultima collisione	
		coll++;
		
		
		// A seconda della direzione attuale, cambio la posizione;
		// se l'alieno ha raggiunto un bordo cambio direzione	
		switch(alien.dir)
		{
			case LEFT:
				if(alien.x <= 0)
				{
					if(movs > 3)
					{
						alien.dir = RIGHT;
						alien.y += alien.size;
						movs = 0;
					}
				}
				else
				{
					alien.x -= 1;
					movs ++;
				}
				break;
				
			
			case RIGHT:
				if(alien.x >= (SCREEN_WIDTH - alien.size))
				{
					if(movs > 3)
					{
						alien.dir = LEFT;
						alien.y += alien.size;
						movs = 0;
					}
				}
				else
				{
					alien.x += 1;
					movs ++;
				}
				break;				
		}
			
	
		// Inserisce nalla coda di update la posizione attuale
		queue_add(alien);
			
		usleep(SLEEP_UTIME);
	}
	
	// Diciamo al controllo di cancellare l'alieno
	alien.type = OT_DELETE;
	queue_add(alien);
	
	// Chiudo il thread
	pthread_exit(NULL);
}
