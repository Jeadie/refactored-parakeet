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

static char current_high_score_name[3]; 

struct High_Score_Place {
	char name[3];
	uint32_t score;
};

uint8_t is_high_score(void){
	uint32_t score = get_score();
	struct High_Score_Place current_score_place;
	eeprom_read_block(&current_score_place, NAME_FIVE, 7);
	return (score >= current_score_place.score || !(EEPROM_High_Score_has_been_initialised())) ;
}

uint16_t high_score_position(void){
	if (!(EEPROM_High_Score_has_been_initialised())){
		return NAME_ONE;
	}
	
	uint32_t score = get_score(); 
	struct High_Score_Place current_score_place; 
	eeprom_read_block(&current_score_place, NAME_ONE, 7);

	if (score> current_score_place.score){
		return NAME_ONE;
	}
	
	eeprom_read_block(&current_score_place, NAME_TWO, 7);
	if (score> current_score_place.score){
		return NAME_TWO;
	}
	
	eeprom_read_block(&current_score_place, NAME_THREE, 7);	
	if (score> current_score_place.score){
		return NAME_THREE;
	}
	
	eeprom_read_block(&current_score_place, NAME_FOUR, 7);
	if (score> current_score_place.score){
		return NAME_FOUR;
	}
	
	eeprom_read_block(&current_score_place, NAME_FIVE, 7);
	if (score> current_score_place.score){
		return NAME_FIVE;
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
			}else if(serial_input =='\n' && score_name_pointer>1){
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
				current_high_score_name[0] = ' ';
				current_high_score_name[1] = ' ';
				current_high_score_name[2] = ' ';
				break; 
			}
		}
	}
void handle_sequence_into_EEPFROM(){
	eeprom_write_dword(HIGHSCORE_SIGNATURE_MEMORY, HIGHSCORE_SIGNATURE);
	uint16_t score_position= high_score_position();
		switch(score_position){
			case NAME_FOUR: move_four_place_down(); break;
			case NAME_THREE: move_three_place_down(); break;
			case NAME_TWO: move_two_place_down(); break;
			case NAME_ONE: move_one_place_down(); break;
		}
	struct High_Score_Place new_score; 
	new_score.name[0] = current_high_score_name[0];
	new_score.name[1] = current_high_score_name[1];
	new_score.name[2] = current_high_score_name[2];
	new_score.score = get_score();
	
	eeprom_write_block(&new_score, score_position, sizeof(new_score));
		
	clear_terminal();
}

void move_place_down(uint16_t mem_position){
	struct High_Score_Place high_score_place;
	eeprom_read_block(&high_score_place, mem_position, 7); 
	eeprom_write_block(&high_score_place, mem_position +64, 7);
}

void move_four_place_down(void){
	move_place_down(NAME_FOUR); 
}

void move_three_place_down(void){
	move_four_place_down(); 
	move_place_down(NAME_THREE);
}

void move_two_place_down(void){
	move_three_place_down();
	move_place_down(NAME_TWO);
}

void move_one_place_down(void){
	move_two_place_down();
	move_place_down(NAME_ONE);
}

void initialise_EEPROM(void){
	eeprom_write_dword(HIGHSCORE_SIGNATURE_MEMORY, HIGHSCORE_SIGNATURE);
	struct High_Score_Place empty_score_place;
	empty_score_place.name[0] = ' ';
	empty_score_place.name[1] = ' ';
	empty_score_place.name[2] = ' ';
	empty_score_place.score = 0; 
	eeprom_write_block(&empty_score_place, NAME_ONE, 7);
	eeprom_write_block(&empty_score_place, NAME_TWO, 7);
	eeprom_write_block(&empty_score_place, NAME_THREE, 7);
	eeprom_write_block(&empty_score_place, NAME_FOUR, 7);
	eeprom_write_block(&empty_score_place, NAME_FIVE, 7);

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
		struct High_Score_Place score_place_to_display; 
		eeprom_read_block(&score_place_to_display, NAME_ONE, 7);
		printf("First: %c%c%c %d", score_place_to_display.name[0], 
									score_place_to_display.name[1], 
									score_place_to_display.name[2], 
									score_place_to_display.score);
		
		move_cursor(10,14);					
		
		eeprom_read_block(&score_place_to_display, NAME_TWO, 7);
		printf("Second: %c%c%c %d", score_place_to_display.name[0],
									score_place_to_display.name[1],
									score_place_to_display.name[2],
									score_place_to_display.score);
		move_cursor(10,15);

		eeprom_read_block(&score_place_to_display, NAME_THREE, 7);
		printf("Third: %c%c%c %d", score_place_to_display.name[0],
									score_place_to_display.name[1],
									score_place_to_display.name[2],
									score_place_to_display.score);
		move_cursor(10,16);

		eeprom_read_block(&score_place_to_display, NAME_FOUR, 7);
		printf("Fourth: %c%c%c %d", score_place_to_display.name[0],
									score_place_to_display.name[1],
									score_place_to_display.name[2],
									score_place_to_display.score);
		move_cursor(10,17);

		eeprom_read_block(&score_place_to_display, NAME_FIVE, 7);
		printf("Fifth: %c%c%c %d", score_place_to_display.name[0],
									score_place_to_display.name[1],
									score_place_to_display.name[2],
									score_place_to_display.score);
	}
	else{
		initialise_EEPROM();
		printf("There Are Currently No HighScores");
	}

}





