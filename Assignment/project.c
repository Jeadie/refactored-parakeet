/*
 * project.c
 *
 * Main file for the Snake Project.
 *
 * Author: Peter Sutton. Modified by <Jack Eadie>
 */ 
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <stdio.h>
#include <stdlib.h>		// For random()

#include "EEPROM_HighScore.h"
#include "EEPROM_Game_Save.h"
#include "ledmatrix.h"
#include "scrolling_char_display.h"
#include "buttons.h"
#include "serialio.h"
#include "terminalio.h"
#include "score.h"
#include "timer0.h"
#include "game.h"
#include "snake.h"
#include "SuperFood.h"
#include "rats.h"

// Define the CPU clock speed so we can use library delay functions
#define F_CPU 8000000L
#include <util/delay.h>

// Function prototypes - these are defined below (after main()) in the order
// given here
void initialise_hardware(void);
void splash_screen(void);
void new_game(void);
void play_game(void);
void handle_game_over(void);
void handle_new_lap(void);

// ASCII code for Escape character
#define ESCAPE_CHAR 27

//  1 iff the next game should be loaded from EEPROM. 
//  if 1, then no game_over handling should occur because game is reset, not lost. 
uint8_t load_EEPROM_data_on_next_game = 0; 

// SSD for tens and ones place

/////////////////////////////// main //////////////////////////////////
int main(void) {
	// Setup hardware and call backs. This will turn on 
	// interrupts.
	initialise_hardware();
	DDRA = 0xff;
	DDRD |= (1<<2);
	DDRD &= ~(1<<3); 
	
	// Show the splash screen message. Returns when display
	// is complete
	splash_screen();
	
	while(1) {
		//  If next game is to be loaded from EEPROM, set up from EEPROM
		//  Else make new game as normal
		if(load_EEPROM_data_on_next_game){new_game_from_EEPROM();}
			else{new_game();}
		play_game();
		
		//  if 1, then no game_over handling should occur because game is reset, not lost. 
		if(!(load_EEPROM_data_on_next_game)){handle_game_over();}
	}
}

// initialise_hardware()
//
// Sets up all of the hardware devices and then turns on global interrupts
void initialise_hardware(void) {
	// Set up SPI communication with LED matrix
	ledmatrix_setup();
	
	// Set up pin change interrupts on the push-buttons
	init_button_interrupts();
	
	// Setup serial port for 19200 baud communication with no echo
	// of incoming characters
	init_serial_stdio(19200, 0);
	
	// Set up our main timer to give us an interrupt every millisecond
	init_timer0();
	
	// Turn on global interrupts
	sei();
}

void splash_screen(void) {
	// Reset display attributes and clear terminal screen then output a message
	set_display_attribute(TERM_RESET);
	clear_terminal();
	
	hide_cursor();	// We don't need to see the cursor when we're just doing output
	display_start_terminal_screen();
	
	// Output the scrolling message to the LED matrix
	// and wait for a push button to be pushed.
	ledmatrix_clear();
	
	// Red message the first time through
	PixelColour colour = COLOUR_RED;
	while(1) {
		set_scrolling_display_text("SNAKE By 44354369", colour);
		// Scroll the message until it has scrolled off the 
		// display or a button is pushed. We pause for 130ms between each scroll.
		while(scroll_display()) {
			_delay_ms(130);
			if(button_pushed() != -1) {
				// A button has been pushed
				return;
			}
		}
		// Message has scrolled off the display. Change colour
		// to a random colour and scroll again.
		switch(random()%4) {
			case 0: colour = COLOUR_LIGHT_ORANGE; break;
			case 1: colour = COLOUR_RED; break;
			case 2: colour = COLOUR_YELLOW; break;
			case 3: colour = COLOUR_GREEN; break;
		}
	}
}

void new_game(void) {
	// Clear the serial terminal
	clear_terminal();
	
	// Initialise the game and display
	init_game();
		
	// Initialise the score
	init_score();
	
	// Delete any pending button pushes or serial input
	empty_button_queue();
	clear_serial_input_buffer();
}

void play_game(void) {
	uint32_t last_move_time;
	int8_t button;
	char serial_input, escape_sequence_char;
	uint8_t characters_into_escape_sequence = 0;
	
	// Record the last time the snake moved as the current time -
	// this ensures we don't move the snake immediately.
	last_move_time = get_clock_ticks();
	
	// We play the game forever. If the game is over, we will break out of
	// this loop. The loop checks for events (button pushes, serial input etc.)
	// and on a regular basis will move the snake forward.

	while(1) {
			if(time_to_add_superfood()){
				add_superfood_to_board();}
			if(time_to_remove_superfood()){
				remove_superfood_from_board();}
			if(is_time_to_move_rat()){move_rat();}
			handle_buzzer_loop();
		// Check for input - which could be a button push or serial input.
		// Serial input may be part of an escape sequence, e.g. ESC [ D
		// is a left cursor key press. We will be processing each character
		// independently and can't do anything until we get the third character.
		// At most one of the following three variables will be set to a value 
		// other than -1 if input is available.
		// (We don't initalise button to -1 since button_pushed() will return -1
		// if no button pushes are waiting to be returned.)
		// Button pushes take priority over serial input. If there are both then
		// we'll retrieve the serial input the next time through this loop
		serial_input = -1;
		escape_sequence_char = -1;
		if (timer_is_paused()){
			button = -1;
			empty_button_queue();
			}else{
		button = button_pushed();}
		// printf("%i\n", timer_is_paused());
		if(button == -1) {

			// No push button was pushed, see if there is any serial input
			if(serial_input_available()) {
				// Serial data was available - read the data from standard input
				serial_input = fgetc(stdin);
				// Check if the character is part of an escape sequence
				if(characters_into_escape_sequence == 0 && serial_input == ESCAPE_CHAR) {
					// We've hit the first character in an escape sequence (escape)
					characters_into_escape_sequence++;
					serial_input = -1; // Don't further process this character
				} else if(characters_into_escape_sequence == 1 && serial_input == '[') {
					// We've hit the second character in an escape sequence
					characters_into_escape_sequence++;
					serial_input = -1; // Don't further process this character
				} else if(characters_into_escape_sequence == 2) {
					// Third (and last) character in the escape sequence
					escape_sequence_char = serial_input;
					serial_input = -1;  // Don't further process this character - we
										// deal with it as part of the escape sequence
					characters_into_escape_sequence = 0;
				} else {
					// Character was not part of an escape sequence (or we received
					// an invalid second character in the sequence). We'll process 
					// the data in the serial_input variable.
					characters_into_escape_sequence = 0;
				}
			}
		}
	if(!(timer_is_paused())){
		// Process the input. 
		if(button==0 || escape_sequence_char=='C') {
			// Set next direction to be moved to be right.
			set_snake_dirn(SNAKE_RIGHT);
		} else  if (button==2 || escape_sequence_char == 'A') {
			// Set next direction to be moved to be up
			set_snake_dirn(SNAKE_UP);
		} else if(button==3 || escape_sequence_char=='D') {
			// Set next direction to be moved to be left
			set_snake_dirn(SNAKE_LEFT);
		} else if (button==1 || escape_sequence_char == 'B') {
			// Set next direction to be moved to be down
			set_snake_dirn(SNAKE_DOWN);
		} else if(serial_input == 'p' || serial_input == 'P') {
			toggle_timer_clock();
		}else if(serial_input == 's'|| serial_input == 's'){
			save_game_to_EPPROM();
		}else if(EEPROM_has_saved_game() && (serial_input == 'o' || serial_input == 'O')){
 			load_EEPROM_data_on_next_game = 1;
 			break;
		}
	}else if(serial_input == 'p' || serial_input == 'P') {
			toggle_timer_clock();
		}else if(serial_input == 's'|| serial_input == 'S'){
			save_game_to_EPPROM();
		}else if(EEPROM_has_saved_game() && (serial_input == 'o' || serial_input == 'O')){
			load_EEPROM_data_on_next_game = 1;
			break;
	}
		// else - invalid input or we're part way through an escape sequence -
		// do nothing
		
		// Check for timer related events here
		if(get_clock_ticks() >= last_move_time + get_current_game_speed()) {
			// 600ms (0.6 second) has passed since the last time we moved the snake,
			// so move it now
			if(!attempt_to_move_snake_forward()) {
				// Move attempt failed - game over
				break;
			}
			last_move_time = get_clock_ticks();
		}
	}
	// If we get here the game is over. 
}

void handle_game_over() {
	clear_terminal();
	move_cursor(10,5);
	printf_P(PSTR("GAME OVER"));
	move_cursor(10,7);
	printf("Score: %u", get_score());
	display_EEPROM_high_score();

	if (is_high_score()){
		set_high_score_name();	
	}
	move_cursor(10,9);
	printf_P(PSTR("Press a button to start again"));
	display_EEPROM_high_score(); 
	while(button_pushed() == -1) {		
		; // wait until a button has been pushed
	}
	clear_terminal(); 
}
