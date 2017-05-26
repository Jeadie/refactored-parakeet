/*
 * EEPROM_Game_Save.h
 *
 * Created: 24/05/2017 4:15:44 PM
 *  Author: Jack Eadie
 */ 


#ifndef EEPROM_GAME_SAVE_H_
#define EEPROM_GAME_SAVE_H_
#define SAVE_GAME_SIGNATURE 00011000
#define SAVE_GAME_SIGNATURE_MEMORY 0x148

#define FOOD_ONE 0x150
#define FOOD_TWO 0x158
#define FOOD_THREE 0x160
#define RAT 0x168
#define SUPERFOOD 0x170
#define GAME_SPEED 0x178
#define GAME_SCORE 0x188
#define SNAKE_LENGTH 0x1A8
#define CLOCK_TICKS 0x1B0
#define SNAKE_POSITION_HEAD 0x1C0

void save_game_to_EPPROM(void);
// void load_food_from_EEPROM(void);
// void load_rat_from_EEPROM(void);
// void load_rat_from_superfood(void);
// void load_snake_from_EPPROM(void);

#endif /* EEPROM_GAME_SAVE_H_ */