//meadow.c Proc gen adventure game with outdoor styled environmentss
//Initially wanting to use Perlin noise/value noise to generate
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ncurses.h>

typedef struct{
	char name[32];
	int xpos;
	int ypos;
	int prevx;
	int prevy;
	int imgchar;
} player;

int initializePlayer(player *p, char name[32]);


int main(int argc, char *argv[]){
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
	mvprintw(10, 20, "Hello meadows!");		//curses version of printf. Print to "window" buffer.
	refresh();						//Dump all changes to window buffer on to screen
	while(game_active == 1){				//27 is escape
		if(mvinch(player.ypos, player.xpos) != player.imgchar){
			mvaddch(player.prevy, player.prevx, overch);
			overch = mvinch(player.ypos, player.xpos);
		}
		mvaddch(player.ypos, player.xpos, player.imgchar);	
		move(0, 0);
		refresh();
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
				if(player.ypos < 25){
					player.prevy = player.ypos;
					player.prevx = player.xpos;
					player.ypos++;
				}
				break;
			}
			case KEY_RIGHT:{
				if(player.xpos < 80){
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
	p->ypos = 24;
	p->prevx = 40;
	p->prevy = 24;
	p->imgchar = '@';	
	return 0;
}
