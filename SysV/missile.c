/**
 * Missile
 * 
 * \file missile.c
 * \date 24 November 2011
 * \author Davide Gessa (45712)
 */
#include "missile.h"

// Variabile globale che inidica se il missile deve ciclare
int missile_alive = 1;


/** Handler del missile per il segnale SIGQUIT */ 
void missile_end_handler(int signum)
{
	if(signum == SIGQUIT)
		missile_alive = 0;	
}

/**
 * Funzione di gestione del missile lanciato dalla navicella
 * 
 * \param direction Direzione del missile (destra/sinistra)
 * \param x Posizione iniziale x
 * \param y Posizione iniziale y
 */
void missile_task(direction_t direction, int x, int y)
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
    
    
    
    
	object_data_t missile;

	// Registro l'handler per il segnale di QUIT
	signal (SIGQUIT, missile_end_handler);	
	
	// Riempio la struttura con le info del missile
	missile.mtype = 1;
	missile.type = OT_MISSILE;
	missile.x = x;
	missile.y = y;
	missile.pid = getpid();
	missile.size = 1;
	
	// Invia al controllo la posizione iniziale
	msgsnd(qid, &missile, sizeof(missile) - sizeof(missile.mtype), 0);
		
		
	// Eseguo sino a che l'oggetto non esce dallo schermo, o sino a che non riceve un segnale d'uscita
	while((! (missile.x < 0 || missile.y < 0 || missile.x > SCREEN_WIDTH || missile.y > SCREEN_HEIGHT)) && missile_alive)
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
		if(missile_alive)	
			msgsnd(qid, &missile, sizeof(missile) - sizeof(missile.mtype), 0);

		
		// Attende un tempo prefissato, prima di fare un altra iterazione
		usleep(SLEEP_UTIME);
	}
	
	// Siamo fuori dal ciclo, diciamo al controllo di cancellare il missile
	missile.type = OT_DELETE;	
	msgsnd(qid, &missile, sizeof(missile) - sizeof(missile.mtype), 0);

	
	// Esco
	exit(0);
}
