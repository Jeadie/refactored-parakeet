/*
 * EEPROM_Game_Save.c
 *
 * Created: 24/05/2017 4:35:09 PM
 *  Author: Jack Eadie
 */ 

//  TODO: CHeck through this with tutor
//  TODO: Try to improve speed and reduction of EEPROM usage => increase speed
#include "rats.h"
#include "SuperFood.h"
#include "food.h"
#include "project.h"
#include "game.h"
#include "snake.h"
#include "position.h"
#include "ledmatrix.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include "EEPROM_Game_Save.h"
#include <avr/eeprom.h>

uint8_t EEPROM_has_saved_game(void){
	uint8_t current_signature = eeprom_read_byte(SAVE_GAME_SIGNATURE_MEMORY); 
	return (current_signature == SAVE_GAME_SIGNATURE); 
}

void save_game_to_EPPROM(void){
	cli();
	// Save the Food Positions
	eeprom_write_byte(SAVE_GAME_SIGNATURE_MEMORY, SAVE_GAME_SIGNATURE); 
	eeprom_write_byte(FOOD_ONE, get_position_of_food(0));//  | (get_position_of_food(1) <<8) | (get_position_of_food(2)) <<16));
	eeprom_write_byte(FOOD_TWO, get_position_of_food(1));
	eeprom_write_byte(FOOD_THREE, get_position_of_food(2));
	
	//  Save Rat Position and Superfood position if superfood is currently on. 
	eeprom_write_byte(RAT, get_rat_position());
	if (superfood_is_on_board()){
		eeprom_write_byte(SUPERFOOD, get_superfood_position());
	} else{
		eeprom_write_byte(SUPERFOOD, 0x08);
	}
	
	//  Save general game values
	eeprom_write_word(GAME_SPEED, get_current_game_speed());
	eeprom_write_dword(GAME_SCORE, get_score());
	eeprom_write_byte(SNAKE_LENGTH, get_snake_length());
	eeprom_write_dword(CLOCK_TICKS, get_clock_ticks());
	
	// To increment after every snake position is saved
	uint16_t current_snake_position_memory = 0; 
	PosnType *ordered_snake; 
	ordered_snake = get_snake_in_order();
	
	//  Add each PosnType of the orderedSnake. 
	for(int i = 0; i< get_snake_length(); i++){
		eeprom_write_byte(SNAKE_POSITION_HEAD + current_snake_position_memory, 
								ordered_snake[i]);
		i+=8; 
	}
	//  Add an invalid PosnType byte to signify the end of the snake positions. 
	eeprom_write_byte(SNAKE_POSITION_HEAD + current_snake_position_memory,0x08);
	
		sei();

}

void new_game_from_EEPROM(void){
	if(EEPROM_has_saved_game()){
		clear_terminal(); 
	
		// Methods from init_game(); 
		ledmatrix_clear();
		init_score_on_terminal();
		reset_game_speed();
		add_new_rat_position();
		initialise_timer_one();
		play_start_game_sound_effect();
	
		// Init food, rats, superfood and snake
		load_food_from_EEPROM();
		load_rat_from_EEPROM();
		load_superfood_from_EEPROM();
		load_snake_from_EPPROM();
		//  Set Score and Clock
	
		// Delete any pending button pushes or serial input
		empty_button_queue();
		clear_serial_input_buffer();
	
		//  set boolean back to zero, so next game will not load from EEPROM
		load_EEPROM_data_on_next_game =0;
	}
}

void load_food_from_EEPROM(void){
	init_food();
	PosnType food_one = eeprom_read_byte(FOOD_ONE);
	add_food_to_board(food_one);
	PosnType food_two = eeprom_read_byte(FOOD_TWO);
	add_food_to_board(food_two);
	
	PosnType food_three = eeprom_read_byte(FOOD_THREE);
	add_food_to_board(food_three);
}

void add_food_to_board(PosnType food_pos){
	if(is_position_valid(food_pos)) {
		foodPositions[numFoodItems] = food_pos;
		update_display_at_position(food_pos, COLOUR_LIGHT_YELLOW);
		numFoodItems++; 
	}
}
void load_rat_from_EEPROM(void){
	PosnType rat_pos = eeprom_read_byte(RAT); 
	if (is_position_valid(rat_pos)){
		set_rat_at_pos(rat_pos); 
	}else{
		add_new_rat_position();
	}
}

void load_superfood_from_EEPROM(void){
	PosnType super_food = eeprom_read_byte(SUPERFOOD); 
	if(is_position_valid(super_food)){
		load_superfood(super_food);
	}else{
		remove_superfood_from_board(); 
	}
}

void load_snake_from_EPPROM(void){
	
}
