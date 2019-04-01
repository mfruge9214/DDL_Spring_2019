/* Includes */
#include "LEUART.h"
#include "main.h"

/* Function Implementations */

void leuart0_init(void){
	LEUART_Init_TypeDef leuart_init;
/* Set the LEUART init values and initialize the LEUART0*/
	leuart_init.enable= false; // Enable LEUART after full initialization
	leuart_init.refFreq= 0;		// Use clock tree freq
	leuart_init.baudrate= 9600;	// Use standard Baud
	leuart_init.databits= leuartDatabits8;	// 8 data bits
	leuart_init.parity= leuartNoParity;		// No parity
	leuart_init.stopbits=leuartStopbits1;	// 1 stop bit

	LEUART_Init(LEUART0, &leuart_init);
	while(LEUART0->SYNCBUSY);
	/* Route the pins for the LEUART0 to output to the pins on the board */
	// The pins for RX and TX on the board are 13 and 11
	LEUART0->ROUTELOC0=LEUART_ROUTELOC0_RXLOC_LOC18 | LEUART_ROUTELOC0_TXLOC_LOC18;
	LEUART0->ROUTEPEN= LEUART_ROUTEPEN_RXPEN | LEUART_ROUTEPEN_TXPEN;


//	LEUART0->IFC |= LEUART_IEN_RXDATAV;
//	Enable the LEUART for both transmit and receive
//	LEUART0->CMD |= LEUART_CMD_RXEN;
//	LEUART0->CMD |= LEUART_CMD_TXEN;
//	Enable read from TXDATA transmission
//	LEUART0->CTRL |= LEUART_CTRL_LOOPBK;

//	Need to set start frame in LEUART_STARTFRAME  : ?
//	Need to enable SFUBRX to unblock RX when a ? is received
//	Enable RXBLOCK to block RXdata until ? is detected

	LEUART0->STARTFRAME= '?';
	LEUART0->SIGFRAME = '#';
	LEUART0->CMD |= LEUART_CMD_RXBLOCKEN;
	LEUART0->CTRL |= LEUART_CTRL_SFUBRX;
	// Enable interrupts from startframe and sigframe
	LEUART0->IEN |= LEUART_IEN_SIGF;
	LEUART0->IEN |= LEUART_IEN_STARTF;

	NVIC_EnableIRQ(LEUART0_IRQn);
}


void transmit_Byte(char byte){
	while(LEUART0->SYNCBUSY);
	if(byte != NULL)
	{
		LEUART0->TXDATA= byte;
	}
	else return;
}



char receive_Byte(){
	while(LEUART0->SYNCBUSY);
	return LEUART0->RXDATA;
}


void LEUART0_IRQHandler(){
	unsigned int int_flag;
	int_flag=LEUART0->IF & LEUART0->IEN;
	LEUART0->IFC=int_flag;

	CORE_ATOMIC_IRQ_DISABLE();
//	while(LEUART0->SYNCBUSY);


	if(int_flag & LEUART_IF_TXBL){
//		Set transmitter Enable Status to 1
//		LEUART0->CMD |= LEUART_CMD_TXEN;
//		Set state variable to true
//		UART_Transmit_OK=true;
		event_trig |= TXBL_EVENT_MASK;
//		Disable the TXBL interrupt so we can leave the handler and send the bit
		LEUART0->IEN &= ~(LEUART_IEN_TXBL);
	}
	if(int_flag & LEUART_IF_RXDATAV){
//		UART_Receive_OK=true;
		event_trig |= RXDATAV_EVENT_MASK;
		LEUART0->IEN &= ~(LEUART_IEN_RXDATAV);
	}
	if(int_flag & LEUART_IF_STARTF)
	{
//		Set the event for the scheduler
		event_trig |= STARTF_EVENT_MASK;
//		Clear the STARTF flag in IF
		LEUART0->IFC |= LEUART_IFC_STARTF;
	}
	if(int_flag & LEUART_IF_SIGF)
	{
		event_trig |= SIGF_EVENT_MASK;
		LEUART0->IFC |= LEUART_IFC_SIGF;
	}
	CORE_ATOMIC_IRQ_ENABLE();
}

void Temp_to_ASCII(float temp)
{
	int j=0;
	int i=0;
	int temp_int;
	int integer_places=1;
	int working;
	int num_zeros=3;
//	Assign the + or - to global variable
	if(temp>0)
	{
		Bluetooth_CMD[i]=0x2B;
	}
	else{
		Bluetooth_CMD[i]=0x2D;
		temp= temp * -1;
	}
	i++;

//	Figure out how many digits go in front of the decimal
	while(integer_places<temp)
	{
		integer_places=integer_places*10;
		num_zeros--;
	}
	// Fill in the leading zeros of the number we need to send
	for(j=0; j<num_zeros; j++)
	{
		Bluetooth_CMD[i+j]=0x30;
	}
//	Keep index continuous so we don't overwrite characters
	i=i+j;
//	Num_zeeros is only equal to 3 if the absolute value of the temp is less than 1
//	Handle special case of -1<temp<1
	if(num_zeros==3)
	{
		Bluetooth_CMD[i]=0x2E;
		i++;
	}
	//	Get tenths place resolution
	temp_int= temp*10;

	while(integer_places>0)
	{
//		Get the integer we need to send
		working=temp_int/integer_places;
//		Convert to ascii
		working += 0x30;
//		Add to String to send
		Bluetooth_CMD[i]=working;
		i++;
//		Get next digit
		temp_int=temp_int%integer_places;
		integer_places= integer_places/10;
		// After we put in the 1's place, insert a decimal
		if(integer_places==1)
		{
			Bluetooth_CMD[i]='.';
			i++;
		}
	}
	if(Celcius)
	{
		Bluetooth_CMD[i++]='C';
	}
	else{
		Bluetooth_CMD[i++]= 'F';
	}
	Bluetooth_CMD[i]='\0';
	return;
}

void parse_Command(char* CMD)
{
//	Assume false
//	Format_Command_Valid = false;
	if(CMD[0] != '?')
	{
		return;
	}
	if((CMD[1] != 'd') && (CMD[1] != 'D'))
	{
		return;
	}
	if(CMD[3]!= '#')
	{
		return;
	}

	if(CMD[2] == 'c' || CMD[2] == 'C')
	{
		Celcius = true;
	}
	else if(CMD[2] == 'f' || CMD[2] == 'F')
	{
		Celcius = false;
	}
	else
	{
		return;
	}
	// If it gets here, the command is valid
	Format_Command_Valid = true;
	return;
}
