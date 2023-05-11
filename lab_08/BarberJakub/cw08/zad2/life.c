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
#include<string.h>

#define GRID_HEIGHT 30
#define GRID_WIDTH 30

typedef struct {
	int start_r;
	int start_c;
	int end_r;
	int end_c;
	char ** foreground;
	char ** background;
} coords;


bool is_num(char * cand){
	char  buff[] = "0123456789";
	size_t index = strspn(cand,buff);
	return (cand[0]!='0' && index == strlen(cand));
}

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
		
		for (int j=coordinates->start_c;j<GRID_WIDTH;j++){
			update_grid_by_cell(*(coordinates->foreground), *(coordinates->background) ,coordinates->start_r,j);
			if (coordinates->end_r==coordinates->start_r){
				if (j>=coordinates->end_c) break;
			}
		}
		
		for ( int i=coordinates->start_r+1;i<coordinates->end_r;i++){
			for (int j=0;j<GRID_WIDTH;j++){
				update_grid_by_cell(*(coordinates->foreground), *(coordinates->background) ,i,j);
			}
		}
		for (int j=0;j<=coordinates->end_c;j++){
			if (coordinates->end_r==coordinates->start_r){
				break;
			}
			update_grid_by_cell(*(coordinates->foreground), *(coordinates->background) ,coordinates->end_r,j);
		}


	}

	// reading works great! 
	// printf("COORDINATES : i: %d, j: %d\n", coordinates->i,coordinates->j);
	return NULL;
}


int main(int argc, char * argv[])
{
	// validating parameters
	if (argc!=2){
		printf("Incorrect number of parameters, one expected \n");
		return 1;
	}
	if (!is_num(argv[1])){
		printf("Incorect parameter type, integer number expected \n");
		return 1;
	}

	// getting thread count parameter as program parameter
	int thread_count = atoi(argv[1]);

	if (thread_count<=0){
		printf("Thread count must be as positive value\n");
		return 1;
	}

	if (thread_count>GRID_HEIGHT*GRID_WIDTH){
		printf("Thread count must be smaller or equal to all cells count [900 actual]\n");
		return 1;
	}

	int cells_per_thread = (GRID_HEIGHT*GRID_WIDTH) / thread_count;
	// printf("HEIGHT: %d, WIDTH: %d, cells_per_thread: %d\n", GRID_HEIGHT,GRID_WIDTH,cells_per_thread);
	// return 0;
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
	pthread_t threads[thread_count];
	coords *local_coords_arr[thread_count];
	for (int i=0;i<thread_count;i++){
		coords *local_coords = malloc(sizeof *local_coords);	
		local_coords->start_r = (i*cells_per_thread / GRID_WIDTH);
		local_coords->start_c = ((i*cells_per_thread )% GRID_WIDTH);
		local_coords->end_r = ((i+1)*cells_per_thread / GRID_WIDTH >= GRID_HEIGHT) ? GRID_HEIGHT-1 : (i+1)*cells_per_thread / GRID_WIDTH;
		local_coords->end_c = ((i+1)*cells_per_thread / GRID_WIDTH >= GRID_HEIGHT) ? GRID_WIDTH-1 : ((i+1)*cells_per_thread ) % GRID_WIDTH;
		local_coords->foreground = &foreground;
		local_coords->background = &background;
		local_coords_arr[i] = local_coords;
		// printf("Iteration: %d:\n\tstart_r: %d\n\tend_r: %d\n\tstart_c: %d\n\t end_c: %d\n",i,local_coords->start_r, local_coords->end_r, local_coords->start_c,local_coords->end_c);
		pthread_create(&threads[i],NULL,updating_thread, local_coords);
	}
	// printf("After loop!");
	// sleep(2);
	// for (int k=0; k<GRID_HEIGHT;k++){
	// 	for (int l=0; l<GRID_WIDTH;l++){
	// 		coords *local_coords = malloc(sizeof *local_coords);
	// 		local_coords->i = k;
	// 		local_coords->j = l;
	// 		local_coords->foreground = &foreground;
	// 		local_coords->background = &background;
	// 		local_coords_arr[k*GRID_WIDTH+l] = local_coords;
	// 		pthread_create(&threads[k*GRID_WIDTH+l],NULL,updating_thread, local_coords);
	// 	}
	// }
	// sleep(3);

	while (true)
	{
		draw_grid(foreground);
		usleep(500 * 1000);

		// Step simulation
		// update_grid(foreground, background);

		for (int i = 0; i < thread_count; ++i)
    	{
        	pthread_kill(threads[i], SIGUSR1);
		}	
		usleep(1000*2);
		tmp = foreground;
		foreground = background;
		background = tmp;
	}

	endwin(); // End curses mode


	// free memory allocated for structs passed to threads's init functions 
	for (int k=0; k<thread_count;k++){
		free(local_coords_arr[k]);		
	}

	destroy_grid(foreground);
	destroy_grid(background);

	return 0;
}
