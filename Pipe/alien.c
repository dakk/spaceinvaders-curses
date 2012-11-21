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
 * 
 * \param pipe_out Pipe nella quale scrivere le info dell'oggetto
 * \param level Livello della navicella
 * \param x Posizione iniziale x
 * \param y Posizione iniziale y
 * \param coll_pipes Pipe per la ricezione delle informazioni relative
 * 			alle collisioni
 */
void alien_task(int pipe_out, int level, int x, int y, int *coll_pipes, direction_t d)
{
	object_data_t alien;			
	
	// Numero di iterazioni
	int n;
	
	// Imposto i valori iniziali dell'oggetto
	alien.type = OT_ALIEN;
	alien.x = x;
	alien.y = y;
	alien.pid = getpid();
	alien.pipes[0] = coll_pipes[0];
	alien.pipes[1] = coll_pipes[1];
	alien.dir = d;
	
	// Movimenti effettuati nella dir corrente
	int movs = 0;
	
	// Iterazioni trascorse dall'ultima collisione. E' necessaria per fare
	// in modo che uno stesso missile non colpisca due volte lo stesso oggetto
	int coll = 0;
	
	// Rendo non bloccante la pipe per riceve lo stato delle collisioni
	#ifdef NONBLOCK
	fcntl(alien.pipes[0], F_SETFL, fcntl(alien.pipes[0], F_GETFL) | O_NONBLOCK);
	#endif
     
	// A seconda del livello dell'alieno, imposto il numero di vite e la
	// grandezza in blocchi
	switch(level)
	{
		case 1:
			alien.size = 3;
			alien.life = 2;
			break;
		case 2:
			alien.size = 4;
			alien.life = 3;
			break;
		case 3:
			alien.size = 5;
			alien.life = 4;
			break;
	}
	
	// Invia al controllo la posizione iniziale
	write(pipe_out, &alien, sizeof(object_data_t));
		
		
	// Finche la vita non raggiunge un numero negativo, ciclo
	while(alien.life != -1)
	{	
		
		// Leggo lo stato delle collisioni
		char data[1];
		if(read(alien.pipes[0], data, 1) == 1)
		{
			// Collisione con un missile, decremento le vite
			if((data[0] == 'c') && (coll >= 6))
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
			else if(data[0] == 'r')
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
			// Se riceve una d, deve uscire dal loop
			else if(data[0] == 'd')
			{
				alien.life = -1;
				break;
			}
		}
		
		
		
		// Se il numero di iterazioni e' uguale a quello necessario
		// per sganciare una bomba, la sgancio
		if(n == BOMB_DEPLOY_TIME)
		{
			pid_t bomb = fork();
			
			switch(bomb)
			{
				case -1:
					perror("Creazione processo bomba non riuscita.");
					break;
				
				// Sono il figlio, eseguo la funzione di gestione della bomba
				case 0:
					bomb_task(pipe_out, alien.x + alien.size / 2, alien.y + alien.size + 1);
					exit(0);
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
			
	
		// Invia al controllo la posizione attuale
		write(pipe_out, &alien, sizeof(object_data_t));
			
		usleep(SLEEP_UTIME);
	}
	
	// Diciamo al controllo di cancellare l'alieno
	alien.type = OT_DELETE;
	write(pipe_out, &alien, sizeof(object_data_t));
	
	// Chiudo le pipes ed esco
	close(pipe_out);
	close(alien.pipes[0]);
	exit(0);
}
