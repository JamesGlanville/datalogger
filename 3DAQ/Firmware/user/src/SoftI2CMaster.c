/*
 * SoftI2CMaster.cpp -- Multi-instance software I2C Master library
 * 
 * 
 * 2010-12 Tod E. Kurt, http://todbot.com/blog/
 *
 * This code takes some tricks from:
 *  http://codinglab.blogspot.com/2008/10/i2c-on-avr-using-bit-banging.html
 */

#include "SoftI2CMaster.h"

#include <string.h>

#define  i2cbitdelay 50

#define  I2C_ACK  1 
#define  I2C_NAK  0

#define i2c_scl_release()                 \
	GPIO_Init_Mode(I2CPORT, SCL, GPIO_Mode_IN_FLOATING);

#define i2c_sda_release()                 \
	GPIO_Init_Mode(I2CPORT, SDA, GPIO_Mode_IN_FLOATING);

#define i2c_sda_lo() \
	GPIO_Init_Mode(I2CPORT, SDA, GPIO_Mode_Out_PP);\
	GPIO_SetBits(I2CPORT,SDA);

#define i2c_scl_lo() \
	GPIO_Init_Mode(I2CPORT, SCL, GPIO_Mode_Out_PP);\
	GPIO_SetBits(I2CPORT,SCL);

#define i2c_sda_hi() \
	GPIO_Init_Mode(I2CPORT, SDA, GPIO_Mode_IN_FLOATING);

#define i2c_scl_hi() \
	GPIO_Init_Mode(I2CPORT, SCL, GPIO_Mode_IN_FLOATING);


void _delay_us(int delay)
{
	delayMicroseconds(delay);
}
		

uint8_t beginTransmission(uint8_t address)
{
    i2c_start();
    return i2c_write((address<<1) | 0); // clr read bit
//return rc;
}

//
uint8_t requestFrom(uint8_t address)
{
    i2c_start();
    return i2c_write((address<<1) | 1); // set read bit
}
//
//uint8_t requestFrom(int address)
//{
//    return requestFrom( (uint8_t) address);
//}

//

//
//
//
uint8_t endTransmission(void)
{
    i2c_stop();
    //return ret;  // FIXME
    return 0;
}

// must be called in:
// slave tx event callback
// or after beginTransmission(address)
uint8_t sendi2c(uint8_t data)
{
    return i2c_write(data);
}

// must be called in:
// slave tx event callback
// or after beginTransmission(address)
//void send(uint8_t* data, uint8_t quantity)
//{
//    for(uint8_t i = 0; i < quantity; ++i){
//        send(data[i]);
//    }
//}

// must be called in:
// slave tx event callback
// or after beginTransmission(address)
//void send(char* data)
//{
//    send((uint8_t*)data, strlen(data));
//}

// must be called in:
// slave tx event callback
// or after beginTransmission(address)
//void send(int data)
//{
//    send((uint8_t)data);
//}

//--------------------------------------------------------------------


void i2c_writebit( uint8_t c )
{
    if ( c > 0 ) {
        i2c_sda_hi();
    } else {
        i2c_sda_lo();
    }

    i2c_scl_hi();
    _delay_us(i2cbitdelay);

    i2c_scl_lo();
    _delay_us(i2cbitdelay);

    if ( c > 0 ) {
        i2c_sda_lo();
    }
    _delay_us(i2cbitdelay);
}

//
uint8_t i2c_readbit(void)
{
    i2c_sda_hi();
    i2c_scl_hi();
    _delay_us(i2cbitdelay);

    i2c_scl_lo();
    _delay_us(i2cbitdelay);

    return (GPIO_ReadInputDataBit(I2CPORT,SDA)) ? 1 : 0;
}

// Inits bitbanging port, must be called before using the functions below
//
void i2c_init(void)
{
    //I2C_PORT &=~ (_BV( I2C_SDA ) | _BV( I2C_SCL ));
    //*_sclPortReg &=~ (_sdaBitMask | _sclBitMask);
    i2c_sda_hi();
    i2c_scl_hi();
    
    _delay_us(i2cbitdelay);
}

// Send a START Condition
//
void i2c_start(void)
{
    // set both to high at the same time
    //I2C_DDR &=~ (_BV( I2C_SDA ) | _BV( I2C_SCL ));
    //*_sclDirReg &=~ (_sdaBitMask | _sclBitMask);
    i2c_sda_hi();
    i2c_scl_hi();

    _delay_us(i2cbitdelay);
   
    i2c_sda_lo();
    _delay_us(i2cbitdelay);

    i2c_scl_lo();
    _delay_us(i2cbitdelay);
}

void i2c_repstart(void)
{
    // set both to high at the same time (releases drive on both lines)
    //I2C_DDR &=~ (_BV( I2C_SDA ) | _BV( I2C_SCL ));
    //*_sclDirReg &=~ (_sdaBitMask | _sclBitMask);
    i2c_sda_hi();
    i2c_scl_hi();

    i2c_scl_lo();                           // force SCL low
    _delay_us(i2cbitdelay);

    i2c_sda_release();                      // release SDA
    _delay_us(i2cbitdelay);

    i2c_scl_release();                      // release SCL
    _delay_us(i2cbitdelay);

    i2c_sda_lo();                           // force SDA low
    _delay_us(i2cbitdelay);
}

// Send a STOP Condition
//
void i2c_stop(void)
{
    i2c_scl_hi();
    _delay_us(i2cbitdelay);

    i2c_sda_hi();
    _delay_us(i2cbitdelay);
}

// write a byte to the I2C slave device
//
uint8_t i2c_write( uint8_t c )
{
	uint8_t i;
    for ( i=0;i<8;i++) {
        i2c_writebit( c & 128 );
        c<<=1;
    }

    return i2c_readbit();
}

// read a byte from the I2C slave device
//
uint8_t i2c_read( uint8_t ack )
{
    uint8_t res = 0;
		uint8_t i;
    for (  i=0;i<8;i++) {
        res <<= 1;
        res |= i2c_readbit();  
    }

    if ( ack )
        i2c_writebit( 0 );
    else
        i2c_writebit( 1 );

    _delay_us(i2cbitdelay);

    return res;
}

// FIXME: this isn't right, surely
//uint8_t receive( uint8_t ack )
//{
//  return i2c_read( ack );
//}

//
uint8_t receive(void)
{
    return i2c_read( I2C_ACK );
}

//
uint8_t receiveLast(void)
{
    return i2c_read( I2C_NAK );
}
