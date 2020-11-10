/*
 * RTC.c
 *
 * Created: 29/12/2019 21:31:58
 *  Author: marti_k
 */ 

#include <avr/interrupt.h>
#include <avr/io.h>
#include "Main.h"
#include "SSD1306.h"
#include "TCA.h"
#include "RTC.h"
#include "TCB.h"

volatile uint8_t f_timer_active;
volatile uint16_t TCA0_counter;

ISR(RTC_CNT_vect)
{
	RTC.INTFLAGS |= RTC_OVF_bm;				// Clear RTC OVF Interrupt Flag
	
	// If timer is on HALL dock and timing, end interrupt immediately
	if((f_hall_connected == 1) && (f_timer_active == 1)) {return;}
	
	if(PORTF.IN & (1 << 2)) {
		
		// If button was pressed while timer was running, pause timer
		if (f_timer_active == 1) {
			f_timer_active = 0;
			RTC_Disable();
			return;
		}
		
		// Check for press-and-hold
		uint16_t temp_cnt = 0;
		
		while(PORTF.IN & (1<<2)) {
			TCB_Enable();
			if(TCB0.CNT > 60000) {
				temp_cnt++;
				if(temp_cnt > 50000) {
					temp_cnt = 0;
					displayShotCounter(shot_counter);
				}
			}
		}
		
		// If button was pressed to display shot_counter, end interrupt
		if(f_display_shot_counter == 1){
			f_display_shot_counter = 0;
			return;
		}
		
		// If button was pressed but not held down while docked, return
		if(f_hall_connected == 1) {return;}
		
		// If button was pressed while timer was paused, restart timer
		else if (f_timer_active == 0) {
			TCA0_counter = 0;	
			f_timer_active = 1;
		}
	}
	RTC_Disable();
} /* ISR */


void RTC_Initialize(void)
{
	// NOTE TO SELF: See datasheet 21.4.2.1; check RTC status for good form
	
	RTC.PER = 500;							// Set RTC OVF value
	RTC.INTCTRL = RTC_OVF_bm;				// Enable RTC OVF Interrupt
} /* RTC_Initialize */


void RTC_Enable(void)
{
	RTC.CNT = 0x0;
	RTC.CTRLA |= RTC_RTCEN_bm;
} /* RTC_Enable */


void RTC_Disable(void)
{
	RTC.CNT = 0x0;
	RTC.CTRLA = 0x0;
} /* RTC_Disable */
