#ifndef MENU

#define MENU

#include "two_players_game.h"

void draw_menu( union pixel buffer[LCD_WIDTH][LCD_HEIGH]);

int listen_menu(union pixel* buffer, unsigned char *mem_base);

#endif
