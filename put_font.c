/*
	File with functions for typing text on a LCD screen.

	Created by Azat Mukhametshin thanks to
	Grisha Nikolaenkov and Hanna Hliavitskaya,
	based on templates of semestral work
*/

#include "put_font.h"  
#include "font_types.h" 
#include "two_players_game.h"
 
unsigned char_width(font_descriptor_t* fdes, int ch) { 
  int width = 0; 
  if ((ch >= fdes->firstchar) && (ch-fdes->firstchar < fdes->size)) { 
    ch -= fdes->firstchar; 
    if (!fdes->width) { 
      width = fdes->maxwidth; 
    } else { 
      width = fdes->width[ch]; 
    } 
  } 
  return width; 
} 
 
void pchar(font_descriptor_t *font, char c, unsigned x, unsigned y, union pixel buffer[480][320], union pixel color, int scale){ 
	for (unsigned w = 0; w < (font->maxwidth) * scale; w+=scale) { 
		for(unsigned h = 0; h < (font->height) * scale; h+=scale) { 
			if(font->bits[(c - font->firstchar) * font->height + h/scale] & (1 << (16 - w/scale))) { 
				for(int i=0; i<scale; i++){
					for(int j=0; j<scale; j++){
						buffer[x + w + i][y + h + j].d = color.d; 
					}
				}
			} 
		} 
	} 
}
