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

	
/* Funzioni */
void	control_task(int pipe_in);
int		control_check_collision(object_data_t *object, object_data_t *current);
void	control_set_collision(object_data_t *object, object_type_t type);

#endif
