/*
  A tool for showing a button on a screen

  Created by Azat Mukhametshin thanks to 
  Grisha Nikolaenkov and Hanna Hliavitskaya 
*/

#include "print_button.h"
#include "two_players_game.h"
#include <string.h>

void print_button_text(int x, int y, union pixel buffer[LCD_WIDTH][LCD_HEIGH], int lvl_num, char* LVL, int scale){ 
 union pixel color; 
 color.d = 0x0000; 
 switch(lvl_num){ 
  case 1: 
   for(unsigned i = 0; i< strlen(LVL); i++){ 
    pchar(&font_rom8x16, LVL[i], (x + 10) + scale*i*(&font_rom8x16)->maxwidth, y + 1 + ((50 - 16)/2), buffer, color, scale); 
   } 
   break; 
  case 2: 
   for(unsigned i = 0; i< strlen(LVL); i++){ 
    pchar(&font_rom8x16, LVL[i], (x + 10) + scale*i*(&font_rom8x16)->maxwidth, y + 1 + ((50 - 16)/2), buffer, color, scale); 
   } 
   break; 
  case 3: 
   for(unsigned i = 0; i< strlen(LVL); i++){ 
    pchar(&font_rom8x16, LVL[i], (x + 10) + scale*i*(&font_rom8x16)->maxwidth, y + 1 + ((50 - 16)/2), buffer, color, scale); 
   } 
   break; 
 } 
 
}
