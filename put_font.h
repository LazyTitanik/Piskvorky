#ifndef PUT_FONT
#define PUT_FONT

#include "font_types.h"
#include "two_players_game.h"

unsigned char_width(font_descriptor_t* fdes, int ch);
void pchar(font_descriptor_t *font, char c, unsigned x, unsigned y, union pixel buffer[480][320], union pixel color, int scale);

#endif


