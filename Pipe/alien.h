/**
 * Astronave aliena
 * 
 * \file alien.h
 * \date 24 November 2011
 * \author Davide Gessa (45712)
 */
#ifndef ALIEN_H
#define ALIEN_H

#include "space_invaders.h"

///> Dimensione sprite alieno livello 1
#define ALIEN_DATA1_SIZE	3

///> Dimensione sprite alieno livello 2
#define ALIEN_DATA2_SIZE	4

///> Dimensione sprite alieno livello 3
#define ALIEN_DATA3_SIZE	5


///> Sprite alieno livello 1
static const char *ALIEN_DATA1[] = { " _ ", "/o\\", "\\_/" };

///> Sprite alieno livello 2
static const char *ALIEN_DATA2[] = { " __ ", "/  \\", "|  |", "\\__/" };

///> Sprite alieno livello 3
static const char *ALIEN_DATA3[] = { " ___ ", "/   \\", "|   |", "|   |", "\\___/" };
							
							
/* Funzioni */							
void alien_task(int pipe_out, int level, int x, int y, int *coll_pipes, direction_t d);


#endif
