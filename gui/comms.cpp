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
{
  BYTE buffer[3];
  int timeout = 100000;

  if (! com_port_open)
    {
      wxLogMessage(wxT("Error:\n No open COM port."));
      return 0;
    }

  // flush RS232 input buffer
  FlushInQ(com_port_no);

  // send ADC read command
  tx_buff[0] = 1;
  send_command(1);

  // wait for returned ADC value
  while((GetInQLen(com_port_no) < 3) && (timeout > 0))
    {
      timeout--;
    }

  // if timeout, something's wrong
  if (timeout == 0)
    {
      wxLogMessage(wxT("Error:\n No received Data"));
      exit(-1);
    }

  // read 3 bytes from input buffer
  ComRd(com_port_no, buffer, 3);

  // assign received value
  *value = buffer[1] * 256 + buffer[2];

  return 0;
}
