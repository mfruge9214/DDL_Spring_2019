/* Includes */
#include "main.h"
#include "em_leuart.h"




/* Function Prototypes */

void leuart0_init(void);
void transmit_Byte(char);
char receive_Byte();
void Temp_to_ASCII(float temp);

