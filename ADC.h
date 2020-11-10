/*
 * ADC.h
 *
 * Created: 08/08/2019 19:17:44
 *  Author: marti_k
 */ 


#ifndef ADC_H_
#define ADC_H_

#include <avr/io.h>

// GLOBAL VARIABLES

volatile uint16_t start_counter;

// FUNCTION DECLARATIONS

void ADC_Initialize(void);
void ADC_Start(void);
void ADC_Enable(void);
void ADC_Disable(void);

#endif /* ADC_H_ */