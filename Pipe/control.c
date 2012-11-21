/**
 * Funzioni di controllo delle iterazioni tra gli oggetti e rendering
 * 
 * \file control.c
 * \date 24 November 2011
 * \author Davide Gessa (45712)
 */
#include "control.h"
#include "utility.h"


/**
 * Funzione di rendering e gestione delle iterazioni tra gli oggetti della scena
 * 
 * \param pipe_in Pipe dal quale leggere le info degli oggetti
 */
void control_task(int pipe_in)
{
	int obj_n = 1;			// Numero di oggetti nella scena
	int scores = 0;			// Punteggio
	char buffer[512];		// Buffer per stampare le stats
	int i;			
	unsigned long iterations = 0;	// Numero di iterazioni		
	int aliens = ALIEN_NUM;	
	
	// Buffer di ricezione
	object_data_t recv_data;
	
	// Informazioni degli oggetti; la navicella e' in posizione 0.
	// L'elemento corrente e' puntato dal puntatore current
	object_data_t objects[OBJECTS_MAX];
	
	// Oggetto corrente e suo id; spaceship = -1;
	object_data_t *current;
	int current_id;
	
	// Variabile che indica se il loop deve essere eseguito
	int playing = 1;
	
	// Variabile temporanea per lo stato delle collisioni dell'oggetto corrente
	int col;
	
	// Variabile per memorizzare il dato da inviare all'alieno per lo stato delle
	// collisioni
	char coll_data[1];
	
	// Inizializzo le ncurses
	initscr();
	
	
	// Inizializzo i colori delle ncurses
	#ifdef COLORS
		start_color();
		init_pair(1, COLOR_WHITE, COLOR_BLACK);
		attron(COLOR_PAIR(1));
	#endif
	
		
	// Non visualizzo i tasti premuti
	noecho();
	
	// Inizializzo il generatore random
	srand((int)time(0));
	
	// Nascondo il cursore
	curs_set(0); 
	
	
	// Inizializzo la navicella
	objects[0].type = OT_SPACESHIP;
	
	// Inizializzo gli oggetti come non utilizzati
	for(i = 0; i < OBJECTS_MAX; i++)
		objects[i].x = -1;
		
		
	// Finche' il gioco non finisce, eseguo
	do 
	{ 					
		// Ricevo i dati di un oggetto dalla pipe
		read(pipe_in, &recv_data, sizeof(object_data_t));
				
		
		// Se non e' la navicella, ricerco nella lista l'oggetto ricevuto
		current = NULL;
		
		if(recv_data.type != OT_SPACESHIP)
		{
			for(i = 1; i < OBJECTS_MAX; i++)
			{
				if(recv_data.pid == objects[i].pid)
				{
					current = &objects[i];
					current_id = i;
					i = OBJECTS_MAX;
				}
			}
			
			// Non l'ho trovato, cerco uno spazio libero
			if(current == NULL)
			{
				for(i = 1; i < OBJECTS_MAX; i++)
					if(objects[i].x == -1)
					{
						obj_n++;
						current = &objects[i];
						i = OBJECTS_MAX;
					}
			}
		}
		// Altrimenti la navicella sta' in posizione 0
		else
		{
			current = &objects[0];
			current_id = 0;
		}
		
		
		// Collision detect
		int collision_count = 0;
			
		// Se l'oggetto corrente ha una posizione inizializzata, faccio
		// il check delle collisioni
		if(current->x != -1)
		{
			for(i = 0; i < OBJECTS_MAX; i++)
			{
				// Un oggetto non collide con se stesso
				if(current_id == i)
					continue;
					
				// Nella posizione i non c'e' nessun oggetto
				if(objects[i].x == -1)
					continue;
					
					
				// Se una bomba colpisce un alieno, me ne frego
				if(((objects[i].type == OT_BOMB) && (current->type == OT_ALIEN)) ||
					((objects[i].type == OT_ALIEN) && (current->type == OT_BOMB)))
					continue;
					
				// Se due missili si colpiscono, me ne frego
				if(((objects[i].type == OT_MISSILE) && (current->type == OT_MISSILE)))
					continue;
					
					
				// Controllo lo stato delle collisioni tra l'oggetto corrente
				// e quello in posizione i
				col = control_check_collision(&objects[i], current);
				
				
				// Se c'e' stata una collisione
				if(col != 0)
				{					
					// Imposto il flag			
					collision_count = 1;
					
					// Se abilitato, stampo una stringa di debug
					#ifdef DEBUG
						char cinfo[1024];
						sprintf(cinfo, "Collision: type %d and type %d, x %d and x %d, y %d and y %d",
								(int) objects[i].type, (int) current->type, objects[i].x, current->x,
								objects[i].y, current->y);
						mvaddstr(1, 1, cinfo);
					#endif
					
					// A seconda del tipo di oggetto corrente, faccio un azione diversa
					// per ogni tipo di collisione
					switch(current->type)
					{
						// Se un missile colpisce un alieno, toglie una vita
						// o lo ammazza, ed incrementa il punteggio
						case OT_MISSILE:
							if(objects[i].type == OT_ALIEN)
							{
								coll_data[0] = 'c';
								write(objects[i].pipes[1], coll_data, 1);
								scores += objects[i].size * 10;
							}
							// Se una bomba ed un missile collidono, killo la bomba
							else if(objects[i].type == OT_BOMB)
								kill(current->pid, SIGQUIT);							
							
							// E killo il missile in ogni caso
							kill(current->pid, SIGQUIT);
							break;
							
						// Collisione alieno
						case OT_ALIEN:
							// Se due alieni collidono rimbalzano
							if(objects[i].type == OT_ALIEN)
							{
								coll_data[0] = 'r';
									
								// Se collidono e stanno andando entrambi
								// nella stessa dir, cambio direzione solo ad uno
								if(objects[i].dir == current->dir)
								{
									if(current->x < objects[i].x)
										write(current->pipes[1], coll_data, 1);
									else
										write(objects[i].pipes[1], coll_data, 1);
								}
								else
								{
									write(objects[i].pipes[1], coll_data, 1);	
									write(current->pipes[1], coll_data, 1);
								}						
							}
							// Se collide con un missile, invio il messaggio
							// all'alieno e killo il missile
							else if(objects[i].type == OT_MISSILE)
							{
								coll_data[0] = 'c';
								write(current->pipes[1], coll_data, 1);
								scores += current->size * 10;
								
								kill(objects[i].pid, SIGQUIT);
							}
							// Se un alieno collide con la navicella, fine gioco
							else if(objects[i].type == OT_SPACESHIP)
							{
								playing = 0;
								usleep(SLEEP_OVER);		
							}
							break;
													
								
						// Collisione bomba	
						case OT_BOMB:
							// Se la bomba collide con la navicella, game over
							if(objects[i].type == OT_SPACESHIP)
							{
								playing = 0;			
								usleep(SLEEP_OVER);				
							}
							// Se una bomba ed un missile collidono, le killo
							else if(objects[i].type == OT_MISSILE)
							{
								kill(objects[i].pid, SIGQUIT);
								kill(current->pid, SIGQUIT);							
							}
							
							break;
							
						// Collisione navicella	
						case OT_SPACESHIP:
							// Se la navicella collide con un missile o con una bomba, fine gioco
							if((objects[i].type == OT_BOMB) || (objects[i].type == OT_ALIEN))
							{
								playing = 0;		
								usleep(SLEEP_OVER);							
							}
							break;
							
					}
					
					
					// Aggiorno lo schermo
					refresh();
				}
			}
		}
		
		// Se la read e' bloccante, devo inviare un carattere per sbloccare
		// l'alieno
		#ifndef NONBLOCK
		if(collision_count == 0)
			if(current->type == OT_ALIEN)
				write(current->pipes[1], " ", 1);
		#endif
		
		// Qualsiasi oggetto sia, cancello prima di tutto la sua vecchia
		// porzione di schermo, ed inserisco nell'array degli oggetti i
		// dati appena ricevuti
		if(current->x != -1)
			clear_quad(current->x, current->y, current->size);
		
		*current = recv_data;
		
		
		// A seconda del tipo di oggetto, eseguo un operazione differente			
		switch(recv_data.type)
		{
			// Astronave aliena
			case OT_ALIEN:				
				// Se la vita di un alieno e' negativa, vuol dire che deve morire;
				// deve comunque inviare la struttura inizializzando come tipo
				// OT_DELETE
				if(current->life == -1)
				{				
					// Chiudo la pipe
					close(current->pipes[1]);
					
					aliens--;
					 
					continue;
				}
					
				// A seconda del livello, disegno l'alieno in modo diverso
				switch(current->size)
				{
					case ALIEN_DATA1_SIZE:
						render_string_array(current->x, current->y, ALIEN_DATA1, ALIEN_DATA1_SIZE);	
						break;
						
					case ALIEN_DATA2_SIZE:
						render_string_array(current->x, current->y, ALIEN_DATA2, ALIEN_DATA2_SIZE);	
						break;
						
					case ALIEN_DATA3_SIZE:
						render_string_array(current->x, current->y, ALIEN_DATA3, ALIEN_DATA3_SIZE);	
						break;
				}
				
				// Il nemico e' sceso troppo in basso, game over
				if(current->y > (SCREEN_HEIGHT - ALIEN_DATA2_SIZE))
				{
					playing = 0;
					usleep(SLEEP_OVER);		
				}
					
				break;
			
			
			// Navicella	
			case OT_SPACESHIP:
				render_string_array(current->x, current->y, SPACE_SHIP_DATA, SPACE_SHIP_SIZE);	

				break;
				
			
			// Bomba aliena	
			case OT_BOMB:			
				mvaddch(current->y, current->x, BOMB_DATA);
				break;
			
			
			// Missile dell'astronave	
			case OT_MISSILE:				
				mvaddch(current->y, current->x, MISSILE_DATA);

				break;	
			
			
			// Un oggetto sta' morendo e vuole esser cancellato dall'array	
			case OT_DELETE:		
				// Invalido la sua entry nell'array
				current->x = -1;
				
				// Decremento il numero di oggetti della scena
				obj_n--;
				
				// Attendo che muoia (per non aver zombie)
				waitpid(current->pid);
				break;
		};
		
				
		
		// Cancello la riga del punteggio
		mvaddstr(0, 0, "\t\t\t\t\t\t\t\t\t\t");
				
		// Visualizzo il punteggio attuale
		sprintf(buffer, "Scores: %d", scores);		
		mvaddstr(0, 1, buffer);
		
		
		// Visualizzo il numero di oggetti della scena
		sprintf(buffer, "Active Processes: %d", obj_n);		
		mvaddstr(0, 20, buffer);
			
		// Visualizzo il numero di alieni
		sprintf(buffer, "Aliens: %d", aliens);		
		mvaddstr(0, 50, buffer);
	
		
			
		// Rendo invisibile il cursore e faccio il refresh
		curs_set(0);
		refresh();
		
		// Aggiorno il numero di iterazioni
		iterations++;
	} while(playing && aliens > 0);
	
	// Fermo le ncurses
	#ifdef COLORS
		attroff(COLOR_PAIR(1));
	#endif
	endwin();
	
	// Se ci son processi rimasti, invio un segnale di kill per terminarli
	for (i = 0; i < OBJECTS_MAX; i++)
	{
		if(objects[i].x != -1)
		{
			// Se e' un alieno utilizzo la pipe delle collisioni
			// per dire di uccidersi
			if(objects[i].type == OT_ALIEN)
			{
				coll_data[0] = 'd';

				write(objects[i].pipes[1], coll_data, 1);				
			}
			else
			{
				kill(objects[i].pid, SIGQUIT);
			}
			
			// Attendo che il processo esca
			waitpid(objects[i].pid);
			obj_n--;
			objects[i].x = -1;
		}
	}
	
	
	// Aggiungo il punteggio alla lista degli highscore e visualizzo
	// un messaggio di fine gioco
	system("clear");
	
	int pos = scores_add(scores);
	
	if(aliens == 0)
		printf("Hai vinto!\n");
	else
		printf("Hai perso!\n");
		
	if(pos == 0)
		printf("Nuovo punteggio high score! %d\n", scores);
	else
		printf("Il tuo punteggio e' %d\n", scores);
	
	// Visualizzo la lista dei punteggi alti
	int scorl[SCORES_N_MAX];
	scores_load(scorl);	
	for(i = 0; i < SCORES_N_MAX; i++)
		printf("\t%d\t%d\n", i+1, scorl[i]);
	
}



/**
 * Funzione che controlla se current e' in collissione con l'oggetto object
 * 
 * \return 1 se vero
 */
int	control_check_collision(object_data_t *object, object_data_t *current)
{	
	if(((current->x >= object->x) && (current->x <= (object->x + object->size))) ||
		((current->x < object->x) && ((current->x + current->size) > object->x)))
	{
		if(((current->y >= object->y) && (current->y < (object->y + object->size))) ||
			((current->y < object->y) && ((current->y + current->size) > object->y)))
			return 1;					
	}
	return 0;	
}
