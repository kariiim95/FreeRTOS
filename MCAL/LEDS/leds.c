/******************************************************************************
 *
 * Module: LEDS OF PF1, PF2 and PF3 (RED, Blue and Green LEDs)
 *
 * File Name: leds.c
 *
 * Description: source file for the TM4C123GH6PM LEDS driver
 *
 * Author: Marawan Ragab
 *
 *******************************************************************************/
#include "leds.h"
#include "tm4c123gh6pm_registers.h"
/*******************************************************************************
 *                         Private Functions Definitions                       *
 *******************************************************************************/
static void GPIO_SetupLEDPins(void)
{
    GPIO_PORTF_AMSEL_REG &= 0xF1;         /* Disable Analog on PF1, PF2 and PF3 */
    GPIO_PORTF_PCTL_REG  &= 0xFFFF000F;   /* Clear PMCx bits for PF1, PF2 and PF3 to use it as GPIO pin */
    GPIO_PORTF_DIR_REG   |= 0x0E;         /* Configure PF1, PF2 and PF3 as output pin */
    GPIO_PORTF_AFSEL_REG &= 0xF1;         /* Disable alternative function on PF1, PF2 and PF3 */
    GPIO_PORTF_DEN_REG   |= 0x0E;         /* Enable Digital I/O on PF1, PF2 and PF3 */
    GPIO_PORTF_DATA_REG  &= 0xF1;         /* Clear bit 0, 1 and 2 in Data register to turn off the leds */
}
/*******************************************************************************
 *                         Public Functions Definitions                        *
 *******************************************************************************/
void Leds_Init(void)
{
   /* Enable clock for PORTF and wait for clock to start */
   SYSCTL_RCGCGPIO_REG |= 0x20;
   while(!(SYSCTL_PRGPIO_REG & 0x20));
   GPIO_SetupLEDPins();
}
