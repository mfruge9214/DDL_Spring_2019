//***********************************************************************************
// Include files
//***********************************************************************************

#include <stdbool.h>
#include <stdint.h>
#include <stdbool.h>
#include "em_core.h"
#include "em_device.h"
#include "em_chip.h"
#include "em_cmu.h"
#include "em_emu.h"


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

#define Letimer0_Period		4			// Letimer0 in seconds
#define Letimer0_On_Time	25			// Letimer0 On Time in milliseconds

#define	Letimer0_EM			EM4			// define first energy mode that it cannot enter for LETIMER0

//***********************************************************************************
// global variables
//***********************************************************************************
#define	Max_Sleep_Modes		6
unsigned int lowest_energy_mode[Max_Sleep_Modes];

//***********************************************************************************
// function prototypes
//***********************************************************************************
