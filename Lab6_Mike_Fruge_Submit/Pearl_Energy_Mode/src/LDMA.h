#include "main.h"
#include "LEUART.h"
#include "em_ldma.h"

#define LDMA_TX_CHANNEL	0

// Global Variables
LDMA_Descriptor_t ldma_descriptor_TX;
LDMA_TransferCfg_t ldma_xfer_TX;


void LDMA0_init(void);
void LDMA_IRQHandler(void);
