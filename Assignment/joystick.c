/*
 * joystick.c
 *
 * Created: 31/05/2017 10:54:00 AM
 *  Author: Jack Eadie
 */ 
#include "serialio.h"
#include <stdio.h>
#include <avr/interrupt.h>
#include "snake.h"
	uint16_t minimum_threshold = 300;
	uint16_t middle_value = 510; 
	uint16_t value;
	uint8_t x_or_y = 0;	/* 0 = x, 1 = y */

void init_joystick(void){
	/* Set up the serial port for stdin communication at 19200 baud, no echo */
	init_serial_stdio(19200,0);
	
	/* Turn on global interrupts */
	sei();
	
	// Set up ADC - AVCC reference, right adjust
	// Input selection doesn't matter yet - we'll swap this around in the while
	// loop below.
	ADMUX = (1<<REFS0);
	// Turn on the ADC (but don't start a conversion yet). Choose a clock
	// divider of 64. (The ADC clock must be somewhere
	// between 50kHz and 200kHz. We will divide our 8MHz clock by 64
	// to give us 125kHz.)
	ADCSRA = (1<<ADEN)|(1<<ADPS2)|(1<<ADPS1);
	
	/* Print a welcome message
	*/
	printf("ADC Test\n");
}

uint8_t read_joystick(void){
	 // Set the ADC mux to choose ADC0 if x_or_y is 0, ADC1 if x_or_y is 1
	if(x_or_y == 0) {
		ADMUX &= ~1;
		} else {
		ADMUX |= 1;
	}
	// Start the ADC conversion
	ADCSRA |= (1<<ADSC);

	while(ADCSRA & (1<<ADSC)) {
		; /* Wait until conversion finished */
	}
	value = ADC; // read the value
	if(x_or_y == 0) {
		printf("X: %4d ", value);
		} else {
		printf("Y: %4d\n", value);
	}
	// Next time through the loop, do the other direction
	x_or_y ^= 1;
	return value; 
}

int8_t get_joystick_direction(void){
	int8_t y_axis, x_axis; 
	
	if (x_or_y){
		y_axis = read_joystick() - middle_value; 
		x_axis = read_joystick()-middle_value; 
	}else{
		x_axis =read_joystick() - middle_value;
		y_axis = read_joystick() - middle_value; 			
		}
	
	if (abs(y_axis)> abs(x_axis) && abs(y_axis) > minimum_threshold){
		if (y_axis >0){return SNAKE_UP;}
			else{return SNAKE_DOWN;}
	}else if(abs(x_axis) >minimum_threshold){
		if (x_axis >0){return SNAKE_RIGHT;}
			else{return SNAKE_LEFT;}
	
	}
	return -1; 
} 
	
	