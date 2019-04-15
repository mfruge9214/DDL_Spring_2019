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
#include "LDMA.h"

/* For Lab 6:
 * CPU only wakes up for:
 * Taking temp and converting to ascii
 * DMA done interrupt
 * TXBL interrupt becomes a TXC interrupt
 *
 * Dont use TXBL, because that will wake your processor every time a DMA transfer occurs
 */


int main(void)
{
  /* initialized variables */
  event_trig=0;
  char CMD[CMD_length];
  float Degrees;
  bool Frame_Received = false;
  bool TxFinished = false;
  bool Reset = false;
  Format_Command_Valid = false;
  // Init Celcius as true by default
  Celcius = true;
  /* Declare local vars */
//  i keeps track of characters sent
  int TxBytes=0;
//  j keeps track of characters received
  int RxBytes=0;
//  I is used to copy from Bluetooth_CMD to local CMD
  int i=0;


  Sleep_Block_Mode(EM4); 	// set lowest energy mode for the system

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

  /* Initialize LDMA */
  LDMA0_init();

  LETIMER_Enable(LETIMER0, true);
  I2C_Enable(I2C0, true);
  LEUART_Enable(LEUART0, leuartEnable);

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

	  if(event_trig & LDMA_TX_DONE_EVENT_MASK)
	  {
		  event_trig &= ~(LDMA_TX_DONE_EVENT_MASK);
		  // Enable TXC to know when the LEUART transfer is done
		  LEUART0->IEN |= LEUART_IF_TXC;
	  }

	  /* Begin receiving incoming frame */
	  if(event_trig & STARTF_EVENT_MASK)
	  {
//		  Since we have received the start frame, enable the RXDATAV
//		  LEUART0->IEN |= LEUART_IEN_RXDATAV;
		  event_trig &= ~(STARTF_EVENT_MASK);
		  // If we have already received a frame, and we receive another StartFrame, reset the 'assembler'
		  if(Frame_Received)
		  {
			// Reset the assembly of the command
			  RxBytes = 0;
		  }
		  Frame_Received= true;
		  // Per the Lab
		  LEUART0->IEN |= LEUART_IEN_RXDATAV;
	  }


  /* Receive the format command from user */
	  if(event_trig & RXDATAV_EVENT_MASK)
	  {
	  // Receive data 1 byte at a time and store in the Data_Received array
	  //  Clear event_trig
		  event_trig &= ~(RXDATAV_EVENT_MASK);
		  Data_Received[RxBytes]=receive_Byte();
		  RxBytes++;
		  LEUART0->IEN |= LEUART_IEN_RXDATAV;
	  }

/* Transmission complete, block EM4 and disable DMA to work in Low Energy Mode*/
	  if(event_trig & TXC_EVENT_MASK)
	  {
		  LEUART0->IEN &= ~(LEUART_IEN_TXC);
		  event_trig &= ~(TXC_EVENT_MASK);
		  Sleep_UnBlock_Mode(EM3);
		  LEUART0->CTRL &= ~(LEUART_CTRL_TXDMAWU);
	  }

/* Stop Receiving bytes and parse the command that was received */
	  if(event_trig & SIGF_EVENT_MASK)
	  {
		  // Set RxBytes to 0 when sigf is detected
		  RxBytes = 0;
//		  Re-enable RXBLOCKEN
		  event_trig &= ~(SIGF_EVENT_MASK);
		  LEUART0->IEN &= ~(LEUART_IEN_RXDATAV);
		  LEUART0->CMD |= LEUART_CMD_RXBLOCKEN;
		  if(Frame_Received)
		  {
			  parse_Command(Data_Received);
		  }
		  Frame_Received = false;

	  }

	  // We have disabled the TXBL interrupt
//	  /* Send the temp reading */
//	  if(event_trig & TXBL_EVENT_MASK)
//	  {
//
//		  LEUART0->CMD |= LEUART_CMD_CLEARTX;
////		 Transmit byte
//		  event_trig &= ~(TXBL_EVENT_MASK);
////		  for(i=0; i<10000; i++);
//		  transmit_Byte(CMD[TxBytes]);
//		  TxBytes++;
////		  Re-enable the interrupt if more bits need to be sent
////		  if(i==CMD_length)
//		  if(TxBytes<= CMD_length)
//		  {
////			  LEUART0->IEN |= LEUART_IEN_TXBL;
//		  }
//		  else{
//			  TxFinished=true;
//		  }
//	  }


	  /* Measure Temp and convert to ASCII to send*/
	  if(event_trig & COMP1_EVENT_MASK){
		  event_trig &= ~(COMP1_EVENT_MASK);
		  TxFinished=false;
		  RxBytes=0;
		  TxBytes=0;
		  LPM_Enable();
		  take_Measurement();
		  Degrees=convert_temp(save_data);
		  LPM_Disable();
		  Temp_to_ASCII(Degrees);
//		  for (i=0;i<8; i++) CMD[i]=Bluetooth_CMD[i];
//		  CMD[i]='\0';
		  i=0;
//		  DOn't enable interrupt because that will wake up the processor, we only use TXBL as a signal for the DMA to transfer another byte
//		  LEUART0->IEN |= LEUART_IEN_TXBL;
		  // Configure the LEUART to support DMA down to EM2
		  // When set to 1, the DMA controller will get requests when there is space available in the transmit buffer
		  	LEUART0->CTRL |= LEUART_CTRL_TXDMAWU;
		  	// Block the system from going into EM3 until all data has been transmitted (TXC goes high)
		  Sleep_Block_Mode(EM3);
		  // Start DMA transfer after the temperature has been converted to ASCII
		  LDMA_StartTransfer(LDMA_TX_CHANNEL, &ldma_xfer_TX, &ldma_descriptor_TX);
	  }

  	}
 }
