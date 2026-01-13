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
#include "include/fastnoiselite.h"
#ifdef _WIN32
#include "ncursesw/ncurses.h"
#endif
#ifndef _WIN32
#include <ncurses.h>
#endif

typedef struct{
	char name[32];
	int xpos;
	int ypos;
	int prevx;
	int prevy;
	int imgchar;
} player;

int initializePlayer(player *p, char name[32]);
void setTerrainToDot(chtype t[][80], int r);
void drawTerrain(chtype t[][80], int r);
void generateFieldArea(chtype t[][80], int r);

int main(int argc, char *argv[]){
	chtype area1[24][80];
	setTerrainToDot(area1, 24);
	int input;
	chtype overch = ' ';						//Ascii char that the player is standing on, so it can be replaced when the player moves.
	int game_active = 1;
	player player;
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
		drawTerrain(area1, 24);
		if(mvinch(player.ypos, player.xpos) != player.imgchar){
			mvaddch(player.prevy, player.prevx, overch);
			overch = mvinch(player.ypos, player.xpos);
		}
		mvaddch(player.ypos, player.xpos, player.imgchar);	
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
				if(player.ypos > 0){
					player.prevy = player.ypos;
					player.prevx = player.xpos;
					player.ypos--;
				}
				break;
			}
			case KEY_DOWN:{
				if(player.ypos < 23){				//24 rows, first row is 0
					player.prevy = player.ypos;
					player.prevx = player.xpos;
					player.ypos++;
				}
				break;
			}
			case KEY_RIGHT:{
				if(player.xpos < 79){				//80 cols, first col is 0
					player.prevy = player.ypos;
					player.prevx = player.xpos;
					player.xpos++;
				}
				break;
			}
			case KEY_LEFT:{
				if(player.xpos > 0){
					player.prevy = player.ypos;
					player.prevx = player.xpos;
					player.xpos--;
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
	p->xpos = 40;
	p->ypos = 15;
	p->prevx = 40;
	p->prevy = 15;
	p->imgchar = '@';	
	return 0;
}

void setTerrainToDot(chtype t[][80], int r){
	for(int i = 0; i < r; i++){
		for(int j = 0; j < 80; j++){
			t[i][j] = '.' | COLOR_PAIR(3);
		}
	}
}

void drawTerrain(chtype t[][80], int r){
	for(int i = 0; i < r; i++){
		for(int j = 0; j < 80; j++){
			mvaddch(i, j, t[i][j]);
		}
	}
}

void generateFieldArea(chtype t[][80], int r){
	fnl_state noise = fnlCreateState();
	noise.noise_type = FNL_NOISE_OPENSIMPLEX2;

	float *noise_data = malloc(240 * 800 * sizeof(float)); //for now, just the size of one screen, but eventually larger areas with edge scrolling.
	int index = 0;
	for(int i=0; i < 240; i++){
		for(int j=0; j < 800; j++){
			noise_data[index++] = fnlGetNoise2D(&noise, i, j);
		}
	}
	//Process noise into valuse I can display on my map (integers 0-5)
	for(int i=0; i < 24; i++){ 			//could sub 24 for r
		for(int j=0; j < 80; j++){
			//Average noise values (/100)
			float avg=0.0;
			for(int a=0; a<100; a++){
				avg += noise_data[(((i*80)+j)*100)+a]; //Row * (80) Columns, + current column, *(100) item numbers, + current item number
			}
			avg /= 100.0; //divide by (100) items

			//probably a faster way to do this
			if(avg <= 1.0 && avg > 0.666)
				t[i][j] = '0';
			else if(avg <= 0.666 && avg > 0.333)
				t[i][j] = '*';
			else if(avg <= 0.333 && avg > 0.0)
				t[i][j] = '.';
			else if(avg <= 0.0 && avg > -0.333)
				t[i][j] = '.';
			else if(avg <= -0.333 && avg > -0.666)
				t[i][j] = '*';
			else if(avg <= -0.666 && avg >= -1)
				t[i][j] = '0';
			else
				t[i][j] = 'x';	
		}
	}
	free(noise_data);
}



