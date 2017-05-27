/*
 * game.h
 *
 * Written by Peter Sutton
 */ 

#ifndef GAME_H_
#define GAME_H_

#include <inttypes.h>

// Initialise game. This initialises the board with snake and food items
// and initialises the display.
void init_game(void);
void set_game_speed(uint16_t new_speed); 
// Attempt to move snake forward. If food is eaten it removes it, grows
// the snake if possible and replaces the food item with a new one.
// Display is updated as required. Returns true if successful, 
// false otherwise (move off board if not permitted, or snake collides
// with self). (Moves off board and collisions permitted in initially 
// supplied code.)
int8_t attempt_to_move_snake_forward(void);
void increase_game_speed(void);
void reset_game_speed(void);
int get_current_game_speed(void);
static volatile uint16_t current_speed; 
void init_score_on_terminal(void);
void update_terminal_score(void);
#endif /* GAME_H_ */