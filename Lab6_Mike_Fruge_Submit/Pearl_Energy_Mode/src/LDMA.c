#include "LDMA.h"



/*---------------------------------
 * void LDMA0_init()
 * - Initializes LDMA Channel Descriptor and Transfer Configuration
 * Arguments: Function takes no arguments
 * Returns: Void function - No return value
 -----------------------------------*/

void LDMA0_init()
{
	// Create Initialization structure
	LDMA_Init_t ldma_init = LDMA_INIT_DEFAULT;
	// Create descriptor init structures
//	LDMA_Descriptor_t ldma_descriptor_TX;
//	LDMA_Descriptor_t descriptor_init_RX;
	// Initialize to transfer a single byte from Bluetooth_CMD to LEUART0->TXDATA
	ldma_descriptor_TX =(LDMA_Descriptor_t) LDMA_DESCRIPTOR_SINGLE_M2P_BYTE(Bluetooth_CMD, &LEUART0->TXDATA, CMD_length);
	// Config to transfer memory the LEUART receives to dma
//	descriptor_init_RX =
	// Create transfer struct
//	LDMA_TransferCfg_t ldma_xfer_TX;
	// Initialize xfer config to start a transfer on LEUART0_TXBL or TXEMPTY
	ldma_xfer_TX =(LDMA_TransferCfg_t) LDMA_TRANSFER_CFG_PERIPHERAL(ldmaPeripheralSignal_LEUART0_TXBL);

	// LDMA Initialize
	LDMA_Init(&ldma_init);

	// May need to enable interrupts and write an interrupt handler
	// Enable DONEIFSEN interrupt to wake up the processor when the transfer of Bluetooth_CMD is complete
//	 THe interrupt may be taken care of by the initialization of ldma_descriptor_t
//	LDMA -> CTRL |= LDMA_CH0_CTRL_DONEIFSEN;
}


/*------------------------------
 * void LDMA_IRQHandler()
 * - Processes interrupts that originate from LDMA controller
 * 	- Sets scheduling event for processor
 * Args: Function has no Input Arguments
 * Returns: Function has no return value
 *
 */
void LDMA_IRQHandler()
{
	unsigned int int_flag;
	int_flag = LDMA->IF;
	LDMA -> IFC = int_flag;
	CORE_ATOMIC_IRQ_DISABLE();
	// When LDMA is finished
	if(int_flag && (1<< LDMA_TX_CHANNEL))
	{
		event_trig |= LDMA_TX_DONE_EVENT_MASK;
	}
	CORE_ATOMIC_IRQ_ENABLE();
}
