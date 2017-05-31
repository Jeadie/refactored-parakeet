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
/// #include "terminalio.h"

#include "timer1.h"
#include "Buzzer.h"
#include "timer0.h"
// TODO: get buzzer sounds working continuously
// TODO: add buzzer switch into configuration
#define START_LENGTH 3
uint16_t START_FREQ[START_LENGTH] = {5000, 2000, 5000};
uint16_t START_DUR[START_LENGTH] = {500, 500, 500};

#define EAT_LENGTH 2
uint16_t EAT_FREQ[START_LENGTH] = {3000, 1500};
uint16_t EAT_DUR[START_LENGTH] = {500, 200};
	
#define BEGIN_LENGTH 4
uint16_t BEGIN_FREQ[BEGIN_LENGTH] = {4500, 3000, 1500, 1};
uint16_t BEGIN_DUR[BEGIN_LENGTH] = {300, 400, 500, 0};
	
#define END_LENGTH 4
uint16_t END_FREQ[END_LENGTH] = {1500, 3000, 4500, 1};
uint16_t END_DUR[END_LENGTH] = {500, 400, 300, 0};

int buzzer_time = 0;
uint32_t  clock_at_tone_start;
uint8_t current_index; // =-1;

typedef struct SoundEffect {
	uint16_t *buzzer_OCR1A;
	uint16_t  *durations;
	uint8_t length;
} SoundEffect; // current_tone, Start_tone, End_tone, Eat_tone; //


SoundEffect end_tone;
SoundEffect eat_tone;
SoundEffect Begin_tone;
SoundEffect Start_tone; 

SoundEffect Start_tone=
{
	.buzzer_OCR1A = &START_FREQ,
	.durations=  &START_DUR,
	.length = START_LENGTH,
};
SoundEffect Eat_tone=
{
	.buzzer_OCR1A = &EAT_FREQ,
	.durations=  &EAT_DUR,
	.length = EAT_LENGTH,
};
SoundEffect Begin_tone=
{
	.buzzer_OCR1A = &BEGIN_FREQ,
	.durations=  &BEGIN_DUR,
	.length = BEGIN_LENGTH,
};
SoundEffect end_tone=
{
	.buzzer_OCR1A = &END_FREQ,
	.durations=  &END_DUR,
	.length = END_LENGTH,
};


SoundEffect *current_tone;
void add_sounds(void){
	//SoundEffect *current_tone = &start_tone;
}

void init_buzzer() {
	// printf("tone_length %u, tone duration %u", current_tone->length, current_tone->durations[0]);
	add_sounds();
}

uint8_t sound_effects_on_mode(void){
	return 1; // (PIND & (1<<3)) != 0 ;
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
		//move_cursor(10, 7);
		//printf("END TONE \n");
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
	current_index ++;
	//printf("next tone");
	change_OCR1A(current_tone->buzzer_OCR1A[current_index]);
	//  printf_P(PSTR("ptr: %d, ocr1a: %d\n"), buzzer_pointer, buzzer_OCR1A[buzzer_pointer]);
	//change_timer_one_frequency(buzzer_frequencies[buzzer_pointer]);

	clock_at_tone_start = get_clock_ticks();
	
	//  How long this should be
	buzzer_time = (*current_tone).durations[current_index];
}


void reset_buzzer(void){
	DDRD &= ~(1<<5);
	//current_index = -1; 
	//printf_P(PSTR("ended")); 
	current_tone = NULL; 
}

void play_start_game_sound_effect(void){
	current_tone = &Begin_tone;
	current_index = -1;
	next_buzzer_tone();
	if(sound_effects_on_mode()){
		enable_timer_one();
	}
}

void play_eating_food_sound_effect(void){
	current_tone = &Eat_tone;
	current_index = -1;
	next_buzzer_tone();
	if(sound_effects_on_mode()){
		enable_timer_one();
	}
}



void play_end_game_sound_effect(void){
	current_tone = &end_tone;
	current_index = -1;
	next_buzzer_tone();
	if(sound_effects_on_mode()){
		enable_timer_one();
	}
}


int last_tone_in_sequence(void){
	return current_index+1 >= current_tone->length;
}




