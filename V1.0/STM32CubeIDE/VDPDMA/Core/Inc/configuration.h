/*
 * configuration.h
 *
 *  Created on: Sep 3, 2022
 *      Author: Duncan
 */

#ifndef INC_CONFIGURATION_H_
#define INC_CONFIGURATION_H_

/**
 * Leave this undefined if you want to use SPI instead of the parallel interface to the TFT
 */
//#define PAR_TFT

#define ADC_RESOLUTION_BITS 6
#define ADC_RESOLUTION_STEPS (1 << ADC_RESOLUTION_BITS)

#define VREF 2.9
#define ONE_ADC_BIT (VREF / ADC_RESOLUTION_STEPS)

#define ENCODER_PORT GPIOB
#define ENCODER_PIN GPIO_PIN_4
#define ENCODER_BUTTON_DEBOUNCE 20
#define ENCODER_BUTTON_LONG_PRESS 1000

#endif /* INC_CONFIGURATION_H_ */
