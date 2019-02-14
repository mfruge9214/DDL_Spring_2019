//	Includes

#include "si7021.h"

// Global Vars





// Functions
void take_Measurement(){

}
void LPM_Enable()
{
	Sleep_Block_Mode(EM2);
//	Reset state machines and enable SCL and SDA
	GPIO_PinModeSet(I2C_SCL_PORT, I2C_SCL_PIN, gpioModeWiredAnd, I2C_SCL_DEFAULT);
	GPIO_PinModeSet(I2C_SDA_PORT, I2C_SDA_PIN, gpioModeWiredAnd, I2C_SDA_DEFAULT);
	reset_I2C_Bus();
//	 Enable the relevant I2C interrupts
	I2C0->IFC |= I2C_IFC_ACK;
	I2C0->IEN |= I2C_IEN_ACK;
	I2C0->IEN |= I2C_IEN_RXDATAV;
}
void LPM_Disable()
{
//	Disable interrupts
	I2C0->IEN &= !(I2C_IEN_ACK);
	I2C0->IEN &= !(I2C_IEN_RXDATAV);
//	Disable SCL and SDA and Sensor_Enable
	GPIO_PinModeSet(I2C_SCL_PORT, I2C_SCL_PIN, gpioModeDisabled, I2C_SCL_DEFAULT);
	GPIO_PinModeSet(I2C_SDA_PORT, I2C_SDA_PIN, gpioModeDisabled, I2C_SDA_DEFAULT);
	GPIO_PinModeSet(SENSOR_ENABLE_PORT, SENSOR_ENABLE_PIN, gpioModeDisabled, SENSOR_DEFAULT);
}
