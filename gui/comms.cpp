/*
File name: gui/comms.cpp
Description: source file for communications stuff
Author: Andy Holt
Date: Mon 20 May 2013 17:21
*/

#include "rs232.h"
#include "comms.h"
#include <iostream>
#include <fstream>
using namespace std;

#define eeprom_size 32000

int com_port_no = 0;
bool com_port_open = false;
BYTE rx_buff[RX_BUFF_LEN];
BYTE tx_buff[10];

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

std::vector <packet> data;
std::vector <BYTE> read_buff;

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
      wxLogMessage(wxT("Successful comport opening"));
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
  
  com_port_open = false;
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


void read_eeprom_data(void)
{
  int i;
  int n = 0;
  BYTE data_a[eeprom_size];
  BYTE tmpdata[eeprom_size];
  config_data cfgdata;
  packet tmppacket;

  ofstream csvfile;
  csvfile.open("csvfile.csv");
  csvfile << "Temperature,Humidity,Accx,Accy,Accz\n";

  if(!com_port_open)
    {
      wxLogMessage(wxT("Error: no com port open"));
      return;
    }

  while (i < eeprom_size)
    {
      // put the n bytes availiable into tmpdata
      n = RS232_PollComport(com_port_no, tmpdata, eeprom_size - 1);
      for (int j = i; j < i+n; j++)
	{
	  // load tmpdata contents into next n elements of data
	  data_a[j] = tmpdata[j-i];
	}
      i += n;
    }

  // put data from data array into more structured vector
  cfgdata.datalen_u = data_a[0];
  cfgdata.datalen_l = data_a[1];
  cfgdata.samp_period = data_a[2];
  cfgdata.config_3 = data_a[3];
  cfgdata.config_4 = data_a[4];
  cfgdata.config_5 = data_a[5];
  cfgdata.config_6 = data_a[6];
  cfgdata.config_7 = data_a[7];
  cfgdata.config_8 = data_a[8];
  cfgdata.config_9 = data_a[9];
  cfgdata.config_10 = data_a[10];
  cfgdata.config_11 = data_a[11];
  cfgdata.config_12 = data_a[12];
  cfgdata.config_13 = data_a[13];
  cfgdata.config_14 = data_a[14];
  cfgdata.config_15 = data_a[15];

  int datalen = cfgdata.datalen_u * 256 + cfgdata.datalen_l;

  for (int k = 16; k < datalen*10; k += 10)
    {
      tmppacket.temp_u  = data_a[k + 0];
      tmppacket.temp_l  = data_a[k + 1];
      tmppacket.humid   = data_a[k + 2];
      tmppacket.accel_0 = data_a[k + 3];
      tmppacket.accel_1 = data_a[k + 4];
      tmppacket.accel_2 = data_a[k + 5];
      tmppacket.accel_3 = data_a[k + 6];
      tmppacket.accel_4 = data_a[k + 7];
      tmppacket.accel_5 = data_a[k + 8];
      tmppacket.accel_6 = data_a[k + 9];
	  csvfile<<tmppacket.temp_u*256+tmppacket.temp_l << "," << (int)tmppacket.humid << "," << (int)tmppacket.accel_0 <<","<< (int)tmppacket.accel_1 <<","<< (int)tmppacket.accel_2 <<"\n";

	  data.push_back(tmppacket);

  }

  csvfile.close();
  return;
}


void get_Readings(void)
{
  int n = 0;
  int i = 0;
  packet tmppacket;
  BYTE tmpdata[eeprom_size];
  
  data.clear();

  n = RS232_PollComport(com_port_no, tmpdata, eeprom_size - 1);

  for (i=0; i<n; i++)
    {
      read_buff.push_back(tmpdata[i]);
    }

  while (read_buff.size() >= 10)
    {
      tmppacket.temp_u  = read_buff.front();
      read_buff.erase(read_buff.begin());
      tmppacket.temp_l  = read_buff.front();
      read_buff.erase(read_buff.begin());
      tmppacket.humid   = read_buff.front();
      read_buff.erase(read_buff.begin());
      tmppacket.accel_0 = read_buff.front();
      read_buff.erase(read_buff.begin());
      tmppacket.accel_1 = read_buff.front();
      read_buff.erase(read_buff.begin());
      tmppacket.accel_2 = read_buff.front();
      read_buff.erase(read_buff.begin());
      tmppacket.accel_3 = read_buff.front();
      read_buff.erase(read_buff.begin());
      tmppacket.accel_4 = read_buff.front();
      read_buff.erase(read_buff.begin());
      tmppacket.accel_5 = read_buff.front();
      read_buff.erase(read_buff.begin());
      tmppacket.accel_6 = read_buff.front();
      read_buff.erase(read_buff.begin());
      data.push_back(tmppacket);
    }
}
