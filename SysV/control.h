/**
 * Funzioni di controllo delle iterazioni tra gli oggetti e rendering
 * 
 * \file control.h
 * \date 24 November 2011
 * \author Davide Gessa (45712)
 */
#ifndef CONTROL_H
#define CONTROL_H

#include "space_invaders.h"

///> Numero minimo di iterazioni tra 2 hit
#define MIN_HIT_DIFF	12

/* Funzioni */
void	control_task();
int		control_check_collision(object_data_t *object, object_data_t *current);


#endif
