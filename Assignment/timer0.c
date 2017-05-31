#include <avr/io.h>
#include <avr/interrupt.h>
#include "timer0.h"
// #include "snake.h"
uint8_t volatile SSD_CC_value = 1; 
uint8_t seven_seg_for_tens[10] = {0,6,91,79,102,109,125,7,127,111};
uint8_t seven_seg_for_ones[10] = { 63,6,91,79,102,109,125,7,127,111};


/* Our internal clock tick count - incremented every 
 * millisecond. Will overflow every ~49 days. */

static volatile uint32_t clock_ticks; 
static volatile uint8_t unpaused_game = 1; 
//static volatile uint8_t SSD_digit_option=0; 
/* Set up timer 0 to generate an interrupt every 1ms. 
 * We will divide the clock by 64 and count up to 124.
 * We will therefore get an interrupt every 64 x 125
 * clock cycles, i.e. every 1 milliseconds with an 8MHz
 * clock. 
 * The counter will be reset to 0 when it reaches it's
 * output compare value.
 */
void init_timer0(void) {
	// Reset clock tick count. L indicates a long (32 bit) constant. 
	clock_ticks = 0L;
	
	/* Clear the timer */
	TCNT0 = 0;

	/* Set the output compare value to be 124 */
	OCR0A = 124;
	
	/* Set the timer to clear on compare match (CTC mode)
	 * and to divide the clock by 64. This starts the timer
	 * running.
	 */
	TCCR0A = (1<<WGM01);
	TCCR0B = (1<<CS01)|(1<<CS00);

	/* Enable an interrupt on output compare match. 
	 * Note that interrupts have to be enabled globally
	 * before the interrupts will fire.
	 */
	TIMSK0 |= (1<<OCIE0A);
	
	/* Make sure the interrupt flag is cleared by writing a 
	 * 1 to it.
	 */
	TIFR0 &= (1<<OCF0A);
}

void toggle_timer_clock(void){
	unpaused_game = 1- unpaused_game;
	// TIMSK0 ^= (1<<OCIE0A);
}
int timer_is_paused(void){
	return !unpaused_game;
	// return !(TIMSK0 & (1<<OCIE0A)); 
}

uint32_t get_clock_ticks(void) {
	uint32_t return_value;

	/* Disable interrupts so we can be sure that the interrupt
	 * doesn't fire when we've copied just a couple of bytes
	 * of the value. Interrupts are re-enabled if they were
	 * enabled at the start.
	 */
	// uint8_t interrupts_were_on = bit_is_set(SREG, SREG_I);
	cli();
	return_value = clock_ticks;
	//if(interrupts_were_on) {
	sei();
	//}
	return return_value;
}

void set_clock_ticks(uint32_t clock){
	//uint8_t interrupts_were_on = bit_is_set(SREG, SREG_I);
	cli();
	clock_ticks =clock;
	sei();
	//}
}

/* Interrupt handler which fires when timer/counter 0 reaches 
 * the defined output compare value (every millisecond)
 */
ISR(TIMER0_COMPA_vect) {
	if(unpaused_game){
		clock_ticks++;
	}
	handle_buzzer_loop();
}

void display_SSD_value(){
	//  1 is 10's space.
	//  0 is 1's space.
	
	if (SSD_CC_value) {
		PORTD |= (1<<2);
		PORTC = seven_seg_for_tens[get_snake_length()/10];
	}
	else{
		PORTD &= ~(1<<2);
		PORTC = seven_seg_for_ones[get_snake_length()%10];
	}
	SSD_CC_value = 1-SSD_CC_value; 
}

