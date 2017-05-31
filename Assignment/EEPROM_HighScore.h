/*
 * EEPROM_HighScore.h
 *
 * Created: 24/05/2017 10:59:51 AM
 *  Author: Jack Eadie
 */ 


#ifndef EEPROM_HIGHSCORE_H_
#define EEPROM_HIGHSCORE_H_

#define ESCAPE_CHAR 27
#define ENTER_CHAR 13
#define BACKSPACE_CHAR 127
#define FOUR_SPACES 0x2D2D2D
#define UNDERSCORE 95

#define NAME_ONE 0x40
#define NAME_TWO 0x80
#define NAME_THREE 0xC0
#define NAME_FOUR 0x100
#define NAME_FIVE 0x140

#define HIGHSCORE_SIGNATURE_MEMORY 0x0
#define HIGHSCORE_SIGNATURE 0x1234




uint8_t is_high_score(void);
uint16_t high_score_position(void);
void insert_into_EEPROM(char name[], int position);
void set_high_score_name(void);
void initialise_EEPROM(void);
void display_EEPROM_high_score(void);
void handle_sequence_into_EEPROM(); 
void move_place_down(uint16_t mem_position); 
void move_four_place_down(void);
void move_three_place_down(void);
void move_two_place_down(void);
void move_one_place_down(void);
uint8_t EEPROM_High_Score_has_been_initialised(void);




#endif /* EEPROM_HIGHSCORE_H_ */