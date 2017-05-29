/*
 * EEPROM_Game_Save.c
 *
 * Created: 24/05/2017 4:35:09 PM
 *  Author: Jack Eadie
 */ 

//  TODO: figure out why high score isn't working 
//  TODO: figure out why top level of score board is blank (everything is down by one). 
//  TODO: not saving gamescore
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
	uint32_t current_signature = eeprom_read_dword(SAVE_GAME_SIGNATURE_MEMORY); 
	return (current_signature == SAVE_GAME_SIGNATURE); 
}

void save_game_to_EPPROM(void){
	cli();
	// Save the Food Positions
	eeprom_write_dword(SAVE_GAME_SIGNATURE_MEMORY, SAVE_GAME_SIGNATURE); 
	eeprom_write_byte(FOOD_ONE, get_position_of_food(0));//  | (get_position_of_food(1) <<8) | (get_position_of_food(2)) <<16));
	eeprom_write_byte(FOOD_TWO, get_position_of_food(1));
	eeprom_write_byte(FOOD_THREE, get_position_of_food(2));
	eeprom_write_byte(FOOD_FOUR, get_position_of_food(3));
	
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
	printf("G Score :%u \n", eeprom_read_dword(GAME_SCORE));  // not working
	eeprom_write_byte(SNAKE_LENGTH, get_snake_length());
	eeprom_write_dword(CLOCK_TICKS, get_clock_ticks());
	eeprom_write_byte(SNAKE_DIRECTION, get_current_drn()); 
	// To increment after every snake position is saved
	uint16_t current_snake_position_memory = 0; 
	
	//  Add each PosnType of the orderedSnake. 
	
	if (get_snake_tail_index()> get_snake_head_index()){
		//printf("wrapped snake \n");
		//SSSH-------------------------TSSS
		for (int i = get_snake_tail_index(); i<=MAX_SNAKE_SIZE; i++){
			eeprom_write_byte(SNAKE_POSITION_TAIL + current_snake_position_memory, 
							 get_snake_position_at_index(i));
			//printf("x: %u y: %u \n", x_position(get_snake_position_at_index(i)), y_position(get_snake_position_at_index(i)));
			current_snake_position_memory += 0x08;
		}
		for (int i = 0; i<= get_snake_head_index(); i++){
			eeprom_write_byte(SNAKE_POSITION_TAIL + current_snake_position_memory,
			get_snake_position_at_index(i));
			//printf("x: %u y: %u \n", x_position(get_snake_position_at_index(i)), y_position(get_snake_position_at_index(i)));
			current_snake_position_memory +=0x08;
		}
		}else{
			//printf("straight Snake \n");
		//  ---TSSSSSSH----
		for(int i =get_snake_tail_index(); i<=get_snake_head_index(); i++){
			eeprom_write_byte(SNAKE_POSITION_TAIL + current_snake_position_memory, get_snake_position_at_index(i));
			current_snake_position_memory +=0x08;
			//printf("x: %u y: %u \n", x_position(get_snake_position_at_index(i)), y_position(get_snake_position_at_index(i)));
			
		}
	}
	
	//  Add an invalid PosnType byte to signify the end of the snake positions. 
	eeprom_write_byte(SNAKE_POSITION_TAIL + current_snake_position_memory,0x08);
	//print_save_details();
	sei();

}
void print_save_details(void){
	printf("F1 :%u \n", eeprom_read_byte(FOOD_ONE));
	printf("F2 :%u \n", eeprom_read_byte(FOOD_TWO));
	printf("F3:%u \n", eeprom_read_byte(FOOD_THREE));
	printf("F4 :%u \n", eeprom_read_byte(FOOD_FOUR));
	printf("S1 :%u \n", eeprom_read_byte(SUPERFOOD));
	printf("R1 :%u \n", eeprom_read_byte(RAT));
	printf("Game Speed :%u \n", eeprom_read_word(GAME_SPEED));
	printf("G Score :%u \n", eeprom_read_dword(GAME_SCORE));  // not working
	printf("Snake Length :%u \n", eeprom_read_byte(SNAKE_LENGTH));
	printf("Snake tail :%u \n", eeprom_read_byte(SNAKE_POSITION_TAIL));
	printf("G time :%i \n", eeprom_read_dword(CLOCK_TICKS));
	
}

void new_game_from_EEPROM(void){
	if(EEPROM_has_saved_game()){
		clear_terminal(); 
		set_clock_ticks(eeprom_read_dword(CLOCK_TICKS));
		set_score(eeprom_read_dword(GAME_SCORE));
		
		// Methods from init_game(); 
		ledmatrix_clear();
		init_score_on_terminal();
		set_game_speed(eeprom_read_word(GAME_SPEED));
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
	
	PosnType food_four = eeprom_read_byte(FOOD_FOUR);
	add_food_to_board(food_four);
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
	uint16_t snake_memory_position = SNAKE_POSITION_TAIL;
	set_snake_length(eeprom_read_byte(SNAKE_LENGTH));
	uint8_t snakePosition_index = 0; 
	set_snake_tail_pointer(0);
	set_snake_head_pointer(get_snake_length() -1); 
	do 
	{
		PosnType snake_position = eeprom_read_byte(snake_memory_position);
		//  printf("x: %u y: %u \n", x_position(snake_position), y_position(snake_position)); 
		set_snake_position_in_array(snake_position, snakePosition_index); 
		update_display_at_position(snake_position, COLOUR_GREEN);
		snake_memory_position +=8; 
		snakePosition_index ++; 
		
	} while (is_position_valid(eeprom_read_byte(snake_memory_position)));
	update_display_at_position(get_snake_head_position(), COLOUR_RED);

	reset_snake_drn(eeprom_read_byte(SNAKE_DIRECTION)); 
}
