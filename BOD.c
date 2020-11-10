/*
 * BOD.c
 *
 * Created: 17/11/2019 10:09:41
 *  Author: marti_k
 */ 

/* 
Most BOD (Brown Out Detection) settings are loaded from fuses: 

	setting:						|	required configuration:
	-----------------------------------------------------------
	- Active/Idle Operation Mode	|	- Sampled
	- Sample Frequency				|	- 125HZ	
	- Sleep Operation Mode			|	- Disabled
	- BOD Threshold Level			|	- 2.9V
*/

#include <avr/interrupt.h>
#include "ADC.h"
#include "Main.h"
#include "SSD1306.h"

volatile uint8_t f_battery_low;

ISR (BOD_VLM_vect)
{
	BOD.INTFLAGS = 0xFF;		// Clear Interrupt Flag

	f_timer_active = 0;
	f_hall_connected = 0;
	f_hall_disconnected = 0;
	f_battery_low = 1;
	
} /* ISR */


void BOD_Initialize(void) 
{
	BOD.INTCTRL |= BOD_VLMIE_bm;											// VLM Interrupt Enabel
	BOD.VLMCTRLA = BOD_VLMLVL_5ABOVE_gc;									// VLM Threshold 5% above BOD Threshold (2.9*1.05 = 3.05V)
	BOD.INTCTRL = (BOD.INTCTRL & ~BOD_VLMCFG_gm) | BOD_VLMCFG_BELOW_gc;		// Interrupt is triggered when voltage crosses VLM from above
																			// WARNING: BOD_VLMCFG group conditions do not correspond
																			// to 4808 datasheet; they seem to be switched
} /* BOD_Initialize */