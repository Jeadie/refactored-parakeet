/*
 * score.h
 * 
 * Author: Peter Sutton
 */

#ifndef SCORE_H_
#define SCORE_H_

#include <stdint.h>

void init_score(void);
void add_to_score(uint16_t value);
uint32_t get_score(void);
void set_score(uint32_t new_score);

#endif /* SCORE_H_ */