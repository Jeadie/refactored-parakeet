/*
*  SuperFood Methods to help with timing and placement
*
*
*
*/
#include "food.h"
#include "timer0.h"
#include "ledmatrix.h"
#include "pixel_colour.h"
#include "position.h"
#include "board.h"
#include "rats.h"
#include "SuperFood.h"
#include "snake.h"
#include <math.h>


uint8_t superfood_on_board; 
PosnType superfood_position; 

uint8_t load_superfood(PosnType loaded_pos){
	superfood_position = loaded_pos; 
	superfood_on_board = 1;
}

uint8_t superfood_is_on_board(void){
	return superfood_on_board;
}

uint8_t time_to_add_superfood(void){
	return ((((get_clock_ticks() -15000)%20000) ==0) && (get_clock_ticks() - 15000 >=0));
}

uint8_t time_to_remove_superfood(void){
	return ((get_clock_ticks()%20000) ==0);
}

void add_superfood_to_board(void){
	/* Generate "random" positions until we get one which
	** is not occupied by a snake or food.
	*/
	remove_superfood_from_board();
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
		|| is_superfood_at(test_position) || is_rat_at(test_position)));
   
	
	// If we get here, we've found an unoccupied position (test_position)
	// Add it to our list, display it, and return its ID.
	superfood_position = test_position;
	ledmatrix_update_pixel(x_position(superfood_position), y_position(superfood_position), COLOUR_ORANGE);
	superfood_on_board = 1;
}

void remove_superfood_from_board(void){
	ledmatrix_update_pixel(x_position(superfood_position), y_position(superfood_position), COLOUR_ORANGE);
	superfood_position = position(0,0);
	superfood_on_board = 0; 
}

void eat_superfood_from_board(void){
	superfood_position = position(0,0);
	superfood_on_board = 0;
}

PosnType get_superfood_position(void){
	return superfood_position;
}

uint8_t is_superfood_at(PosnType pos){
	return (superfood_on_board && pos == superfood_position);
}
