/*
 * timer1.h
 *
 * Created: 22/05/2017 2:30:14 PM
 *  Author: Jack Eadie
 */ 


#ifndef TIMER1_H_
#define TIMER1_H_

void initialise_timer_one(void);

void disable_timer_one(void);
void enable_timer_one(void);
void handle_buzzer_loop(void);
void change_timer_one_frequency(int desired_frequency);
void change_OCR1A(int new_OCR1A);



#endif /* TIMER1_H_ */