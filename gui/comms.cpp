/*
File name: gui/comms.cpp
Description: source file for communications stuff
Author: Andy Holt
Date: Mon 20 May 2013 17:21
*/

#include "rs232.h"
#include "comms.h"

BYTE rx_buff[RX_BUFF_LEN];
BYTE tx_buff[2];

// RS232 functions
void RS232_Init(int port_no)
{
  // Open COM port given by port_no parameters: COM port,
  // device name, baud rate, parity, data bits, stop bits,
  // input queue size, output queue size
  //  OpenComConfig(port_no, "", 115200, 0, 8, 1, 1024, 16);
  
	int comportopen = 0;
  comportopen = RS232_OpenComport(port_no, 115200);

  if (comportopen == 1)
  {
	wxLogMessage(wxT("Comport opening has failed, returned 1"));
  }
  else
  {
      // remember COM port number and state
      com_port_no = port_no;
      com_port_open = true;
  }
}

// Close the open COM port
void RS232_Close(void)
{
  if (com_port_open)
    {
      RS232_CloseComport(com_port_no);
    }
}

// Read in a block of data from the COM port's input buffer
/*
BYTE* Read_Data_Block(void)
{
  // wait for enough data (4 bytes)
  while(GetInQLen(4) < RX_BUFF_LEN);

  // perform read of the COM port buffer, placing result in
  // rx_buff
  //  n = ComRd(4, rx_buff, RX_BUFF_LEN - 1);
  int n = RS232_PollComport(com_port_no, rx_buff, RX_BUFF_LEN - 1);

  rx_buff[n] = 0;		// end string with null

  // return pointer to rx_buff
  return rx_buff;
}
*/

// Read in a block of data from the COM port's input buffer
BYTE* Read_Data_Block(void)
{
  int blocksize = 4;		// no of bytes to be read
  int i = 0;
  int n = 0;
  BYTE tmpbuff[RX_BUFF_LEN];
  while (i < blocksize)
    {
      n = RS232_PollComport(com_port_no, tmpbuff, RX_BUFF_LEN - 1);
      for(int j = i; j < i+n; j++)
	{
	  rx_buff[j] = tmpbuff[j-i]; // load up rx_buff, preserving
				     // whats already there
	}
      i += n;
    }
  rx_buff[n] = 0;		// end string with null
  return rx_buff;
}



// send n byte command
void send_command(int n)
{
  if(com_port_open)
    {
      // ComWrt(com_port_no, tx_buff, n);
      int pass = RS232_SendBuf(com_port_no, tx_buff, n);
    }
}

/******** User comms functions ********/

int read_sensor_data(int *value)
{
  BYTE buffer[3];
  int timeout = 100000;
  int blocksize = 4;		// no of bytes to be read
  int i = 0;
  int n = 0;
  BYTE tmpbuff[RX_BUFF_LEN];

  if (! com_port_open)
    {
      wxLogMessage(wxT("Error:\n No open COM port."));
      return 0;
    }

  // flush RS232 input buffer
  //  FlushInQ(com_port_no);

  // send ADC read command
  tx_buff[0] = 1;
  send_command(1);

  // wait for returned ADC value
  /*  while((GetInQLen(com_port_no) < 3) && (timeout > 0))
    {
      timeout--;
    }
  */
  while (i < blocksize && timeout > 0)
    {
      n = RS232_PollComport(com_port_no, tmpbuff, RX_BUFF_LEN - 1);
      for(int j = i; j < i+n; j++)
	{
	  rx_buff[j] = tmpbuff[j-i]; // load up rx_buff, preserving
				     // whats already there
	}
      i += n;
      timeout--;
    }


  // if timeout, something's wrong
  if (timeout == 0)
    {
      wxLogMessage(wxT("Error:\n No received Data"));
      exit(-1);
    }

  // read 3 bytes from input buffer
  //  ComRd(com_port_no, buffer, 3);
  RS232_PollComport(com_port_no, buffer, 3);
  
    // assign received value
  *value = buffer[1] * 256 + buffer[2];

  return 0;
}
