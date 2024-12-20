/*
 ============================================================================
 Name        : lm35.h
 Module Name : LM35
 Author      : asus
 Date        : 28 Sept. 2024
 Description : Header file for the LM35 driver
 ============================================================================
 */

#ifndef LM35_H_
#define LM35_H_

#include "std_types.h"

/*******************************************************************************
 *                                Definitions                                  *
 *******************************************************************************/

#define SENSOR0_CHANNEL_ID          AIN0_CHANNEL
#define SENSOR1_CHANNEL_ID          AIN1_CHANNEL

/* Mapping for testing with a potentiometer */
#define SENSOR_MAX_VOLT_VALUE          3.3
#define SENSOR_MAX_TEMPERATURE         45

/*******************************************************************************
 *                      Functions Prototypes                                   *
 *******************************************************************************/

/*
 * Description :
 * Function responsible for calculate the temperature from the ADC digital value.
 */
uint8 LM35_getTemperature(uint8 channel_num);

#endif /* LM35_H_ */
