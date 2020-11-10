/*
 * Main.h
 *
 * Created: 08/08/2019 19:16:19
 *  Author: marti_k
 */ 


#ifndef SMART_TIMER_H_
#define SMART_TIMER_H_

#define TIMING 1
#define IDLE 2

#include <avr/io.h>

// GLOBAL VARIABLES

uint8_t f_display_shot_counter;
extern volatile uint8_t f_timer_active;
extern volatile uint8_t f_hall_disconnected;
extern volatile uint8_t f_hall_connected;
extern volatile uint16_t TCA0_counter;
extern volatile uint8_t f_battery_low;
extern uint8_t buffer[1024];

uint16_t shot_counter;

volatile int f_charging;

// FUNCTION DECLARATIONS

void reset(void);
void setClock(void);
void charging(void);
void timing(void);
void displayShotCounter(uint16_t x);

#endif /* SMART TIMER_H_ */