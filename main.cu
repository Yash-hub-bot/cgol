#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <SDL2/SDL.h>
#include <stdbool.h>
#include <time.h>
#include <cuds_runtime.h>

#define HB 1000
#define VB 1000
#define GT 5
#define MAX(a, b) ((a) > (b) ? (a) : (b))



bool grid[HB][VB];
bool temp[HB][VB];

int interval=100000;
bool game_quit = false;
bool game_pause = false;
bool is_dragging_l = false,is_dragging_r=false;

int camera_x=0,camera_y=0;
int camera_h=200*GT,camera_v=150*GT;
float zoom=1;

void print_g(SDL_Renderer* renderer){
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_RenderClear(renderer);
	int cur_gt=(float)GT*zoom;
	for(int j=0;j<VB; j++){
		for(int i=0;i<HB;i++){
			int abs_x=i*GT;
			int abs_y=j*GT;
			if(abs_x+GT<camera_x || abs_y+GT<camera_y) continue;
			if(i*GT>camera_x+(float)camera_h*(1.0/(float)zoom) || j*GT>camera_y+(float)camera_v*(1.0/(float)zoom)   ) continue;
			SDL_Rect fillRect = { i*cur_gt-(float)camera_x*zoom, j*cur_gt-(float)camera_y*zoom, cur_gt, cur_gt}; 
			SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
			if(grid[i][j]) SDL_RenderFillRect(renderer, &fillRect);
		}
	}
	SDL_RenderPresent(renderer);
}


bool valid(int i,int j){
	return i>=0 && j>=0 && i<HB && j<VB ;
}

int neigbour_count(int i, int j){
	int s=0;
	for(int jj=-1; jj<2;jj++){
		for(int ii=-1; ii<2; ii++){
			if(ii==0 && jj==0) continue;
			if(valid(i+ii,j+jj) && grid[i+ii][j+jj]) s++;
		}
	}
	return s;
}


void update(){
	/*
	Any live cell with fewer than two live neighbours dies, as if by underpopulation.
	Any live cell with two or three live neighbours lives on to the next generation.
	Any live cell with more than three live neighbours dies, as if by overpopulation.
	Any dead cell with exactly three live neighbours becomes a live cell, as if by reproduction.
	*/
	for(int j=0;j<VB; j++){
		for(int i=0;i<HB;i++){
			int n=neigbour_count(i,j);
			if(grid[i][j] && n<2) temp[i][j]=0;
			else if(grid[i][j] && (n==2 || n==3)) temp[i][j]=1;
			else if(grid[i][j] && n>3) temp[i][j]=0;
			else if(!grid[i][j] && n==3) temp[i][j]=1;
			else temp[i][j]=grid[i][j];
			//printf("%d,%d,%d,%d,%d\n",i,j,n,temp[i][j],grid[i][j]);
		}
		//printf("\n");
	}
	for(int j=0;j<VB; j++){
		for(int i=0;i<HB;i++){
			grid[i][j]=temp[i][j];
		}
	}
}


void read_config(char  *file){
	FILE *fp;
	fp=fopen(file,"r");
	if(fp==NULL){
		perror("not able to read files");
		exit(1);
	}
	int character;
	int i=0, j=0;
	while ((character = fgetc(fp)) != EOF) {
		if(character=='\n'){
			i=-1;
			j++;
			continue;
		}
		else{
			i++;
		}
		if(!valid(i,j)){
			perror("file value is out of boundery");
			exit(1);
		}
		if(character=='#') {
			grid[i][j]=1;
		}
	}

	fclose(fp);
}

void input_read(SDL_Event* e){
	switch(e->type){
		case SDL_QUIT :
			game_quit = true;
			break;
		case SDL_KEYDOWN :
			switch (e->key.keysym.sym){
				case SDLK_q:
					game_quit=true;
					break;
				case SDLK_w:
					interval=MAX(0,interval-1000);
					break;	
				case SDLK_s:
					interval+=1000;
					break;	
				case SDLK_r:
					memset(grid,0,sizeof(grid));
					break;	
				case SDLK_SPACE:
					game_pause=!game_pause;
					break;	
				case SDLK_LEFTBRACKET:
					zoom-=0.2;
					if((int)(zoom*GT)==0) zoom+=0.2;
					break;	
				case SDLK_RIGHTBRACKET:
					zoom+=0.2;
					break;	
				case SDLK_DOWN:
					camera_y+=10;
					break;	
				case SDLK_UP:
					camera_y-=10;
					break;	
				case SDLK_LEFT:
					camera_x-=10;
					break;	
				case SDLK_RIGHT:
					camera_x+=10;
					break;	
			}
			break;
		case SDL_MOUSEBUTTONDOWN :
			switch (e->button.button){
				case SDL_BUTTON_LEFT:
					//printf("x=%d ,y=%d\n",e.button.x,e.button.y);
					is_dragging_l=true;
					grid[e->button.x/GT][e->button.y/GT]=1;
					break; 
				case SDL_BUTTON_RIGHT:
					//printf("x=%d ,y=%d\n",e.button.x,e.button.y);
					is_dragging_r=true;
					grid[e->button.x/GT][e->button.y/GT]=0;
					break; 
			}	
			break;
		case SDL_MOUSEBUTTONUP :
			is_dragging_l=false;
			is_dragging_r=false;
			break;
		case SDL_MOUSEMOTION:
			if(is_dragging_l) grid[e->motion.x/GT][e->motion.y/GT]=1;
			if(is_dragging_r) grid[e->motion.x/GT][e->motion.y/GT]=0;
			break;
	}
}

int main(int argc,char *argv[]){
	memset(grid,0,sizeof(grid));
	memset(temp,0,sizeof(temp));
	srand(time(NULL));
	time_t cur_time=time(NULL);	
	if(argc>2){
		perror("more than 1 argument not allowed");
		exit(1);
	}
	else if(argc==2){
		read_config(argv[1]);	
	}
	else {
		for(int j=0;j<VB; j++){
			for(int i=0;i<HB;i++){
				grid[i][j]=rand()&1;
			}
		}
	}

	// 1. Initialize SDL Video Subsystem
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
		return 1; }

	// 2. Create Window SDL_Window* window = SDL_CreateWindow(
	SDL_Window* window= SDL_CreateWindow(
		"Conway's Game Of Life",           // window title
		SDL_WINDOWPOS_CENTERED,  // initial x position
		SDL_WINDOWPOS_CENTERED,  // initial y position
		camera_h,                     // width, in pixels
		camera_v,                     // height, in pixels
		SDL_WINDOW_RESIZABLE         // flags
	);

	if (window == NULL) {
		printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
		SDL_Quit();
		return 1;
	}

	// 3. Event Loop (keep the window open)
	SDL_Event e;
	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	while (!game_quit) {
		while (SDL_PollEvent(&e) != 0) {
			if(e.window.event==SDL_WINDOWEVENT_RESIZED)
			camera_h=e.window.data1 , camera_v=e.window.data2;
			//printf("%d,%d\n",e.window.data1,e.window.data2);
			input_read(&e);
		}
		if(!game_pause){
			update();
		}
		print_g(renderer);
		usleep(MAX(interval-(double)difftime(time(NULL),cur_time)*1000000,0));
		cur_time=time(NULL);
	}

	// 4. Cleanup
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
	return 0;
}
