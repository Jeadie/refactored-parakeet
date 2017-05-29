/*
 * terminalio.c
 *
 * Author: Peter Sutton
 */

#include <stdio.h>
#include <stdint.h>

#include <avr/pgmspace.h>
#include "score.h"
#include "terminalio.h"




void move_cursor(int8_t x, int8_t y) {
   printf_P(PSTR("\x1b[%d;%dH"), y, x);
}

void normal_display_mode(void) {
	printf_P(PSTR("\x1b[0m"));
}

void reverse_video(void) {
	printf_P(PSTR("\x1b[7m"));
}

void clear_terminal(void) {
	printf_P(PSTR("\x1b[2J"));
}

void clear_to_end_of_line(void) {
	printf_P(PSTR("\x1b[K"));
}

void set_display_attribute(DisplayParameter parameter) {
	printf_P(PSTR("\x1b[%dm"), parameter);
}

void hide_cursor() {
	printf_P(PSTR("\x1b[?25l"));
}

void show_cursor() {
	printf_P(PSTR("\x1b[?25h"));
}

void enable_scrolling_for_whole_display(void) {
	printf_P(PSTR("\x1b[r"));
}

void set_scroll_region(int8_t y1, int8_t y2) {
	printf_P(PSTR("\x1b[%d;%dr"), y1, y2);
}

void scroll_down(void) {
	printf_P(PSTR("\x1bM"));	// ESC-M
}

void scroll_up(void) {
	printf_P(PSTR("\x1b\x44"));	// ESC-D
}

void draw_horizontal_line(int8_t y, int8_t start_x, int8_t end_x) {
	int8_t i;
	move_cursor(start_x, y);
	reverse_video();
	for(i=start_x; i <= end_x; i++) {
		printf(" ");	/* No need to use printf_P - printing 
						 * a single character gets optimised
						 * to a putchar call 
						 */
	}
	normal_display_mode();
}

void draw_vertical_line(int8_t x, int8_t start_y, int8_t end_y) {
	int8_t i;
	move_cursor(x, start_y);
	reverse_video();
	for(i=start_y; i < end_y; i++) {
		printf(" ");
		/* Move down one and back to the left one */
		printf_P(PSTR("\x1b[B\x1b[D"));
	}
	printf(" ");
	normal_display_mode();
}

void display_start_terminal_screen(void){
	move_cursor(3,3);
	printf_P(PSTR("Snake"));

	move_cursor(3,5);
	set_display_attribute(FG_GREEN);	// Make the text green
	// Modify the following line
	printf_P(PSTR("CSSE2010/7201 Snake Project by Jack Eadie"));
	// Return to default colour (White)
	set_display_attribute(FG_WHITE);
	display_EEPROM_high_score();
}

void init_score_on_terminal(void){
	
	hide_cursor();
	
	move_cursor(10,1);
	printf("Snake");

	move_cursor(1,3);
	printf("Score:  ");
	
}

void update_terminal_score(void){
	hide_cursor();
	move_cursor(1, 3);
	printf("Score: %8u", get_score());
}
