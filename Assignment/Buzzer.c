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

//  Will get incremented to zero when a sound effect is loaded. 
int buzzer_pointer = -1;
int buzzer_durations[5];
int buzzer_OCR1A[5];
int number_of_tones;  

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
		//  printf("END TONE \n");
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
	
	buzzer_pointer++;
	change_OCR1A(buzzer_OCR1A[buzzer_pointer]);
	//  printf_P(PSTR("ptr: %d, ocr1a: %d\n"), buzzer_pointer, buzzer_OCR1A[buzzer_pointer]);
	//change_timer_one_frequency(buzzer_frequencies[buzzer_pointer]);

	clock_at_tone_start = get_clock_ticks();
	
	//  How long this should be 
	buzzer_time = buzzer_durations[buzzer_pointer];
}


void reset_buzzer(void){
	DDRD &= ~(1<<5);
	buzzer_pointer = -1; 
	// printf("reset Buzzer \n");
}

void play_start_game_sound_effect(void){
	
	number_of_tones = 3;
	buzzer_durations[0] = 500; 
	buzzer_durations[1] = 200; 
	buzzer_durations[2] = 500;
	buzzer_OCR1A[0] = 2000;
	buzzer_OCR1A[1] = 1000;
	buzzer_OCR1A[2] = 2000;

	buzzer_pointer = -1;
	next_buzzer_tone(); 
	if(sound_effects_on_mode()){
		enable_timer_one(); 
	}
}

void play_snake_move_sound_effect(void){
		number_of_tones = 1; 
		buzzer_OCR1A[0] = 2000;
		buzzer_durations[0] = 50;
		next_buzzer_tone();
		if(sound_effects_on_mode()){
			enable_timer_one();
		}		
}


void play_eating_food_sound_effect(void){
		number_of_tones = 2; 
		buzzer_durations[0] = 50;
		buzzer_durations[1] = 100;
		//  printf_P(PSTR("eating food, %d: \n"), number_of_tones); 
		buzzer_OCR1A[0] = 2000;
		buzzer_OCR1A[1] = 1000;

		buzzer_pointer = -1;
		next_buzzer_tone();
		if(sound_effects_on_mode()){
			enable_timer_one();
		}
}


int last_tone_in_sequence(void){
	return (buzzer_pointer >= number_of_tones-1);
	}

