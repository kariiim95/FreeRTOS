/*
 ============================================================================
 Name        : adc.c
 Module Name : ADC
 Author      : asus
 Date        : 28 Sept. 2024
 Description : Source file for the TM4C123GH6PM Microcontroller ADC driver
 ============================================================================
 */

#include "adc.h"
#include "tm4c123gh6pm_registers.h"

/*
 * Description :
 * Function responsible for initializing the ADC0 and ADC1 driver.
 * sets up the ADC0 and ADC1 sequencer.
 */
void ADC_Init(void)
{
    /* Enable ADC0 and ADC1 clock */
    SYSCTL_RCGCADC_REG |= 0x03;
    while (!(SYSCTL_PRADC_REG & 0x03))
        ;

   /* Enable PORTE clock */
   SYSCTL_RCGCGPIO_REG |= 0x10;
   while (!(SYSCTL_PRGPIO_REG & 0x10))
       ;

   /* Enable alternative function on PE3 and PE2 (ADC input pins) */
   GPIO_PORTE_AFSEL_REG |= 0x0C;

   /* Disable digital enable on PE3 and PE2 */
   GPIO_PORTE_DEN_REG &= ~0x0C;

   /* Enable analog mode select on PE3 and PE2 */
   GPIO_PORTE_AMSEL_REG |= 0x0C;

   /* Configure PE3 and PE2 as input pins */
   GPIO_PORTE_DIR_REG &= ~0x0C;

    /********** Configure ADC0 **********/
    /* Disable sample sequencer 0 */
    ADC0_ACTSS_REG &= ~SAMPLE_SEQ_0_MASK;

    /* Configure trigger event for sequencer 0 (0xF means always sample) */
    ADC0_EMUX_REG |= TRIGGER_ALWAYS_MASK;

    /* Configure input source for sequencer 0 (PE3/Ain0) */
    ADC0_SSMUX0_REG = AIN0_CHANNEL;

    /* Configure sample control bits for sequencer 0 */
    ADC0_SSCTL0_REG |= SAMPLE_CONTROL_MASK;

    /* Enable dither mode */
    ADC0_CTL_REG |= DITHER_MODE_ENABLE;

    /* Clear VDDA and GNDA reference bits for all ADC modules */
    ADC0_CTL_REG &= ~VOLTAGE_REF_CLEAR_MASK;

    /* Enable sample sequencer 0 */
    ADC0_ACTSS_REG |= SAMPLE_SEQ_0_MASK;

    /********** Configure ADC1 **********/
    /* Disable sample sequencer 0 */
    ADC1_ACTSS_REG &= ~SAMPLE_SEQ_0_MASK;

    /* Configure trigger event for sequencer 0 (0xF means always sample) */
    ADC1_EMUX_REG |= TRIGGER_ALWAYS_MASK;

    /* Configure input source for sequencer 0 (PE2/Ain1) */
    ADC1_SSMUX0_REG = AIN1_CHANNEL;

    /* Configure sample control bits for sequencer 0 */
    ADC1_SSCTL0_REG |= SAMPLE_CONTROL_MASK;

    /* Enable dither mode */
    ADC1_CTL_REG |= DITHER_MODE_ENABLE;

    /* Clear VDDA and GNDA reference bits for all ADC modules */
    ADC1_CTL_REG &= ~VOLTAGE_REF_CLEAR_MASK;

    /* Enable sample sequencer 0 */
    ADC1_ACTSS_REG |= SAMPLE_SEQ_0_MASK;
}

/*
 * Description :
 * Function responsible for reading analog data from a certain ADC0 channel
 * and converting it to digital using the ADC0 driver.
 * The function starts the conversion, waits for it to complete,
 * and returns the digital result.
 */
uint16 ADC_ReadChannel(uint8 channel_num)
{
    uint16 ADC_Value;
    if (channel_num == AIN0_CHANNEL)
    {
        /* Start SS0 conversion for ADC0 */
        ADC0_PSSI_REG |= SAMPLE_SEQ_0_MASK;

        /* Check if the raw interrupt status is set (Wait for conversion to complete) */
        while (!(ADC0_RIS_REG & SAMPLE_SEQ_0_MASK))
            ;

        /* Read the 12-bit result from ADC0 */
        ADC_Value = ADC0_SSFIFO0_REG & ADC_RESULT_MASK;

        /* Clear the interrupt flag for ADC0 */
        ADC0_ISC_REG |= SAMPLE_SEQ_0_MASK;
    }
    else if (channel_num == AIN1_CHANNEL)
    {
        /* Start SS0 conversion for ADC1 */
        ADC1_PSSI_REG |= SAMPLE_SEQ_0_MASK;

        /* Check if the raw interrupt status is set (Wait for conversion to complete) */
        while (!(ADC1_RIS_REG & SAMPLE_SEQ_0_MASK))
            ;

        /* Read the 12-bit result from ADC1 */
        ADC_Value = ADC1_SSFIFO0_REG & ADC_RESULT_MASK; // Mask to 12 bits

        /* Clear the interrupt flag for ADC1 */
        ADC1_ISC_REG |= SAMPLE_SEQ_0_MASK;
    }
    return ADC_Value;
}
