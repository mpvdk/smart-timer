/*
 * TWI.c
 *
 * Created: 08/08/2019 19:17:11
 *  Author: marti_k
 */ 

#include <avr/io.h>

void TWI_Initialize(void)
{
	PORTMUX.TWISPIROUTEA |= PORTMUX_TWI0_ALT2_gc;		// PC3 = SCL, PC2 = SDA
	TWI0.MBAUD = 15;									// Set MBAUD to get Fscl ~400kHz
														// Formula (p352 DS): Fscl = Fclk_per / (10 + 2*BAUD + Fclk_per * Trise)	
	TWI0.MCTRLB |= TWI_FLUSH_bm;						// Reset MDATA and MADDR
	TWI0.CTRLA |= 1 << TWI_FMPEN_bp;					// Fast Mode+: enable
	TWI0.MSTATUS |= TWI_RIF_bm							// Read Interrupt Flag: enable
				 |  TWI_WIF_bm;							// Write Interrupt Flag: enable
} /* TWI_Initialize */


void TWI_Enable()
{
	TWI0.MCTRLA = 1 << TWI_ENABLE_bp;
	TWI0.MSTATUS |= 1 << TWI_BUSSTATE0_bp;
} /* TWI_Enable */


uint8_t TWI_Start(uint8_t address)
{
	if ((TWI0.MSTATUS & TWI_BUSSTATE_gm) != TWI_BUSSTATE_BUSY_gc) {
		TWI0.MADDR = address;
		
		if (address & 1) {
			while (!(TWI0_MSTATUS & TWI_RIF_bm));
		}
		else {
			while (!(TWI0_MSTATUS & TWI_WIF_bm));
		}
		return(0);
	}
	else
	return TWI0.MSTATUS;
} /* TWI_Start */


uint8_t TWI_Write(uint8_t data)
{
	if ((TWI0.MSTATUS&TWI_BUSSTATE_gm) == TWI_BUSSTATE_OWNER_gc) {
		 while (!((TWI0.MSTATUS & TWI_WIF_bm) | (TWI0_MSTATUS & TWI_RXACK_bm)));
		
		TWI0.MDATA = data;
		return TWI0.MSTATUS;
	}
	else
	return TWI0.MSTATUS;
} /* TWI_Write */


void TWI_SendStop (void)
{
	TWI0.MCTRLB |= 0x3;
} /* TWI_SendStop */