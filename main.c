/*
	Communicates between different parts of programm.
	(menu, game and endgame screen)
	main.c - main file

	Created by Azat Mukhametshin, Tomáš Březina
*/

#include <sys/mman.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
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
#include "menu.h"

unsigned char *parlcd_reg_base;

int two_in_power(int power){
	int result = 0;
	for(int i=0; i<power;i++){
		result = (result << 1) | 1;
	}
	return result;
}

void run (int is_robot, int* skoreX, int* skoreO, int* led_line, union pixel buffer[LCD_WIDTH][LCD_HEIGH], unsigned char *mem_base){
	int winner = two_players_game(buffer, is_robot);
	if(winner == 1){
		(*skoreO)++;
		(*led_line) = (two_in_power((*skoreX))) << (32 - (*skoreX));
		(*led_line) = (*led_line) | (two_in_power((*skoreO)));
		*(volatile uint32_t*)(mem_base + SPILED_REG_LED_LINE_o) = (*led_line);
	}
	else {
		(*skoreX)++;
		(*led_line) = (two_in_power((*skoreX))) << (32 - (*skoreX));
		(*led_line) = (*led_line) | (two_in_power((*skoreO)));
		*(volatile uint32_t*)(mem_base + SPILED_REG_LED_LINE_o) = (*led_line);
	}
}

void empty_buffer(union pixel buffer[LCD_WIDTH][LCD_HEIGH]){
	for(int x=0; x<LCD_WIDTH; x++){
		for(int y=0; y<LCD_HEIGH; y++){
			buffer[x][y].d = 0xffff;
		}
	}
}

int main(){
	parlcd_reg_base = map_phys_address(PARLCD_REG_BASE_PHYS, PARLCD_REG_SIZE, 0);
	volatile void *spiled_reg_base = map_phys_address(SPILED_REG_BASE_PHYS, SPILED_REG_SIZE, 0);
	unsigned char *mem_base;
	union pixel buffer[LCD_WIDTH][LCD_HEIGH];
	uint32_t led_line = 0;
	int skoreX = 0;
	int skoreO = 0; 
	int winner;
	int max = 5;
	/*
	* Setup memory mapping which provides access to the peripheral
	* registers region of RGB LEDs, knobs and line of yellow LEDs.
	*/
	mem_base = map_phys_address(SPILED_REG_BASE_PHYS, SPILED_REG_SIZE, 0);
	
	empty_buffer(buffer);
	*(volatile uint32_t*)(mem_base + SPILED_REG_LED_LINE_o) = led_line;
	
	draw_menu(buffer);
	
	int game = listen_menu(buffer, mem_base);
	
	while (skoreX < max && skoreO<max){
		if(game == 1){
			run(0, &skoreX, &skoreO, &led_line, buffer, mem_base);
		} 
		if(game == 0){
			run(1, &skoreX, &skoreO, &led_line, buffer, mem_base);
		} 
		printf("\nledline = %d \tskoreX = %d \tskoreO = %d\n", led_line, skoreX, skoreO);
	}
	
	empty_buffer(buffer);
	
	print_button_text(45, 100, buffer, 1, "CONGRATULATIONS", 3);
	if(skoreX == max){
		print_button_text(45, 160, buffer, 1, "  TO PLAYER 1", 3);
	}
	else{
		print_button_text(45, 160, buffer, 1, "  TO PLAYER 2", 3);
	}
	
	parlcd_write_cmd(parlcd_reg_base, 0x2c);
	for (unsigned y = 0; y < LCD_HEIGH; y++)
		for (unsigned x = 0; x < LCD_WIDTH; x++)
			parlcd_write_data(parlcd_reg_base, buffer[x][y].d);
	
	return 1;
}
