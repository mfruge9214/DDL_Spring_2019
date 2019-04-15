//	Includes
#include <stdlib.h>
#include <stdint.h>

#define CUTOFF_TEMP	23

// Global Vars



// Function Prototypes
void take_Measurement(void);
void LPM_Enable(void);
void LPM_Disable(void);
float convert_temp(uint16_t temp_code);
