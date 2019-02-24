// Include Files

#include "i2c.h"
#include "main.h"
#include "gpio.h"

void i2c0_init(void)
{
	// Initialize I2C struct
	I2C_Init_TypeDef init;
	init.enable=false;
	init.master=true;
	init.refFreq=0;
	init.freq=I2C_FREQ_FAST_MAX;
	init.clhr= _I2C_CTRL_CLHR_FAST;

	I2C_Init(I2C0, &init);

	// Route the pins
	I2C0 ->ROUTELOC0= I2C_ROUTELOC0_SCLLOC_LOC15 | I2C_ROUTELOC0_SDALOC_LOC15;
	I2C0->ROUTEPEN= I2C_ROUTEPEN_SCLPEN | I2C_ROUTEPEN_SDAPEN;

//	 Enable I2C interrupts
//	 Done in the LPM_Enable function

	ACK_cont=false;
	RXDATA_cont=false;

	//	Reset bus if busy
	//	Toggle I2C 9 times to reset any slaves that need it
	reset_I2C_Bus();
//	Enable the interrupts
//	NVIC_EnableIRQ(I2C0_IRQn);
}


void read_I2C_User_Reg1(void)
{
//	 Follow flowchart
//	 Start and slave Address and Write
	I2C0->CMD = I2C_CMD_ABORT;
	I2C0->CMD = I2C_CMD_CLEARPC;

	start_I2C(I2C_WRITE);
//	Response
	wait_for_ACK();

	I2C0->TXDATA=READ_COMMAND;		// Read reg cmd
//	Response
	wait_for_ACK();
//	Restart
	start_I2C(I2C_READ);

	wait_for_ACK();
	// Read data
//	while(!(I2C0->IF & I2C_IF_RXDATAV));
	wait_for_RXDATAV();
//	Automatically clears the flag
//	 58 when it is working
	uint8_t data=in_data;  // in_data does not show up in the variables window during debug because it is global...
//	... Using local var for now
	in_data=0;
	stop_I2C();
}

void read_I2C_Temp_Sensor()
{
	I2C0->CMD= I2C_CMD_ABORT;
	I2C0->CMD= I2C_CMD_CLEARPC;

	start_I2C(I2C_WRITE);
	wait_for_ACK();
	I2C0->TXDATA=MEASURE_COMMAND;
	wait_for_ACK();
	start_I2C(I2C_READ);
	wait_for_ACK();
//	Wait for the data to be valid in the RXDATA buffer
	wait_for_RXDATAV();
//	Received most significant byte
	save_data= in_data;
//	Shift the data over 8
	save_data=save_data<<8;
//	Send an ACK
	I2C0->CMD=I2C_CMD_ACK;
	wait_for_RXDATAV();
	save_data= save_data | in_data;
	stop_I2C();
}

void start_I2C(bool read_or_write)		// Start I2C with a read or a write bit
{
	I2C0->CMD = I2C_CMD_START;
	I2C0->TXDATA =(SENSOR_ADDRESS<<1)| read_or_write;
}

void write_I2C(uint8_t data)
{
	start_I2C(I2C_WRITE);
	wait_for_ACK();
	I2C0->TXDATA=WRITE_COMMAND;
	wait_for_ACK();
	I2C0->TXDATA=data;
//	Send ACK
	I2C0->CMD=I2C_CMD_ACK;
//	Send Stop
	I2C0->CMD=I2C_CMD_STOP;
}

void wait_for_RXDATAV(void)
{
	while (!(I2C0->IF & I2C_IF_RXDATAV));
	in_data=I2C0->RXDATA;
//	Disabled next line because cant call interrupts within interrupts
//	RXDATA_cont=false;
}

void wait_for_ACK(void)
{
	while(!(I2C0->IF & I2C_IF_ACK)); // Wait for the ACK bit to be sent
	I2C0->IFC= I2C_IFC_ACK;		// Clear the flag for the ACK bit
//	while (!ACK_cont);
//	ACK_cont=false;
}

void stop_I2C(void)
{
	I2C0->CMD=I2C_CMD_NACK;
	I2C0->CMD=I2C_CMD_STOP;
}

void reset_I2C_Bus(void)
{
//	Toggle data pins to reset the si7021
	for (int i=0; i<9; i++)
		{
			GPIO_PinOutClear(I2C_SCL_PORT, I2C_SCL_PIN);
			GPIO_PinOutSet(I2C_SCL_PORT, I2C_SCL_PIN);
		}
//	Reset the state machine of bus
	if(I2C0->STATE & I2C_STATE_BUSY)
	{
		I2C0->CMD=I2C_CMD_ABORT;
	}
}


/*void I2C0_IRQHandler(void)
{

//	 * For the Extra credit, handle all the interrupts within this IRQ handler
//	 * If int_flag== RXDATAV, then read the data
//	 * if int_flag== ACK, clear the ack

	unsigned int int_flag;

	CORE_ATOMIC_IRQ_DISABLE();		// prevent nesting of interrupts for initial interrupt handling
	int_flag=I2C0->IF;
	I2C0->IFC=int_flag;

	if (int_flag & I2C_IF_ACK)
	{
		I2C0->IFC = I2C_IFC_ACK;
		ACK_cont=true;
	}

	if (int_flag & I2C_IF_RXDATAV)
	{
		RXDATA_cont=true;
		in_data=I2C0->RXDATA;
	}

	CORE_ATOMIC_IRQ_ENABLE();

}*/

