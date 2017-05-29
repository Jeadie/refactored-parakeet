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
#include "score.h"

struct Game_Save_Data{
	PosnType food_one;
	PosnType food_two;
	PosnType food_three;
	PosnType food_four;
	PosnType superfood; 
	PosnType rat; 
	uint16_t game_speed;
	uint32_t game_score;
	uint8_t snake_length; 
	uint32_t clock_ticks; 
	uint8_t snake_direction; 
	};
	


uint8_t EEPROM_has_saved_game(void){
	uint32_t current_signature = eeprom_read_dword(SAVE_GAME_SIGNATURE_MEMORY); 
	return (current_signature == SAVE_GAME_SIGNATURE); 
}

void save_game_to_EPPROM(void){
	cli();
	// Save the Food Positions
	eeprom_write_dword(SAVE_GAME_SIGNATURE_MEMORY, SAVE_GAME_SIGNATURE);
	
	struct Game_Save_Data new_saved_game; 
	new_saved_game.food_one =get_position_of_food(0);
	new_saved_game.food_two =get_position_of_food(1);
	new_saved_game.food_three =get_position_of_food(2);
	new_saved_game.food_four = get_position_of_food(3);
	new_saved_game.rat = get_rat_position(); 
	new_saved_game.game_score = get_score(); 
	new_saved_game.game_speed = get_current_game_speed(); 
	new_saved_game.snake_length = get_snake_length(); 
	new_saved_game.clock_ticks = get_clock_ticks(); 
	new_saved_game.snake_direction = get_current_drn(); 
	
	//  Save Rat Position and Superfood position if superfood is currently on. 
	if (superfood_is_on_board()){
		new_saved_game.superfood = get_superfood_position();
	} else{
		new_saved_game.superfood =  0x08; 
	}
	// Save struct to EEPROM
	//  printf("size of game struct: %u", sizeof(new_saved_game));
	eeprom_write_block(&new_saved_game, SAVED_DATA_START, sizeof(new_saved_game)); 

	// To increment after every snake position is saved
	uint16_t current_snake_position_memory = SNAKE_POSITION_TAIL; 
	
	//  Add each PosnType of the Snake. Will load game with tail at 0 and head at snake_length-1
	if (get_snake_tail_index()> get_snake_head_index()){
		//printf("wrapped snake \n");
		//SSSH-------------------------TSSS
		for (int i = get_snake_tail_index(); i<=MAX_SNAKE_SIZE; i++){
			eeprom_write_byte(current_snake_position_memory, 
							 get_snake_position_at_index(i));
			//printf("x: %u y: %u \n", x_position(get_snake_position_at_index(i)), y_position(get_snake_position_at_index(i)));
			current_snake_position_memory += 0x08;
		}
		for (int i = 0; i<= get_snake_head_index(); i++){
			eeprom_write_byte(current_snake_position_memory,
			get_snake_position_at_index(i));
			//printf("x: %u y: %u \n", x_position(get_snake_position_at_index(i)), y_position(get_snake_position_at_index(i)));
			current_snake_position_memory +=0x08;
		}
		}else{
			//printf("straight Snake \n");
		//  ---TSSSSSSH----
		for(int i =get_snake_tail_index(); i<=get_snake_head_index(); i++){
			eeprom_write_byte(current_snake_position_memory, get_snake_position_at_index(i));
			current_snake_position_memory +=0x08;
			//printf("x: %u y: %u \n", x_position(get_snake_position_at_index(i)), y_position(get_snake_position_at_index(i)));
			
		}
	}
	
	//  Add an invalid PosnType byte to signify the end of the snake positions. 
	eeprom_write_byte(current_snake_position_memory,0x08);
	//print_save_details();
	sei();

}

void new_game_from_EEPROM(void){
	if(EEPROM_has_saved_game()){
		clear_terminal(); 
		ledmatrix_clear();
		init_score_on_terminal();
		
		struct Game_Save_Data game_from_EEPROM; 
		eeprom_read_block(&game_from_EEPROM, SAVED_DATA_START, 18); 
		
		// Methods from init_game(); 
		set_score(game_from_EEPROM.game_score);
		set_clock_ticks(game_from_EEPROM.clock_ticks); 
		update_terminal_score(); 
		set_game_speed(game_from_EEPROM.game_speed);
		load_rat_from_EEPROM(game_from_EEPROM.rat);
		initialise_timer_one();
	
		// Init food, rats, superfood and snake
		add_food_to_board(game_from_EEPROM.food_one);
		add_food_to_board(game_from_EEPROM.food_two);
		add_food_to_board(game_from_EEPROM.food_three);
		add_food_to_board(game_from_EEPROM.food_four);
		load_superfood_from_EEPROM( game_from_EEPROM.superfood);
		set_snake_length(game_from_EEPROM.snake_length); 
		reset_snake_drn(game_from_EEPROM.snake_direction); 
		load_snake_from_EPPROM();
		//  Set Score and Clock
	
		// Delete any pending button pushes or serial input
		empty_button_queue();
		clear_serial_input_buffer();
	
		//  set boolean back to zero, so next game will not load from EEPROM
		load_EEPROM_data_on_next_game =0;
	}
}

void add_food_to_board(PosnType food_pos){
	if(is_position_valid(food_pos)) {
		foodPositions[numFoodItems] = food_pos;
		update_display_at_position(food_pos, COLOUR_LIGHT_YELLOW);
		numFoodItems++; 
	}
}
void load_rat_from_EEPROM(PosnType rat_pos){
	if (is_position_valid(rat_pos)){
		set_rat_at_pos(rat_pos); 
	}else{
		add_new_rat_position();
	}
}

void load_superfood_from_EEPROM(PosnType super_food){
	if(is_position_valid(super_food)){
		load_superfood(super_food);
	}else{
		remove_superfood_from_board(); 
	}
}

void load_snake_from_EPPROM(void){
	uint16_t snake_memory_position = SNAKE_POSITION_TAIL;
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

}
