/*
 * EEPROM_Game_Save.h
 *
 * Created: 24/05/2017 4:15:44 PM
 *  Author: Jack Eadie
 */ 


#ifndef EEPROM_GAME_SAVE_H_
#define EEPROM_GAME_SAVE_H_
#define SAVE_GAME_SIGNATURE 00011000
#define SAVE_GAME_SIGNATURE_MEMORY 0x180 // (uint8_t*)0x180

#define SAVED_DATA_START (uint16_t*)0x1A0
#define FOOD_TWO 0x1A8
#define FOOD_THREE 0x1B0
#define FOOD_FOUR 0x1B8
#define RAT 0x1C0
#define SUPERFOOD 0x1C8
#define GAME_SPEED 0x1D0
#define GAME_SCORE 0x1E0
#define SNAKE_LENGTH 0x200
#define CLOCK_TICKS 0x208
#define SNAKE_DIRECTION 0x218
#define SNAKE_POSITION_TAIL 0x230
#include "position.h"
 
void save_game_to_EPPROM(void);
uint8_t EEPROM_has_saved_game(void); 
void new_game_from_EEPROM(void);
void load_food_from_EEPROM(void);
void load_rat_from_EEPROM(PosnType rat_pos);
void load_superfood_from_EEPROM(PosnType super_food);
void add_food_to_board(PosnType food_pos);

void load_snake_from_EPPROM(void);

#endif /* EEPROM_GAME_SAVE_H_ */