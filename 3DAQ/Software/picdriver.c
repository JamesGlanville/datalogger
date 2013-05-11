#include <ansi_c.h>
#include <cvirte.h>		
#include <userint.h>
 
#include <stdio.h>
#include <windows.h> 
#include <rs232.h>

#include "picdriver.h"


#define READ_VERSION    0
#define MSG    8

//This file contains low level functions to send and receive data over the RS232 serial link
//You will need to modify some of these functions for your application

//Define transmit and receive buffer
BYTE rx_buff[RX_BUFF_LEN];
BYTE tx_buff[2];

static int com_port_no;
static int com_port_open = 0;

//RS232 functions

//Open a COM port
void RS232_Init(int port_no)
{
	//Open COM port given by port_no using parameters:
	// COM port, device name, baud rate, parity, data bits, stop bits, input queue size, outptu queue size
	OpenComConfig (port_no, "", 115200, 0, 8, 1, 1024, 16); 
	
	//remember COM port number and state
	com_port_no = port_no;
	com_port_open = 1;
}

//Close the open COM port
void RS232_Close(void)
{
	if (com_port_open == 1)
	{
		CloseCom(com_port_no);
	}
}


//Read in a block of data from the COM port's input buffer
BYTE* Read_Data_Block(void)
{
	int n;
	int i;
	
	//Wait for enough data (4 bytes here)
	while(GetInQLen(4) < RX_BUFF_LEN);
	
	//Perform read of the COM port buffer, placing results in rx_buff
	n = ComRd(4, rx_buff, RX_BUFF_LEN - 1);

	//return pointer to rx_buff
	return rx_buff;
}

//Send n-byte command
void send_command(int n)
{
	if(com_port_open == 1)
	{
		ComWrt(com_port_no, tx_buff, n);	
	}
}

//-------------------------------

//User comms functions

int read_analog_input (int *value)
{
	BYTE buffer[3];
	int timeout = 100000;
	
	if (com_port_open == 0)
	{
		MessagePopup ("Error", "No Open COM Port");
		return 0;
	}

	//flush RS232 input buffer
	FlushInQ(com_port_no);
	
	//send ADC read command
	tx_buff[0] = 1;
	send_command(1);
	
	//wait for returned ADC value
	while((GetInQLen(com_port_no) < 3) && (timeout > 0))
	{
		timeout--;
	}
	
	//If we time out something's wrong
	if (timeout == 0)
	{
		MessagePopup ("Error", "No received Data");
		exit(-1);
	}
	
	//read 3 bytes from input buffer
	ComRd(com_port_no, buffer, 3);
	
	//assign received value
	*value = buffer[1] * 256 + buffer[2];
	
  	return no_error;
}

//Command to set LED to on or off state
int set_led (int value)
{
	if (com_port_open == 0)
	{
		MessagePopup ("Error", "No Open COM Port");
		return 0;
	}
	
	tx_buff[0] = 2;	//Command
	tx_buff[1] = value; 	//value (on or off)
	
	send_command(2);
	
	
  return no_error;
}
