//***********************************************************************************
// Include files
//***********************************************************************************

#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "em_core.h"
#include "em_device.h"
#include "em_chip.h"
#include "em_cmu.h"
#include "em_emu.h"
#include "em_leuart.h"


//***********************************************************************************
// enumerations
//***********************************************************************************

#define	EM0					0
#define EM1					1
#define EM2					2
#define	EM3					3
#define	EM4					4
#define	EM4s				5

//***********************************************************************************
// defined files
//***********************************************************************************

#define Letimer0_Period		3			// Turns on sensor enable
#define Letimer0_On_Time	25			// Determines the amount of time until the sensor takes a measurement after LETIMER0_Period

#define	Letimer0_EM			EM3			// define first energy mode that it cannot enter for LETIMER0
#define	Max_Sleep_Modes		6


#define COMP1_EVENT_MASK	0x1		// Comp1 mask to set event trig
#define COMP0_EVENT_MASK	0x2	// Comp0 mask...
#define STARTF_EVENT_MASK	0x4	// STARTF mask
#define SIGF_EVENT_MASK		0x8	// SIGF mask
#define TXBL_EVENT_MASK		0x10
#define RXDATAV_EVENT_MASK	0x20
#define TXC_EVENT_MASK		0x40
#define LDMA_TX_DONE_EVENT_MASK	0x80
// TX and RX as well


#define CMD_length			7
#define CHANGE_FORMAT_CMD_LEN	4
//***********************************************************************************
// global variables
//***********************************************************************************

/* Global Variables for Program */

uint32_t event_trig;
char Data_Received [CHANGE_FORMAT_CMD_LEN];

char Bluetooth_CMD[CMD_length];
unsigned int lowest_energy_mode[Max_Sleep_Modes];
bool Celcius;
bool Format_Command_Valid;

//***********************************************************************************
// function prototypes
//***********************************************************************************
