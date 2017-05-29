/*
 * Buzzer.c
 *
 * Created: 22/05/2017 2:49:17 PM
 *  Author: Jack Eadie
 */ 
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <stdio.h>
#include "terminalio.h"

#include "timer1.h"
#include "Buzzer.h"
#include "timer0.h"
// TODO: get buzzer sounds working continuously
// TODO: add buzzer switch into configuration

int buzzer_time = 0; 
uint32_t  clock_at_tone_start; 

struct sound_effect{
	uint16_t buzzer_OCR1A[3];
	uint8_t  durations[3];
	uint8_t  array_pointer;
};

struct sound_effect current_tone; 
struct sound_effect silent_sound; 

// silent_sound.buzzer_OCR1A[0] = 0; 
// silent_sound.buzzer_OCR1A[1] = 0;
// silent_sound.buzzer_OCR1A[2] = 0;
// silent_sound.durations[0] = 1;
// silent_sound.durations[1] = 1;
// silent_sound.durations[2] = 1; 
// silent_sound.array_pointer = -1; 

//  Will get incremented to zero when a sound effect is loaded. 
int buzzer_pointer = -1;
int buzzer_durations[5];
int buzzer_OCR1A[5];
int number_of_tones;  

void set_silent_sound(void){
	current_tone = silent_sound; 
	silent_sound.array_pointer = -1; 
	next_buzzer_tone(); 
}

uint8_t sound_effects_on_mode(void){
	return (PIND & (1<<3)) != 0 ;
}

int buzzer_time_left(void){
	if(get_clock_ticks() < (clock_at_tone_start + buzzer_time)){
		return 1;
	}else{
		return 0;
	}
}
void handle_buzzer_loop(void){
	if((!buzzer_time_left())&& last_tone_in_sequence()){
		move_cursor(10, 7); 
		printf("END TONE \n");
		reset_buzzer();
	}
	else if((!buzzer_time_left())){
		next_buzzer_tone();
	}
}


int buzzer_times(void){
	return buzzer_time;
}

void next_buzzer_tone(void){
	
	current_tone.array_pointer++;
	change_OCR1A(current_tone.buzzer_OCR1A[current_tone.array_pointer]);
	//  printf_P(PSTR("ptr: %d, ocr1a: %d\n"), buzzer_pointer, buzzer_OCR1A[buzzer_pointer]);
	//change_timer_one_frequency(buzzer_frequencies[buzzer_pointer]);

	clock_at_tone_start = get_clock_ticks();
	
	//  How long this should be 
	buzzer_time = current_tone.durations[current_tone.array_pointer]; 
}


void reset_buzzer(void){
	DDRD &= ~(1<<5);
// 	current_tone = silent_sound; 
// 	current_tone.array_pointer = -1; 
// 	next_buzzer_tone(); 
// 	while (buzzer_pointer >=0)
// 	{
// 		buzzer_durations[buzzer_pointer] = 0; 
// 		buzzer_OCR1A[buzzer_pointer] = 0; 
// 		buzzer_pointer--; 
// 	}
// 	printf("reset Buzzer \n");
}

void play_start_game_sound_effect(void){
	struct sound_effect start_tone; 
	start_tone.buzzer_OCR1A[0] = 2000;
	start_tone.buzzer_OCR1A[1] = 1000;
	start_tone.buzzer_OCR1A[2] = 1500;
	start_tone.durations[0] = 500;
	start_tone.durations[1] = 1500;
	start_tone.durations[2] = 500;
	start_tone.array_pointer = -1;

	next_buzzer_tone(); 
	if(sound_effects_on_mode()){
		enable_timer_one(); 
	}
}

void play_snake_move_sound_effect(void){
		struct sound_effect move_snake;
		move_snake.buzzer_OCR1A[0] = 2000;
		move_snake.buzzer_OCR1A[1] = 1000;
		move_snake.durations[0] = 50;
		move_snake.durations[1] = 100;
		move_snake.array_pointer = -1;

		next_buzzer_tone();
		if(sound_effects_on_mode()){
			enable_timer_one();
		}
}


void play_eating_food_sound_effect(void){
		play_snake_move_sound_effect();
}


int last_tone_in_sequence(void){
	return (current_tone.array_pointer >=2);
	}

