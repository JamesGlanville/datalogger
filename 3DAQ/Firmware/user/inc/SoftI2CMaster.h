/*
 * SoftI2CMaster.h -- Multi-instance software I2C Master library
 * 
 * 2010-2012 Tod E. Kurt, http://todbot.com/blog/
 *
 */

#ifndef SoftI2CMaster_h
#define SoftI2CMaster_h
#include "stm32f10x_gpio.h"

#include <inttypes.h>
#include "LCD.h"
#include "humidity.h"


#define I2CPORT GPIOB
#define SDA GPIO_Pin_7
#define SCL GPIO_Pin_6
#define _SOFTI2CMASTER_VERSION 12  // software version of this library



  // per object data
/*  uint8_t _sdaPin;
  uint8_t _sclPin;
  uint8_t _sdaBitMask;
  uint8_t _sclBitMask;
  volatile uint8_t *_sdaPortReg;
  volatile uint8_t *_sclPortReg;
  volatile uint8_t *_sdaDirReg;
  volatile uint8_t *_sclDirReg;
  */
  // private methods
  void setPins(uint8_t sdaPin, uint8_t sclPin, uint8_t usePullups);

  void i2c_writebit( uint8_t c );
  uint8_t i2c_readbit(void);
  void i2c_init(void);
  void i2c_start(void);
  void i2c_repstart(void);
  void i2c_stop(void);
  uint8_t i2c_write( uint8_t c );
  uint8_t i2c_read( uint8_t ack );
  
  // public methods


  uint8_t beginTransmission(uint8_t address);
  uint8_t endTransmission(void);
  uint8_t sendi2c(uint8_t);
///  void send(uint8_t*, uint8_t);
//  void send(int);
 // void send(char*);

//  uint8_t requestFrom(int address);
  uint8_t requestFrom(uint8_t address);
 // uint8_t receive( uint8_t ack );
  uint8_t receive(void);
  uint8_t receiveLast(void);



#endif
