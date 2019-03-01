/***************************************************************************//**
 * @file
 * @brief Simple LED Blink Demo for SLSTK3402A
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc. Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement. This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/


#include "main.h"
#include "bsp.h"
#include "gpio.h"
#include "cmu.h"
#include "letimer.h"
#include "i2c.h"
#include "sleep_routines.h"
#include "LEUART.h"




int main(void)
{
  /* initialized variables */
  UART_Transmit_OK=false;
  UART_Receive_OK=false;
  event_trig=0;

  /* Declare local vars */
  char CMD[]="AT+NAMEmifr";
//  i keeps track of characters sent
  int i=0;
//  j keeps track of characters recieved
  int j=0;


  Sleep_Block_Mode(EM3); 	// set lowest energy mode for the system

  EMU_DCDCInit_TypeDef dcdcInit = EMU_DCDCINIT_DEFAULT;
  CMU_HFXOInit_TypeDef hfxoInit = CMU_HFXOINIT_DEFAULT;

  /* Chip errata */
  CHIP_Init();

  /* Init DCDC regulator and HFXO with kit specific parameters */
  /* Initialize DCDC. Always start in low-noise mode. */
  EMU_EM23Init_TypeDef em23Init = EMU_EM23INIT_DEFAULT;
  EMU_DCDCInit(&dcdcInit);
  em23Init.vScaleEM23Voltage = emuVScaleEM23_LowPower;
  EMU_EM23Init(&em23Init);
  CMU_HFXOInit(&hfxoInit);

  /* Switch HFCLK to HFXO and disable HFRCO */
  CMU_ClockSelectSet(cmuClock_HF, cmuSelect_HFXO);
  CMU_OscillatorEnable(cmuOsc_HFRCO, false, false);

  /* Initialize clocks */
  cmu_init();

  /* Initialize GPIO */
  gpio_init();

  /* Initialize LETIMER0 */
//  letimer0_init();

  // Initialize i2c0
//  i2c0_init();

  /* Initialize LEUART */
  leuart0_init();

//  LETIMER_Enable(LETIMER0, true);
//  I2C_Enable(I2C0, true);
  LEUART_Enable(LEUART0, true);



  // enable interrupts after chip has been configured
  CORE_ATOMIC_IRQ_ENABLE();
//  Turn on TXBL interrupt for the scheduler
  LEUART0->IEN |= LEUART_IEN_TXBL;

  while (1) {
	  if(!event_trig){
		  Enter_Sleep();
	  }

	  if(UART_Transmit_OK && (event_trig | LEUART_IF_TXBL))
	  {
//		 Transmit byte
		  event_trig &= ~(LEUART_IF_TXBL);
		  transmit_Byte(CMD[i]);
		  i++;
//		  Re-enable the interrupt
		  LEUART0->IEN |= LEUART_IEN_TXBL;
	  }
	  if(UART_Receive_OK && (event_trig | LEUART_IF_RXDATAV))
	  {
//		  Receive data 1 byte at a time and store in the Data_Received array
//		  Clear event_trig
		  event_trig &= ~(LEUART_IF_RXDATAV);
		  Data_Received[j]=receive_Byte();
		  j++;
//		  Re-enable the interrupt
		  LEUART0->IEN |= LEUART_IEN_RXDATAV;
	  }
  	}
 }
