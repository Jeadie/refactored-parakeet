/*
 * timer1.c
 *
 * Created: 22/05/2017 2:27:31 PM
 *  Author: Jack Eadie
 */ 


#include <avr/io.h>
#include <avr/interrupt.h>
#include "timer1.h"
#include <stdint.h>
#include "Buzzer.h"

#define  timer_frequency 8000000

void initialise_timer_one(void){
		TCNT1 = 0;
		//  printf("init timer one"); 
		//change_timer_one_frequency(5000);
		// TCCR1B |= 1<<CS12; // | (1<<CS10));
		TCCR1B |= ((1<<CS11)  | (1<<WGM12)); //  | 1<<CS12; 
		TCCR1A = 1<<COM1A0;
		OCR1A = 1000;
		DDRD &= ~(1<<5); 
		DDRD &= ~(1<<3); 
}
void change_timer_one_frequency(int new_frequency){
	//  
	uint16_t new_OCR1A = (8000000/(new_frequency * 8)) -1;
	OCR1A = new_OCR1A;
	//  printf("frequency: %u \n", new_frequency);
	
	//  printf("OCR1a: %u \n", OCR1A); 
	// OCR1A = 1000;
}

void change_OCR1A(int new_OCR1A){
	OCR1A = new_OCR1A;
}
 void enable_timer_one(void){
	DDRD |= (1<<5); 
	//  printf("enable Timer one \n");
 }
 
 
