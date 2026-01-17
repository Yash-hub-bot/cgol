#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <SDL2/SDL.h>
#include <stdbool.h>

#define HB 230
#define VB 170
#define GT 5
#define MAX(a, b) ((a) > (b) ? (a) : (b))



bool grid[HB][VB];
bool temp[HB][VB];

int interval=100000;

void print_g(SDL_Renderer* renderer){
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_RenderClear(renderer);
	for(int j=0;j<VB; j++){
		for(int i=0;i<HB;i++){
			SDL_Rect fillRect = { i*GT, j*GT, GT, GT }; 
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

void blinker(){
	grid[1][1]=1;
	grid[2][1]=1;
	grid[3][1]=1;
}

void glider(){
	grid[1][1]=1;	
	grid[2][2]=1;	
	grid[2][3]=1;	
	grid[3][2]=1;	
	grid[3][1]=1;	
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

int main(int argc,char *argv[]){
	memset(grid,0,sizeof(grid));
	memset(temp,0,sizeof(temp));
	
	if(argc>2){
		perror("more than 1 argument not allowed");
		exit(1);
	}
	if(argc==2){
		read_config(argv[1]);	
	}

	// 1. Initialize SDL Video Subsystem
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
		return 1;
	}

	// 2. Create Window
	SDL_Window* window = SDL_CreateWindow(
		"Conway's Game Of Life",           // window title
		SDL_WINDOWPOS_CENTERED,  // initial x position
		SDL_WINDOWPOS_CENTERED,  // initial y position
		HB*GT,                     // width, in pixels
		VB*GT,                     // height, in pixels
		SDL_WINDOW_SHOWN         // flags
	);

	if (window == NULL) {
		printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
		SDL_Quit();
		return 1;
	}

	// 3. Event Loop (keep the window open)
	bool quit = false;
	SDL_Event e;
	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	while (!quit) {
		while (SDL_PollEvent(&e) != 0) {
		    if (e.type == SDL_QUIT) {
			quit = true;
		    }
		    else if(e.type==SDL_KEYDOWN){
			switch (e.key.keysym.sym){
				case SDLK_w:
					interval=MAX(0,interval-1000);
					break;	
				case SDLK_s:
					interval+=1000;
					break;	
			}
		    }
		}
		print_g(renderer);
		update();
		usleep(interval);
	}

	// 4. Cleanup
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
	return 0;
}
