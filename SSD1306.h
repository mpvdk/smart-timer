/*
 * OLED.h
 *
 * Created: 09/08/2019 11:57:43
 *  Author: marti_k
 */ 


#ifndef SSD1306_H_
#define SSD1306_H_

// MISC.

#define DISPLAY_ADDRESS 0x78	// 0x3C (SA0 = low) + write bit (0) = 0x78 
								// 0x3D (SA0 = high) + write bit (0) = 0x7A
								// 0x3C (SA0 = low) + read bit (1) = 0x3D
								// 0x3D (SA0 = high) + read bit (1) = 0x7A

// GLOBAL VARIABLES

uint8_t buffer[1024];
uint8_t buffer_backup[1024];

// FUNCTION DECLARATIONS

void SSD1306_Initialize(void);
void SSD1306_WritePixel(uint8_t x, uint8_t y);
void SSD1306_ClearPixel(uint8_t x, uint8_t y);
void SSD1306_WriteNumber(uint8_t x, uint8_t y, uint8_t nr);
void SSD1306_WriteSmallNumber(uint8_t x, uint8_t y, uint8_t nr);	
void SSD1306_DrawRectangle(uint8_t x, uint8_t, uint8_t width, uint8_t height);	
void SSD1306_ClearRectangle(uint8_t x, uint8_t y, uint8_t width, uint8_t height);									
void SSD1306_DrawBattery(void);	
void SSD1306_DrawBatteryLow(void);
void SSD1306_DrawLogo(void);
void SSD1306_DrawChargingLogo(void);
void SSD1306_FillBuffer(void);
void SSD1306_ClearBuffer(void);
void SSD1306_SendBuffer(void);
void SSD1306_ClearDisplay(void);
void SSD1306_FillDisplay(void);
void SSD1306_DisplayOff(void);
void SSD1306_DisplayOn(void);
void SSD1306_SetContrast(uint8_t x);

// DISPLAY COMMANDS 
// cf. SSD1306 datasheet

#define SINGLE_CMD			0x80
#define MULTIPLE_CMD		0x0
#define SINGLE_DATA			0xc0
#define MULTIPLE_DATA		0x40

#define ADDRESSING_MODE				0x20
#define HORIZONTAL_ADDRESSING		0x0
#define VERTICAL_ADDRESSING			0x1
#define PAGE_ADDRESSING				0x2
#define SET_COLUMN_ADDRESS			0x21
#define SET_PAGE_ADDRESS			0x22

#endif /* SSD1306_H_ */