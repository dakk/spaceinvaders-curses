/**
 * Definizioni globali del gioco
 * 
 * \file space_invaders.h
 * \date 24 November 2011
 * \author Davide Gessa (45712)
 */
#ifndef SPACE_INVADERS_H
#define SPACE_INVADERS_H


#include <stdio.h> 
#include <curses.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <signal.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <linux/ipc.h>
#include <linux/msg.h>


/* Info per le system V */
#define FTOK_PATH				"./spaceinvaders"		///< Ftoken path 
#define FTOK_PROJ_ID			1						///< Ftoken project

/** Ftoken project id di base utilizzato per le code dei messaggi per le collisioni
 *  degli oggetti */
#define FTOK_PROJ_ID_COLL_BASE	2						




/** Attivo i messaggi di debug */
//#define DEBUG	

/** Attiva i colori */
#define COLORS

/** Tempo di attesa tra le varie iterazioni */
#define SLEEP_UTIME				80000

/** Tempo di attesa tra le varie iterazioni delle bombe aliene */
#define SLEEP_BOMB_UTIME		100000

/** Tempo di attesa tra la perdita ed il messaggio di gameover */
#define SLEEP_OVER				1000000

/** Tempo di attesa per il lancio di una bomba (iterazioni) */
#define BOMB_DEPLOY_TIME		100


/** Dimensione del terminale */
#define SCREEN_DEFAULT_WIDTH	80
#define SCREEN_DEFAULT_HEIGHT	40
#define SCREEN_MAX_WIDTH		80
#define SCREEN_MAX_HEIGHT		40
#define SCREEN_MIN_WIDTH		20
#define SCREEN_MIN_HEIGHT		10
int		SCREEN_WIDTH;
int		SCREEN_HEIGHT;

 
/** Numero massimo di oggetti nella scena*/
#define OBJECTS_MAX				50

/** Numero delle navicelle aliene */
#define ALIEN_NUM				4


/**
 * Tipi di oggetto presenti nel gioco
 * 
 * OT_DELETE indica al controllo che l'oggetto sta' morendo, e deve essere
 * solo cancellato dallo schermo e non stampato
 */
typedef enum { OT_SPACESHIP, OT_ALIEN, OT_BOMB, OT_MISSILE, OT_DELETE } object_type_t;

///> Direzione
typedef enum { LEFT, RIGHT } direction_t;


///> Struttura contenente le informazioni relative all'oggetto
typedef struct
{
	long 			mtype;		///< Tipo di messaggio, usato dalle system V
	
	int 			x;			///< Posizione x dell'oggetto
	int				y;			///< Posizione y dell'oggetto
	int 			size;		///< Dimensione dell'oggetto (sia x che y)
	
	object_type_t	type;		///< Tipo di oggetto
	int				life;		///< Vita rimanente all'oggetto
	
	pid_t			pid;		///< Pid dell'oggetto
	int				id;			///< Id locale dell'oggetto (usato per creare la queue dei messaggi)
	
	direction_t		dir;		///< Direzione oggetto (usata per i marzianetti)
} object_data_t;


///> Struttura di un messaggio contenente le info di un avvenuta collisione
typedef struct
{
	long			mtype; 		///< Tipo di messaggio, usato dalle system V
	char			coll;		///< Tipo di collisione	
} collision_msg_t;

#include "control.h"
#include "space_ship.h"
#include "missile.h"
#include "alien.h"
#include "bomb.h"
#include "scores.h"

#endif
