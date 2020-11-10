/*
 * ATmega4808 Main.c
 *
 * Created: 04/08/2019 21:45:49
 * Author : marti_k
 */ 

#include "RTC.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include "TWI.h"
#include "main.h"
#include "SSD1306.h"
#include "ADC.h"
#include "TCA.h"
#include "TCB.h"
#include "BOD.h"

// MCP73832 (lipo charging IC) STATUS INTERRUPT

ISR(PORTD_PORT_vect)
{
	PORTD.INTFLAGS = PORT_INT1_bm;				// Clear PORTD1 interrupt flag

	f_timer_active = 0;
	f_hall_disconnected = 0;
	f_hall_connected = 0;
	f_charging = 1;
} /* ISR */


// TACTILE PUSH BUTTON INTERRUPT

ISR(PORTF_PORT_vect)
{
	PORTF.INTFLAGS = PORT_INT2_bm;				// Clear PORTF2 interrupt flag	
	RTC_Enable();
} /* ISR */


int main(void)
{	
	// PORT SETTINGS
	
	PORTD.DIRSET = 1<<0;					// PD0 (connected to SSD1306 reset pin) = output
	PORTD.OUTSET = 1<<0;					// PD0 = high

	PORTD.DIRCLR = 1<<1;					// PD1 (connected to LIPO charging IC (MCP73832) STAT output) = input
	PORTD.PIN1CTRL = PORT_ISC_LEVEL_gc;		// PD1 interrupt enable

	PORTF.DIRCLR = 1<<2;					// PF2 (connected to tactile push button) = input

	// PERIPHERAL INITIALIZATIONS 

	BOD_Initialize();
	
	setClock();
	sei();
	
	TWI_Initialize();
	TWI_Enable();
	
	SSD1306_Initialize();
	SSD1306_ClearDisplay();
		
	ADC_Initialize();
	ADC_Start();
		
	TCA_Initialize(IDLE);
	TCA_Enable();
	
	RTC_Initialize();
	
	// On chip reset, check if timer is in charging dock and charging	
	if (!(PORTD.IN & 0x1)) {
		SSD1306_SetContrast(200);
		f_charging = 1;
		f_hall_connected = 0;
		f_hall_disconnected = 1;
		f_battery_low = 0;
		f_timer_active = 0;
	}
	
	// If not charging, check if voltage level is below VLM threshold (3.05V)	
	if (BOD.STATUS == 1) {
		f_battery_low = 1;
		f_hall_connected = 0;
		f_hall_disconnected = 0;
	}
	
	// If neither, display logo	
	else {
		f_battery_low = 0;
		f_charging = 0;
		f_hall_connected = 0;
		f_hall_disconnected = 1;
		SSD1306_DrawLogo();
		SSD1306_SendBuffer();
	}
	
	// 4 MAIN STATES OF TIMER
	
	while(1) {
		
		// STATE 1: The Timer is docked in the HALL docking station
		
		while (f_hall_connected == 1) {
			if (f_timer_active == 0) {
				// Wait for solenoid to be activated
				while (f_timer_active == 0) {
					// If timer is taken off HALL dock, break from while loop
					if (f_hall_connected == 0) {
						break;
					}
				}		
			}
			// If solenoid is activated (i.e. shot is being pulled), start timing
			else if (f_timer_active == 1) {
				timing();
			}
		}
		
		// STATE 2: The Timer is docked and charging in the charging docking station
		
		if (f_charging == 1) {
			charging();
		}
		
		// STATE 3: the Timer is either: 
		//			3.1) not connected to either docking station, or
		//			3.2) docked into the charging docking station but fully charged
		
		if (f_hall_disconnected == 1) {
			ADC0.CTRLE = ADC_WINCM_INSIDE_gc;
			PORTF.PIN2CTRL = PORT_ISC_RISING_gc;		// PF2 (connected to tactile push button) interrupt enable
			while (f_hall_disconnected == 1) {
				if (f_timer_active == 0) {
				}
				else if (f_timer_active == 1) {
					timing();
				}
			}
		}
		
		// STATE 4: The battery is critically low
		
		if (f_battery_low == 1) {
			PORTF.PIN2CTRL = (PORTF.PIN2CTRL &= ~PORT_ISC_gm) | PORT_ISC_INPUT_DISABLE_gc;	// PF2 (connected to tactile push button) interrupt disable
			ADC_Disable();
			SSD1306_DrawBatteryLow();
			SSD1306_SetContrast(1);
			SSD1306_SendBuffer();
			// Wait to be docked in charging station
			while (f_battery_low == 1){
				if (f_charging == 1){break;}
			}
		}		
	}
} /* main */


void charging(void)
{	
	SSD1306_DrawChargingLogo();
	SSD1306_SendBuffer();
	SSD1306_SetContrast(200);
	
	PORTF.PIN2CTRL = (PORTF.PIN2CTRL &= ~PORT_ISC_gm) | PORT_ISC_INPUT_DISABLE_gc;		// PF2 (connected to tactile push button) interrupt disable
	
	// Setup RTC for battery animation
	RTC.PER = 0xFFFF;						// Set RTC OVF value
	RTC.CTRLA |= RTC_RTCEN_bm;				// Enable RTC

	uint8_t current_icon = 0;					// Used to keep track of the state of the charging animation
	uint16_t charging_time_counter = 0;			// See further down for usage
	
	// While MCP73832 STAT pin is pulled low, display charging animation	
	while(!(PORTD.IN & (1<<1))) {
		
		if(RTC.CNT >= 32768) {
			
			switch(current_icon)
			{
				case 0:
					SSD1306_DrawRectangle(116, 61, 2, 2);
					SSD1306_SendBuffer();
					current_icon = 1;
					RTC.CNT = 0;
					charging_time_counter++;
					break;				
				case 1:
					SSD1306_DrawRectangle(121, 61, 2, 2);
					SSD1306_SendBuffer();
					current_icon = 2;
					RTC.CNT = 0;
					charging_time_counter++;
					break;			
				case 2:
					SSD1306_DrawRectangle(126, 61, 2, 2);
					SSD1306_SendBuffer();
					current_icon = 3;
					RTC.CNT = 0;
					charging_time_counter++;
					break;			
				case 3: 
					SSD1306_ClearRectangle(116, 61, 12, 2);
					SSD1306_SendBuffer();
					current_icon = 0;
					RTC.CNT = 0;
					charging_time_counter++;
					break;
			}
		}
		
		// The following if-statement is a way to ensure the end of the charging cycle
		// by lowering the current draw for a moment by turning off the OLED		
		if(charging_time_counter >= 1000) {
			PORTD.OUTCLR = 1 << 0;					// PD0 = low (RESET SSD1306)
			PORTD.OUTSET = 1 << 0;					// PD0 = high
			SSD1306_Initialize();
			SSD1306_SetContrast(200);
			charging_time_counter = 0;
		}
	}
	
	// If the timer is taken off the charging dock while 
	// battery is critically low, set the appropriate flag	
	if (BOD.STATUS == 1) {
		f_battery_low = 1;
	}
	
	
	// If it is taken off while sufficiently charged, or timer
	// is simply done charging, reset for user interaction	
	else {
		f_battery_low = 0;
		
		SSD1306_DrawLogo();
		SSD1306_SendBuffer();
	
		PORTF.PIN2CTRL = (PORTF.PIN2CTRL & ~PORT_ISC_gm) | PORT_ISC_RISING_gc;	// PF2 (connected to tactile push button) interrupt enable
		RTC.CNT = 0;															// set up RTC for tactile push button interaction
		RTC.PER = 500;
		RTC.CTRLA |= RTC_RTCEN_bm;
		f_hall_disconnected = 1;
		f_hall_connected = 0;
	}
	
	SSD1306_SetContrast(25);
	f_charging = 0;
} /* charging */


void timing(void)
{
	TCA_Initialize(TIMING);
	
	// LOCAL VARIABLES
	
	unsigned long temp_counter = 0;
	uint8_t decaseconds;
	uint8_t seconds;
	uint8_t deciseconds;
	uint8_t centiseconds;
	
	TCA_Enable();
		
	while (f_timer_active == 1) {
		
		// If the timer is taken off its station mid-timing, it's paused	
 		if (ADC0.RES >= 1023) {
	 		if (f_hall_connected == 1) {
				f_timer_active = 0;
				f_hall_disconnected = 1;
		 		f_hall_connected = 0;
	 		}
		 }
		 
		// The temp_counter variable is used to store the current time in 
		// centiseconds to work with in the next few lines. If we were to
		// use TCA0_counter, it will increase during the calculations below,
		// and gradually become more and more out of sync.		
		temp_counter = TCA0_counter;
		
		decaseconds = temp_counter / 1000;
		seconds = (temp_counter - 1000*decaseconds)/100;
		deciseconds = (temp_counter - 1000*decaseconds - 100*seconds)/10;
		centiseconds = temp_counter - 1000*decaseconds - 100*seconds - 10*deciseconds;
		
		// If timer hits 100 seconds, loop back to 0
		if (decaseconds > 9){ TCA0_counter = 0; } 
		
		// Update OLED with new timer value
		SSD1306_ClearBuffer(); 			
		SSD1306_WriteSmallNumber(100,7,centiseconds);
		SSD1306_WriteSmallNumber(83,7,deciseconds);
		SSD1306_WriteNumber(45,7,seconds);
		SSD1306_WriteNumber(7,7,decaseconds);
		SSD1306_SendBuffer();	
	}
	
	// The shot_counter variable is used to display
	// shots pulled in a day upon button press & hold.
	// Flushes < 15 seconds are not counted.	
	if(((decaseconds >= 1) && (seconds >= 5)) || (decaseconds >= 2))  {
		shot_counter++;
	}	
	TCA_Initialize(IDLE);
	TCA_Enable();
} /* timing */


void setClock(void)
{
	// This function sets the main clock by changing
	// protected registers. In order to do so, it writes a 
	// 'signature' (0xD8) to the CPU CCP register. This 
	// un-protects protected I/O registers for 4 CPU cycles.
	// "&=" and "|=" commands often consist of more than 4 CPU 
	// cycles, and can therefore not be reliably used here.
	
	// Set Main Clock to 20MHz internal oscillator
	CCP = 0xD8;
	//CLKCTRL.MCLKCTRLA = CLKCTRL_CLKSEL_OSC20M_gc;
	CLKCTRL.MCLKCTRLA = 0x00;
	
	// Disable prescaler
	CCP = 0xD8;
	//CLKCTRL.MCLKCTRLB |= ~CLKCTRL_PEN_bm;
	CLKCTRL.MCLKCTRLB = 0x00;
} /* setClock */


void reset(void)
{
	// As with the setClock function above, this software reset 
	// function also changes protected registers, and also needs
	// to write to the CPU CCP register before doing so. 
	
	CCP = 0xD8;
	RSTCTRL.SWRR = RSTCTRL_SWRE_bm;		// Software reset
} /* reset */


void displayShotCounter(uint16_t x)
{
	f_display_shot_counter = 1;

	for(int i=0; i<1024; i++) {
		buffer_backup[i] = buffer[i];
	}
	
	uint8_t thousands = x / 1000;
	uint8_t hundreds = (x - thousands * 1000) / 100;
	uint8_t tens = (x - thousands * 1000 - hundreds * 100) / 10;
	uint8_t ones = x - thousands * 1000 - hundreds * 100 - tens * 10;	

	SSD1306_ClearBuffer();
	SSD1306_WriteSmallNumber(30,18,thousands);
	SSD1306_WriteSmallNumber(49,18,hundreds);
	SSD1306_WriteSmallNumber(67,18,tens);
	SSD1306_WriteSmallNumber(86,18,ones);
	SSD1306_SendBuffer();

	while(PORTF.IN & (1 << 2)) {}
	
	for(int i=0; i<1024; i++) {
		buffer[i] = buffer_backup[i];
	}
	SSD1306_SendBuffer();
	
	//RTC_Disable();
	//TCB_Disable();
} /* displayShotCounter */