#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <SDL2/SDL.h>
#include <stdbool.h>
#include <time.h>
#include <math.h>

#define HB 2000
#define VB 1100
#define GT 5
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define SCALEUP(i) ceil((float)(i)*zoom)
#define SCALEDOWN(i) floor((float)(i)*(1.0/zoom))
#define GRID2SCREEN(i,base) SCALEUP((i)*GT-base)
#define SCREEN2GRID(i,base) (int)((SCALEDOWN(i)+base)/GT)



//bool **grid;
//bool **temp;

typedef struct CELL{
	int x;
	int y;
};

typedef struct CELLS{
	int capacity=0;
	int size=0;
	CELL *arr=NULL;	
}

void cell_append(CELLS *Cs,)



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
	for(int i=0;i<HB;i++){
		for(int j=0;j<VB; j++){
			if(GRID2SCREEN(i+1,camera_x)<=0 || GRID2SCREEN(j+1,camera_y)<=0)    continue;
			if(GRID2SCREEN(i,camera_x)>camera_h || GRID2SCREEN(j,camera_y)>camera_v) continue;
			SDL_Rect fillRect = { GRID2SCREEN(i,camera_x),GRID2SCREEN(j,camera_y), SCALEUP(GT),SCALEUP(GT) }; 
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
		int n=0;
		if(valid(0,j-1)) n+=grid[0][j-1];
		if(valid(0,j)) n+=grid[0][j];
		if(valid(0,j+1)) n+=grid[0][j+1];
		for(int i=0;i<HB;i++){
			//int n=neigbour_count(i,j);
			if(valid(i-2,j-1)) n-=grid[i-2][j-1];
			if(valid(i-2,j)) n-=grid[i-2][j];
			if(valid(i-2,j+1)) n-=grid[i-2][j+1];

			if(valid(i+1,j-1)) n+=grid[i+1][j-1];
			if(valid(i+1,j)) n+=grid[i+1][j];
			if(valid(i+1,j+1)) n+=grid[i+1][j+1];
			
			n-=grid[i][j];
			//printf("%d,%d\n",n,neigbour_count(i,j));
			if(grid[i][j] && n<2) temp[i][j]=0;
			else if(grid[i][j] && (n==2 || n==3)) temp[i][j]=1;
			else if(grid[i][j] && n>3) temp[i][j]=0;
			else if(!grid[i][j] && n==3) temp[i][j]=1;
			else temp[i][j]=grid[i][j];
			n+=grid[i][j];
			//printf("%d,%d,%d,%d,%d\n",i,j,n,temp[i][j],grid[i][j]);
		}
		
		//printf("\n");
	}
	//for(int i=0;i<HB;i++){
	//	for(int j=0;j<VB; j++){
	//		grid[i][j]=temp[i][j];
	//	}
	//}
	bool **t=grid;
	grid=temp;
	temp=t;	
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
	//int tran_x=((float)(e->button.x)*(1.0/zoom)+camera_x)/GT;
	//int tran_y=((float)(e->button.y)*(1.0/zoom)+camera_y)/GT;
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
					for(int i=0; i<HB; i++){
						memset(grid[i],0,VB*sizeof(bool));
					}
					break;	
				case SDLK_SPACE:
					game_pause=!game_pause;
					break;	
				case SDLK_LEFTBRACKET:
					zoom*=1.1;
					break;	
				case SDLK_RIGHTBRACKET:
					zoom/=1.1;
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
					grid[SCREEN2GRID(e->button.x,camera_x)][SCREEN2GRID(e->button.y,camera_y)]=1;
					break; 
				case SDL_BUTTON_RIGHT:
					//printf("x=%d ,y=%d\n",e.button.x,e.button.y);
					is_dragging_r=true;
					grid[SCREEN2GRID(e->button.x,camera_x)][SCREEN2GRID(e->button.y,camera_y)]=0;
					break; 
			}	
			break;
		case SDL_MOUSEBUTTONUP :
			is_dragging_l=false;
			is_dragging_r=false;
			break;
		case SDL_MOUSEMOTION:
			if(is_dragging_l) grid[SCREEN2GRID(e->button.x,camera_x)][SCREEN2GRID(e->button.y,camera_y)]=1;
			if(is_dragging_r) grid[SCREEN2GRID(e->button.x,camera_x)][SCREEN2GRID(e->button.y,camera_y)]=0;
			break;
		case SDL_MOUSEWHEEL :
			if(e->wheel.y>0){
				zoom*=1.1;
			} else if(e->wheel.y<0){
				zoom/=1.1;
			}

	}
}

int main(int argc,char *argv[]){
	grid=(bool **)malloc(HB*sizeof(bool *));
	temp=(bool **)malloc(HB*sizeof(bool *));
	for (int i = 0; i < HB; i++) {
		grid[i] = (bool *)malloc(VB * sizeof(bool));
		temp[i] = (bool *)malloc(VB * sizeof(bool));
	}
	for(int i=0; i<HB; i++){
		memset(grid[i],0,VB*sizeof(bool));
		memset(temp[i],0,VB*sizeof(bool));
	}
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
		for(int i=0;i<HB;i++){
			for(int j=0;j<VB; j++){
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
