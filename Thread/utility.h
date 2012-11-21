/**
 * Funzioni varie utili nel programma
 * 
 * \file utility.h
 * \date 30 November 2011
 * \author Davide Gessa (45712)
 */
#ifndef UTILITY_H
#define UTILITY_H

#include <time.h>
#include <sys/time.h>

long timevaldiff(struct timeval *starttime, struct timeval *finishtime);
void render_string_array(int x, int y, const char **data, int size);
void clear_quad(int x, int y, int size);

#endif
