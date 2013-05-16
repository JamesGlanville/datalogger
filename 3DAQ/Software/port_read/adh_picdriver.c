#include <ansi_c.h>

#include <stdio.h>
#include <windows.h>
#include <rs323.h>

#include "stdio.h"

#include "adh_picdriver.h"

#define READ_VERSION 0
#define MSG 8

/* Define transmit and recieve buffer */
BYTE rx_buff[RX_BUFF_LEN];
BYTE tx_buff[2];

static int com_port_no;
static int com_port_open = 0;

/************* RS323 Functions *************/

/* open a comm port */
void RS323_Init(int port_no)
{
  /* Parameters: COM Port, device name, baud rate, parity, data bits,
     stop bits, input queue size, output queue size */
  OpenComConfig(portno, "", 115200, 0, 8, 1, 1024, 16);

  /* remember port no and state */
  com_port_no = port_no;
  com_port_open = 1;
}

/* Close the open COM port */
void RS232_Close(void)
{
  if (com_port_open == 1)
    {
      CloseCom(com_port_no);
    }
}

/* Read in a block of data from port input buffer */
BYTE* Read_Data_Block(void)
{
  int n;
  int i;

  /* wait for data (4 bytes) */
  while (GetInQLen(4) < RX_BUFF_LEN);

  /* read port buffer, put result in rx_buff */
  n = ComRd(4, rx_buff, RX_BUFF_LEN - 1);

  /* return pointer to rx_buff */
  return rx_buff;
}

/* send an n byte command */
void send_command(int n)
{
  if (com_port_open == 1)
    {
      ComWrt(com_port_no, tx_buff, n);
    }
}

/***********************************************/
/* User functions */

int read_analogue_input(int *value)
{
  BYTE buffer[3];
  int timeout = 100000;

  if (com_port_open == 0)
    {
      printf("Error: No COM port open");
      return 0;
    }

  /* flush RS232 input buffer */
  FlushInQ(com_port_no);

  /* send ADC read command */
  tx_buff[0] = 1;
  send_command(1);

  /* wait for returned ADC value */
  while((GetInQLen(com_port_no) < 3) && (timeout > 0))
    {
      timeout--;
    }
  
  /* if time out, something is wrong */
  if (timeout == 0)
    {
      printf("Error: No recieved data");
      exit(-1);
    }

  /* read 3 bytes from input buffer */
  ComRd(com_port_no, buffer, 3);

  /* assign received values */
  *value = buffer[1] * 256 + buffer[2];

  return no_error;
}
