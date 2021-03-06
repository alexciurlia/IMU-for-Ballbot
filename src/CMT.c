/*******************************************************************************
* DISCLAIMER
* This software is supplied by Renesas Electronics Corporation and is only 
* intended for use with Renesas products. No other uses are authorized. This 
* software is owned by Renesas Electronics Corporation and is protected under
* all applicable laws, including copyright laws.
* THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
* THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT
* LIMITED TO WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE 
* AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED.
* TO THE MAXIMUM EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS 
* ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES SHALL BE LIABLE 
* FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR
* ANY REASON RELATED TO THIS SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE
* BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
* Renesas reserves the right, without notice, to make changes to this software
* and to discontinue the availability of this software. By using this software,
* you agree to the additional terms and conditions found by accessing the 
* following link:
* http://www.renesas.com/disclaimer *
* Copyright (C) 2012 Renesas Electronics Corporation. All rights reserved.    
*******************************************************************************/ 
/*******************************************************************************
* File Name    : CMT.c
* Version      : 1.00
* Description  : Simple driver for Compare Match Timer (CMT) peripheral
*******************************************************************************/
/*******************************************************************************
* History : DD.MM.YYYY     Version     Description
*         : 16.02.2012     1.00        First release
*******************************************************************************/

/******************************************************************************
Includes   <System Includes> , "Project Includes"
*******************************************************************************/
#include <stdint.h>
#include <stdio.h>
#include "platform.h"
#include "CMT.h"

/******************************************************************************
Macro Definitions
*******************************************************************************/
/* The CMT in this example is clocked at a rate of 
   (PCLK / 128) or (48 MHz / 128) = 375 kHz */
/* TICK_INTERVAL defines how many times CMT is clocked in 100 ms 
   (or 1/10th of second) */
#define TICK_INTERVAL (48000000 / 128 / 1000)  /* 375 counts of CMT = 100 ms */

volatile int32_t millis = 0;

int32_t get_ms(){
	return millis;
}

void ms_delay(int32_t t){
	int32_t last = millis;
	while(millis-last<t);
}


/*******************************************************************************
* Function name: CMT_init
* Description  : Sets up CMT0 to generate interrupts at 100 ms
* Arguments    : none
* Return value : none
*******************************************************************************/
void CMT_init (void)
{
#ifdef PLATFORM_BOARD_RDKRX63N
	SYSTEM.PRCR.WORD = 0xA50B; /* Protect off */
#endif

    /* Power up CMT0 */
    MSTP(CMT0) = 0;   
     
#ifdef PLATFORM_BOARD_RDKRX63N
	SYSTEM.PRCR.WORD = 0xA500; /* Protect on  */
#endif  
    
    /* Stop the clock */
    CMT.CMSTR0.BIT.STR0 = 0;

    /* Trigger 100 ms from now */
    CMT0.CMCOR = CMT0.CMCNT + TICK_INTERVAL;
    
    /* CMCR - Compare Match Timer Control Register
    b6      CMIE: 1 = Compare match interrupt (CMIn) enabled
    b1:b0   CKS:  2 = Clock selects is PCLK/128 (375 kHz @ PCLK = 48 MHz) 
    */
    CMT0.CMCR.WORD = 0x0042;
    
    /* Set interrupt priority in ICU */
    IPR(CMT0, CMI0) = 0x01;
    
    /* Enable the interrupt in the ICU */
    IEN(CMT0, CMI0) = 1;
    
    /* Start the clock running */ 
    CMT.CMSTR0.BIT.STR0 = 1;
} /* End of function CMT_init() */


/*******************************************************************************
* Function name: CMT_isr
* Description  : Interrupt Service Routine for CMT match interrupt.
*                The CMT counter (CMCNT) is reset to zero when this interrupt is
*                taken so no further action is required to keep the CMT firing
*                at regular intervals.  
*                During this ISR the state of the green LED's on the YRDK RX63N
*                are toggled.
*                The processing in this ISR must take less time than the CMT 
*                compare interval. Use the Performance Counters in HEW to measure
*                execution of your own code.
* Arguments    : none
* Return value : none
*******************************************************************************/
#pragma interrupt (CMT_isr(vect = VECT(CMT0, CMI0)))
static void CMT_isr (void)
{
    millis++;
} /* End of CMT_isr() */

