/*
 * Buzzer.h
 *
 * Created: 22/05/2017 2:49:34 PM
 *  Author: Jack Eadie
 */ 


#ifndef BUZZER_H_
#define BUZZER_H_
uint8_t sound_effects_on_mode(void);
void init_buzzer(void);
void reduce_buzzer_timer(void);
void set_silent_sound(void);
void reset_buzzer(void);
int buzzer_time_left(void);
void next_buzzer_tone(void);
void play_start_game_sound_effect(void);
void play_eating_food_sound_effect(void);
void play_end_game_sound_effect(void);
void handle_buzzer_loop(void); 
int last_tone_in_sequence(void);

#endif /* BUZZER_H_ */