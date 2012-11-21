/**
 * Gestione dei punteggi
 * 
 * \file scores.c
 * \date 24 November 2011
 * \author Davide Gessa (45712)
 */
#include "scores.h"
#include <stdlib.h>
#include <stdio.h>


/**
 * Registra un nuovo punteggio
 * 
 * \param score Punteggio da aggiungere
 * \return Posizione in classifica (0 == high score)
 */
int scores_add(int score)
{
	int i = 0;
	int scores[SCORES_N_MAX] = {};
	
	scores_load(scores);
	
	// Scorre sino a trovare la posizione nella quale inserire
	while((scores[i] > score) && (i < SCORES_N_MAX)) i++;
	
	
	// Se i non ha superato il massimo
	if(i < SCORES_N_MAX)
	{
		// Scalo gli altri punteggi, eliminando l'ultimo
		int j;
		
		for(j = i + 1; j < SCORES_N_MAX - 1; j++)
			scores[i+1] = scores[i];
		
		// Inserisco alla posizione i il nuovo punteggio
		scores[i] = score;
	}
	
	scores_save(scores);
	
	return i;
}



/**
 * Carica i punteggi da file, e riempie l'array scores
 * 
 * \param scores Array nel quale salvare i punteggi letti
 * \return 0 se andato a buon fine
 */
int scores_load(int *scores)
{
	int i;
	FILE *f = fopen(SCORES_FILE, "r");
	
	if(f != NULL)
	{
		for(i = 0; i < SCORES_N_MAX; i++)
			fscanf(f, "%d", &scores[i]);
		
		fclose(f);
	}	
	else
	{
		f = fopen(SCORES_FILE, "w");
		
		if(f == NULL)
			return -1;
			
		for(i = 0; i < SCORES_N_MAX; i++)
		{
			fprintf(f, "%d\n", 0);
			scores[i] = 0;
		}
		
		fclose(f);
	}
	return 0;
}


/**
 * Salva i punteggi nel file, prendendo i dati dall'array scores
 * 
 * \param scores Array di punteggi da salvare nel file
 * \return 0 se andato a buon fine
 */
int scores_save(int *scores)
{
	int i;
	FILE *f = fopen(SCORES_FILE, "w");
	
	if(f != NULL)
	{
		for(i = 0; i < SCORES_N_MAX; i++)
			fprintf(f, "%d\n", scores[i]);
		
		fclose(f);
	}	
	else
		return -1;
	return 0;
}
