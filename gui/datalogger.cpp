/*
File name: gui/datalogger.cpp
Description: main file for gui framework
Author: Andy Holt
Date: Mon 20 May 2013 15:30
*/

#include "datalogger.h"
#include "comms.h"

 BEGIN_EVENT_TABLE(MyFrame, wxFrame)
  EVT_MENU(wxID_EXIT, MyFrame::OnExit)
  EVT_MENU(wxID_ABOUT, MyFrame::OnAbout)
  EVT_MENU(LOG_START, MyFrame::OnLogStart)
  EVT_MENU(LOG_STOP, MyFrame::OnLogStop)
  EVT_MENU(DATA_GET, MyFrame::OnDataGet)
  EVT_MENU(DATA_ERASE, MyFrame::OnDataErase)
  EVT_SPINCTRL(PORT_SELECT, MyFrame::OnPortSelect)
  EVT_BUTTON(PORT_CONNECT, MyFrame::OnPortConnect)
  EVT_BUTTON(LOG_START, MyFrame::OnLogStart)
  EVT_BUTTON(LOG_STOP, MyFrame::OnLogStop)
  EVT_BUTTON(DATA_GET, MyFrame::OnDataGet)
  EVT_BUTTON(DATA_ERASE, MyFrame::OnDataErase)
  EVT_BUTTON(FIND_EVENTS, MyFrame::OnFind_Events)
  EVT_BUTTON(GRAPH_DATA, MyFrame::OnGraph_Data)
END_EVENT_TABLE()


IMPLEMENT_APP(MyApp)

bool MyApp::OnInit()
{
  MyFrame *frame = new MyFrame ( wxT("Parcel Tracker"), wxPoint(50, 50),
				 wxSize(450, 340));
  frame->Show(true);
  return true;
}

MyFrame::MyFrame(const wxString& title, const wxPoint& pos, const
		 wxSize& size)
  : wxFrame(NULL, wxID_ANY, title, pos, size)
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

  wxBoxSizer *connectsizer = new wxBoxSizer(wxVERTICAL);
  connectsizer->Add(new wxStaticText(this, wxID_ANY, wxT("Data Port")),
		    0, wxALL | wxALIGN_CENTER_HORIZONTAL, 5);
  spin_port = new wxSpinCtrl(this, PORT_SELECT, wxString(wxT("0")));
  connectsizer->Add(spin_port, 0, wxALL | wxALIGN_CENTER_HORIZONTAL, 5);
  connectsizer->Add(new wxButton(this, PORT_CONNECT, wxT("Connect")),
		 0, wxALL | wxALIGN_CENTER_HORIZONTAL, 5);
  ctrlsizer->Add(connectsizer, 1, wxALIGN_CENTER_HORIZONTAL | wxALL, 5);

  wxBoxSizer *datalink_sizer = new wxBoxSizer(wxVERTICAL);
  datalink_sizer->Add(new wxStaticText(this, wxID_ANY, wxT("Linked Mode")),
		      0, wxALL | wxALIGN_CENTER_HORIZONTAL, 5);
  datalink_sizer->Add(new wxButton(this, LOG_START, wxT("Start Logging")),
		      0, wxALL | wxALIGN_CENTER_HORIZONTAL, 5);
  datalink_sizer->Add(new wxButton(this, LOG_STOP, wxT("Stop Logging")),
		      0, wxALL | wxALIGN_CENTER_HORIZONTAL, 5);
  ctrlsizer->Add(datalink_sizer, 1, wxALIGN_CENTER_HORIZONTAL | wxALL, 5);

  wxBoxSizer *upload_sizer = new wxBoxSizer(wxVERTICAL);
  upload_sizer->Add(new wxStaticText(this, wxID_ANY, wxT("Standalone Mode")),
		    0, wxALL | wxALIGN_CENTER_HORIZONTAL, 5);
  upload_sizer->Add(new wxButton(this, DATA_GET, wxT("Get Data")),
		      0, wxALL | wxALIGN_CENTER_HORIZONTAL, 5);
  upload_sizer->Add(new wxButton(this, DATA_ERASE, wxT("Erase Data")),
		      0, wxALL | wxALIGN_CENTER_HORIZONTAL, 5);
  ctrlsizer->Add(upload_sizer, 1, wxALIGN_CENTER_HORIZONTAL | wxALL, 5);
  
  wxBoxSizer *analysis_sizer = new wxBoxSizer(wxVERTICAL);
  analysis_sizer->Add(new wxStaticText(this, wxID_ANY, wxT("Analyse")),
		      0, wxALL | wxALIGN_CENTER_HORIZONTAL, 5);
  analysis_sizer->Add(new wxButton(this, FIND_EVENTS, wxT("Find Events")),
		      0, wxALL | wxALIGN_CENTER_HORIZONTAL, 5);
  analysis_sizer->Add(new wxButton(this, GRAPH_DATA, wxT("View All")),
		      0, wxALL | wxALIGN_CENTER_HORIZONTAL, 5);
  ctrlsizer->Add(analysis_sizer, 1, wxALIGN_CENTER_HORIZONTAL | wxALL, 5);

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
  com_port_no = event.GetPosition();
}

void MyFrame::OnPortConnect(wxCommandEvent& event)
{
  /*
  wxString message = wxT("Attempting to connect to module thorugh comm port");
  message = message << com_port_no;
  wxLogMessage(message);
  */
  if (com_port_open)
    {
      wxLogMessage(wxT("Already connected!"));
      return;
    }
  
  RS232_Init(com_port_no);

  if (com_port_open)
    {
      wxLogMessage(wxT("Error: com port was unable to open"));
    }

  return;
}

void MyFrame::OnLogStart(wxCommandEvent& event)
{
  wxLogMessage(wxT("Logging!"));
}

void MyFrame::OnLogStop(wxCommandEvent& event)
{
  wxLogMessage(wxT("Finished logging!"));
}

void MyFrame::OnDataGet(wxCommandEvent& event)
{
  wxLogMessage(wxT("Uploading data from standalone mode."));
}

void MyFrame::OnDataErase(wxCommandEvent& event)
{
  wxLogMessage(wxT("Erasing board memory"));
}

void  MyFrame::OnFind_Events(wxCommandEvent& event)
{
  wxLogMessage(wxT("Finding interest points"));
}

void  MyFrame::OnGraph_Data(wxCommandEvent& event)
{
    wxLogMessage(wxT("Graphing all data"));
}
