/*
 * EEPROM_HighScore.C
 *
 * Created: 24/05/2017 10:59:13 AM
 *  Author: Jack Eadie
 */ 

#include "serialio.h"
#include "EEPROM_HighScore.h"
#include <stdio.h>
#include <stdint.h>

#include <avr/pgmspace.h>
#include "score.h"
#include <avr/eeprom.h>
#include "terminalio.h"


static char current_high_score_name[4]; 
uint8_t is_high_score(void){
	return (get_score() >= eeprom_read_dword(SCORE_FIVE)) || !(EEPROM_High_Score_has_been_initialised()) ;
}

uint16_t high_score_name_position(uint16_t score_position){
// 	if (!(EEPROM_High_Score_has_been_initialised())){
// 		return NAME_ONE;
// 	}
	switch(score_position){
		case SCORE_FIVE: return NAME_FIVE; break; 
		case SCORE_FOUR: return NAME_FOUR; break;
		case SCORE_THREE: return NAME_THREE; break;
		case SCORE_TWO: return NAME_TWO; break;
		case SCORE_ONE: return NAME_ONE; break;
	}
}

uint16_t high_score_position(void){
	if (!(EEPROM_High_Score_has_been_initialised())){
		return SCORE_ONE;
	}
	if (get_score() > eeprom_read_dword(SCORE_ONE)){
		return SCORE_ONE;
	}
	if (get_score() > eeprom_read_dword(SCORE_TWO)){
		return SCORE_TWO;
	}
	if (get_score() > eeprom_read_dword(SCORE_THREE)){
		return SCORE_THREE;
	}
	if (get_score() > eeprom_read_dword(SCORE_FOUR)){
		return SCORE_FOUR;
	}
	if (get_score() > eeprom_read_dword(SCORE_FIVE)){
		return SCORE_FIVE;
	}
}

void set_high_score_name(void){
	move_cursor(10,8);
	printf("You've set a high score. please set your initial below: ");
	int score_name_pointer = 0; 
	char serial_input, escape_sequence_char;
	uint8_t characters_into_escape_sequence = 0;
	while (1){
		show_cursor(); 
		serial_input = -1;
		escape_sequence_char = -1;
		
		if(serial_input_available()) {
			// Serial data was available - read the data from standard input
			serial_input = fgetc(stdin);
			if (serial_input>= 65 && serial_input <= 122 && score_name_pointer <=2){
				move_cursor(10 + score_name_pointer, 9);
				current_high_score_name[score_name_pointer] = serial_input;
				printf("%c", serial_input);
				score_name_pointer++;
			}else if(serial_input =='\n'){
				break; 
			}else if (serial_input == BACKSPACE_CHAR){
				printf("%c", ' ');
				if (score_name_pointer >0){score_name_pointer--;}
				move_cursor(10 + score_name_pointer, 9);
			}
			}
		}
		// handle sequence into EEPFROM
		while(1){
			if(eeprom_is_ready()){
				handle_sequence_into_EEPFROM();
				break; 
			}
		}
	}
void handle_sequence_into_EEPFROM(){
	eeprom_write_dword(HIGHSCORE_SIGNATURE_MEMORY, HIGHSCORE_SIGNATURE);
	switch(high_score_position()){
		case SCORE_FOUR: move_four_place_down(); break; 
		case SCORE_THREE: move_three_place_down(); break; 
		case SCORE_TWO: move_two_place_down(); break;
		case SCORE_ONE: move_one_place_down(); break; 
	}
	
	eeprom_write_dword(high_score_position(), get_score());
	eeprom_write_byte(high_score_name_position(high_score_position()), current_high_score_name[0]);
	eeprom_write_byte(high_score_name_position(high_score_position()) +0x08, current_high_score_name[1]);
	eeprom_write_byte(high_score_name_position(high_score_position()) +0x10, current_high_score_name[2]);
	
	clear_terminal();
}

void move_place_down(uint16_t score_position, uint16_t name_position){
	uint32_t score = eeprom_read_dword(score_position); 
	uint8_t char1 = eeprom_read_byte(name_position);
	uint8_t char2 = eeprom_read_byte(name_position+0x08);
	uint8_t char3 = eeprom_read_byte(name_position+0x10);
	eeprom_write_dword((score_position+0x20), score); 
	eeprom_write_byte(name_position +0x20, char1);
	eeprom_write_byte(name_position +0x28, char2);
	eeprom_write_byte(name_position +0x30, char3);
}

void move_four_place_down(void){
	move_place_down(SCORE_FOUR, NAME_FOUR); 
}

void move_three_place_down(void){
	move_four_place_down(); 
	move_place_down(SCORE_THREE, NAME_THREE);
}

void move_two_place_down(void){
	move_three_place_down();
	move_place_down(SCORE_TWO, NAME_TWO);
}

void move_one_place_down(void){
	move_two_place_down();
	move_place_down(SCORE_ONE, NAME_ONE);
}

void initialise_EEPROM(void){
	eeprom_write_dword(SCORE_ONE, 0x00);
	eeprom_write_dword(SCORE_TWO, 0x00);
	eeprom_write_dword(SCORE_THREE, 0x00);
	eeprom_write_dword(SCORE_FOUR, 0x00);
	eeprom_write_dword(SCORE_FIVE, 0x00);
	reset_name_at_memory_position(NAME_ONE);
	reset_name_at_memory_position(NAME_TWO);
	reset_name_at_memory_position(NAME_THREE);
	reset_name_at_memory_position(NAME_FOUR);
	reset_name_at_memory_position(NAME_FIVE);
	eeprom_write_dword(HIGHSCORE_SIGNATURE_MEMORY, HIGHSCORE_SIGNATURE);
	}
	
void reset_name_at_memory_position(uint16_t mem_pos){
	eeprom_write_byte(mem_pos, UNDERSCORE);
	eeprom_write_byte(mem_pos+0x08, UNDERSCORE);
	eeprom_write_byte(mem_pos+0x10, UNDERSCORE);
}


int EEPROM_High_Score_has_been_initialised(void){
	uint32_t current_signature = eeprom_read_dword(HIGHSCORE_SIGNATURE_MEMORY);
	return (current_signature == HIGHSCORE_SIGNATURE);
}

void display_EEPROM_high_score(void){
	move_cursor(10,11);
	printf_P(PSTR("###HIGH SCORES###"));
	move_cursor(10,13);
	//initialise_EEPROM();
	if(EEPROM_High_Score_has_been_initialised()){

		printf("First: %c%c%c %d", eeprom_read_byte(NAME_ONE), 
									eeprom_read_byte(NAME_ONE+0x08), 
									eeprom_read_byte(NAME_ONE+0x10), 
									eeprom_read_dword(SCORE_ONE));
		move_cursor(10,14);					
		
		printf("Second: %c%c%c %d", eeprom_read_byte(NAME_TWO),
									eeprom_read_byte(NAME_TWO+0x08),
									eeprom_read_byte(NAME_TWO+0x10),
									eeprom_read_dword(SCORE_TWO));
		move_cursor(10,15);

		printf("Third: %c%c%c %d", eeprom_read_byte(NAME_THREE),
									eeprom_read_byte(NAME_THREE +8),
									eeprom_read_byte(NAME_THREE + 16),
									eeprom_read_dword(SCORE_THREE));
		move_cursor(10,16);

		printf("Four: %c%c%c %d \n", eeprom_read_byte(NAME_FOUR),
									eeprom_read_byte(NAME_FOUR+8),
									eeprom_read_byte(NAME_FOUR+16),
									eeprom_read_dword(SCORE_FOUR));
		move_cursor(10,17);

		printf("Five: %c%c%c %d", eeprom_read_byte(NAME_FIVE),
								eeprom_read_byte(NAME_FIVE+8),
								eeprom_read_byte(NAME_FIVE+16),
								eeprom_read_dword(SCORE_FIVE));
	}
	else{
		initialise_EEPROM();
		printf("There Are Currently No HighScores");
	}

}





