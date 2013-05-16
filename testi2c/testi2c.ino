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
     /*   switch (x)
        {
          case 0:  Serial.print("  0.00 'C"); break;
          case 1:  Serial.print("  2.69 'C"); break;
          case 2:  Serial.print("  5.38 'C"); break;
          case 3:  Serial.print("  8.08 'C"); break;
          case 4:  Serial.print(" 10.81 'C"); break;
          case 5:  Serial.print(" 13.55 'C"); break;
          case 6:  Serial.print(" 16.33 'C"); break;
          case 7:  Serial.print(" 19.16 'C"); break;
          case 8:  Serial.print(" 22.02 'C"); break;
          case 9:  Serial.print(" 24.95 'C"); break;
          case 10: Serial.print(" 27.95 'C"); break;
          case 11: Serial.print(" 31.04 'C"); break;
          case 12: Serial.print(" 34.23 'C"); break;
          case 13: Serial.print(" 37.54 'C"); break;
          case 14: Serial.print(" 41.01 'C"); break;
          case 15: Serial.print(" 44.68 'C"); break;
          case 16: Serial.print(" 48.59 'C"); break;
          case 17: Serial.print(" 52.83 'C"); break;
          case 18: Serial.print(" 57.54 'C"); break;
          case 19: Serial.print(" 62.95 'C"); break;
          case 20: Serial.print(" 69.64 'C"); break;
          case 21: Serial.print(" 79.86 'C"); break;
          default: Serial.print("   ---   "); break;
          case 43: Serial.print("-79.86 'C"); break;
          case 44: Serial.print("-69.64 'C"); break;
          case 45: Serial.print("-62.95 'C"); break;
          case 46: Serial.print("-57.54 'C"); break;
          case 47: Serial.print("-52.83 'C"); break;
          case 48: Serial.print("-48.59 'C"); break;
          case 49: Serial.print("-44.68 'C"); break;
          case 50: Serial.print("-41.01 'C"); break;
          case 51: Serial.print("-37.54 'C"); break;
          case 52: Serial.print("-34.23 'C"); break;
          case 53: Serial.print("-31.04 'C"); break;
          case 54: Serial.print("-27.95 'C"); break;
          case 55: Serial.print("-24.95 'C"); break;
          case 56: Serial.print("-22.02 'C"); break;
          case 57: Serial.print("-19.16 'C"); break;
          case 58: Serial.print("-16.33 'C"); break;
          case 59: Serial.print("-13.55 'C"); break;
          case 60: Serial.print("-10.81 'C"); break;
          case 61: Serial.print("- 8.08 'C"); break;
          case 62: Serial.print("- 5.38 'C"); break;
          case 63: Serial.print("- 2.69 'C"); break;
        }*/
      break;
      }
      case 1: { Serial.print("     Y-as = ");  
       char y = REG_ADDRESS[i];
       Serial.print((int)y);
     /*   switch (y)
        {
          case 0:  Serial.print("  0.00 'C"); break;
          case 1:  Serial.print("  2.69 'C"); break;
          case 2:  Serial.print("  5.38 'C"); break;
          case 3:  Serial.print("  8.08 'C"); break;
          case 4:  Serial.print(" 10.81 'C"); break;
          case 5:  Serial.print(" 13.55 'C"); break;
          case 6:  Serial.print(" 16.33 'C"); break;
          case 7:  Serial.print(" 19.16 'C"); break;
          case 8:  Serial.print(" 22.02 'C"); break;
          case 9:  Serial.print(" 24.95 'C"); break;
          case 10: Serial.print(" 27.95 'C"); break;
          case 11: Serial.print(" 31.04 'C"); break;
          case 12: Serial.print(" 34.23 'C"); break;
          case 13: Serial.print(" 37.54 'C"); break;
          case 14: Serial.print(" 41.01 'C"); break;
          case 15: Serial.print(" 44.68 'C"); break;
          case 16: Serial.print(" 48.59 'C"); break;
          case 17: Serial.print(" 52.83 'C"); break;
          case 18: Serial.print(" 57.54 'C"); break;
          case 19: Serial.print(" 62.95 'C"); break;
          case 20: Serial.print(" 69.64 'C"); break;
          case 21: Serial.print(" 79.86 'C"); break;
          default: Serial.print("   ---   "); break;
          case 43: Serial.print("-79.86 'C"); break;
          case 44: Serial.print("-69.64 'C"); break;
          case 45: Serial.print("-62.95 'C"); break;
          case 46: Serial.print("-57.54 'C"); break;
          case 47: Serial.print("-52.83 'C"); break;
          case 48: Serial.print("-48.59 'C"); break;
          case 49: Serial.print("-44.68 'C"); break;
          case 50: Serial.print("-41.01 'C"); break;
          case 51: Serial.print("-37.54 'C"); break;
          case 52: Serial.print("-34.23 'C"); break;
          case 53: Serial.print("-31.04 'C"); break;
          case 54: Serial.print("-27.95 'C"); break;
          case 55: Serial.print("-24.95 'C"); break;
          case 56: Serial.print("-22.02 'C"); break;
          case 57: Serial.print("-19.16 'C"); break;
          case 58: Serial.print("-16.33 'C"); break;
          case 59: Serial.print("-13.55 'C"); break;
          case 60: Serial.print("-10.81 'C"); break;
          case 61: Serial.print("- 8.08 'C"); break;
          case 62: Serial.print("- 5.38 'C"); break;
          case 63: Serial.print("- 2.69 'C"); break;
        }*/
      break;
      }
      case 2:   { Serial.print("     Z-as = ");  
       char x = REG_ADDRESS[i];
       Serial.print((int)x);
        /*switch (x)
        {
          case 0:  Serial.print(" 90.00 'C"); break;
          case 1:  Serial.print(" 87.31 'C"); break;
          case 2:  Serial.print(" 84.62 'C"); break;
          case 3:  Serial.print(" 81.92 'C"); break;
          case 4:  Serial.print(" 79.19 'C"); break;
          case 5:  Serial.print(" 76.45 'C"); break;
          case 6:  Serial.print(" 73.67 'C"); break;
          case 7:  Serial.print(" 70.84 'C"); break;
          case 8:  Serial.print(" 67.98 'C"); break;
          case 9:  Serial.print(" 65.05 'C"); break;
          case 10: Serial.print(" 62.05 'C"); break;
          case 11: Serial.print(" 58.96 'C"); break;
          case 12: Serial.print(" 55.77 'C"); break;
          case 13: Serial.print(" 52.46 'C"); break;
          case 14: Serial.print(" 48.99 'C"); break;
          case 15: Serial.print(" 45.32 'C"); break;
          case 16: Serial.print(" 41.41 'C"); break;
          case 17: Serial.print(" 37.17 'C"); break;
          case 18: Serial.print(" 32.46 'C"); break;
          case 19: Serial.print(" 27.05 'C"); break;
          case 20: Serial.print(" 20.36 'C"); break;
          case 21: Serial.print(" 10.14 'C"); break;
          default: Serial.print("   ---   "); break;
          case 43: Serial.print("-87.31 'C"); break;
          case 44: Serial.print("-84.62 'C"); break;
          case 45: Serial.print("-81.92 'C"); break;
          case 46: Serial.print("-79.19 'C"); break;
          case 47: Serial.print("-76.45 'C"); break;
          case 48: Serial.print("-73.67 'C"); break;
          case 49: Serial.print("-70.84 'C"); break;
          case 50: Serial.print("-67.98 'C"); break;
          case 51: Serial.print("-65.05 'C"); break;
          case 52: Serial.print("-62.05 'C"); break;
          case 53: Serial.print("-58.96 'C"); break;
          case 54: Serial.print("-55.77 'C"); break;
          case 55: Serial.print("-52.46 'C"); break;
          case 56: Serial.print("-48.99 'C"); break;
          case 57: Serial.print("-45.32 'C"); break;
          case 58: Serial.print("-41.41 'C"); break;
          case 59: Serial.print("-37.17 'C"); break;
          case 60: Serial.print("-32.46 'C"); break;
          case 61: Serial.print("-27.05 'C"); break;
          case 62: Serial.print("-20.36 'C"); break;
          case 63: Serial.print("-10.14 'C"); break;
        }*/
      Serial.println("");
      delay(200);
      break;
      }
    }
  }
}
