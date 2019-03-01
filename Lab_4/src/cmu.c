//***********************************************************************************
// Include files
//***********************************************************************************
#include "cmu.h"
#include "main.h"

//***********************************************************************************
// defined files
//***********************************************************************************


//***********************************************************************************
// global variables
//***********************************************************************************


//***********************************************************************************
// function prototypes
//***********************************************************************************
void cmu_init(void){
	// Enable the HFPERCLK for desired peripherals such as ADC
	CMU_ClockSelectSet(cmuClock_HFPER, cmuSelect_HFXO);
	CMU_ClockEnable(cmuClock_HFPER, true);
	CMU_HFXOAutostartEnable(true, true, true);


	// By default, LFRCO is disabled
	CMU_OscillatorEnable(cmuOsc_LFRCO, false, false);	// using LFXO or ULFRCO

	// Route LF clock to the LF clock tree
	if (Letimer0_EM == EM4) {
		CMU_OscillatorEnable(cmuOsc_ULFRCO, true, true);
		CMU_OscillatorEnable(cmuOsc_LFXO, false, false);		// Disable LFXO
		CMU_ClockSelectSet(cmuClock_LFA, cmuSelect_ULFRCO);	// Use ULFRCO in EM3 for LETIMER0
		CMU_ClockSelectSet(cmuClock_LFB, cmuSelect_ULFRCO);
	}
	else {
		CMU_OscillatorEnable(cmuOsc_ULFRCO, false, false);
		CMU_OscillatorEnable(cmuOsc_LFXO, true, true);		// Enable LFXO
		CMU_ClockSelectSet(cmuClock_LFA, cmuSelect_LFXO);	// routing LFXO clock to LFA
		CMU_ClockSelectSet(cmuClock_LFB, cmuSelect_LFXO);
	}

	CMU_ClockEnable(cmuClock_HFPER, true);
	CMU_ClockEnable(cmuClock_CORELE, true);


	// Peripheral clocks enabled
	CMU_ClockEnable(cmuClock_GPIO, true);
	CMU_ClockEnable(cmuClock_LETIMER0, true);
	CMU_ClockEnable(cmuClock_I2C0, true);
	CMU_ClockEnable(cmuClock_LEUART0, true);

}

