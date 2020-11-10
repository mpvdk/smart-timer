/*
 * TCB.c
 *
 * Created: 24/09/2019 22:06:08
 *  Author: marti_k
 */ 

#include "TCB.h"
#include <avr/interrupt.h>

volatile uint8_t TCB_overflow_counter;

ISR(TCB0_INT_vect){
	TCB_overflow_counter++;
	TCB0.INTFLAGS = 0xFF;
} /* ISR */


void TCB_Initialize(void) 
{
	TCB0.CCMP = 50000;
} /* TCB_Initialize */


void TCB_Enable(void)
{
	TCB0.CTRLA |= TCB_ENABLE_bm;
} /* TCB_Start */


void TCB_Disable(void)
{
	TCB0.CTRLA &= ~TCB_ENABLE_bm;
	TCB_overflow_counter = 0;
} /* TCB_Stop */
