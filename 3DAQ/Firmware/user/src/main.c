#include "stm32F10x.h"
#include "LEDs.h"
#include "UART.h"
#include "ADC.h"
#include "stm32F10x_i2c.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "LCD.h"
#include "humidity.h"

#define EEPROM_ADDR 0xA1   
#define ACCEL_ADDR 0x4C   

#define   X_OUT       0x00
#define   Y_OUT       0x01
#define   Z_OUT       0x02
#define   INTSU       0x06
#define   MODE        0x07
#define   SR          0x08

//Hardware:

//LED3 is connected to PC9 (active high)
//Test circuit should have potentiometer connected to PC0 for analogue sampling
//UART TX and RX pins are PA9 and PA10 respectively (with MAX232 interface)

//Firmware:

//User files that can be modified/replaced
//LEDs.c and LEDs.h - functions to control LED on dev board
//ADC.c and ADC.h - functions to initalise ADC and take samples from it
//UART.c and UART.h - functions to initialise and use UART for serial RS232 comms with PC

//Standard includes that should not be modified
//startup_stm32f10x_md_vl.s - startup file (in assembler) that intialises the MCU before it enters main()
//core_cm3.c - definitions specific to the ARM Cortex M3 core
//system_stm32f10.c and stm32f10x.h - functions and definitions specific to the STM32F10x series

//There are also a number of Standard Peripheral Library files that should not be modified (additional SPL files can be included if needed, see "Firmware/lib" folder)

//Software:
//This firmware works with the Labwindows/CVI demonstration appllication sofwtare

//------------------------------------------------------------------------------

//Global variables

//Command from UART
volatile unsigned int LEDbyte;

volatile int command_flag;
volatile int value;
volatile int value_received;

//ADC variables
volatile uint8_t new_data;


//------------------------------------------------------------------------------

//Function prototypes for functions in main.c file

void check_and_process_received_command(void);
void init_I2C1(void){

	GPIO_InitTypeDef GPIO_InitStruct;
	I2C_InitTypeDef I2C_InitStruct;

	// enable APB1 peripheral clock for I2C1
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);
	// enable clock for SCL and SDA pins
//	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE); 

	/* setup SCL and SDA pins
	 * You can connect I2C1 to two different
	 * pairs of pins:
	 * 1. SCL on PB6 and SDA on PB7
	 * 2. SCL on PB8 and SDA on PB9
	 */
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7; // we are going to use PB6 and PB7
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_OD;			// set pins to alternate function
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;		// set GPIO speed
	GPIO_Init(GPIOB, &GPIO_InitStruct);					// init GPIOB
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;		// set GPIO speed
	GPIO_Init(GPIOB, &GPIO_InitStruct);					// init GPIOB

	// Connect I2C1 pins to AF
//	GPIO_PinAFConfig(GPIOB, GPIO_PinSource6, GPIO_AF_I2C1);	// SCL
//	GPIO_PinAFConfig(GPIOB, GPIO_PinSource7, GPIO_AF_I2C1); // SDA

	// configure I2C1
	I2C_InitStruct.I2C_ClockSpeed = 100000; 		// 100kHz
	I2C_InitStruct.I2C_Mode = I2C_Mode_I2C;			// I2C mode
	I2C_InitStruct.I2C_DutyCycle = I2C_DutyCycle_2;	// 50% duty cycle --> standard
	I2C_InitStruct.I2C_OwnAddress1 = 0x00;			// own address, not relevant in master mode
	I2C_InitStruct.I2C_Ack = I2C_Ack_Disable;		// disable acknowledge when reading (can be changed later on)
	I2C_InitStruct.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit; // set address length to 7 bit addresses
	I2C_Init(I2C1, &I2C_InitStruct);				// init I2C1

	// enable I2C1
	I2C_Cmd(I2C1, ENABLE);
}

/* This function issues a start condition and
 * transmits the slave address + R/W bit
 *
 * Parameters:
 * 		I2Cx --> the I2C peripheral e.g. I2C1
 * 		address --> the 7 bit slave address
 * 		direction --> the tranmission direction can be:
 * 						I2C_Direction_Tranmitter for Master transmitter mode
 * 						I2C_Direction_Receiver for Master receiver
 */
void I2C_start(I2C_TypeDef* I2Cx, uint8_t address, uint8_t direction){
	// wait until I2C1 is not busy anymore
	while(I2C_GetFlagStatus(I2Cx, I2C_FLAG_BUSY));

	// Send I2C1 START condition
	I2C_GenerateSTART(I2Cx, ENABLE);

	// wait for I2C1 EV5 --> Slave has acknowledged start condition
	while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_MODE_SELECT));

	// Send slave Address for write
	I2C_Send7bitAddress(I2Cx, address, direction);

	/* wait for I2C1 EV6, check if
	 * either Slave has acknowledged Master transmitter or
	 * Master receiver mode, depending on the transmission
	 * direction
	 */
	if(direction == I2C_Direction_Transmitter){
		while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
	}
	else if(direction == I2C_Direction_Receiver){
		while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));
	}
}
#ifdef cheese
void init_I2C1(void){

	GPIO_InitTypeDef GPIO_InitStruct;
	I2C_InitTypeDef I2C_InitStruct;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE); 
   	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1|RCC_APB2Periph_AFIO, ENABLE);

	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7; // we are going to use PB6 and PB7
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_OD;			// set pins to alternate function
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;		// set GPIO speed
	GPIO_Init(GPIOB, &GPIO_InitStruct);					// init GPIOB

	I2C_DeInit(I2C1);
	I2C_Cmd(I2C1, ENABLE);

	I2C_InitStruct.I2C_ClockSpeed = 100000; 		// 100kHz
	I2C_InitStruct.I2C_Mode = I2C_Mode_I2C;			// I2C mode
	I2C_InitStruct.I2C_DutyCycle = I2C_DutyCycle_2;	// 50% duty cycle --> standard
	I2C_InitStruct.I2C_OwnAddress1 = 0x039;			// own address, not relevant in master mode
	I2C_InitStruct.I2C_Ack = I2C_Ack_Enable;		// disable acknowledge when reading (can be changed later on)
	I2C_InitStruct.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit; // set address length to 7 bit addresses
	I2C_Init(I2C1, &I2C_InitStruct);				// init I2C1
	// enable I2C1
}
void I2C_start(I2C_TypeDef* I2Cx, uint8_t address, uint8_t direction){
	// wait until I2C1 is not busy anymore
	int t;
	t=0;
	while(I2C_GetFlagStatus(I2Cx, I2C_FLAG_BUSY));

	// Send I2C1 START condition
	I2C_GenerateSTART(I2Cx, ENABLE);

	// wait for I2C1 EV5 --> Slave has acknowledged start condition
	while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_MODE_SELECT));

	// Send slave Address for write
	I2C_Send7bitAddress(I2Cx, address, direction);

	/* wait for I2C1 EV6, check if
	 * either Slave has acknowledged Master transmitter or
	 * Master receiver mode, depending on the transmission
	 * direction
	 */
	if(direction == I2C_Direction_Transmitter){
		while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));//{if (t>100){}else{t++;}}
	}
	else if(direction == I2C_Direction_Receiver){
		while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));
	}
}

/* This function transmits one byte to the slave device
 * Parameters:
 *		I2Cx --> the I2C peripheral e.g. I2C1
 *		data --> the data byte to be transmitted
 */
#endif
void I2C_write(I2C_TypeDef* I2Cx, uint8_t data){
	I2C_SendData(I2Cx, data);
	// wait for I2C1 EV8_2 --> byte has been transmitted
	while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
}

/* This function reads one byte from the slave device
 * and acknowledges the byte (requests another byte)
 */
uint8_t I2C_read_ack(I2C_TypeDef* I2Cx){
	uint8_t data;
	// enable acknowledge of recieved data
	I2C_AcknowledgeConfig(I2Cx, ENABLE);
	// wait until one byte has been received
	while( !I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_RECEIVED) );
	// read data from I2C data register and return data byte
	data = I2C_ReceiveData(I2Cx);
	return data;
}

/* This function reads one byte from the slave device
 * and doesn't acknowledge the recieved data
 */
uint8_t I2C_read_nack(I2C_TypeDef* I2Cx){
	uint8_t data;
	// disabe acknowledge of received data
	I2C_AcknowledgeConfig(I2Cx, DISABLE);
	// wait until one byte has been received
	while( !I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_RECEIVED) );
	// read data from I2C data register and return data byte
	data = I2C_ReceiveData(I2Cx);
	return data;
}

/* This funtion issues a stop condition and therefore
 * releases the bus
 */
void I2C_stop(I2C_TypeDef* I2Cx){
	// Send I2C1 STOP Condition
	I2C_GenerateSTOP(I2Cx, ENABLE);
}


//------------------------------------------------------------------------------

//Main function (execution starts here after startup file)
int main(void)
{
	int i;
	int j;
	int acctemp;
	int yumbyte;
	int temporary;
	uint8_t received_data[2];
	unsigned char REG_ADDRESS[3];
	int lcdadc;

	init_GPIO_pins();
	
	//Short delay during which we can communicate with MCU via debugger even if later user code causes error such as sleep state with no wakeup event that prevents debugger interface working
	//THIS MUST COME BEFORE ALL USER CODE TO ENSURE CHIPS CAN BE REPROGRAMMED EVEN IF THEY GET STUCK IN A SLEEP STATE LATER
	for (i = 0; i < 1000000; i++)
	{
		LED_on();
	}
		
	LED_off();
	
  /*while(1){


    		I2C_start(I2C1, 0x4C, I2C_Direction_Transmitter); // start a transmission in Master transmitter mode
		I2C_write(I2C1, 0x20); // write one byte to the slave
		I2C_write(I2C1, 0x03); // write another byte to the slave
		I2C_stop(I2C1); // stop the transmission
	
		I2C_start(I2C1, SLAVE_ADDRESS, I2C_Direction_Receiver); // start a transmission in Master receiver mode
		received_data[0] = I2C_read_ack(I2C1); // read one byte and request another byte
		received_data[1] = I2C_read_nack(I2C1); // read one byte and don't request another byte
		I2C_stop(I2C1); // stop the transmission

	}	*/
	
	//I2C_start(I2C1, SLAVE_ADDRESS, I2C_Direction_Transmitter);
	LEDbyte =0;
	yumbyte=0;
	
	LCDINIT();
	home();
	clear();
	display();
	noCursor();
	noBlink();
	GPIO_SetBits(GPIOC, GPIO_Pin_2);

	write('H');
	write('E');
	write('L');
	write('L');
	write('O');
delayMicroseconds(200000);
	clear();

//Initialise UART for serial comms with PC
	UART_init();
	
	humidity_init();
	//Initialise ADC
	ADC_init();
		LEDbyte=1;

	while(1){
		if(LEDbyte==512){LEDbyte=1;}
		else {LEDbyte=LEDbyte<<1;}
		setLEDS();
		
		
	setCursor(0,1);
		lcdadc = ADC_perform_single_conversion();
		write('0'+(lcdadc/1000));
		lcdadc = lcdadc %1000;
		write ('0'+(lcdadc/100));
		lcdadc = lcdadc %100;
		write ('0'+(lcdadc/10));
		lcdadc = lcdadc %10;
		write ('0'+(lcdadc));
	delayMicroseconds(10000);
			setCursor(0,0);
		temporary=readcapacitance();	
		write('0'+(temporary/1000));
		temporary=temporary%1000;
		write('0'+(temporary)/100);
		temporary=temporary%100;
		write('0'+(temporary/10));
		temporary=temporary%10;
		write('0'+(temporary));

		/*setCursor(0,0);
		temporary = readcapacitance();
		write('0'+(temporary/1000000));
		temporary = temporary %1000000;
		write ('0'+(temporary/100000));
		temporary = temporary %100000;
		write ('0'+(temporary/10000));
		temporary = temporary %10000;
		write ('0'+(temporary/1000));
		temporary=0;*/
		
		
		
	}
	//	for (j=0;j<5;j++)
		//{
	//		write('0'+lcd
			
	//	}
			//Put the 12-bit result into 2 bytes
			//byte1 = (uint8_t)((ADC_word >>8) & 0xFF);
			//byte2 = (uint8_t)((ADC_word >>0) & 0xFF);
//	I2C_Init(I2Cx, I2C_InitStruct);
/*	init_I2C1();

   	I2C_start(I2C1, ACCEL_ADDR, I2C_Direction_Transmitter); // start a transmission in Master transmitter mode
	I2C_write(I2C1, MODE); // write one byte to the slave
	I2C_write(I2C1, 0x00); // write another byte to the slave
	I2C_stop(I2C1); // stop the transmission
	
	delayMicroseconds(2);
	
   	I2C_start(I2C1, ACCEL_ADDR, I2C_Direction_Transmitter); // start a transmission in Master transmitter mode
	I2C_write(I2C1, INTSU); // write one byte to the slave
	I2C_write(I2C1, 0x10); // write another byte to the slave
	I2C_stop(I2C1); // stop the transmission
	
	delayMicroseconds(2);
	
   	I2C_start(I2C1, ACCEL_ADDR, I2C_Direction_Transmitter); // start a transmission in Master transmitter mode
	I2C_write(I2C1, SR); // write one byte to the slave
	I2C_write(I2C1, 0x00); // write another byte to the slave
	I2C_stop(I2C1); // stop the transmission
	
	delayMicroseconds(2);
	
   	I2C_start(I2C1, ACCEL_ADDR, I2C_Direction_Transmitter); // start a transmission in Master transmitter mode
	I2C_write(I2C1, MODE); // write one byte to the slave
	I2C_write(I2C1, 0x01); // write another byte to the slave
	I2C_stop(I2C1); // stop the transmission
	
	delayMicroseconds(2);
	
	I2C_start(I2C1, ACCEL_ADDR, I2C_Direction_Transmitter); // start a transmission in Master receiver mode

	I2C_write(I2C1,0x00);
	I2C_start(I2C1, ACCEL_ADDR, I2C_Direction_Receiver); // start a transmission in Master receiver mode

	REG_ADDRESS[0] = I2C_read_ack(I2C1); // read one byte and request another byte
	REG_ADDRESS[1] = I2C_read_ack(I2C1); // read one byte and request another byte
	REG_ADDRESS[2] = I2C_read_nack(I2C1); // read one byte and don't request another byte
	I2C_stop(I2C1); // stop the transmission

	delayMicroseconds(2);
	acctemp=REG_ADDRESS[0]/100;
	write(acctemp+'0');
	acctemp=(REG_ADDRESS[0]%100)/10;
	write(acctemp+'0');
	acctemp=(REG_ADDRESS[0]%10);
	write(acctemp+'0');
	
	I2C_start(I2C1, EEPROM_ADDR, I2C_Direction_Transmitter); // start a transmission in Master receiver mode
	I2C_write(I2C1, 0x00); // write one byte to the slave
	I2C_write(I2C1, 0x00); // write one byte to the slave
	I2C_write(I2C1, 0xBA); // write one byte to the slave
	I2C_stop(I2C1); // stop the transmission

	delayMicroseconds(2);

	I2C_start(I2C1, EEPROM_ADDR, I2C_Direction_Transmitter); // start a transmission in Master receiver mode
	I2C_write(I2C1, 0x00); // write one byte to the slave
	I2C_write(I2C1, 0x00); // write one byte to the slave
	I2C_start(I2C1, EEPROM_ADDR, I2C_Direction_Transmitter); // start a transmission in Master receiver mode

//	I2C_stop(I2C1); // stop the transmission
	yumbyte = I2C_read_nack(I2C1);
	I2C_stop(I2C1); // stop the transmission
	delayMicroseconds(2);

*/
	

	//Main loop
	//------------------------------------------------------------------------------
	while (1)
	{
		int i;
	//	check_and_process_received_command();
	}
	//------------------------------------------------------------------------------
}

//------------------------------------------------------------------------------

//Other functions

//Simple function to check for and process one of two received commands from PC via global varoable flags
void check_and_process_received_command(void)
{
	uint16_t ADC_word;
	uint8_t byte1, byte2;
	
	//Check for a receievd (non-zero) command flag from the PC
	if (command_flag == 1)
	{
		//An ADC sample command has been received
		
		//Perform an ADC conversion
		ADC_word = ADC_perform_single_conversion();
		
		//Put the 12-bit result into 2 bytes
		byte1 = (uint8_t)((ADC_word >>8) & 0xFF);
		byte2 = (uint8_t)((ADC_word >>0) & 0xFF);
		
		//Send the message (command flag followed by two bytes with the analogue value)
		UART_send_byte(1);
		UART_send_byte(byte1);
		UART_send_byte(byte2);
		
		//Reset command flag
		command_flag = 0;
	}
	else if (command_flag == 2)
	{
		//An LED command has been received
		
		//Check whether we have also received the value attached to the command (ie on or off)
		if (value_received == 1)
		{
			//Reset command flag
			command_flag = 0;
			
			//Set LED based on value (on or off)
			if (value == 0)
			{
				LED_off();
			}
			else
			{
				LED_on();
			}
		}
	}
	else if (command_flag ==3)
	{
		command_flag=0;
		while (1)
		{
			ADC_word = ADC_perform_single_conversion();
		
			//Put the 12-bit result into 2 bytes
			byte1 = (uint8_t)((ADC_word >>8) & 0xFF);
			byte2 = (uint8_t)((ADC_word >>0) & 0xFF);
		
			//Send the message (command flag followed by two bytes with the analogue value)
			UART_send_byte(1);
			UART_send_byte(byte1);
			UART_send_byte(byte2);
		
		}
	}
}
