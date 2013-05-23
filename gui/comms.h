/*
File name: gui/comms.h
Description: header file for communications stuff
Author: Andy Holt
Date: Mon 20 May 2013 17:15
*/

#ifndef comms_h
#define coms_h

void RS232_Init(int port_no);
void RS232_Close(void);
unsigned char* Read_Data_Block(void);
void send_command(int n);

static int com_port_no = 0;
static bool com_port_open = false;

int read_sensor_data(int *value);

#endif	// datalogger_h
