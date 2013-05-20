#include "LCD.h"

#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include "stm32F10x.h"
#include "stm32f10x_gpio.h"
#include "LEDs.h"

  uint8_t _rs_pin; // LOW: command.  HIGH: character.
  uint8_t _rw_pin; // LOW: write to LCD.  HIGH: read from LCD.
  uint8_t _enable_pin; // activated by a HIGH pulse.
  uint8_t _data_pins[8];

  uint8_t _displayfunction;
  uint8_t _displaycontrol;
  uint8_t _displaymode;

  uint8_t _initialized;

  uint8_t _numlines,_currline;

	/*
	PB0 DB5
	PA1 RS
	PA0 E 
	PC5 DB7
	PC4 DB4
	PA7 DB6
	*/

void delayMicroseconds(int delay)
{
	int i;
		for (i = 0; i < delay*100; i++)
	{
		LED_on();
	}
}

void LCDINIT(void)
{
	_displayfunction = LCD_4BITMODE | LCD_2LINE | LCD_5x8DOTS;
	begin(8,2,LCD_5x8DOTS);
}

void begin(uint8_t cols, uint8_t lines, uint8_t dotsize) {
  if (lines > 1) {
    _displayfunction |= LCD_2LINE;
  }
  _numlines = lines;
  _currline = 0;

  // for some 1 line displays you can select a 10 pixel high font
  if ((dotsize != 0) && (lines == 1)) {
    _displayfunction |= LCD_5x10DOTS;
  }

  // SEE PAGE 45/46 FOR INITIALIZATION SPECIFICATION!
  // according to datasheet, we need at least 40ms after power rises above 2.7V
  // before sending commands. Arduino can turn on way befer 4.5V so we'll wait 50
  delayMicroseconds(50000); 
  // Now we pull both RS and R/W low to begin commands
	GPIO_ResetBits (GPIOA,GPIO_Pin_1);
	//digitalWrite(_rs_pin, LOW);
	GPIO_ResetBits (GPIOA,GPIO_Pin_0);
  //digitalWrite(_enable_pin, LOW);
  //if (_rw_pin != 255) { 
  // digitalWrite(_rw_pin, LOW);
  //}
  

    // this is according to the hitachi HD44780 datasheet
    // page 45 figure 23

   if (1) {
    // this is according to the hitachi HD44780 datasheet
    // figure 24, pg 46

    // we start in 8bit mode, try to set 4 bit mode
    write4bits(0x03);
    delayMicroseconds(4500); // wait min 4.1ms

    // second try
    write4bits(0x03);
    delayMicroseconds(4500); // wait min 4.1ms
    
    // third go!
    write4bits(0x03); 
    delayMicroseconds(150);

    // finally, set to 4-bit interface
    write4bits(0x02); 
  } else {
    // this is according to the hitachi HD44780 datasheet
    // page 45 figure 23

    // Send function set command sequence
    command(LCD_FUNCTIONSET | _displayfunction);
    delayMicroseconds(4500);  // wait more than 4.1ms

    // second try
    command(LCD_FUNCTIONSET | _displayfunction);
    delayMicroseconds(150);

    // third go
    command(LCD_FUNCTIONSET | _displayfunction);
  }

  // turn the display on with no cursor or blinking default
  _displaycontrol = LCD_DISPLAYON | LCD_CURSOROFF | LCD_BLINKOFF;  
  display();

  // clear it off
  clear();

  // Initialize to default text direction (for romance languages)
  _displaymode = LCD_ENTRYLEFT | LCD_ENTRYSHIFTDECREMENT;
  // set the entry mode
  command(LCD_ENTRYMODESET | _displaymode);

}

/********** high level commands, for the user! */
void clear()
{
  command(LCD_CLEARDISPLAY);  // clear display, set cursor position to zero
  delayMicroseconds(2000);  // this command takes a long time!
}

void home()
{
  command(LCD_RETURNHOME);  // set cursor position to zero
  delayMicroseconds(2000);  // this command takes a long time!
}

void setCursor(uint8_t col, uint8_t row)
{
  int row_offsets[] = { 0x00, 0x40, 0x14, 0x54 };
  if ( row >= _numlines ) {
    row = _numlines-1;    // we count rows starting w/0
  }
  
  command(LCD_SETDDRAMADDR | (col + row_offsets[row]));
}

// Turn the display on/off (quickly)
void noDisplay() {
  _displaycontrol &= ~LCD_DISPLAYON;
  command(LCD_DISPLAYCONTROL | _displaycontrol);
}
void display() {
  _displaycontrol |= LCD_DISPLAYON;
  command(LCD_DISPLAYCONTROL | _displaycontrol);
}

// Turns the underline cursor on/off
void noCursor() {
  _displaycontrol &= ~LCD_CURSORON;
  command(LCD_DISPLAYCONTROL | _displaycontrol);
}
void cursor() {
  _displaycontrol |= LCD_CURSORON;
  command(LCD_DISPLAYCONTROL | _displaycontrol);
}

// Turn on and off the blinking cursor
void noBlink() {
  _displaycontrol &= ~LCD_BLINKON;
  command(LCD_DISPLAYCONTROL | _displaycontrol);
}
void blink() {
  _displaycontrol |= LCD_BLINKON;
  command(LCD_DISPLAYCONTROL | _displaycontrol);
}

// These commands scroll the display without changing the RAM
void scrollDisplayLeft(void) {
  command(LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVELEFT);
}
void scrollDisplayRight(void) {
  command(LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVERIGHT);
}

// This is for text that flows Left to Right
void leftToRight(void) {
  _displaymode |= LCD_ENTRYLEFT;
  command(LCD_ENTRYMODESET | _displaymode);
}

// This is for text that flows Right to Left
void rightToLeft(void) {
  _displaymode &= ~LCD_ENTRYLEFT;
  command(LCD_ENTRYMODESET | _displaymode);
}

// This will 'right justify' text from the cursor
void autoscroll(void) {
  _displaymode |= LCD_ENTRYSHIFTINCREMENT;
  command(LCD_ENTRYMODESET | _displaymode);
}

// This will 'left justify' text from the cursor
void noAutoscroll(void) {
  _displaymode &= ~LCD_ENTRYSHIFTINCREMENT;
  command(LCD_ENTRYMODESET | _displaymode);
}



/*********** mid level commands, for sending data/cmds */

void command(uint8_t value) {
  send(value, LOW);
}

int write(uint8_t value) {
  send(value, HIGH);
  return 1; // assume sucess
}

/************ low level data pushing commands **********/

// write either command or data, with automatic 4/8-bit selection
void send(uint8_t value, uint8_t mode) {
	if (mode==1)
	{
			GPIO_SetBits (GPIOA,GPIO_Pin_1);

	}
	else
	{
			GPIO_ResetBits (GPIOA,GPIO_Pin_1);
	}
  //digitalWrite(_rs_pin, mode);
  
    write4bits(value>>4);
    write4bits(value);
  
}

void pulseEnable(void) {
		GPIO_ResetBits (GPIOA,GPIO_Pin_0);
  delayMicroseconds(10);    
	GPIO_SetBits (GPIOA,GPIO_Pin_0);
  delayMicroseconds(10);    // enable pulse must be >450ns
	GPIO_ResetBits (GPIOA,GPIO_Pin_0);
	delayMicroseconds(100);   // commands need > 37us to settle
}

void write4bits(uint8_t value) {
		if ((value >> 0) & 0x01){GPIO_SetBits (GPIOC,GPIO_Pin_4);} //4
		else{GPIO_ResetBits (GPIOC,GPIO_Pin_4);}
		if ((value >> 1) & 0x01){GPIO_SetBits (GPIOB,GPIO_Pin_0);} //5
		else{GPIO_ResetBits (GPIOB,GPIO_Pin_0);}
		if ((value >> 2) & 0x01){GPIO_SetBits (GPIOA,GPIO_Pin_7);} //6
		else{GPIO_ResetBits (GPIOA,GPIO_Pin_7);}
		if ((value >> 3) & 0x01){GPIO_SetBits (GPIOC,GPIO_Pin_5);} //7
		else{GPIO_ResetBits (GPIOC,GPIO_Pin_5);}
  

   pulseEnable();
}