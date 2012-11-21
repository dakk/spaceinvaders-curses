/**
 * Gestione dei punteggi
 * 
 * \file scores.h
 * \date 24 November 2011
 * \author Davide Gessa (45712)
 */
#ifndef SCORES_H
#define SCORES_H


/** Numero massimo di high scores memorizzati */
#define SCORES_N_MAX		10

/** Nome del file contenente i punteggi */
#define SCORES_FILE			"scores.db"


int 	scores_add(int score);
int		scores_load(int *scores);
int		scores_save(int *scores);

#endif
