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
#include "score.h"
#include <avr/eeprom.h>


uint8_t is_high_score(void){
	return (get_score() >= eeprom_read_dword(SCORE_FIVE)) || !(EEPROM_High_Score_has_been_initialised()) ;
}

uint16_t high_score_name_position(void){
	if (!(EEPROM_High_Score_has_been_initialised())){
		return NAME_ONE;
	}
	if (get_score() > eeprom_read_dword(SCORE_ONE)){
		return NAME_ONE;
	}
	if (get_score() > eeprom_read_dword(SCORE_TWO)){
		return NAME_TWO;
	}
	if (get_score() > eeprom_read_dword(SCORE_THREE)){
		return NAME_THREE;
	}
	if (get_score() > eeprom_read_dword(SCORE_FOUR)){
		return NAME_FOUR;
	}
	if (get_score() > eeprom_read_dword(SCORE_FIVE)){
		return NAME_FIVE;
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
	char score_name[4];
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
			if (serial_input>= 65 && serial_input <= 122 && score_name_pointer <=3){
				move_cursor(10 + score_name_pointer, 9);
				score_name[score_name_pointer] = serial_input;
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
				eeprom_write_byte(HIGHSCORE_SIGNATURE_MEMORY, HIGHSCORE_SIGNATURE);

				eeprom_write_dword(high_score_position(), get_score());
				eeprom_write_byte(high_score_name_position(), score_name[0]);
				eeprom_write_byte(high_score_name_position() +8, score_name[1]); 
				eeprom_write_byte(high_score_name_position() +16, score_name[2]);
				
				clear_terminal();
				printf("a: %u", get_score());
				printf("b: %u ", eeprom_read_dword(high_score_position()));
				printf("c: %c", eeprom_read_byte(high_score_name_position()));
				break;
			}
		}
	}

void initialise_EEPFROM(void){
	eeprom_write_dword(SCORE_ONE, 0x00 );
	eeprom_write_dword(SCORE_TWO, 0x00);
	eeprom_write_dword(SCORE_THREE, 0x00);
	eeprom_write_dword(SCORE_FOUR, 0x00);
	eeprom_write_dword(SCORE_FIVE, 0x00);
	eeprom_write_byte(HIGHSCORE_SIGNATURE_MEMORY, HIGHSCORE_SIGNATURE);
	}


int EEPROM_High_Score_has_been_initialised(void){
	uint8_t current_signature = eeprom_read_byte(HIGHSCORE_SIGNATURE_MEMORY);
	return (current_signature == HIGHSCORE_SIGNATURE);
}

void display_EEPROM_high_score(void){
	move_cursor(10,17);
	initialise_EEPFROM();
	if(EEPROM_High_Score_has_been_initialised()){

		printf("First: %s: %d", eeprom_read_dword(NAME_ONE), eeprom_read_dword(SCORE_ONE));
		move_cursor(10,18);
		printf("Second: %s: %d", eeprom_read_dword(NAME_TWO), eeprom_read_dword(SCORE_TWO));
		move_cursor(10,19);
		printf("Third: %s: %d", eeprom_read_dword(NAME_THREE), eeprom_read_dword(SCORE_THREE));
		move_cursor(10,20);
		printf("Fourth: %s: %d", eeprom_read_dword(NAME_FOUR), eeprom_read_dword(SCORE_FOUR));
		move_cursor(10,21);
		printf("Fifth: %s: %d", eeprom_read_dword(NAME_FIVE), eeprom_read_dword(SCORE_FIVE));
	}
	else{
		printf("There Are Currently No HighScores");
	}
}




