/*
 * Buzzer.h
 *
 * Created: 22/05/2017 2:49:34 PM
 *  Author: Jack Eadie
 */ 


#ifndef BUZZER_H_
#define BUZZER_H_
uint8_t sound_effects_on_mode(void);
void reduce_buzzer_timer(void);
void reset_buzzer(void);
int buzzer_time_left(void);
void next_buzzer_tone(void);
void play_start_game_sound_effect(void);
void play_eating_food_sound_effect(void);
int last_tone_in_sequence(void);

#endif /* BUZZER_H_ */