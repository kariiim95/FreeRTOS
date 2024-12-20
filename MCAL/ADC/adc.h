/*
 ============================================================================
 Name        : adc.h
 Module Name : ADC
 Author      : asus
 Date        : 28 Sept. 2024
 Description : Header file for the TM4C123GH6PM Microcontroller ADC driver
 ============================================================================
 */

#ifndef ADC_H_
#define ADC_H_

#include "std_types.h"

/*******************************************************************************
 *                                Definitions                                  *
 *******************************************************************************/

/* ADC Configuration for TM4C123GH6PM */

/* The TM4C123GH6PM has a 12-bit ADC, so the maximum ADC value is 0xFFF (4095) */
#define ADC_MAXIMUM_VALUE       0xFFF

/* Reference voltage is 3.3V for the ADC (floating-point constants remain as is) */
#define ADC_REFERENCE_VOLTAGE   3.3

/* Analog Input Channels */
#define AIN0_CHANNEL            0x0   /* PE3 corresponds to AIN0 */
#define AIN1_CHANNEL            0x1   /* PE2 corresponds to AIN1 */

/* ADC Sequencer and Trigger Masks */
#define SEQUENCER_0_MASK        0x01  /* Mask for enabling/disabling sequencer 0 */
#define TRIGGER_ALWAYS_MASK     0x0F  /* Mask for configuring trigger event (always sample) */

/* ADC Input Source Select */
#define INPUT_SOURCE_AIN1       0x01  /* Input source for PE2/AIN1 */
#define INPUT_SOURCE_AIN0       0x00  /* Input source for PE3/AIN0 */

/* Sample Sequencer Control Bits */
#define SAMPLE_CONTROL_MASK     0x06  /* Sample control bits for sequencer 0 (1 << 1 | 1 << 2) */

/* Dither Mode */
#define DITHER_MODE_ENABLE      0x40  /* Dither mode enable (1 << 6) */

/* Voltage Reference Configuration */
#define VOLTAGE_REF_CLEAR_MASK  0x01  /* Mask to clear voltage reference bits (1 << 0) */

/* ADC Sample Sequencer 0 Mask */
#define SAMPLE_SEQ_0_MASK       0x01  /* Mask for sample sequencer 0 (1 << 0) */

/* ADC Result Mask */
#define ADC_RESULT_MASK         0xFFF /* Mask for the 12-bit result from the FIFO */

#define ADC0_IRQ_NUM                  14
#define ADC0_INTERRUPT_PRIORITY       5

#define ADC1_IRQ_NUM                  48
#define ADC1_INTERRUPT_PRIORITY       5

/*******************************************************************************
 *                      Functions Prototypes                                   *
 *******************************************************************************/

/*
 * Description :
 * Function responsible for initializing the ADC0 driver.
 * It enables the clock for the ADC0 module and configures the GPIO pins for
 * analog input, and sets up the ADC0 sequencer.
 */
void ADC_Init(void);

/*
 * Description :
 * Function responsible for reading analog data from a certain ADC0 channel
 * and converting it to digital using the ADC0 driver.
 * The function starts the conversion, waits for it to complete,
 * and returns the digital result.
 */
uint16 ADC_ReadChannel(uint8 channel_num);
#endif
