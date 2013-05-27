/*
File name: gui/comms.h
Description: header file for communications stuff
Author: Andy Holt
Date: Mon 20 May 2013 17:15
*/


#ifndef comms_h
#define comms_h
#include <wx/wx.h>
#include <vector>

typedef unsigned char BYTE;

#define RX_BUFF_LEN 64



void RS232_Init(int port_no);
void RS232_Close(void);
BYTE* Read_Data_Block(void);
void send_command(int n);

int read_sensor_data(int *value);
void read_eeprom_data(void);
void get_Readings(void);

#endif	// datalogger_h
