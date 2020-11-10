/*
 * ADC.c
 *
 * Created: 08/08/2019 19:17:37
 *  Author: marti_k
 */ 

#include "ADC.h"
#include <avr/interrupt.h>
#include <avr/io.h>
#include "SSD1306.h"
#include "Main.h"
#include "TCB.h"

volatile uint8_t f_timer_active = 0;
volatile uint16_t stop_counter;
volatile uint16_t temp_tcb_cnt;
volatile uint8_t f_hall_disconnected = 0;
volatile uint8_t f_hall_connected = 0;

ISR (ADC0_WCOMP_vect) 
{
	ADC0.INTFLAGS = ADC_WCMP_bm;					// Clear Window Comparator Interrupt Flag
	
	if ((ADC0.CTRLE & ADC_WINCM_gm ) == ADC_WINCM_OUTSIDE_gc) {
		if (ADC0.RES >= 1000) {
			ADC0.CTRLE = ADC_WINCM_INSIDE_gc;		// Change WINCOMP mode
			//PORTF.PIN2CTRL = PORT_ISC_RISING_gc;
			f_hall_connected = 0;
			f_hall_disconnected = 1;
		}
		else {
			ADC0.CTRLE = ADC_WINCM_INSIDE_gc;		// Change Window Comparator Mode 
			f_hall_disconnected = 0;
			f_timer_active = 1;	
		}
	}
	
	else if ((ADC0.CTRLE & ADC_WINCM_gm) == ADC_WINCM_INSIDE_gc) {
		
		if (f_hall_connected == 0) {
			f_hall_connected = 1;
			ADC0.CTRLE = ADC_WINCM_OUTSIDE_gc;
		}
		
		else if (!((TCB0.CTRLA & TCB_ENABLE_bm) == TCB_ENABLE_bm)) {
			TCB_Enable();
		}
		else {
			temp_tcb_cnt = TCB0.CNT;
			TCB_Disable();
			TCB0.CNT = 0;
			
			if (temp_tcb_cnt > 200) {
				stop_counter = 0;
				TCB_Enable();
			}
			else {
				stop_counter++;
				//PORTF.PIN2CTRL &= ~PORT_ISC_gm;						// Disable interrupt for PORTF2 (connected to tactile push button)
				if (stop_counter >= 1000) {
					f_timer_active = 0;
					stop_counter = 0;
					ADC0.CTRLE = ADC_WINCM_OUTSIDE_gc;
					TCA0_counter = 0;
					TCB_Disable();
				}
				else { 
					TCB_Enable(); 
				}
			}
		}
	}
} /* ISR */


void ADC_Initialize(void)
{
	ADC0.CTRLE = ADC_WINCM_OUTSIDE_gc;			// Window Comparator Mode: Outside
	ADC0.WINLT = 520;							// Low Threshold
	ADC0.WINHT = 680;							// High Threshold
	ADC0.INTCTRL |= ADC_WCMP_bm;				// Window Comparator Interrupt Enable

	ADC0.CTRLA |= ADC_FREERUN_bm;				// Free-running: enable
	ADC0.CTRLC |= ADC_SAMPCAP_bm				// Sampling Capacitance: reduced
				| ADC_REFSEL_VDDREF_gc			// Reference Selection: VDD
				| ADC_PRESC_DIV16_gc;			// ADC Prescaler: 16 (CLK_ADC = CLK_PER / 16)
	
	ADC0.MUXPOS = ADC_MUXPOS_AIN6_gc;			// Analog Input: AIN6
	
	ADC0.CTRLA |= ADC_ENABLE_bm;				// ADC Enable
} /* ADC_Initialize */


void ADC_Start(void)
{
	ADC0.COMMAND |= ADC_STCONV_bm;
} /* ADC_Start */


void ADC_Enable(void)
{
	ADC0.CTRLA |= ADC_ENABLE_bm;	
} /* ADC_Enable */


void ADC_Disable(void)
{
	ADC0.CTRLA &= ~ADC_ENABLE_bm;
} /* ADC_Disable */