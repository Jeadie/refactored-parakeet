/*
 * game.c
 *
 * Written by Peter Sutton
 */ 

#include "game.h"
#include "snake.h"
#include "food.h"
#include "pixel_colour.h"
#include "board.h"
#include "ledmatrix.h"
#include "terminalio.h"
#include "score.h"
#include <stdio.h>
#include <stdint.h>
#define F_CPU 8000000UL
#include "util/delay.h"
#include <avr/pgmspace.h>
#include "SuperFood.h"
#include "rats.h"
#include "Buzzer.h"


#include <avr/pgmspace.h>


//   current speed is time between Snake movement
//  Originally set to 600ms.
static volatile uint16_t current_speed = 600;


// Colours that we'll use
#define SNAKE_HEAD_COLOUR	COLOUR_RED
#define SNAKE_BODY_COLOUR	COLOUR_GREEN
#define FOOD_COLOUR			COLOUR_LIGHT_YELLOW
#define BACKGROUND_COLOUR	COLOUR_BLACK
#define SUPERFOOD_COLOUR    COLOUR_ORANGE

// Helper function
void update_display_at_position(PosnType posn, PixelColour colour) {
	ledmatrix_update_pixel(x_position(posn), y_position(posn), colour);
}

// Initialise game. This initialises the board with snake and food items 
// and puts them on the display.
void init_game(void) {
	// Clear display
	ledmatrix_clear();
	init_score_on_terminal();
	reset_game_speed();
	add_new_rat_position();
	play_start_game_sound_effect();

	// Initialise the snake and display it. We know the initial snake is only
	// of length two so we can just retrieve the tail and head positions
	init_snake();
	update_display_at_position(get_snake_head_position(), SNAKE_HEAD_COLOUR);
	update_display_at_position(get_snake_tail_position(), SNAKE_BODY_COLOUR);
	
	// Initialise our food store, then add three items of food and display them
	init_food();
	for(int8_t i = 0; i < 3; i++) {
		PosnType food_position = add_food_item();
		if(is_position_valid(food_position)) {
			update_display_at_position(food_position, FOOD_COLOUR);
		}
	}	
}

// Attempt to move snake forward. Returns true if successful, false otherwise
int8_t attempt_to_move_snake_forward(void) {
	PosnType prior_head_position = get_snake_head_position();
	int8_t move_result = advance_snake_head();
	if(move_result < 0) {
		// Snake moved out of bounds (if this is not permitted) or
		// collided it with itself. Return false because we couldn't
		// move the snake
		return 0;
	}
// 	if(move_result ==1){
// 		play_snake_move_sound_effect(); 
// 	}

	add_to_score(1);
	PosnType new_head_position = get_snake_head_position();
	if(move_result == ATE_FOOD || move_result == ATE_FOOD_BUT_CANT_GROW) {
		// remove food item
		int8_t foodID = food_at(new_head_position);
		remove_food(foodID);
		play_eating_food_sound_effect();

		//  Account for additional two points for eating food. 
		add_to_score(2);
		increase_game_speed();
		
		// Add a new food item. Might fail if a free position can't be 
		// found on the board but shouldn't usually.
		PosnType new_food_posn = add_food_item();
		if(is_position_valid(new_food_posn)) {
			update_display_at_position(new_food_posn, FOOD_COLOUR);
		}
	}
	if(move_result == ATE_SUPERFOOD || move_result == ATE_SUPERFOOD_BUT_CANT_GROW){
		eat_superfood_from_board();
		add_to_score(9);
		increase_game_speed();
		
		play_eating_food_sound_effect();

	}
	
	if(move_result == ATE_RAT || move_result == ATE_RAT_BUT_CANT_GROW){
		add_new_rat_position();
		add_to_score(4);
		increase_game_speed();
		play_eating_food_sound_effect();

	}
		
	// We don't remove the eaten food from the display since we'll just
	// display the snake head at that position.
	
	// If we didn't eat food OR if we ate food but the snake is at 
	// maximum length, then we move the tail forward and remove this 
	// element from the display
	if(move_result == MOVE_OK || move_result == ATE_FOOD_BUT_CANT_GROW ||
		 move_result == ATE_SUPERFOOD_BUT_CANT_GROW || move_result == ATE_RAT_BUT_CANT_GROW) {

		//  Because Score has changed, update Score on Terminal
		PosnType prev_tail_posn = advance_snake_tail();
		update_display_at_position(prev_tail_posn, BACKGROUND_COLOUR);
	}

	// We update the previous head position to become a body part and 
	// update the new head position.
	update_display_at_position(prior_head_position, SNAKE_BODY_COLOUR);
	update_display_at_position(new_head_position, SNAKE_HEAD_COLOUR);
	update_terminal_score();	

	return 1;
}

void increase_game_speed(void){
	if (get_current_game_speed()>250){
		current_speed *= 0.90;
	}
}

int get_current_game_speed(void){
	return current_speed;
}
void reset_game_speed(void){
	current_speed = 600;
}

void set_game_speed(uint16_t new_speed){
	current_speed = new_speed;
}




