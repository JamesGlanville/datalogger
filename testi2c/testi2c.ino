#include <Wire.h>

#define   I2C_ADDRESS 0x4C
#define   X_OUT       0x00
#define   Y_OUT       0x01
#define   Z_OUT       0x02
#define   INTSU       0x06
#define   MODE        0x07
#define   SR          0x08

void setup()
{
  Wire.begin();
  Serial.begin(9600);  // start serial for output
  while (!Serial);
  Serial.println("Setting up I2C protocol...");
  Serial.println("Powering up SLAVE interface...");
  SLAVE_I2C_INIT();
}

//------------------------------------------------------------------

void loop()
{
SLAVE_I2C_READ();
}

//------------------------------------------------------------------

void SLAVE_I2C_INIT()
{
    SLAVE_I2C_SEND(MODE ,0x00); // Setting up MODE to Stand by to set SR
    delay(2);

    SLAVE_I2C_SEND(INTSU,0x10);
    delay(2);

    SLAVE_I2C_SEND(SR   ,0x00);  // Setting up SR register to 120 samples active and auto sleep mode
    delay(2);

    SLAVE_I2C_SEND(MODE ,0x01); //Setting up MODE Active to START measures
}

//------------------------------------------------------------------

void SLAVE_I2C_SEND(unsigned char REG_ADDRESS, unsigned  char DATA)  //SEND data to MMA7660
{
  Wire.beginTransmission(I2C_ADDRESS);
  Wire.write(REG_ADDRESS);
  Wire.write(DATA);
  Wire.endTransmission();
}

//------------------------------------------------------------------

void SLAVE_I2C_READ() //READ MMA7660 data
{
  unsigned char REG_ADDRESS[3];
  int i=0;
  Wire.beginTransmission(I2C_ADDRESS);
  Wire.write(0x00);  // register to read
  Wire.endTransmission();
  Wire.requestFrom(I2C_ADDRESS, 3); // read a byte

  for(i=0; i<3; i++)
  {
   if(Wire.available())REG_ADDRESS[i]=Wire.read();
  }
  for(i=0; i<3; i++){
    switch (i) 
     {
      case 0: { Serial.print("     X-as = ");  
       char x = REG_ADDRESS[i];
       Serial.print((int)x);
          break;
      }
      case 1: { Serial.print("     Y-as = ");  
       char y = REG_ADDRESS[i];
       Serial.print((int)y);
          break;
      }
      case 2:   { Serial.print("     Z-as = ");  
       char x = REG_ADDRESS[i];
       Serial.print((int)x);
             Serial.println("");
      delay(200);
      break;
      }
    }
  }
}
