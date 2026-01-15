//meadow.c Proc gen adventure game with outdoor styled environmentss
//Initially wanting to use Perlin noise/value noise to generate
//
//Now that the code works in both Linux and Windows, I can move ahead with using the ncurses library
//1) Move a character around the screen - check
//2) Create a data structure for holding the world
//3) Generate terrain within the data structure
//4) Format terrain with symbols and colors


#define FNL_IMPL
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "include/fastnoiselite.h"
#ifdef _WIN32
#include "ncursesw/ncurses.h"
#endif
#ifndef _WIN32
#include <ncurses.h>
#endif

#define DISP_MAX_WIDTH 130
#define DISP_MAX_HEIGHT 74
#define DISP_MIN_WIDTH 70
#define DISP_MIN_HEIGHT 14

typedef struct{
	int x;
	int y;
} vec2;

typedef struct{
	char name[32];
	vec2 pos;
	vec2 world_pos;
	vec2 prev_pos;
	int imgchar;
	unsigned int steps;
	
} player;

typedef struct{
	int state;			//1 - Start menu, 2 - Gameplay, 3 - Game over
	int disp_width;
	int disp_height;
	vec2 viewport_pos; //top-left corner of viewport
	int current_area_index;
	int current_area_name;
} gameinfo;

typedef struct{
	chtype **terrain;
	char name[64];
	int width;
	int height;
	//other area attirbute here. Exits? Obj count? enemy count? World map orientation?
} area;

WINDOW *create_newwin(int h, int w, int starty, int startx);
void destroy_win(WINDOW *local_win);

//Game functions
player createCharacter();
int initializePlayer(player *p);
void clearScreen();
void setTerrainToDot(chtype **t, int w, int h);		//debug: set area to all periods
void setTerrainToTitleScreen(chtype **t, int w, int h);
chtype **mallocNewArea(int w, int h);					//malloc a new area with dimensions by parameters. Return pointer to area matrix.

//Draw functions
void drawTerrain(chtype **t, int w, int h, int disp_w, int disp_h, vec2 disp_origin);
void drawStatusBar(gameinfo *g, player *p);

//Procedural generation functions
void generateFieldArea(chtype **t, int w, int h);		//Field area 400x400

int main(int argc, char *argv[]){
	chtype **area1 = mallocNewArea(400, 400);
	setTerrainToDot(area1, 400, 400);
	int input;
	chtype overch = ' ';						//Ascii char that the player is standing on, so it can be replaced when the player moves.
	int game_active = 1;
	player player;
	gameinfo game;
	game.state = 1;
	game.viewport_pos.x = 190; 		//starting position of viewport
	game.viewport_pos.y = 190; 		
	game.disp_width = 80;
	game.disp_height = 24;
	char name[32] = "Player1";

	initscr();						//ncurses mode on
	cbreak();					
	keypad(stdscr, TRUE);
	noecho();
	start_color();
	init_pair(1, COLOR_RED, COLOR_BLACK);
	init_pair(2, COLOR_CYAN, COLOR_BLACK);
	init_pair(3, COLOR_GREEN, COLOR_BLACK);

	

	/*------------------------------*/
	mvprintw(game.disp_height/2, 20, "Hello meadows! Press any key to continue...");		//curses version of printf. Print to "window" buffer.
	refresh();
	getch();
	clearScreen();
	move(0,0);
	player = createCharacter();
	initializePlayer(&player);
	player.imgchar = '@' | COLOR_PAIR(2);
	/*------------------------------*/
	generateFieldArea(area1, 400, 400);
	refresh();								//Dump all changes to window buffer on to screen
	while(game_active == 1){				//27 is escape
		//Draw
		drawTerrain(area1, 400, 400, game.disp_width, game.disp_height, game.viewport_pos);
		drawStatusBar(&game, &player);
		if(mvinch(player.pos.y, player.pos.x) != player.imgchar){
			mvaddch(player.prev_pos.y, player.prev_pos.x, overch);
			overch = mvinch(player.pos.y, player.pos.x);
		}
		mvaddch(player.pos.y, player.pos.x, player.imgchar);	
		move(0, 0);
		refresh();

		//Player input
		input = getch();
		switch(input){
			case 27:{
				game_active = 0;
				break;
			}
			case KEY_UP:{
				if(game.viewport_pos.y > 0){
					game.viewport_pos.y--;
					//adjust player's world pos
				}
				else if(player.pos.y > 0){
					player.prev_pos.y = player.pos.y;
					player.prev_pos.x = player.pos.x;
					player.pos.y--;
				}
				break;
			}
			case KEY_DOWN:{
				if(game.viewport_pos.y+game.disp_height < 400){		//modify to use disp_height
					game.viewport_pos.y++;
					//adjust [;ayer's world pos
				}
				else if(player.pos.y < 23){				//24 rows, first row is 0
					player.prev_pos.y = player.pos.y;
					player.prev_pos.x = player.pos.x;
					player.pos.y++;
				}
				break;
			}
			case KEY_RIGHT:{
				if(game.viewport_pos.x + game.disp_width < 400){    //modify to use disp_width
					game.viewport_pos.x++;
					//adjust player's world_pos
				}
				else if(player.pos.x < 79){				//80 cols, first col is 0
					player.prev_pos.y = player.pos.y;
					player.prev_pos.x = player.pos.x;
					player.pos.x++;
				}
				break;
			}
			case KEY_LEFT:{
				if(game.viewport_pos.x > 0){
					game.viewport_pos.x--;
					//adjust player's porld pos
				}
				else if(player.pos.x > 0){
					player.prev_pos.y = player.pos.y;
					player.prev_pos.x = player.pos.x;
					player.pos.x--;
				}
				break;
			}
			case 'r':{
				generateFieldArea(area1, 400, 400);
				break;
			}
			case 'c':{
				clearScreen();
				getch();
				break;
			}
			case '+':{
				if(game.disp_width < DISP_MAX_WIDTH && game.disp_height < DISP_MAX_HEIGHT){
					game.disp_width++;
					game.disp_height++;
				}
				break;
			}
			case '-':{
				if(game.disp_width > DISP_MIN_WIDTH && game.disp_height > DISP_MIN_HEIGHT){
					game.disp_width--;
					game.disp_height--;
				}
				break;
			}

		}
	}
	endwin();						//ncurses mode off
									
	return 0;
}

player createCharacter(){
	player p;
	move(0,0);
	printw("What should I call you, who would begin this journey?\n");
	getstr(p.name);
	printw("Very well, %s. I anticipate I will have more questions for you later. As for now, see the world unfolding before you...\n");
	getch();
	return p;
}

int initializePlayer(player *p){
	//strcpy(p->name, name);
	p->pos.x = 39;
	p->pos.y = 12;
	p->prev_pos.x = 39;
	p->prev_pos.y = 12;
	p->imgchar = '@';	
	return 0;
}

void clearScreen(){
	move(0, 0);
	int row, col;
	getmaxyx(stdscr, row, col);
	for(int i=0; i<row; i++){
		for(int j=0; j<col; j++){
			printw(" ");
		}
	}
}

void setTerrainToDot(chtype **t, int w, int h){
	for(int i = 0; i < h; i++){
		for(int j = 0; j < w; j++){
			t[i][j] = '.' | COLOR_PAIR(3);
		}
	}
}

//TODO: Do I need to add checks against the width/height of the area array? This is already being filtered once before calling.
void drawTerrain(chtype **t, int w, int h, int disp_w, int disp_h, vec2 disp_origin){
	for(int i = 0; i < disp_h; i++){
		for(int j = 0; j < disp_w; j++){
			mvaddch(i, j, t[i+disp_origin.y][j+disp_origin.x]);
		}
	}
}

void drawStatusBar(gameinfo *g, player *p){
	move(g->disp_height, 0);
	printw("Name: %s", p->name);
}

//Optimize with malloc for variable length
void generateFieldArea(chtype **t, int w, int h){
	srand(time(NULL));
	int rand_seed = rand(); 
	fnl_state noise = fnlCreateState();
	noise.noise_type = FNL_NOISE_OPENSIMPLEX2;
	noise.seed = rand_seed;
	float *noise_data = malloc(w * h * sizeof(float)); //for now, just the size of one screen, but eventually larger areas with edge scrolling.
	int index = 0;
	for(int i=0; i < h; i++){
		for(int j=0; j < w; j++){
			noise_data[index++] = fnlGetNoise2D(&noise, i, j);
		}
	}
	//Process noise into valuse I can display on my map (integers 0-5)
	for(int i=0; i < h; i++){ 			//could sub 24 for r
		for(int j=0; j < w; j++){
			float val = noise_data[(i*w)+j];
			//probably a faster way to do this
			if(val <= 1.0 && val > 0.666)
				t[i][j] = '0';
			else if(val <= 0.666 && val > 0.333)
				t[i][j] = '*';
			else if(val <= 0.333 && val > 0.0)
				t[i][j] = '.' | COLOR_PAIR(3);
			else if(val <= 0.0 && val > -0.333)
				t[i][j] = '.' | COLOR_PAIR(3);
			else if(val <= -0.333 && val > -0.666)
				t[i][j] = '*';
			else if(val <= -0.666 && val >= -1)
				t[i][j] = '0';
			else
				t[i][j] = 'x';	
		}
	}
	free(noise_data);
}

chtype **mallocNewArea(int w, int h){
	chtype **area = (chtype **)malloc(sizeof(chtype *) * h); //malloc rows (height of map) 
	if(area == NULL){
		perror("Unable to allocate memory for game world.");
		return NULL;	
	}
	for(int i=0; i<h; i++){
		area[i] = (chtype *)malloc(sizeof(chtype) * w);      //malloc w-amount of cells in for each row (width of map)
		if(area[i] == NULL){
			perror("Unable to allocate memory for game world.");
			for(int j=0; j<i; j++){
				free(area[j]);
			}	
			free(area);
			return NULL;
		}
		memset(area[i], 'x', (sizeof(chtype)*w));			//Set all cells to 'x'
	}
	return area;
}

WINDOW *create_newwin(int h, int w, int starty, int startx){
	WINDOW *local_win;

	local_win = newwin(h, w, starty, startx);
	box(local_win, 0, 0);		//0,0 is default border. Change border later
	wrefresh(local_win);
	return local_win;
}

void destroy_win(WINDOW *local_win){
	wborder(local_win, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ');
	wrefresh(local_win);
	delwin(local_win);
}

void setTerrainToTitleScreen(chtype **t, int w, int h){
	//This is gonna be stupid.
	t[2][6] = 'o';
	t[3][4] = 'o'; t[3][7] = 'o';
	t[4][2] = 'o'; t[4][6] = 'o'; t[4][9] = 'o'; t[4][10] = 'o'; t[4][18] = 'o'; t[4][51] = 'o';
	t[5][1] = 'o'; t[5][5] = 'o'; t[5][7] = 'o'; t[5][11] = 'o'; t[5][16] = 'o'; t[5][19] = 'o'; t[5][50] = 'o'; t[5][52] = 'o';
	t[6][0] = 'o'; t[6][5] = 'o'; t[6][6] = 'o'; t[6][12] = 'o'; t[6][14] = 'o'; t[6][20] = 'o'; t[6][50] = 'o'; t[6][52] = 'o';	
	t[7][4] = 'o'; t[7][12] = 'o'; t[7][13] = 'o'; t[7][20] = 'o'; t[7][49] = 'o'; t[7][51] = 'o';	
	t[8][4] = 'o'; t[8][12] = 'o'; t[8][21] = 'o'; t[8][26] = 'o'; t[8][27] = 'o'; t[8][28] = 'o'; t[8][29] = 'o'; t[8][36] = 'o'; t[8][37] = 'o'; t[8][37] = 'o'; 
	t[8][49] = 'o'; t[8][50] = 'o'; t[8][58] = 'o'; t[8][59] = 'o'; t[8][60] = 'o'; t[8][66] = 'o'; t[8][67] = 'o'; t[8][78] = 'o';	

	t[6][0] = 'o'; t[6][5] = 'o'; t[6][6] = 'o'; t[6][12] = 'o'; t[6][14] = 'o'; t[6][20] = 'o'; t[6][50] = 'o'; t[6][52] = 'o';	
	t[6][0] = 'o'; t[6][5] = 'o'; t[6][6] = 'o'; t[6][12] = 'o'; t[6][14] = 'o'; t[6][20] = 'o'; t[6][50] = 'o'; t[6][52] = 'o';	
	t[6][0] = 'o'; t[6][5] = 'o'; t[6][6] = 'o'; t[6][12] = 'o'; t[6][14] = 'o'; t[6][20] = 'o'; t[6][50] = 'o'; t[6][52] = 'o';	
	t[6][0] = 'o'; t[6][5] = 'o'; t[6][6] = 'o'; t[6][12] = 'o'; t[6][14] = 'o'; t[6][20] = 'o'; t[6][50] = 'o'; t[6][52] = 'o';	
	t[6][0] = 'o'; t[6][5] = 'o'; t[6][6] = 'o'; t[6][12] = 'o'; t[6][14] = 'o'; t[6][20] = 'o'; t[6][50] = 'o'; t[6][52] = 'o';	
	t[6][0] = 'o'; t[6][5] = 'o'; t[6][6] = 'o'; t[6][12] = 'o'; t[6][14] = 'o'; t[6][20] = 'o'; t[6][50] = 'o'; t[6][52] = 'o';	
	t[6][0] = 'o'; t[6][5] = 'o'; t[6][6] = 'o'; t[6][12] = 'o'; t[6][14] = 'o'; t[6][20] = 'o'; t[6][50] = 'o'; t[6][52] = 'o';	
	t[6][0] = 'o'; t[6][5] = 'o'; t[6][6] = 'o'; t[6][12] = 'o'; t[6][14] = 'o'; t[6][20] = 'o'; t[6][50] = 'o'; t[6][52] = 'o';	
	t[6][0] = 'o'; t[6][5] = 'o'; t[6][6] = 'o'; t[6][12] = 'o'; t[6][14] = 'o'; t[6][20] = 'o'; t[6][50] = 'o'; t[6][52] = 'o';	
	t[6][0] = 'o'; t[6][5] = 'o'; t[6][6] = 'o'; t[6][12] = 'o'; t[6][14] = 'o'; t[6][20] = 'o'; t[6][50] = 'o'; t[6][52] = 'o';	
	t[6][0] = 'o'; t[6][5] = 'o'; t[6][6] = 'o'; t[6][12] = 'o'; t[6][14] = 'o'; t[6][20] = 'o'; t[6][50] = 'o'; t[6][52] = 'o';	
	t[6][0] = 'o'; t[6][5] = 'o'; t[6][6] = 'o'; t[6][12] = 'o'; t[6][14] = 'o'; t[6][20] = 'o'; t[6][50] = 'o'; t[6][52] = 'o';	
	t[6][0] = 'o'; t[6][5] = 'o'; t[6][6] = 'o'; t[6][12] = 'o'; t[6][14] = 'o'; t[6][20] = 'o'; t[6][50] = 'o'; t[6][52] = 'o';	

}
