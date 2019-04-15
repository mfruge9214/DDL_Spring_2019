// Include Files

#include <em_i2c.h>
#include <stdbool.h>


#define SI7021_FREQ		400000 // kHz
#define SENSOR_ADDRESS	0x40 // User register 1
#define I2C_WRITE		0
#define I2C_READ		1
#define READ_COMMAND	0xE7
#define WRITE_COMMAND	0xE6
#define MEASURE_COMMAND	0xE3
#define BIT_RESOLUTION  0x3B

// Global Vars
bool ACK_cont;
bool RXDATA_cont;

volatile uint16_t in_data;		// The data received from the I2C read;
volatile uint16_t save_data;		// The data needed to calculate the temp

// Function Prototypes
void i2c0_init(void);
void read_I2C_User_Reg1(void);
void read_I2C_Temp_Sensor(void);
void write_I2C(uint8_t data);
void start_I2C(bool read_or_write);
void wait_for_ACK(void);
void wait_for_RXDATAV(void);
void stop_I2C(void);
void reset_I2C_Bus(void);
void enable_I2C_Interrupts(void);
//void I2C_IRQHandler(void);

