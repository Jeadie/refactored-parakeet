/*
*  SuperFood Methods to help with timing and placement
*
*
*
*/
#ifndef SUPERFOOD_H_
#define SUPERFOOD_H_

uint8_t time_to_add_superfood(void);
uint8_t load_superfood(PosnType loaded_pos);

void eat_superfood_from_board(void);
uint8_t time_to_remove_superfood(void);

void add_superfood_to_board(void);

void remove_superfood_from_board(void);
uint8_t superfood_is_on_board(void);


PosnType get_superfood_position(void);

uint8_t is_superfood_at(PosnType pos);

#endif