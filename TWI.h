/*
 * IncFile1.h
 *
 * Created: 08/08/2019 17:44:46
 *  Author: marti_k
 */ 
#include <stdint.h>

#ifndef TWI_H_
#define TWI_H_

// FUNCTION DECLARATIONS

void TWI_Initialize(void);
void TWI_Enable();
uint8_t TWI_Start(uint8_t addr);
uint8_t TWI_Write(uint8_t data);
void TWI_SendStop (void);

#endif /* TWI_H_ */