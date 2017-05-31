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
	eeprom_read_block(&current_score_place, *(uint16_t*)NAME_FIVE, 7);
	return (score >= current_score_place.score || !(EEPROM_High_Score_has_been_initialised())) ;
}

uint16_t high_score_position(void){
	if (!(EEPROM_High_Score_has_been_initialised())){
		return NAME_ONE;
	}
	
	uint32_t score = get_score(); 
	struct High_Score_Place current_score_place; 
	eeprom_read_block(&current_score_place, *(uint16_t*)NAME_ONE, 7);

	if (score> current_score_place.score){
		return NAME_ONE;
	}
	
	eeprom_read_block(&current_score_place, *(uint16_t*)NAME_TWO, 7);
	if (score> current_score_place.score){
		return NAME_TWO;
	}
	
	eeprom_read_block(&current_score_place, *(uint16_t*)NAME_THREE, 7);	
	if (score> current_score_place.score){
		return NAME_THREE;
	}
	
	eeprom_read_block(&current_score_place, *(uint16_t*)NAME_FOUR, 7);
	if (score> current_score_place.score){
		return NAME_FOUR;
}
	eeprom_read_block(&current_score_place, *(uint16_t*)NAME_FIVE, 7);
	if (score> current_score_place.score){
		return NAME_FIVE;
	}
	return NAME_FIVE; 
	
}

void set_high_score_name(void){
	move_cursor(10,8);
	printf("You've set a high score. please set your initial below: ");
	int score_name_pointer = 0; 
	char serial_input;
	while (1){
		show_cursor(); 
		serial_input = -1;
		
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
				handle_sequence_into_EEPROM();
				current_high_score_name[0] = ' ';
				current_high_score_name[1] = ' ';
				current_high_score_name[2] = ' ';
				break; 
			}
		}
	}
void handle_sequence_into_EEPROM(){
	eeprom_write_dword((uint32_t*)HIGHSCORE_SIGNATURE_MEMORY, HIGHSCORE_SIGNATURE);
	uint16_t score_position= *(uint16_t*)high_score_position();
	// dereferene and cast to int 
	
		switch(score_position){
			case 0x100: move_four_place_down(); break;
			case 0xC0: move_three_place_down(); break;
			case 0x80: move_two_place_down(); break;
			case 0x40: move_one_place_down(); break;
		}
	struct High_Score_Place new_score; 
	new_score.name[0] = current_high_score_name[0];
	new_score.name[1] = current_high_score_name[1];
	new_score.name[2] = current_high_score_name[2];
	new_score.score = get_score();
	
	eeprom_write_block(&new_score, (uint8_t*)score_position, sizeof(new_score));
		
	// clear_terminal();
}

void move_place_down(uint16_t mem_position){
	struct High_Score_Place high_score_place;
	eeprom_read_block(&high_score_place, (uint8_t*)mem_position, 7); 
	uint16_t moved_place = mem_position +64;
	eeprom_write_block(&high_score_place, (uint8_t*)moved_place, 7);
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
	eeprom_write_dword((uint32_t*)HIGHSCORE_SIGNATURE_MEMORY, HIGHSCORE_SIGNATURE);
	struct High_Score_Place empty_score_place;
	empty_score_place.name[0] = ' ';
	empty_score_place.name[1] = ' ';
	empty_score_place.name[2] = ' ';
	empty_score_place.score = 0; 
	eeprom_write_block(&empty_score_place, *(uint16_t*)NAME_ONE, 7);
	eeprom_write_block(&empty_score_place, *(uint16_t*)NAME_TWO, 7);
	eeprom_write_block(&empty_score_place, *(uint16_t*)NAME_THREE, 7);
	eeprom_write_block(&empty_score_place, *(uint16_t*)NAME_FOUR, 7);
	eeprom_write_block(&empty_score_place, *(uint16_t*)NAME_FIVE, 7);

	}


uint8_t EEPROM_High_Score_has_been_initialised(void){
	uint32_t current_signature = eeprom_read_dword((uint32_t*)HIGHSCORE_SIGNATURE_MEMORY);
	return (current_signature == HIGHSCORE_SIGNATURE);
}

void display_EEPROM_high_score(void){
	move_cursor(10,11);
	printf_P(PSTR("###HIGH SCORES###"));
	move_cursor(10,13);
	//initialise_EEPROM();
	if(EEPROM_High_Score_has_been_initialised()){
		struct High_Score_Place score_place_to_display; 
		eeprom_read_block(&score_place_to_display, *(uint16_t*)NAME_ONE, 7);
		printf("First: %c%c%c %lu", score_place_to_display.name[0], 
									score_place_to_display.name[1], 
									score_place_to_display.name[2], 
									score_place_to_display.score);
		
		move_cursor(10,14);					
		
		eeprom_read_block(&score_place_to_display, *(uint16_t*)NAME_TWO, 7);
		printf("Second: %c%c%c %lu", score_place_to_display.name[0],
									score_place_to_display.name[1],
									score_place_to_display.name[2],
									score_place_to_display.score);
		move_cursor(10,15);

		eeprom_read_block(&score_place_to_display, *(uint16_t*)NAME_THREE, 7);
		printf("Third: %c%c%c %lu", score_place_to_display.name[0],
									score_place_to_display.name[1],
									score_place_to_display.name[2],
									score_place_to_display.score);
		move_cursor(10,16);

		eeprom_read_block(&score_place_to_display, *(uint16_t*)NAME_FOUR, 7);
		printf("Fourth: %c%c%c %lu", score_place_to_display.name[0],
									score_place_to_display.name[1],
									score_place_to_display.name[2],
									score_place_to_display.score);
		move_cursor(10,17);

		eeprom_read_block(&score_place_to_display, *(uint16_t*)NAME_FIVE, 7);
		printf("Fifth: %c%c%c %lu", score_place_to_display.name[0],
									score_place_to_display.name[1],
									score_place_to_display.name[2],
									score_place_to_display.score);
	}
	else{
		initialise_EEPROM();
		printf("There Are Currently No HighScores");
	}

}





