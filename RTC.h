/*
 * RTC.h
 *
 * Created: 29/12/2019 21:32:06
 *  Author: marti_k
 */ 


#ifndef RTC_H_
#define RTC_H_

#include <avr/io.h>

extern volatile uint8_t TCB_overflow_counter;
extern uint16_t shot_counter;
extern uint8_t f_display_shot_counter;

void RTC_Initialize(void);
void RTC_Enable(void);
void RTC_Disable(void);

#endif /* RTC_H_ */