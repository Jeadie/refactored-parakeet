


#ifndef RATS_H_
#define RATS_H_

#include "position.h"
PosnType get_rat_position(void);

void move_rat(void);

void add_new_rat_position(void);

uint8_t is_item_at(PosnType position);
void set_rat_at_pos(PosnType new_pos);
PosnType get_rat_position(void);
uint8_t rat_jumped_the_board(PosnType new_pos);
uint8_t is_rat_at(PosnType pos);

uint8_t is_time_to_move_rat(void);

#endif

