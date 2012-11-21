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
 * \param level Livello della navicella
 * \param x Posizione iniziale x
 * \param y Posizione iniziale y
 * \param id Project id per la coda delle collisioni
 */
void alien_task(int id, int level, int x, int y, direction_t d)
{
	/* System V: inizializzazione */
	// Queue id per i messaggi
	int    qid;			
	
	// Chiave del progetto per individuare la coda dei messaggi
    key_t  msgkey;
    
    // Genero la chiave ipc    
    msgkey = ftok(FTOK_PATH, FTOK_PROJ_ID);    
    
    // Apro (o creo) la coda
    if((qid = msgget(msgkey, 0666 | IPC_CREAT)) == -1)
    {
		perror("Impossibile creare la coda");
		exit(1);
    }


	/* System V: inizializzazione della coda per ricevere le collisioni */
	// Queue id per i messaggi
	int    qid_coll;			
	
	// Chiave del progetto per individuare la coda dei messaggi
    key_t  msgkey_coll;
    
    // Genero la chiave ipc    
    msgkey_coll = ftok(FTOK_PATH, FTOK_PROJ_ID_COLL_BASE + id);    
    
    // Apro (o creo) la coda
    if((qid_coll = msgget(msgkey_coll, 0666 | IPC_CREAT)) == -1)
    {
		perror("Impossibile creare la coda");
		exit(1);
    }        
	
	// Struttura per l'aggiornamento dei dati dell'oggetto
	object_data_t alien;			
	
	// Numero di iterazioni
	int n;
	
	// Imposto i valori iniziali dell'oggetto
	alien.mtype = 1;					// Tipo di messaggio da inviare
	alien.type = OT_ALIEN;
	alien.x = x;
	alien.y = y;
	alien.pid = getpid();
	alien.dir = d;
	alien.id = id;
	
	// Movimenti effettuati nella dir corrente
	int movs = 0;
	
	// Iterazioni trascorse dall'ultima collisione. E' necessaria per fare
	// in modo che uno stesso missile non colpisca due volte lo stesso oggetto
	int coll = 0;
	
     
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
	msgsnd(qid, &alien, sizeof(alien) - sizeof(alien.mtype), 0);
		
		
	// Finche la vita non raggiunge un numero negativo, ciclo
	while(alien.life != -1)
	{	
		
		// Leggo lo stato delle collisioni
		collision_msg_t msg;
		
		if(msgrcv(qid_coll, &msg, sizeof(msg) - sizeof(msg.mtype), 1, IPC_NOWAIT) != -1)
		{
			// Se solo al primo livello e vengo colpito due volte di seguito, cancello l'alieno
			if((msg.coll == 'z') && (alien.size == ALIEN_DATA1_SIZE))
			{
				alien.life = -1;
				break;
			}
			// Collisione con un missile, decremento le vite
			else if((msg.coll == 'c') && (coll >= 6))
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
			else if(msg.coll == 'r')
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
			else if(msg.coll == 'd')
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
					bomb_task(alien.x + alien.size / 2, alien.y + alien.size + 1);
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
		msgsnd(qid, &alien, sizeof(alien) - sizeof(alien.mtype), 0);
		
			
		usleep(SLEEP_UTIME);
	}
	
	// Diciamo al controllo di cancellare l'alieno
	alien.type = OT_DELETE;
	msgsnd(qid, &alien, sizeof(alien) - sizeof(alien.mtype), 0);
	
	// Esco
	exit(0);
}
