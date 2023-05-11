#include <ncurses.h>
#include <locale.h>
#include <unistd.h>
#include <stdbool.h>
#include "grid.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include<pthread.h>
#include<signal.h>

#define GRID_HEIGHT 30
#define GRID_WIDTH 30

typedef struct {
	int i;
	int j;
	char ** foreground;
	char ** background;
} coords;

void do_nothing(int signo){
	// printf("Doing nothing ...\n");
}

void * updating_thread(void * args){
	coords * coordinates = (coords *) args;
	sigset_t mask;
	sigfillset(&mask);
	sigdelset(&mask,SIGUSR1);
	pthread_sigmask(SIG_BLOCK,&mask,NULL);
	// printf("Processed sigmask!\n");
	while (true){
		// printf("Before getting signal!\n");
		pause();
		// printf("After getting signal!");
		update_grid_by_cell(*(coordinates->foreground), *(coordinates->background) ,coordinates->i,coordinates->j);
	}

	// reading works great! 
	// printf("COORDINATES : i: %d, j: %d\n", coordinates->i,coordinates->j);
	return NULL;
}


int main()
{
	srand(time(NULL));
	setlocale(LC_CTYPE, "");
	initscr(); // Start curses mode

	char *foreground = create_grid();
	char *background = create_grid();
	char *tmp;

	init_grid(foreground);

	// signal(SIGUSR1, SIG_IGN);
	signal(SIGUSR1, do_nothing);

	// thread table of (one thread for each position): 
	pthread_t threads[GRID_HEIGHT*GRID_WIDTH];
	coords *local_coords_arr[GRID_HEIGHT*GRID_WIDTH];
	for (int k=0; k<GRID_HEIGHT;k++){
		for (int l=0; l<GRID_WIDTH;l++){
			coords *local_coords = malloc(sizeof *local_coords);
			local_coords->i = k;
			local_coords->j = l;
			local_coords->foreground = &foreground;
			local_coords->background = &background;
			local_coords_arr[k*GRID_WIDTH+l] = local_coords;
			pthread_create(&threads[k*GRID_WIDTH+l],NULL,updating_thread, local_coords);
		}
	}
	// sleep(3);

	while (true)
	{
		draw_grid(foreground);
		usleep(500 * 1000);

		// Step simulation
		// update_grid(foreground, background);

		for (int i = 0; i < GRID_HEIGHT; ++i)
    	{
        	for (int j = 0; j < GRID_WIDTH; ++j)
        	{
            	pthread_kill(threads[i*GRID_WIDTH+j], SIGUSR1);
			}
		}
		tmp = foreground;
		foreground = background;
		background = tmp;
	}

	endwin(); // End curses mode


	// free memory allocated for structs passed to threads's init functions 
	for (int k=0; k<GRID_HEIGHT;k++){
		for (int l=0; l<GRID_WIDTH;l++){
			free(local_coords_arr[k*GRID_WIDTH+l]);
		}
	}

	destroy_grid(foreground);
	destroy_grid(background);

	return 0;
}
