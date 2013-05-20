/*
File name: gui/comms.cpp
Description: source file for communications stuff
Author: Andy Holt
Date: Mon 20 May 2013 17:21
*/

#include <rs232.h>
#include "comms.h"

BYTE rx_buff[RX_BUFF_LEN];
BYTE tx_buff[2];

static int com_port_no;
static bool com_port_open = false;

// RS232 functions

void RS232_Init(int port_no)
{
  // Open COM port given by port_no parameters: COM port,
  // device name, baud rate, parity, data bits, stop bits,
  // input queue size, output queue size
  OpenComConfig(port_no, "", 115200, 0, 8, 1, 1024, 16);

  // remember COM port number and state
  com_port_no = port_no;
  com_port_open = true;
}

// Close the open COM port
void RS232_Close(void)
{
  if (com_port_open)
    {
      CloseCom(com_port_no);
    }
}

// Read in a block of data from the COM port's input buffer
BYTE* Read_Data_Block(void)
{
  int n;
  int i;

  // wait for enough data (4 bytes)
  while(GetInQLen(4) < RX_BUFF_LEN);

  // perform read of the COM port buffer, placing result in
  // rx_buff
  n = ComRd(4, rx_buff, RX_BUFF_LEN - 1);

  // return pointer to rx_buff
  return rx_buff;
}

// send n byte command
void send_command(int n)
{
  if(com_port_open)
    {
      ComWrt(com_port_no, tx_buff, n);
    }
}

/******** User comms functions ********/

int read_sensor_data(int *value)
