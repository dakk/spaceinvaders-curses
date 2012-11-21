/**
 * Bomba aliena
 * 
 * \file bomb.c
 * \date 24 November 2011
 * \author Davide Gessa (45712)
 */
#include "bomb.h"


// Variabile globale che inidica se la bomba deve ciclare
int bomb_alive = 1;


/** Handler della bomba per il segnale SIGQUIT */ 
void bomb_end_handler(int signum)
{
	if(signum == SIGQUIT)
		bomb_alive = 0;	
}

/**
 * Funzione di gestione della bomba lanciata da un alieno
 * 
 * \param x Posizione iniziale x
 * \param y Posizione iniziale y
 */
void bomb_task(int x, int y)
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
    
    
    
	object_data_t bomb;
	
	
	// Registro l'handler per il segnale di QUIT
	signal (SIGQUIT, bomb_end_handler);
	
	
	// Riempio la struttura con le info della bomba
	bomb.type = OT_BOMB;
	bomb.mtype = 1;
	bomb.x = x;
	bomb.y = y;
	bomb.pid = getpid();
	bomb.size = 1;
	
	
	// Invia al controllo la posizione iniziale
	msgsnd(qid, &bomb, sizeof(bomb) - sizeof(bomb.mtype), 0);
		
		
	// Ciclo finche la bomba non esce dallo schermo e finche non riceve un segnale di quit
	while((!(bomb.x < 0 || bomb.y < 0 || bomb.x > SCREEN_WIDTH || bomb.y > SCREEN_HEIGHT)) && bomb_alive)
	{
		// La faccio scendere di una posizione			
		bomb.y += 1;
	
		// Invia al controllo la posizione attuale
		msgsnd( qid, &bomb, sizeof(bomb) - sizeof(bomb.mtype), 0);
		
		// Attende un tempo prestabilito
		usleep(SLEEP_BOMB_UTIME);
	}
	
	// Diciamo al controllo di cancellare la bomba
	bomb.type = OT_DELETE;
	msgsnd(qid, &bomb, sizeof(bomb) - sizeof(bomb.mtype), 0);
	
	// Usciamo
	exit(0);
}
