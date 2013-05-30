/*
File name: gui/datalogger.cpp
Description: main file for gui framework
Author: Andy Holt
Date: Mon 20 May 2013 15:30
*/

#include "datalogger.h"
#include "comms.h"

extern int com_port_no;
extern bool com_port_open;
extern config_data cfgdata;
extern std::vector <packet> data;

wxStaticText	*temperature_label;
wxStaticText	*temperature;
wxStaticText	*humidity;
wxStaticText	*accelerationx;
wxStaticText	*accelerationy;
wxStaticText	*accelerationz;

extern BYTE rx_buff[RX_BUFF_LEN];
extern BYTE tx_buff[10];

int sample_period = 60;

 BEGIN_EVENT_TABLE(MyFrame, wxFrame)
  EVT_MENU(wxID_EXIT, MyFrame::OnExit)
  EVT_MENU(wxID_ABOUT, MyFrame::OnAbout)
  EVT_MENU(LOG_START, MyFrame::OnLogStart)
  EVT_MENU(LOG_STOP, MyFrame::OnLogStop)
  EVT_MENU(DATA_GET, MyFrame::OnDataGet)
  EVT_MENU(DATA_ERASE, MyFrame::OnDataErase)
  EVT_SPINCTRL(PORT_SELECT, MyFrame::OnPortSelect)
  EVT_SPINCTRL(SAMPLE_SELECT, MyFrame::OnSampleSelect)
  EVT_BUTTON(PORT_CONNECT, MyFrame::OnPortConnect)
  EVT_BUTTON(PORT_DISCONNECT, MyFrame::OnPortDisconnect)
  EVT_BUTTON(SAMPLE_SEND, MyFrame::OnSampleSend)
  EVT_BUTTON(LOG_START, MyFrame::OnLogStart)
  EVT_BUTTON(LOG_STOP, MyFrame::OnLogStop)
  EVT_BUTTON(DATA_GET, MyFrame::OnDataGet)
  EVT_BUTTON(DATA_ERASE, MyFrame::OnDataErase)
  EVT_BUTTON(SAVE_CSV, MyFrame::OnCSVWrite)
  EVT_BUTTON(FIND_EVENTS, MyFrame::OnFind_Events)
  EVT_BUTTON(GRAPH_DATA, MyFrame::OnGraph_Data)
END_EVENT_TABLE()


IMPLEMENT_APP(MyApp)

bool MyApp::OnInit()
{
	read_loop_on=false;
  MyFrame *frame = new MyFrame ( wxT("Parcel Tracker"), wxPoint(50, 50),
				 wxSize(450, 340));
  frame->Show(true);
  return true;
}


// idea from http://wiki.wxwidgets.org/Making_a_render_loop
void MyApp::onIdle(wxIdleEvent& evt)
{
  if(read_loop_on)
    {
      get_Readings();
      evt.RequestMore();
    }
}

MyFrame::MyFrame(const wxString& title, const wxPoint& pos, const
		 wxSize& size)
  : wxFrame(NULL, wxID_ANY, title, pos, wxSize(700,400), wxDEFAULT_FRAME_STYLE | wxFULL_REPAINT_ON_RESIZE)
{
  wxMenu *menuFile = new wxMenu;
  menuFile->Append(wxID_EXIT);
  wxMenu *menuData = new wxMenu;
  menuData->Append(LOG_START, wxT("&Start Logging\tCtrl-L"),
		   wxT("Help string: Begin logging with board plugged in."));
  menuData->Append(LOG_STOP, wxT("&Stop Logging\tCtrl-K"),
		   wxT("Help string: Stop logging with board plugged in."));
  menuData->Append(DATA_GET, wxT("&Get Data\tCtrl-D"),
		   wxT("Help string: Get data from board."));
  menuData->Append(DATA_ERASE, wxT("&Erase Data\tCtrl-E"),
		   wxT("Help string: Erase chip memory."));
  wxMenu *menuHelp = new wxMenu;
  menuHelp->Append(wxID_ABOUT);
  
  wxMenuBar *menuBar = new wxMenuBar;
  menuBar->Append(menuFile, wxT("&File"));
  menuBar->Append(menuData, wxT("&Data"));
  menuBar->Append(menuHelp, wxT("&Help"));
  
  SetMenuBar(menuBar);

  wxBoxSizer *topsizer = new wxBoxSizer(wxVERTICAL);

  wxBoxSizer *ctrlsizer = new wxBoxSizer(wxHORIZONTAL);

  wxBoxSizer *connect_sizer = new wxBoxSizer(wxVERTICAL);

  temperature_label = new wxStaticText(this,TEMP_LABEL,wxT("Temperature/C"));
  temperature = new wxStaticText(this,TEMP,wxT("?"));
  humidity = new wxStaticText(this,TEMP,wxT("?"));
  accelerationx = new wxStaticText(this,TEMP,wxT("?"));
  accelerationy = new wxStaticText(this,TEMP,wxT("?"));
  accelerationz = new wxStaticText(this,TEMP,wxT("?"));

  connect_sizer->Add(new wxStaticText(this, wxID_ANY, wxT("Data Port")),
		    0, wxALL | wxALIGN_CENTER_HORIZONTAL, 5);
  spin_port = new wxSpinCtrl(this, PORT_SELECT, wxString(wxT("1")));
  connect_sizer->Add(spin_port, 0, wxALL | wxALIGN_CENTER_HORIZONTAL, 5);
  connect_sizer->Add(new wxButton(this, PORT_CONNECT, wxT("Connect")),
		    0, wxALL | wxALIGN_CENTER_HORIZONTAL, 5);
  connect_sizer->Add(new wxButton(this, PORT_DISCONNECT, wxT("Disconnect")),
		    0, wxALL | wxALIGN_CENTER_HORIZONTAL, 5);
  connect_sizer->Add(temperature_label,0,wxALL|wxALIGN_CENTER_HORIZONTAL, 5);
  connect_sizer->Add(temperature,0,wxALL|wxALIGN_CENTER_HORIZONTAL, 5);
  ctrlsizer->Add(connect_sizer, 1, wxEXPAND | wxALL, 5);

  wxBoxSizer *samplerate_sizer = new wxBoxSizer(wxVERTICAL);
  samplerate_sizer->Add(new wxStaticText(this, wxID_ANY, wxT("Sample Period")),
			0, wxALL | wxALIGN_CENTER_HORIZONTAL, 5);
  spin_sample = new wxSpinCtrl(this, SAMPLE_SELECT, wxString(wxT("60")));
  samplerate_sizer->Add(spin_sample, 0, wxALL | wxALIGN_CENTER_HORIZONTAL, 5);
  samplerate_sizer->Add(new wxButton(this, SAMPLE_SEND, wxT("Set Rate")),
			0, wxALL | wxALIGN_CENTER_HORIZONTAL, 5);
  samplerate_sizer->Add(new wxStaticText(this,HUMID_LABEL,wxT("Humidity/%RH")),0,wxALL|wxALIGN_CENTER_HORIZONTAL, 5);
  samplerate_sizer->Add(humidity,0,wxALL|wxALIGN_CENTER_HORIZONTAL, 5);
  ctrlsizer->Add(samplerate_sizer, 1, wxEXPAND | wxALL, 5);

  wxBoxSizer *datalink_sizer = new wxBoxSizer(wxVERTICAL);
  datalink_sizer->Add(new wxStaticText(this, wxID_ANY, wxT("Linked Mode")),
		      0, wxALL | wxALIGN_CENTER_HORIZONTAL, 5);
  datalink_sizer->Add(new wxButton(this, LOG_START, wxT("Start Logging")),
		      0, wxALL | wxALIGN_CENTER_HORIZONTAL, 5);
  datalink_sizer->Add(new wxButton(this, LOG_STOP, wxT("Stop Logging")),
		      0, wxALL | wxALIGN_CENTER_HORIZONTAL, 5);
  datalink_sizer->Add(new wxStaticText(this,ACCX_LABEL,wxT("AccelX /mg")),0,wxALL|wxALIGN_CENTER_HORIZONTAL, 5);
  datalink_sizer->Add(accelerationx,0,wxALL|wxALIGN_CENTER_HORIZONTAL, 5);

  ctrlsizer->Add(datalink_sizer, 1, wxEXPAND | wxALL, 5);

  wxBoxSizer *upload_sizer = new wxBoxSizer(wxVERTICAL);
  upload_sizer->Add(new wxStaticText(this, wxID_ANY, wxT("Standalone Mode")),
		    0, wxALL | wxALIGN_CENTER_HORIZONTAL, 5);
  upload_sizer->Add(new wxButton(this, DATA_GET, wxT("Get Data")),
		      0, wxALL | wxALIGN_CENTER_HORIZONTAL, 5);
  upload_sizer->Add(new wxButton(this, DATA_ERASE, wxT("Erase Data")),
		      0, wxALL | wxALIGN_CENTER_HORIZONTAL, 5);
  upload_sizer->Add(new wxStaticText(this,ACCY_LABEL,wxT("AccelY /mg")),0,wxALL|wxALIGN_CENTER_HORIZONTAL, 5);
    upload_sizer->Add(accelerationy,0,wxALL|wxALIGN_CENTER_HORIZONTAL, 5);

  ctrlsizer->Add(upload_sizer, 1, wxEXPAND | wxALL, 5);
  
  wxBoxSizer *analysis_sizer = new wxBoxSizer(wxVERTICAL);
  analysis_sizer->Add(new wxStaticText(this, wxID_ANY, wxT("Analyse")),
		      0, wxALL | wxALIGN_CENTER_HORIZONTAL, 5);
  analysis_sizer->Add(new wxButton(this, SAVE_CSV, wxT("Write to CSV")),
		      0, wxALL | wxALIGN_CENTER_HORIZONTAL, 5);
  analysis_sizer->Add(new wxButton(this, FIND_EVENTS, wxT("Find Events")),
		      0, wxALL | wxALIGN_CENTER_HORIZONTAL, 5);
  analysis_sizer->Add(new wxButton(this, GRAPH_DATA, wxT("View All")),
		      0, wxALL | wxALIGN_CENTER_HORIZONTAL, 5);
  analysis_sizer->Add(new wxStaticText(this,ACCZ_LABEL,wxT("AccelZ /mg")),0,wxALL|wxALIGN_CENTER_HORIZONTAL, 5);
  analysis_sizer->Add(accelerationz,0,wxALL|wxALIGN_CENTER_HORIZONTAL, 5);

  ctrlsizer->Add(analysis_sizer, 1, wxEXPAND | wxALL, 5);

  topsizer->Add(ctrlsizer, 1, wxALL | wxEXPAND, 5);

  SetSizeHints(400,400);
  SetSizer(topsizer);
  
  CreateStatusBar();
  SetStatusText(wxT("Welcome to Parcel Tracker!"));
}

/********************** Callback Functions ******************************/

void MyFrame::OnExit(wxCommandEvent& event)
{
  Close(true);
}

void MyFrame::OnAbout(wxCommandEvent& event)
{
  wxMessageBox(wxT("This is the interface for Parcel Tracker.\n\
                    By James Glanville and Andy Holt.\n\
                    Created using wxWidgets."),
	       wxT("About Parcel Tracker"), wxOK | wxICON_INFORMATION);
}

void MyFrame::OnPortSelect(wxSpinEvent& event)
{
  com_port_no = event.GetPosition()-1;
}

void MyFrame::OnPortConnect(wxCommandEvent& event)
{
  if (com_port_open)
    {
      wxLogMessage(wxT("Already connected!"));
      return;
    }
  
  RS232_Init(com_port_no);

  if (!com_port_open)
    {
      wxLogMessage(wxT("Error: com port was unable to open"));
    }

  return;
}

void MyFrame::OnPortDisconnect(wxCommandEvent& event)
{
  if (!com_port_open)
    {
      wxLogMessage(wxT("No com port to close!"));
      return;
    }

  RS232_Close();

  if (!com_port_open)
    {
      wxLogMessage(wxT("Com port closed."));
    }
  else
    {
      wxLogMessage(wxT("Error, com port has not closed properly"));
    }
  return;
}

void MyFrame::OnSampleSelect(wxSpinEvent& event)
{
  sample_period = event.GetPosition()-1;
}

void MyFrame::OnSampleSend(wxCommandEvent& event)
{
  if (!com_port_open)
    {
      wxLogMessage(wxT("Error: no com port open!"));
      return;
    }

  // set first char in array to R and second to value of sample_period
  for (int i = 0; i < 10; i++)
    {
      tx_buff[i] = '\0';
    }
  tx_buff[0] = 'R';
  tx_buff[1] = sample_period;

  send_command(10);
}

void MyFrame::OnLogStart(wxCommandEvent& event)
{
  if (!com_port_open)
    {
      wxLogMessage(wxT("Error: no com port open!"));
      return;
    }

  // set first char in array to L for beginning logging
   for (int i = 0; i < 10; i++)
	{
	  tx_buff[i] = '\0';
    }
  tx_buff[0] = 'L';
    data.clear();

  // send via serial port
  
  send_command(10);

  Connect( wxID_ANY, wxEVT_IDLE, wxIdleEventHandler(MyApp::onIdle) );  
  wxGetApp().read_loop_on = true;
}

void MyFrame::OnLogStop(wxCommandEvent& event)
{
  // set first char in array to S to stop logging
  for (int i = 0; i < 10; i++)
	{
		tx_buff[i] = '\0';
    }
  tx_buff[0] = 'S';

  // send via serial port
  
  send_command(10);
  
  Disconnect( wxEVT_IDLE, wxIdleEventHandler(MyApp::onIdle) );
  wxGetApp().read_loop_on = false;
}

void MyFrame::OnDataGet(wxCommandEvent& event)
{
  if (!com_port_open)
    {
      wxLogMessage(wxT("Error: no com port open!"));
      return;
    }

  // set first char in array to U to upload data
  for (int i = 0; i < 10; i++)
    {
      tx_buff[i] = '\0';
    }
  tx_buff[0] = 'U';
    data.clear();

  // send via serial port
  
  send_command(1);
  read_eeprom_data();

  
}

void MyFrame::OnDataErase(wxCommandEvent& event)
{
  if (!com_port_open)
    {
      wxLogMessage(wxT("Error: no com port open!"));
      return;
    }

  // set first char in array to E to erase eeprom data
  for (int i = 0; i < 10; i++)
	{
	  tx_buff[i] = '\0';
    }
  tx_buff[0] = 'E';

  // send via serial port
  
  send_command(10);
}

void MyFrame::OnCSVWrite(wxCommandEvent& event)
{
	int maccelx,maccely,maccelz;
  wxFileDialog *writetocsv = new wxFileDialog(this,
					     wxT("Choose file to save in"),
					     wxEmptyString,
					     wxEmptyString,
					     _("CSV files (*.csv)|*.csv"),
					     wxFD_SAVE,
					     wxDefaultPosition);

  if (writetocsv->ShowModal() == wxID_OK) // if the user click "Open" instead of "Cancel"
    {
      currentdocpath = writetocsv->GetPath();
    }

  writetocsv->Destroy();

  std::ofstream csvfile;
  csvfile.open(currentdocpath.fn_str());
  csvfile << "Temperature/C,Humidity/%RH,Accx/mg,Accy/mg,Accz/mg,Shaken?\n";
  int datalen = cfgdata.datalen_u * 256 + cfgdata.datalen_l;
  for (int k = 0; k < data.size(); k += 1)
    {
		maccelx = data[k].accel_0 & 0x0F;
		if (data[k].accel_0 & 0x10) {maccelx=-maccelx;}
		maccelx = (maccelx*1000)/21.33;
		maccely = data[k].accel_1 & 0x0F;
		if (data[k].accel_1 & 0x10) {maccely=-maccely;}
		maccely = (maccely*1000)/21.33;
		maccelz = data[k].accel_1 & 0x0F;
		if (data[k].accel_2 & 0x10) {maccelz=-maccelz;}
		maccelz = (maccelz*1000)/21.33;
		csvfile<<float(data[k].temp_u*256+data[k].temp_l)/100 << "," << (int)data[k].humid << "," << maccelx <<","<< maccely <<","<< maccelz <<"," << (int)data[k].accel_3 <<"\n";
    }
  csvfile.close();

}

void  MyFrame::OnFind_Events(wxCommandEvent& event)
{
  wxLogMessage(wxT("Finding interest points"));
}

void  MyFrame::OnGraph_Data(wxCommandEvent& event)
{
  
}
