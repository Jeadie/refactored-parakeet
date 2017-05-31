/*
*
*  functions for dealing with the movement and display of rats
*
*
*
*/
#include "food.h"
#include "timer0.h"
#include "ledmatrix.h"
#include "ledmatrix.h"
#include "pixel_colour.h"
#include "position.h"
#include "board.h"
#include "snake.h"
#include "SuperFood.h"
#include <math.h>
#include "rats.h"
#include <stdlib.h>

PosnType rat_position; 

uint16_t RAT_WAIT_TIME = 1200;

PosnType get_rat_position(void){
	return rat_position;
}

void set_rat_at_pos(PosnType new_pos){
	ledmatrix_update_pixel(x_position(rat_position), y_position(rat_position), COLOUR_BLACK);
	rat_position = new_pos;
	ledmatrix_update_pixel(x_position(new_pos), y_position(new_pos), COLOUR_LIGHT_YELLOW);

}

uint8_t is_item_at(PosnType position){
	if (is_snake_at(position) || is_food_at(position)|| is_superfood_at(position)){
		return 1;
	}
	return 0;
}

void move_rat(void){
	uint8_t attempts = 0; 
	PosnType new_rat_pos = position(0,0);
	uint8_t move_option = rand()%4; 
	do 
	{
		switch(move_option){
			case 0:
				new_rat_pos = position(x_position(rat_position), y_position(rat_position)+1);
			break; 
			
			case 1:
				new_rat_pos = position(x_position(rat_position), y_position(rat_position)-1);
			break;
			
			case 2:
				new_rat_pos = position(x_position(rat_position)+1, y_position(rat_position));
			break;
			
			case 3:
				new_rat_pos = position(x_position(rat_position)-1, y_position(rat_position));
			break;
			
		}
		attempts ++;
		move_option = (move_option +1)%4; 
		
	} while ((is_snake_at(new_rat_pos) || rat_jumped_the_board(new_rat_pos) || is_superfood_at(new_rat_pos) || is_food_at(new_rat_pos)) &&(attempts <4)); // || (rat_jumped_the_board(new_rat_pos))); (attempts <4) && 
	// in feature summary about rat jumping the boarder
	if (attempts<=4){
		set_rat_at_pos(new_rat_pos); 
	}
	}

uint8_t rat_jumped_the_board(PosnType new_pos){
	uint8_t rat_x = x_position(rat_position);
	uint8_t rat_y = y_position(rat_position);
	
	//  if rat is currently not on boarder, cannot jump boarder. 
	//  Checked first to increase performance. 
	if ((rat_x> 0 && rat_x< BOARD_WIDTH-1) 
		&& (rat_y> 0 && rat_y< BOARD_HEIGHT-1)){
			return 0; 
		}
	// All four possible jumps
	if ((rat_x ==0 && (x_position(new_pos) == BOARD_WIDTH-1)) ||
		(rat_x == BOARD_WIDTH-1 && (x_position(new_pos) == 0)) ||
		(rat_y==0 && (y_position(new_pos) == BOARD_HEIGHT-1)) ||
		(rat_y == BOARD_HEIGHT-1 && (y_position(new_pos) == 0))){
			return 1; 
		}
	return 0; 
}

uint8_t is_rat_at(PosnType pos){
	return (rat_position == pos);
}

void add_new_rat_position(void){
	/* Generate "random" positions until we get one which
	** is not occupied by a snake or food.
	*/
	
	int8_t x, y, attempts;
	PosnType test_position;
	x = 0;
	y = 0;
	attempts = 0;
	do {
		// Generate a new position - this is based on a sequence rather
		// then being random
        x = rand()%BOARD_WIDTH;
        y = rand()%BOARD_HEIGHT;
        test_position = position(x,y);
        attempts++;
    } while(attempts < 100 &&
		(is_snake_at(test_position) || is_food_at(test_position)
		|| test_position == get_superfood_position() || test_position == get_rat_position()));
	
	// If we get here, we've found an unoccupied position (test_position)
	// Add it to our list, display it, and return its ID.
	rat_position = test_position;
	ledmatrix_update_pixel(x_position(rat_position), y_position(rat_position), COLOUR_LIGHT_YELLOW);
}

uint8_t is_time_to_move_rat(void){
	return ((get_clock_ticks() %RAT_WAIT_TIME) ==0);
}


