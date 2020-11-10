/*
 * TCA.c
 *
 * Created: 08/08/2019 19:16:37
 *  Author: marti_k
 */

#include "Main.h"
#include "TCA.h"
#include <avr/interrupt.h>

volatile uint16_t idle_counter = 0;
volatile uint16_t TCA0_counter = 0;

ISR(TCA0_OVF_vect)
{
	TCA0.SINGLE.INTFLAGS = 0xFF;		// Clear all interrupt flags
	
	idle_counter++;
	// If the TCA has overflowed 21600 times, the timer
	// has been idle for 6 hours, and issues a reset
	if(idle_counter >= 21600) {
		reset();
	}	
} /* ISR OVF */


ISR (TCA0_CMP0_vect) 
{
	TCA0.SINGLE.INTFLAGS = 0xFF;		// Clear all interrupt flags

	TCA0.SINGLE.CNT = 0x0;			// Reset counter register
	
	if(TCA0_counter >= 9999) {
		TCA0_counter = 0;
	}
	else { 
		TCA0_counter++;
	}

} /* ISR CMP */


void TCA_Initialize(uint8_t x)
{
	// Initialize TCA for timing
	if(x == 1) {
		idle_counter = 0;
		//TCA0.SINGLE.CTRLA = (TCA0.SINGLE.CTRLA & ~TCA_SINGLE_CLKSEL_gm) | TCA_SINGLE_CLKSEL_DIV16_gc;	// F_tca = F_per/16 (=1.25MHz)
		TCA0.SINGLE.CTRLA = TCA_SINGLE_CLKSEL_DIV16_gc;
		TCA0.SINGLE.INTCTRL = TCA_SINGLE_CMP0_bm;
		TCA0.SINGLE.CMP0 = 0x30D3;					// Set compare register to 12499 (interrupt every 10ms or 1 centisecond)
	}
	
	// Initialize TCA for idle reset
	else if (x == 2) {
		//TCA0.SINGLE.CTRLA = (TCA0.SINGLE.CTRLA & ~TCA_SINGLE_CLKSEL_gm) | TCA_SINGLE_CLKSEL_DIV1024_gc; // F_tca = F_per/1024 (= 19.531kHz)
		TCA0.SINGLE.CTRLA = TCA_SINGLE_CLKSEL_DIV1024_gc;
		TCA0.SINGLE.INTCTRL = TCA_SINGLE_OVF_bm;	// Enable OVF interrupt
		TCA0.SINGLE.PER = 0x4C4B;					// Set OVF register to 19531 (OVF every second)
	}
} /* TCA_Initialize */


void TCA_Enable(void)
{
	TCA0.SINGLE.CNT = 0x0;
	TCA0.SINGLE.CTRLA |= TCA_SINGLE_ENABLE_bm;
} /* TCA0_Enable */


void TCA_Disable(void)
{
	TCA0.SINGLE.CTRLA &= ~TCA_SINGLE_ENABLE_bm;
} /* TCA_Disable */