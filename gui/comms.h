/*
File name: gui/comms.h
Description: header file for communications stuff
Author: Andy Holt
Date: Mon 20 May 2013 17:15
*/

#include <wx/wx.h>

#ifndef comms_h
#define comms_h

typedef unsigned char BYTE;

#define RX_BUFF_LEN 64

static int com_port_no = 0;
static bool com_port_open = false;

void RS232_Init(int port_no);
void RS232_Close(void);
BYTE* Read_Data_Block(void);
void send_command(int n);

int read_sensor_data(int *value);

#endif	// datalogger_h
