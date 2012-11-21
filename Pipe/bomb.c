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
 * \param pipe_out Pipe nella quale scrivere le info dell'oggetto
 * \param x Posizione iniziale x
 * \param y Posizione iniziale y
 */
void bomb_task(int pipe_out, int x, int y)
{
	object_data_t bomb;
	
	
	// Registro l'handler per il segnale di QUIT
	signal (SIGQUIT, bomb_end_handler);
	
	
	// Riempio la struttura con le info della bomba
	bomb.type = OT_BOMB;
	bomb.x = x;
	bomb.y = y;
	bomb.pid = getpid();
	bomb.size = 1;
	bomb.pipes[0] = bomb.pipes[1] = 0;
	
	
	// Invia al controllo la posizione iniziale
	write(pipe_out, &bomb, sizeof(object_data_t));
		
		
	// Ciclo finche la bomba non esce dallo schermo e finche non riceve un segnale di quit
	while((!(bomb.x < 0 || bomb.y < 0 || bomb.x > SCREEN_WIDTH || bomb.y > SCREEN_HEIGHT)) && bomb_alive)
	{
		// La faccio scendere di una posizione			
		bomb.y += 1;
	
		// Invia al controllo la posizione attuale
		write(pipe_out, &bomb, sizeof(object_data_t));
		
		// Attende un tempo prestabilito
		usleep(SLEEP_BOMB_UTIME);
	}
	
	// Diciamo al controllo di cancellare la bomba
	bomb.type = OT_DELETE;
	write(pipe_out, &bomb, sizeof(object_data_t));
	
	// Chiudiamo la pipe ed usciamo
	close(pipe_out);
	exit(0);
}
