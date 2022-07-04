#ifndef DRAWING_GAME
#define DRAWING_GAME

#include <unistd.h>
#include <fcntl.h>
#include <malloc.h>
#include <string.h>
#include <byteswap.h>
#include <getopt.h>
#include <inttypes.h>
#include <time.h>

#include "font_types.h"
#include "mzapo_parlcd.h"
#include "mzapo_phys.h"
#include "mzapo_regs.h"
#include "two_players_game.h"

void draw_empty(int h, int v, union pixel buffer[LCD_WIDTH][LCD_HEIGH]);

void draw_X(int h, int v, union pixel buffer[LCD_WIDTH][LCD_HEIGH]);

void draw_circle(int h, int v, union pixel buffer[LCD_WIDTH][LCD_HEIGH]);

void show_field(int h, int v, union pixel buffer[LCD_WIDTH][LCD_HEIGH]);

void hide_field(int h, int v, union pixel buffer[LCD_WIDTH][LCD_HEIGH]);

void change_picture(char board[16][16], union pixel buffer[LCD_WIDTH][LCD_HEIGH]);

int check_end(char board[16][16], int chosenX, int chosenY);

void make_move(char matrix[16][16], int* chosenX, int* chosenY);

void init_circle();

extern union pixel circle[FIELD_SIZE][FIELD_SIZE];
#endif
