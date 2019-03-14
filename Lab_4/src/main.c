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
#include "si7021.h"




int main(void)
{
  /* initialized variables */
  event_trig=0;
  char CMD[CMD_length];
  float DegreesC;
  /* Declare local vars */
//  i keeps track of characters sent
  int TxBytes=0;
//  j keeps track of characters received
  int RxBytes=0;
//  I is used to copy from Bluetooth_CMD to local CMD
  int i=0;


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
  letimer0_init();

//   Initialize i2c0
  i2c0_init();

  /* Initialize LEUART */
  leuart0_init();

  LETIMER_Enable(LETIMER0, true);
  I2C_Enable(I2C0, true);
//  LEUART_Enable(LEUART0, leuartEnable);

  while(LEUART0->SYNCBUSY);

  // enable interrupts after chip has been configured
  CORE_ATOMIC_IRQ_ENABLE();

//  Put CPU to sleep until interrupt
  Enter_Sleep();

  while (1) {
	  if(!event_trig){
//		  TxBytes=0;
//		  RxBytes=0;
		  Enter_Sleep();
	  }

	  if(event_trig & LEUART_IF_TXBL)
	  {

		  LEUART0->CMD |= LEUART_CMD_CLEARTX;
//		 Transmit byte
		  event_trig &= ~(LEUART_IF_TXBL);
		  for(i=0; i<10000; i++);
		  transmit_Byte(CMD[TxBytes]);
		  TxBytes++;
//		  Re-enable the interrupt if more bits need to be sent
//		  if(i==CMD_length)
		  if(TxBytes>CMD_length)
		  {
			  LEUART0->CMD = LEUART_CMD_TXDIS;
      		  TxBytes=0;
		  }
		  else{
			  LEUART0->IEN |= LEUART_IEN_TXBL;
		  }
	  }

	  if(event_trig & LEUART_IF_RXDATAV )
	  {
	  // Receive data 1 byte at a time and store in the Data_Received array
	  //  Clear event_trig
		  event_trig &= ~(LEUART_IF_RXDATAV);
		  Data_Received[RxBytes]=receive_Byte();
//		  RxBytes++;
	//	Re-enable the interrupt if there are more bytes to be received
		  if(RxBytes>=CMD_length)
		  {
			  LEUART0->CMD = LEUART_CMD_RXDIS;
			  RxBytes=0;
		  }
		  else
		  {
			  LEUART0->IEN |= LEUART_IEN_RXDATAV;
		  }
	  }

	  if(event_trig & COMP1_EVENT_MASK){
		  event_trig &= ~(COMP1_EVENT_MASK);
		  RxBytes=0;
		  TxBytes=0;
		  LPM_Enable();
		  take_Measurement();
		  DegreesC=convert_temp(save_data);
//		  if(DegreesC<CUTOFF_TEMP)
//		  {
//			  GPIO_PinOutSet(LED0_port, LED0_pin);
//		  }
//		  else{
//			  GPIO_PinOutClear(LED0_port, LED0_pin);
//		  }
		  LPM_Disable();
		  Temp_to_ASCII(DegreesC);
		  for (i=0;i<8; i++) CMD[i]=Bluetooth_CMD[i];
		  CMD[i]='\0';
		  i=0;
		  LEUART_Enable(LEUART0, leuartEnable);
//		  LEUART0->CMD |= LEUART_CMD_CLEARRX | LEUART_CMD_CLEARTX;
		  LEUART0->IEN |= LEUART_IEN_TXBL;
		 // LEUART0->IEN |= LEUART_IEN_RXDATAV;
	  }

  	}
 }
