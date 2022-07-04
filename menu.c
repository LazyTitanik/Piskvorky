/*
	Tool kit for working with menu.

	Created by Azat Mukhametshin
*/

#include "menu.h"
#include "two_players_game.h"
#include "print_button.h"
#include <stddef.h>
#include <stdio.h>
#include <time.h>

#include "mzapo_parlcd.h"
#include "mzapo_phys.h"
#include "mzapo_regs.h"

void show_chosen(union pixel* buffer, int shiftY){
	
	for(int x = 180; x<300; x++){ // horizontal
		buffer[x*LCD_HEIGH+shiftY].d = 0x0000;
		buffer[x*LCD_HEIGH+shiftY+40].d = 0x0000;
	}
	for(int y = shiftY; y<shiftY+40; y++){ // vertical
		buffer[180*LCD_HEIGH + y].d = 0x0000;
		buffer[300*LCD_HEIGH + y].d = 0x0000;
	}
}

void hide_chosen(union pixel* buffer, int shiftY){
	for(int x = 180; x<300; x++){
		buffer[x*LCD_HEIGH+shiftY].d = 0xffff;
		buffer[x*LCD_HEIGH+shiftY+40].d = 0xffff;
	}
	for(int y = shiftY; y<shiftY+40; y++){
		buffer[180*LCD_HEIGH + y].d = 0xffff;
		buffer[300*LCD_HEIGH + y].d = 0xffff;
	}
}

void draw_menu(union pixel buffer[LCD_WIDTH][LCD_HEIGH]){
	printf("entered drawing menu\n");
	print_button_text(190, 100, buffer, 1, "1 PLAYER", 1);
	print_button_text(190, 150, buffer, 2, "2 PLAYERS", 1);
	
	parlcd_write_cmd(parlcd_reg_base, 0x2c);
	for (unsigned y = 0; y < LCD_HEIGH; y++){
		for (unsigned x = 0; x < LCD_WIDTH; x++){
			//printf("drawing pixel %d %d\n", x, y);
			parlcd_write_data(parlcd_reg_base, buffer[x][y].d);
		}
	}
}

// Returns index of chosen menu item (0 - "1 Player", 1 - "2 Players")
int listen_menu(union pixel* buffer, unsigned char *mem_base){
	printf("listening to menu\n");
	uint32_t rgb_knobs_value = *(volatile uint32_t*)(mem_base + SPILED_REG_KNOBS_8BIT_o);
	
	unsigned int uint_val = rgb_knobs_value;
	int press = 0;
	int turn_g = (uint_val & 0x0000ff00) >> 8;
	int last_turn = turn_g - turn_g%4;
	int chosen_menu = 0;
	
	// 105 for 1st rectangle, 155 2nd
	show_chosen(buffer, 105);
	
	/* Initialize structure to 0 seconds and 10 milliseconds */
	struct timespec loop_delay = {.tv_sec = 0, .tv_nsec = 10 * 1000 * 1000};
	
	while (1) {
		//printf("1 \n");
		// Redrawing
		parlcd_write_cmd(parlcd_reg_base, 0x2c);
		for (unsigned y = 0; y < LCD_HEIGH; y++){
			for (unsigned x = 0; x < LCD_WIDTH; x++){
				parlcd_write_data(parlcd_reg_base, buffer[x*LCD_HEIGH + y].d);
			}
		}
		
		printf("%d %d\n", turn_g, last_turn);
		
		//printf("2 \n");
		// Reading values of knobs
		rgb_knobs_value = *(volatile uint32_t*)(mem_base + SPILED_REG_KNOBS_8BIT_o);
		uint_val = rgb_knobs_value;
		press = (uint_val & 0xff000000) >> 24;
		turn_g = (uint_val & 0x0000ff00) >> 8;
		
		//printf("3 \n");
		// Analizing knobs values
		
		if((last_turn - turn_g)%4 == 0 && last_turn != turn_g){
			chosen_menu = (chosen_menu + 1) % 2;
			last_turn = turn_g;
		}
		if(press != 0){
			return chosen_menu;
		}
		// changing buffer
		if(chosen_menu == 0){
			show_chosen(buffer, 105);
			hide_chosen(buffer, 155);
		}else{
			show_chosen(buffer, 155);
			hide_chosen(buffer, 105);
		}
		
		//printf("4 \n");
		// Sleep
		clock_nanosleep(CLOCK_MONOTONIC, 0, &loop_delay, NULL);
		
	} // end while
	return -1;
}
