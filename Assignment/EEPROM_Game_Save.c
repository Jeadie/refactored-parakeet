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
	eeprom_write_byte(SNAKE_LENGTH, get_snake_length());
	eeprom_write_dword(CLOCK_TICKS, get_clock_ticks());
	
	// To increment after every snake position is saved
	uint16_t current_snake_position_memory = 0; 
	
	//  Add each PosnType of the orderedSnake. 
	
	if (get_snake_tail_position()> get_snake_head_position()){
		//SSSH-------------------------TSSS
		for (int i = get_snake_head_position(); i>=0; i--){
			eeprom_write_byte(SNAKE_POSITION_HEAD + current_snake_position_memory, 
							 get_snake_position_at_index(i));
			current_snake_position_memory +=8;
		}
		for (int i = MAX_SNAKE_SIZE; i>= get_snake_tail_position(); i--){
			eeprom_write_byte(SNAKE_POSITION_HEAD + current_snake_position_memory,
			get_snake_position_at_index(i));
			current_snake_position_memory +=8;
		}
		}else{
		//  ---TSSSSSSH----
		for(int i =get_snake_tail_position(); i<=get_snake_head_position(); i++){
			eeprom_write_byte(SNAKE_POSITION_HEAD + current_snake_position_memory, get_snake_position_at_index(i));
			current_snake_position_memory +=8;
		}
	}
	
	//  Add an invalid PosnType byte to signify the end of the snake positions. 
	eeprom_write_byte(SNAKE_POSITION_HEAD + current_snake_position_memory,0x08);
	print_save_details();
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
	printf("G Score :%lu \n", eeprom_read_dword(GAME_SCORE));  // not working
	printf("Snake Length :%u \n", eeprom_read_byte(SNAKE_LENGTH));
	printf("Snake Head :%u \n", eeprom_read_byte(SNAKE_POSITION_HEAD)); // not working
	printf("G time :%lu \n", eeprom_read_dword(CLOCK_TICKS));
	
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
	uint16_t snake_memory_position = SNAKE_POSITION_HEAD;
	uint8_t snake_length  = eeprom_read_byte(SNAKE_LENGTH);
	uint8_t snakePosition_index = 0; 
	set_snake_head_pointer(0);
	set_snake_tail_pointer(snake_length -1); 
	do 
	{
		PosnType snake_position = eeprom_read_byte(snake_memory_position);
		set_snake_position_in_array(snake_position, snakePosition_index); 
		update_display_at_position(snake_position, COLOUR_GREEN);
		snake_memory_position +=8; 
		snakePosition_index ++; 
		
	} while (is_position_valid(eeprom_read_byte(snake_memory_position)));
	update_display_at_position(get_snake_head_position(), COLOUR_RED);
}

void set_snake_direction(void){
	PosnType head_node = get_snake_head_position();
	PosnType first_body_node = get_snake_position_at_index(1); 
	SnakeDirnType snake_direction; 
	// Either up or down
	if((x_position(head_node) -x_position(first_body_node)) ==0){
		if((y_position(head_node) -y_position(first_body_node)) >0){
			snake_direction = SNAKE_UP; 
		}else{
			snake_direction = SNAKE_DOWN; 
		}
			
	}else{
		//  Either Left or Right
		if((x_position(head_node) -x_position(first_body_node)) >0){
			snake_direction = SNAKE_RIGHT; 
		}else{
			snake_direction = SNAKE_LEFT; 
		}
	}
	reset_snake_drn(snake_direction); 
}
