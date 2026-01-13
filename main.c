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
	int disp_width;
	int disp_height;
	vec2 viewport_pos; //top-left corner of viewport
	int current_area_index;
	int current_area_name;
} gameinfo;

int initializePlayer(player *p, char name[32]);
void setTerrainToDot(chtype t[][400], int r);		//debug: set area to all periods
void drawTerrain(chtype t[][400], int disp_w, int disp_h, vec2 disp_origin);
void generateFieldArea(chtype t[][400], int r);		//Field area 400x400

int main(int argc, char *argv[]){
	chtype area1[400][400];
	setTerrainToDot(area1, 24);
	int input;
	chtype overch = ' ';						//Ascii char that the player is standing on, so it can be replaced when the player moves.
	int game_active = 1;
	player player;
	gameinfo game;
	game.viewport_pos.x = 190; 		//starting position of viewport
	game.viewport_pos.y = 190; 		
	game.disp_width = 80;
	game.disp_height = 24;
	char name[32] = "Player1";
	initializePlayer(&player, name);

	initscr();						//ncurses mode on
	cbreak();					
	keypad(stdscr, TRUE);
	noecho();
	start_color();
	init_pair(1, COLOR_RED, COLOR_BLACK);
	init_pair(2, COLOR_CYAN, COLOR_BLACK);
	init_pair(3, COLOR_GREEN, COLOR_BLACK);

	player.imgchar = '@' | COLOR_PAIR(2);
	mvprintw(10, 20, "Hello meadows! Press any key to continue...");		//curses version of printf. Print to "window" buffer.
	getch();
	generateFieldArea(area1, 24);
	refresh();								//Dump all changes to window buffer on to screen
	while(game_active == 1){				//27 is escape
		//Draw
		drawTerrain(area1, game.disp_width, game.disp_height, game.viewport_pos);
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
				generateFieldArea(area1, 24);
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


int initializePlayer(player *p, char name[32]){
	strcpy(p->name, name);
	p->pos.x = 39;
	p->pos.y = 12;
	p->prev_pos.x = 39;
	p->prev_pos.y = 12;
	p->imgchar = '@';	
	return 0;
}

void setTerrainToDot(chtype t[][400], int r){
	for(int i = 0; i < r; i++){
		for(int j = 0; j < 400; j++){
			t[i][j] = '.' | COLOR_PAIR(3);
		}
	}
}

void drawTerrain(chtype t[][400], int disp_w, int disp_h, vec2 disp_origin){
	for(int i = 0; i < disp_h; i++){
		for(int j = 0; j < disp_w; j++){
			mvaddch(i, j, t[i+disp_origin.y][j+disp_origin.x]);
		}
	}
}

//Optimize with malloc for variable length
void generateFieldArea(chtype t[][400], int r){
	srand(time(NULL));
	int rand_seed = rand(); 
	fnl_state noise = fnlCreateState();
	noise.noise_type = FNL_NOISE_OPENSIMPLEX2;
	noise.seed = rand_seed;
	float *noise_data = malloc(400 * 400 * sizeof(float)); //for now, just the size of one screen, but eventually larger areas with edge scrolling.
	int index = 0;
	for(int i=0; i < 400; i++){
		for(int j=0; j < 400; j++){
			noise_data[index++] = fnlGetNoise2D(&noise, i, j);
		}
	}
	//Process noise into valuse I can display on my map (integers 0-5)
	for(int i=0; i < 400; i++){ 			//could sub 24 for r
		for(int j=0; j < 400; j++){
			float val = noise_data[(i*400)+j];
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

