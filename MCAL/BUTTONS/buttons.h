 /******************************************************************************
 *
 * Module: BUTTONS of PF0 and PF4
 *
 * File Name: buttons.h
 *
 * Description: Header file for the TM4C123GH6PM BUTTONS driver
 *
 * Author: Marawan Ragab
 *
 *******************************************************************************/
#ifndef BUTTONS_H_
#define BUTTONS_H_
#include "std_types.h"
/*******************************************************************************
 *                             Preprocessor Macros                             *
 *******************************************************************************/
#define GPIO_PORTF_PRIORITY_MASK      0xFF1FFFFF
#define GPIO_PORTF_PRIORITY_BITS_POS  21
#define GPIO_PORTF_INTERRUPT_PRIORITY 2
/*******************************************************************************
 *                            Functions Prototypes                             *
 *******************************************************************************/
void Buttons_Init(void);

#endif
