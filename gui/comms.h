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

struct packet {
  BYTE temp_u;
  BYTE temp_l;
  BYTE humid;
  BYTE accel_0;
  BYTE accel_1;
  BYTE accel_2;
  BYTE accel_3;
  BYTE accel_4;
  BYTE accel_5;
  BYTE accel_6;
};

struct config_data {
  BYTE datalen_u;
  BYTE datalen_l;
  BYTE samp_period;
  BYTE config_3;
  BYTE config_4;
  BYTE config_5;
  BYTE config_6;
  BYTE config_7;
  BYTE config_8;
  BYTE config_9;
  BYTE config_10;
  BYTE config_11;
  BYTE config_12;
  BYTE config_13;
  BYTE config_14;
  BYTE config_15;
};

void RS232_Init(int port_no);
void RS232_Close(void);
BYTE* Read_Data_Block(void);
void send_command(int n);

int read_sensor_data(int *value);
void read_eeprom_data(void);
void get_Readings(void);

#endif	// datalogger_h
