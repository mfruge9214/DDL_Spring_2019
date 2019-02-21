//***********************************************************************************
// Include files
//***********************************************************************************

#include "em_gpio.h"

//***********************************************************************************
// defined files
//***********************************************************************************

// LED0 pin is PF4
#define	LED0_port		gpioPortF
#define LED0_pin		04U
#define LED0_default	false 	// off
// LED1 pin is PF5
#define LED1_port		gpioPortF
#define LED1_pin		05U
#define LED1_default	false	// off

// Ports for I2C
#define I2C_SDA_PORT	gpioPortC
#define I2C_SDA_PIN		10
#define I2C_SDA_DEFAULT	true
#define I2C_SCL_PORT	gpioPortC
#define I2C_SCL_PIN		11
#define I2C_SCL_DEFAULT	true

// Sensor Enable
#define SENSOR_ENABLE_PORT	gpioPortB
#define SENSOR_ENABLE_PIN	10
#define SENSOR_DEFAULT		true
// SCL Port C pin 11

// Enable GPIO
// Route locations
// Enable Locations

//***********************************************************************************
// global variables
//***********************************************************************************


//***********************************************************************************
// function prototypes
//***********************************************************************************
void gpio_init(void);

