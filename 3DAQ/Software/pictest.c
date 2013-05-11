#include <cvirte.h>		
#include <userint.h>
#include "pictest.h"
#include "picdriver.h"  

#define true 1
#define false 0

//This file contains high level functions to respond to user inputs from the GUI
//You will need to develop a GUI and corresponding callback functions for your own application

static int panelHandle;

//Main - displays the GUI
int main (int argc, char *argv[])
{
	//Check for errors
	if (InitCVIRTE (0, argv, 0) == 0)
		return -1;	/* out of memory */
	if ((panelHandle = LoadPanel (0, "pictest.uir", PANEL)) < 0)
		return -1;
	
	//Run GUI
	DisplayPanel (panelHandle);
	
	RunUserInterface ();

	DiscardPanel (panelHandle);
	return 0;
}

int CVICALLBACK COM_button_hit (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	int com_port_no;
	
	if (event!=EVENT_COMMIT) return 0;
	
	//Get user input COM port number
	GetCtrlVal(panel, PANEL_USER_COM_PORT, &com_port_no);
	
	//Open specified COM port
	RS232_Init(com_port_no);
	
	return 0;
}

int CVICALLBACK read_analog_button_hit (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{   
	static int value=0;
	
	if (event!=EVENT_COMMIT) return 0;
	
	//Read value
	read_analog_input(&value);
	
	//Display Value
	SetCtrlVal(panel, PANEL_analog_input_slide, value);
    
    return 0;
}
int CVICALLBACK continuous_hit (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{   
	while(1){
	static int value=0;
	
	if (event!=EVENT_COMMIT) return 0;
	
	//Read value
	read_analog_input(&value);
	
	//Display Value
	SetCtrlVal(panel, PANEL_analog_input_slide, value); }
    
    return 0;
}

int CVICALLBACK quit_button_hit (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
    if (event!=EVENT_COMMIT) return 0;
	
	//Close COM Port
	RS232_Close();
	
	//Close GUI
    QuitUserInterface (0);
    return 0;
}

int CVICALLBACK led_button_hit (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{   int button_state;

    if (event!=EVENT_COMMIT) return 0; 
    
	//Get value (on or off)
    GetCtrlVal(panel, control, &button_state);
    
	//Set LED to specified value
    set_led (button_state);
    
	return 0;
}
