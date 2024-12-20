/*
 ============================================================================
 Name        : lm35.h
 Module Name : LM35
 Author      : asus
 Date        : 28 Sept. 2024
 Description : Header file for the LM35 driver
 ============================================================================
 */

#include "lm35.h"
#include "../ADC/adc.h"

/*
 * Description :
 * Function responsible for calculate the temperature from the ADC digital value.
 */
uint8 LM35_getTemperature(uint8 channel_num)
{
    uint16 adc_value = ADC_ReadChannel(channel_num);

    /* Calculate temperature from 0V-3.3V mapped to 0°C-45°C */
    uint8 temperature = (uint8) (((uint32) adc_value * SENSOR_MAX_TEMPERATURE * ADC_REFERENCE_VOLTAGE) / (ADC_MAXIMUM_VALUE * SENSOR_MAX_VOLT_VALUE));

    return temperature;
}
