//***********************************************************************************
// Include files
//***********************************************************************************

#include <stdbool.h>
#include <stdint.h>
#include <string.h>
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

#define Letimer0_Period		5			// Turns on sensor enable
#define Letimer0_On_Time	25			// Determines the amount of time until the sensor takes a measurement after LETIMER0_Period

#define	Letimer0_EM			EM3			// define first energy mode that it cannot enter for LETIMER0
#define	Max_Sleep_Modes		6
#define COMP1_EVENT_MASK	0x1		// Comp1 mask to set event trig
#define CMD_length			7
//***********************************************************************************
// global variables
//***********************************************************************************

/* Global Variables for Program */

uint8_t event_trig;
char Data_Received [13];

char Bluetooth_CMD[CMD_length];
unsigned int lowest_energy_mode[Max_Sleep_Modes];

//***********************************************************************************
// function prototypes
//***********************************************************************************
