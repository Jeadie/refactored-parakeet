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
#define NAME_ONE 0x20
#define NAME_TWO 0x40
#define NAME_THREE 0x60
#define NAME_FOUR 0x80
#define NAME_FIVE 0xA0

#define SCORE_ONE 0xC0
#define SCORE_TWO 0xE0
#define SCORE_THREE 0x100
#define SCORE_FOUR 0x120
#define SCORE_FIVE 0x140
#define HIGHSCORE_SIGNATURE_MEMORY 0x160
#define HIGHSCORE_SIGNATURE 0x1234




uint8_t is_high_score(void);
uint16_t high_score_position(void);
void insert_into_EEPROM(char name[], int position);
void set_high_score_name(void);
void initialise_EEPROM(void);
uint16_t EEPROM_has_been_initialised(void);
void display_EEPROM_high_score(void);




#endif /* EEPROM_HIGHSCORE_H_ */