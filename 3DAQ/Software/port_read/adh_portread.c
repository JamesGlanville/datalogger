#include "adh_picdriver.h"
#include "stdio.h"

#define true 1
#define false 0

int main ()
{
  /* open com port */
  int com_port_no = 0;
  RS232_Init(com_port_no);

  /* main loop: read adc value and print */
  while (1) {
    static int value = 0;
    
    read_analogue_input(&value);

    printf(value);
  }
}
