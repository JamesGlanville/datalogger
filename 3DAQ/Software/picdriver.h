#define no_error 0
#define RX_BUFF_LEN 64 

void RS232_Init(int port_no); 
void RS232_Close(void);
unsigned char* Read_Data_Block(void);
void send_command(int n);

int read_analog_input (int *value);
int set_led (int value);
