/* Includes */
#include "LEUART.h"
#include "main.h"

/* Function Implementations */

void leuart0_init(void){
	LEUART_Init_TypeDef leuart_init;
/* Set the LEUART init values and initialize the LEUART0*/
	leuart_init.enable= false; // Enable LEUART after full initialization
	leuart_init.refFreq= 0;		// Use clock tree freq
	leuart_init.baudrate= 9600;	// Use standard Baud
	leuart_init.databits= leuartDatabits8;	// 8 data bits
	leuart_init.parity= leuartNoParity;		// No parity
	leuart_init.stopbits=leuartStopbits1;	// 1 stop bit

	LEUART_Init(LEUART0, &leuart_init);

	/* Route the pins for the LEUART0 to output to the pins on the board */
	// The pins for RX and TX on the board are 13 and 11
//	LEUART0->ROUTELOC0=LEUART_ROUTELOC0_RXLOC_LOC13 | LEUART_ROUTELOC0_TXLOC_LOC11;
	LEUART0->ROUTELOC0=LEUART_ROUTELOC0_RXLOC_LOC18 | LEUART_ROUTELOC0_TXLOC_LOC18;
	// Enable the TX and RX pins
	LEUART0->ROUTEPEN= LEUART_ROUTEPEN_RXPEN | LEUART_ROUTEPEN_TXPEN;

	LEUART0->IEN |= LEUART_IEN_RXDATAV;
	LEUART0->IFC |= LEUART_IEN_RXDATAV;
//	Enable the LEUART for both transmit and receive
	LEUART0->CMD |= LEUART_CMD_RXEN;
	LEUART0->CMD |= LEUART_CMD_TXEN;
//	Enable read from TXDATA transmission
	LEUART0->CTRL |= LEUART_CTRL_LOOPBK;

	NVIC_EnableIRQ(LEUART0_IRQn);
}


void transmit_Byte(char byte){
	while(LEUART0->SYNCBUSY);
	LEUART0->TXDATA= byte;
	UART_Transmit_OK=false;
}

char receive_Byte(){
	UART_Receive_OK=false;
	return LEUART0->RXDATA;
}


void LEUART0_IRQHandler(){
	unsigned int int_flag;
	int_flag=LEUART0->IF;
	LEUART0->IFC=int_flag;

	CORE_ATOMIC_IRQ_DISABLE();

	if(int_flag & LEUART_IF_TXBL){
//		Set transmitter Enable Status to 1
		LEUART0->CMD |= LEUART_CMD_TXEN;
//		Set state variable to true
		UART_Transmit_OK=true;
		event_trig |= LEUART_IF_TXBL;
//		Disable the TXBL interrupt so we can leave the handler and send the bit
		LEUART0->IEN &= ~(LEUART_IEN_TXBL);
	}
	if(int_flag & LEUART_IF_RXDATAV){
		UART_Receive_OK=true;
		event_trig |= LEUART_IF_RXDATAV;
		LEUART0->IEN &= ~(LEUART_IEN_RXDATAV);
	}
	CORE_ATOMIC_IRQ_ENABLE();
}
