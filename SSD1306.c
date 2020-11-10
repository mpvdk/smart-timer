/*
 * OLED.c
 *
 * Created: 09/08/2019 11:57:33
 *  Author: marti_k
 */ 

#include "TWI.h"
#include "SSD1306.h"
#include "Main.h"
#include <avr/pgmspace.h>
#include "Fonts.h"

void SSD1306_Initialize(void)
{		
		TWI_Start(DISPLAY_ADDRESS);
		TWI_Write(MULTIPLE_CMD);
		TWI_Write(0xA8);	// Set MUX ratio
		TWI_Write(0x3F);	// To reset
		TWI_Write(0xD3);	// Set Display Offset
		TWI_Write(0x00);	// To reset
		TWI_Write(0x81);	// Set contrast
		TWI_Write(25);		// To value ranging from 0 - 255 (0x0 - 0xFF)
		TWI_Write(0x40);	// Set Display Startline to 0
		TWI_Write(0xA4);	// DISABLE entire display on (i.e. follow GDDRAM)
		TWI_Write(0xA6);	// Set Normal display
		
		TWI_Write(0xA0);	// Set segment remap		
		TWI_Write(0xC0);	// COM output direction
		
		TWI_Write(0xDA);	// Set COM pins hardware configuration
		TWI_Write(0x12);	// !!!! MADE DIFFERENCE !!!

		TWI_Write(ADDRESSING_MODE);
		TWI_Write(HORIZONTAL_ADDRESSING);
		TWI_Write(SET_COLUMN_ADDRESS);
		TWI_Write(0x0);						// Column start address
		TWI_Write(0x7f);					// Column end address
		TWI_Write(SET_PAGE_ADDRESS);
		TWI_Write(0x0);						// Page start address
		TWI_Write(0x7);						// Page end address

		TWI_Write(0xD5);	// clock settings
		TWI_Write(0x0F);

		TWI_Write(0xD5);	// Set osc. freq
		TWI_Write(0x80);
		TWI_Write(0x8D);	// Set charge pump
		TWI_Write(0x14);	// to enable

		TWI_Write(0xAF);	// Display ON
		
		TWI_SendStop();
} /* SSD1306_Initialize */


void SSD1306_WritePixel(uint8_t x, uint8_t y)
{
	int byte_nr = (y/8)*128+x;
	buffer[byte_nr] |= 1 << (y%8);
} /* SSD1306_WritePixel */


void SSD1306_ClearPixel(uint8_t x, uint8_t y)
{
	int byte_nr = (y/8)*128+x;
	buffer[byte_nr] &= ~(1 << (y%8));
} /* SSD1306_ClearPixel */


void SSD1306_WriteNumber(uint8_t x, uint8_t y, uint8_t nr)
{
	for (int i=0; i<6; i++) {
		for (int j=0; j<34; j++) {
			uint8_t byte = pgm_read_byte(&large_numbers[(nr*204)+(i*34)+j]);
			for (int k=0; k<8; k++, byte <<= 1) {
				if (byte & 0x80) {
					SSD1306_WritePixel(x+j, y+(i*8)+k);
				}
				else {
					SSD1306_ClearPixel(x+j, y+(i*8)+k);
				}
			}
		}
	}
} /* SSD1306_WriteNumber */


void SSD1306_WriteSmallNumber(uint8_t x, uint8_t y, uint8_t nr)
{
	for (int i=0; i<3; i++) {
		for (int j=0; j<17; j++) {
			uint8_t byte = pgm_read_byte(&small_numbers[(nr*51)+(i*17)+j]);
			for (int k=0; k<8; k++, byte <<= 1) {
				if (byte & 0x80) {
					SSD1306_WritePixel(x+j, y+(i*8)+k);
				}
				else {
					SSD1306_ClearPixel(x+j, y+(i*8)+k);
				}
			}
		}
	}
} /* SSD1306_WriteSmallNumber */


void SSD1306_DrawRectangle(uint8_t x, uint8_t y, uint8_t width, uint8_t height)
{
	for(int i=y; i<(y+height); i++) {
		for(int j=x; j<(x+width); j++) {
			SSD1306_WritePixel(j,i);
		}
	}
} /* SSD1306_DrawRectangle */


void SSD1306_ClearRectangle(uint8_t x, uint8_t y, uint8_t width, uint8_t height)
{
	for(int i=y; i<(y+height); i++) {
		for(int j=x; j<(x+width); j++) {
			SSD1306_ClearPixel(j,i);
		}
	}	
} /* SSD1306_ClearRectangle */


void SSD1306_DrawBattery(void)
{
	for(int i=0; i<1024; i++) {
		uint8_t byte = pgm_read_byte(&battery[i]);
		for (int j=0; j<8; j++, byte <<= 1) {
			if (byte & 0x80) {
				SSD1306_WritePixel(i%128, (8*(i/128)+j));
			}
			else {
				SSD1306_ClearPixel(i%128, (8*(i/128)+j));
			}
		}
	}
} /* SSD1306_DrawBattery */


void SSD1306_DrawBatteryLow(void)
{
	for(int i=0; i<1024; i++) {
		uint8_t byte = pgm_read_byte(&battery_low[i]);
		for (int j=0; j<8; j++, byte <<= 1) {
			if (byte & 0x80) {
				SSD1306_WritePixel(i%128, (8*(i/128)+j));
			}
			else {
				SSD1306_ClearPixel(i%128, (8*(i/128)+j));
			}
		}
	}	
} /* SSD1306_DrawBatteryLow */


void SSD1306_DrawLogo(void)
{
	for(int i=0; i<1024; i++) {
		uint8_t byte = pgm_read_byte(&coffeelovers[i]);
		for (int j=0; j<8; j++, byte <<= 1) {
			if (byte & 0x80) {
				SSD1306_WritePixel(i%128, (8*(i/128)+j));
			}
			else {
				SSD1306_ClearPixel(i%128, (8*(i/128)+j));
			}
		}
	}
} /* SSD1306_DrawLogo */
	

void SSD1306_DrawChargingLogo(void)
{
	for(int i=0; i<1024; i++) {
		uint8_t byte = pgm_read_byte(&coffeelovers_charging[i]);
		for (int j=0; j<8; j++, byte <<= 1) {
			if (byte & 0x80) {
				SSD1306_WritePixel(i%128, (8*(i/128)+j));
			}
			else {
				SSD1306_ClearPixel(i%128, (8*(i/128)+j));
			}
		}
	}	
} /* SSD1306_DrawChargingLogo */


void SSD1306_FillBuffer(void)
{
	for (int i=0; i<1024; i++) {
		buffer[i] = 0xff;
	}
} /* SSD1306_ClearBuffer */


void SSD1306_ClearBuffer(void)
{
	for (int i = 0; i < 1024; i++) {
		buffer[i] = 0x0;
	}
} /* SSD1306_ClearBuffer */


void SSD1306_SendBuffer(void)
{
	TWI_Start(DISPLAY_ADDRESS);
	TWI_Write(MULTIPLE_DATA);
	for (int i = 0; i < 1024; i++) {
		TWI_Write(buffer[i]);
	}
	TWI_SendStop();
} /* SSD1306_SendBuffer */


/* The Clear- and FillDisplay commands affect OLED directly.	*/
/* i.e. They do not require the SSD1306_SendBuffer() command.	*/


void SSD1306_ClearDisplay(void)
{
	TWI_Start(DISPLAY_ADDRESS);
	TWI_Write(MULTIPLE_CMD);
	TWI_Write(ADDRESSING_MODE);
	TWI_Write(HORIZONTAL_ADDRESSING);
	TWI_Write(SET_COLUMN_ADDRESS);
	TWI_Write(0x0);						// Column start address
	TWI_Write(0x7F);					// Column end address
	TWI_Write(SET_PAGE_ADDRESS);
	TWI_Write(0x0);						// Page start address
	TWI_Write(0x7);						// Page end address
	
	TWI_SendStop();
	
	TWI_Start(DISPLAY_ADDRESS);
	TWI_Write(MULTIPLE_DATA);
	for (int i = 0; i < 1024; i++) {
		TWI_Write(0x0);
	}
	TWI_SendStop();
} /* SSD1306_ClearDisplay */


void SSD1306_FillDisplay(void)
{
	TWI_Start(DISPLAY_ADDRESS);
	TWI_Write(MULTIPLE_CMD);
	TWI_Write(ADDRESSING_MODE);
	TWI_Write(HORIZONTAL_ADDRESSING);
	TWI_Write(SET_COLUMN_ADDRESS);
	TWI_Write(0x0);						// Column start address
	TWI_Write(0x7F);					// Column end address
	TWI_Write(SET_PAGE_ADDRESS);
	TWI_Write(0x0);						// Page start address
	TWI_Write(0x7);						// Page end address
	
	TWI_SendStop();
	
	TWI_Start(DISPLAY_ADDRESS);
	TWI_Write(MULTIPLE_DATA);
	for (int i = 0; i < 1024; i++) {
		TWI_Write(0xff);
	}
	TWI_SendStop();	
} /* SSD1306_FillDisplay */


void SSD1306_DisplayOff(void)
{
	TWI_Start(DISPLAY_ADDRESS);
	TWI_Write(SINGLE_CMD);
	TWI_Write(0xAE);
	TWI_SendStop();
} /* SSD1306_DisplayOff */


void SSD1306_DisplayOn(void)
{
	TWI_Start(DISPLAY_ADDRESS);
	TWI_Write(SINGLE_CMD);
	TWI_Write(0xAF);
	TWI_SendStop();
} /* SSD1306_DisplayOn */


// SSD1306_SetContrast takes as an argument a value between 0 and 255

void SSD1306_SetContrast(uint8_t x)
{
	TWI_Start(DISPLAY_ADDRESS);
	TWI_Write(MULTIPLE_CMD);
	TWI_Write(0x81);
	TWI_Write(x);
	TWI_SendStop();
} /* SSD1306_SetContrast */