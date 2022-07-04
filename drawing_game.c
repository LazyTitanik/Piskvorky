/*
	Tool kit for showing game board on screen

	Created by Azat Mukhametshin, Tomáš Březina
*/


#include "drawing_game.h"
#include "two_players_game.h"

union pixel circle[FIELD_SIZE][FIELD_SIZE];

void draw_empty(int h, int v, union pixel buffer[LCD_WIDTH][LCD_HEIGH]){
	for(int x=82+20*h; x<98+20*h; x++){
		for(int y=2+v*20; y<18+v*20; y++){
			buffer[x][y].d = 0xffff;
		}
	}
}

void draw_X(int h, int v, union pixel buffer[LCD_WIDTH][LCD_HEIGH]){
	int shift_x = 80+20*h;
	int shift_y = v*20;
	for(int x=shift_x; x<100+20*h; x++){
		for(int y=shift_y; y<20+v*20; y++){
			if((x-shift_x == y-shift_y || (y-shift_y)+(x-shift_x) == 20) && buffer[x+1][y].d == 0xffff){
				buffer[x][y] = (union pixel){.r = 0x1f}; 
				buffer[x+1][y] = (union pixel){.r = 0x1f}; 
			}
		}
	}
}

void draw_circle(int h, int v, union pixel buffer[LCD_WIDTH][LCD_HEIGH]){
	int shift_x = 2+h*20;
	int shift_y = 2+v*20;
	for(int x=shift_x; x<shift_x+18; x++){
		for(int y=shift_y; y<shift_y+18; y++){
			if(circle[x-shift_x][y-shift_y].d != 0xffff ){
				buffer[x+80][y].d = circle[x-shift_x][y-shift_y].d;
				if(x<shift_x + FIELD_SIZE/2){
					buffer[x+80+1][y].d = circle[x-shift_x][y-shift_y].d;
				}else {
					buffer[x+80-1][y].d = circle[x-shift_x][y-shift_y].d;
				}
				if(y<shift_y + FIELD_SIZE/2){
					buffer[x+80][y+1].d = circle[x-shift_x][y-shift_y].d;
				}else{
					buffer[x+80][y-1].d = circle[x-shift_x][y-shift_y].d;
				}
			}
		}
	}
}

void show_field(int h, int v, union pixel buffer[LCD_WIDTH][LCD_HEIGH]){
	int shift_x = 80+h*20;
	int shift_y = v*20;
	for(int x=shift_x; x<shift_x+20; x++){
		buffer[x][shift_y].d = 0xff00;
		buffer[x][shift_y+1].d = 0xff00;
		buffer[x][shift_y+20].d = 0xff00;
		buffer[x][shift_y+21].d = 0xff00;
	}
	for(int y=shift_y; y<shift_y+20; y++){
		buffer[shift_x][y].d = 0xff00;
		buffer[shift_x+1][y].d = 0xff00;
		buffer[shift_x+20][y].d = 0xff00;
		buffer[shift_x+21][y].d = 0xff00;
	}
}

void hide_field(int h, int v, union pixel buffer[LCD_WIDTH][LCD_HEIGH]){
	int shift_x = 80+h*20;
	int shift_y = v*20;
	for(int x=shift_x; x<shift_x+20; x++){
		buffer[x][shift_y] = (union pixel){.b = 0x1f};
		buffer[x][shift_y+1] = (union pixel){.b = 0x1f};
		buffer[x][shift_y+20] = (union pixel){.b = 0x1f};
		buffer[x][shift_y+21] = (union pixel){.b = 0x1f};
	}
	for(int y=shift_y; y<shift_y+20; y++){
		buffer[shift_x][y] = (union pixel){.b = 0x1f};
		buffer[shift_x+1][y] = (union pixel){.b = 0x1f};
		buffer[shift_x+20][y] = (union pixel){.b = 0x1f};
		buffer[shift_x+21][y] = (union pixel){.b = 0x1f};
	}
}

void change_picture(char board[16][16], union pixel buffer[LCD_WIDTH][LCD_HEIGH]){
	for (int x=0; x<16; x++){
		for (int y=0; y<16; y++){
			if(board[x][y] == PIECE_EMPTY){
				draw_empty(x,y, buffer);
			}
			else if(board[x][y] == PIECE_X){
				draw_X(x,y, buffer);
			}
			else if(board[x][y] == PIECE_O){
				draw_circle(x,y, buffer);
			}
		}
	}
}

int check_end(char board[16][16], int chosenX, int chosenY){
	// orizontally
	int piece = board[chosenX][chosenY];
	int count = 0;
	int x = chosenX;
	int y = chosenY;
	while(board[x][y] == piece && x<16){
		count++;
		x++;
	}
	count--;
	x = chosenX;
	y = chosenY;
	while(board[x][y] == piece && x>0){
		count++;
		x--;
	}
	if (count >=5) return 1;
	
	//vertically
	count = 0;
	x = chosenX;
	y = chosenY;
	while(board[x][y] == piece && y<16){
		count++;
		y++;
	}
	count--;
	x = chosenX;
	y = chosenY;
	while(board[x][y] == piece && y>0){
		count++;
		y--;
	}
	if (count >=5) return 1;
	
	// main diagonal
	count = 0;
	x = chosenX;
	y = chosenY;
	while(board[x][y] == piece && y<16 && x<16){
		count++;
		y++;
		x++;
	}
	count--;
	x = chosenX;
	y = chosenY;
	while(board[x][y] == piece && y>0 && x>0){
		count++;
		y--;
		x--;
	}
	if (count >=5) return 1;
	
	// second diagonal
	count = 0;
	x = chosenX;
	y = chosenY;
	while(board[x][y] == piece && y>0 && x<16){
		count++;
		y--;
		x++;
	}
	count--;
	x = chosenX;
	y = chosenY;
	while(board[x][y] == piece && y<16 && x>0){
		count++;
		y++;
		x--;
	}
	if (count >=5) return 1;
	return 0;
}

void make_move(char matrix[16][16], int* chosenX, int* chosenY){
	int x = rand() % 16;
	int y = rand() % 16;
	while(matrix[x][y] != 0){
		x = rand() % 16;
		y = rand() % 16;
	} 
	matrix[x][y] = 2;
	(*chosenX) = x;
	(*chosenY) = y;
}

void init_circle(){
	for (unsigned x = 0; x < FIELD_SIZE; x++){
		for (unsigned y = 0; y < FIELD_SIZE; y++){
			circle[x][y].d = 0xffff;
		}
	}
	circle[6][0] = (union pixel){.r = 0x3f}; circle[7][0] = (union pixel){.r = 0x3f};circle[8][0] = (union pixel){.r = 0x3f};
	circle[9][0] = (union pixel){.r = 0x3f};circle[10][0] = (union pixel){.r = 0x3f};circle[11][0] = (union pixel){.r = 0x3f};
	
	circle[4][1] = (union pixel){.r = 0x3f}; circle[5][1] = (union pixel){.r = 0x3f};circle[12][1] = (union pixel){.r = 0x3f}; circle[13][1] = (union pixel){.r = 0x3f};
	
	circle[3][2] = (union pixel){.r = 0x3f}; circle[14][2] = (union pixel){.r = 0x3f}; 
	
	circle[2][3] = (union pixel){.r = 0x3f};circle[15][3] = (union pixel){.r = 0x3f};
	
	circle[1][4] = (union pixel){.r = 0x3f};circle[16][4] = (union pixel){.r = 0x3f};
	
	circle[1][5] = (union pixel){.r = 0x3f};circle[16][5] = (union pixel){.r = 0x3f};
	
	circle[0][6] = (union pixel){.r = 0x3f};circle[17][6] = (union pixel){.r = 0x3f};
	circle[0][7] = (union pixel){.r = 0x3f};circle[17][7] = (union pixel){.r = 0x3f};
	circle[0][8] = (union pixel){.r = 0x3f};circle[17][8] = (union pixel){.r = 0x3f};
	circle[0][9] = (union pixel){.r = 0x3f};circle[17][9] = (union pixel){.r = 0x3f};
	circle[0][10] = (union pixel){.r = 0x3f};circle[17][10] = (union pixel){.r = 0x3f};	
	circle[0][11] = (union pixel){.r = 0x3f};circle[17][11] = (union pixel){.r = 0x3f};
	
	circle[4][16] = (union pixel){.r = 0x3f}; circle[5][16] = (union pixel){.r = 0x3f};circle[12][16] = (union pixel){.r = 0x3f}; circle[13][16] = (union pixel){.r = 0x3f};
	
	circle[3][15] = (union pixel){.r = 0x3f}; circle[14][15] = (union pixel){.r = 0x3f}; 
	
	circle[2][14] = (union pixel){.r = 0x3f};circle[15][14] = (union pixel){.r = 0x3f};
	
	circle[1][13] = (union pixel){.r = 0x3f};circle[16][13] = (union pixel){.r = 0x3f};
	
	circle[1][12] = (union pixel){.r = 0x3f};circle[16][12] = (union pixel){.r = 0x3f};
	
	circle[6][FIELD_SIZE-1] = (union pixel){.r = 0x3f}; circle[7][FIELD_SIZE-1] = (union pixel){.r = 0x3f};circle[8][FIELD_SIZE-1] = (union pixel){.r = 0x3f};
	circle[9][FIELD_SIZE-1] = (union pixel){.r = 0x3f};circle[10][FIELD_SIZE-1] = (union pixel){.r = 0x3f};circle[11][FIELD_SIZE-1] = (union pixel){.r = 0x3f};
}
