/*******************************************************************
  File with functions for runing a game between 2 players, or between a player
  and a robot, which makes random moves, but moves are always the same

  Created by Azat Mukhametshin.

 *******************************************************************/
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
#include "drawing_game.h"

void draw_board(union pixel buffer[LCD_WIDTH][LCD_HEIGH]){
	for (unsigned x = 0; x < LCD_WIDTH; x++)
		for (unsigned y = 0; y < LCD_HEIGH; y++)
		buffer[x][y].d = 0xffff;

	for(int x=80; x<420; x+=20){
		for(int y=0; y<320; y++){
			buffer[x][y] = (union pixel){.b = 0x1f}; 
			buffer[x+1][y+1] = (union pixel){.b = 0x1f};
		}
	}
  
	for(int y=0; y<320; y+=20){
		for(int x=80; x<400; x++){
			buffer[x][y] = (union pixel){.b = 0x1f}; 
			buffer[x+1][y+1] = (union pixel){.b = 0x1f};
		}
	}
}

void update_screen(union pixel buffer[LCD_WIDTH][LCD_HEIGH]){
	parlcd_write_cmd(parlcd_reg_base, 0x2c); // reset location to the top left corner
	for (unsigned y = 0; y < LCD_HEIGH; y++)
		for (unsigned x = 0; x < LCD_WIDTH; x++)
			parlcd_write_data(parlcd_reg_base, buffer[x][y].d);
}

void move(int vector, int current_turn, int* last_turn, int* chosen){
	if(((*last_turn) - current_turn)%4 == 0 && ((*last_turn) - current_turn)*vector > 0){
		printf("changing field < 0\n");
		int tmp = ((*last_turn) - current_turn) / 4;
		if(tmp<0) tmp = -tmp;
		(*chosen) = ((*chosen)+tmp)>15 ? 0 : ((*chosen)+tmp);
		(*last_turn) = current_turn;
	}
	else if(((*last_turn) - current_turn)%4 == 0 && ((*last_turn) - current_turn)*vector < 0){
		printf("changing field > 0\n");
		int tmp = ((*last_turn) - current_turn) / 4;
		if(tmp<0) tmp = -tmp;
		(*chosen) = ((*chosen)-tmp)<0 ? 15 : ((*chosen)-tmp);
		(*last_turn) = current_turn;
	}
}

void redraw_led_line(uint32_t* current, char piece,unsigned char* mem_base){
	uint32_t shift = (*current);
	
	*(volatile uint32_t*)(mem_base + SPILED_REG_LED_LINE_o) = (*(volatile uint32_t*)(mem_base + SPILED_REG_LED_LINE_o) & 0xff0000ff ) | shift;
	if(piece == PIECE_X) shift = (shift << 1);
	else shift = (shift >> 1);
	if(shift > 0x00ff0000) shift = (shift & 0x00ffff00) | 0x00000100;
	if((shift & 0x000000ff) > 0x00000000) shift = (shift & 0x00ffff00) | 0x00800000;
	
	(*current) = shift;
}

int two_players_game(union pixel buffer[LCD_WIDTH][LCD_HEIGH], int is_robot)
{
	struct timespec start_delay = {.tv_sec = 0, .tv_nsec = 500 * 1000 * 1000};
	clock_nanosleep(CLOCK_MONOTONIC, 0, &start_delay, NULL);
	volatile void *spiled_reg_base = map_phys_address(SPILED_REG_BASE_PHYS, SPILED_REG_SIZE, 0);
	volatile uint32_t *rgb1 = (volatile uint32_t*)(spiled_reg_base + SPILED_REG_LED_RGB1_o);
	*rgb1 = ((union led){.r = 0xff}).data;

	/*
	* Setup memory mapping which provides access to the peripheral
	* registers region of RGB LEDs, knobs and line of yellow LEDs.
	*/
	unsigned char *mem_base;
	mem_base = map_phys_address(SPILED_REG_BASE_PHYS, SPILED_REG_SIZE, 0);
	
	/* If mapping fails exit with error code */
	if (mem_base == NULL){
		printf("Neco se pokazilo\n");
		exit(1);
	}
  
    uint32_t rgb_knobs_value = *(volatile uint32_t*)(mem_base + SPILED_REG_KNOBS_8BIT_o);
     
    unsigned int uint_val = rgb_knobs_value;
    char piece = PIECE_X; 
	int turn_r = (uint_val & 0x00ff0000) >> 16;
	int turn_g = (uint_val & 0x0000ff00) >> 8;
	int turn_b = uint_val & 0x000000ff;
	int last_turn_r = turn_r;
	int last_turn_g = turn_g;
	int press;
	uint32_t shift = 0x00000f00;
	turn_r = turn_r - turn_r%4;
	turn_g = turn_g - turn_g%4;
      
    char matrix[16][16];
    for (int i=0; i<16; i++){
		for (int j=0; j<16; j++){
			matrix[i][j] = PIECE_EMPTY;
		} 
	}
	int chosenX = 8;
	int chosenY = 8;
	
	init_circle();
	draw_board(buffer);
	update_screen(buffer);
	show_field(8,8, buffer);
	
	/* Initialize structure to 0 seconds and 10 milliseconds */
	struct timespec loop_delay = {.tv_sec = 0, .tv_nsec = 10 * 1000 * 1000};
	
	while (1) {
		// Redrawing
		update_screen(buffer);
		redraw_led_line(&shift, piece, mem_base);
		
				
		// Updating knobs values
		rgb_knobs_value = *(volatile uint32_t*)(mem_base + SPILED_REG_KNOBS_8BIT_o);
		uint_val = rgb_knobs_value;
		
		// Showing, whose move it is
		if(piece == PIECE_X){
			*(volatile uint32_t*)(mem_base + SPILED_REG_LED_RGB1_o) = 0xffff;
			*(volatile uint32_t*)(mem_base + SPILED_REG_LED_RGB2_o) = 0x0000;
		}
		else{
			*(volatile uint32_t*)(mem_base + SPILED_REG_LED_RGB1_o) = 0x0000;
			*(volatile uint32_t*)(mem_base + SPILED_REG_LED_RGB2_o) = 0xffff;
		}
		
	
		turn_r = (uint_val & 0x00ff0000) >> 16;
		turn_g = (uint_val & 0x0000ff00) >> 8;
		turn_b = uint_val & 0x000000ff;
		
		printf("%d %d %d   %d %d\t", turn_r, turn_g, turn_b, last_turn_r, last_turn_g);
		
		// Choosing field by rotating knobs 
		hide_field(chosenX, chosenY, buffer);
		move(-1, turn_r, &last_turn_r, &chosenX);	
		move(1, turn_g, &last_turn_g, &chosenY);
		show_field(chosenX, chosenY, buffer);
		
		// Putting piece by pressing a button
		press = (uint_val & 0xff000000) >> 24;		
		if (press && matrix[chosenX][chosenY] == 0){
			matrix[chosenX][chosenY]= piece;
			if(piece == PIECE_X){	
				piece = PIECE_O;
			} else if(piece == PIECE_O && is_robot == 0){
				piece = PIECE_X;
			}	
			change_picture(matrix, buffer);	
			if(check_end(matrix, chosenX, chosenY) == 1) break;
			
			if(piece == PIECE_O && is_robot==1){
				int tmpX = chosenX;
				int tmpY = chosenY;
				make_move(matrix, &tmpX, &tmpY);
				piece = PIECE_X;
				change_picture(matrix, buffer);	
				if(check_end(matrix, tmpX, tmpY) == 1) break;
			}
		}
	
		printf("%d %d\n", chosenX, chosenY);
		
		// Sleep
		clock_nanosleep(CLOCK_MONOTONIC, 0, &loop_delay, NULL);
	} // while end

	// Final redrawing
	update_screen(buffer);
	
	printf("\nReturning val = %d\n", piece);
	return piece;
}
