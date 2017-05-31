/*
 * joystick.h
 *
 * Created: 31/05/2017 10:56:40 AM
 *  Author: Jack Eadie
 */ 


#ifndef JOYSTICK_H_
#define JOYSTICK_H_

uint8_t read_joystick(void);
int init_joystick(void);
int8_t get_joystick_direction(void); 




#endif /* JOYSTICK_H_ */