/**
 * Contiene le funzioni condivise dagli oggetti
 * 
 * \file space_invaders.c
 * \date 24 Dicembre 2011
 * \author Davide Gessa (45712)
 */
#include "space_invaders.h"


///> Buffer
object_data_t		queue[QUEUE_SIZE];


///> Numero di posizioni vuote nel buffer
sem_t vuoti;

///> Numero di numeri presenti nel buffer
sem_t occupati;

///> Sincronizza gli accessi al buffer
sem_t mutex;

///> Indice per inserire
int inserisci = 0;

///> Indice per prelevare
int preleva = 0;



/** Inizializza la coda di update */
void queue_init()
{
	sem_init(&mutex, 0, 1);
	sem_init(&occupati, 0, 0);
	sem_init(&vuoti, 0, QUEUE_SIZE);	
}


/** Libera la coda di update */
void queue_exit()
{
	sem_destroy(&mutex);
	sem_destroy(&vuoti);
	sem_destroy(&occupati);	
}


/** Aggiunge un elemento alla coda di update (thread_safe) */
int	queue_add(object_data_t ob)
{
	sem_wait(&vuoti);
	sem_wait(&mutex);
				
	/* Inserisce l'elemento */
	queue[inserisci] = ob;
		
	/* Aggiorna inserisci */
	inserisci = (inserisci + 1) % QUEUE_SIZE;		
		
	sem_post(&mutex);
	sem_post(&occupati);		
		
	return 1;	
}


/** 
 * Prende il primo elemento dalla coda di update (thread_safe)
 * 
 * \param ob Oggetto da inserire
 * \return stato dell'inserimento
 */
int queue_get_first(object_data_t *ob)
{
	sem_wait(&occupati);
	sem_wait(&mutex);
		
	/* Preleva l'elemento */
	*ob = queue[preleva];
		
	/* Aggiorna preleva */
	preleva = (preleva + 1) % QUEUE_SIZE;	
		
		
	sem_post(&mutex);
	sem_post(&vuoti);	
		
	return 1;
}



/** Restituisce una posizione vuota nell'array degli oggett "objects" */
int get_free_object_index()
{
	int i;
	
	for(i = 1; i < OBJECTS_MAX; i++)
		if(objects[i].id == -1)
		{
			objects[i].id = i;
			return i;	
		}
			
	return -1;
}


/** 
 * Dato l'id di un oggetto, restituisce la variabile coll e la azzera (thread_safe)
 */
object_type_t get_collision_state(int id)
{
	if(id > OBJECTS_MAX)
		return OT_NULL;
		
	pthread_mutex_lock(&objects[id].coll_mutex);
	object_type_t coll_type = objects[id].coll;
	objects[id].coll = OT_NULL;
	pthread_mutex_unlock(&objects[id].coll_mutex);
	
	return coll_type;
}
