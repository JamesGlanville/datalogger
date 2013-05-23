/*
File name: gui/datalogger.cpp
Description: main file for gui framework
Author: Andy Holt
Date: Mon 20 May 2013 15:30
*/

//#include <wx/wx.h>
#include "datalogger.h"

BEGIN_EVENT_TABLE(MyFrame, wxFrame)
  EVT_MENU(ID_StartLog, MyFrame::OnStartLog)
  EVT_MENU(ID_StopLog, MyFrame::OnStopLog)
  EVT_MENU(ID_GetData, MyFrame::OnGetData)
  EVT_MENU(ID_EraseData, MyFrame::OnEraseData)
  EVT_MENU(wxID_EXIT, MyFrame::OnExit)
  EVT_MENU(wxID_ABOUT, MyFrame::OnAbout)
  EVT_SPINCTRL(PORT_SELECT, MyFrame::OnPortSelect)
  EVT_BUTTON(PORT_CONNECT, MyFrame::OnPortConnect)
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
  menuData->Append(ID_StartLog, wxT("&Start Logging\tCtrl-L"),
		   wxT("Help string: Begin logging with board plugged in."));
  menuData->Append(ID_StopLog, wxT("&Stop Logging\tCtrl-K"),
		   wxT("Help string: Stop logging with board plugged in."));
  menuData->Append(ID_GetData, wxT("&Get Data\tCtrl-D"),
		   wxT("Help string: Get data from board."));
  menuData->Append(ID_EraseData, wxT("&Erase Data\tCtrl-E"),
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
  connectsizer->Add(new wxStaticText(this, wxID_ANY, wxT("Data Port:")),
		    0,
		    wxALL | wxALIGN_CENTER_VERTICAL,
		    5);
  spin_port = new wxSpinCtrl(this, PORT_SELECT, wxString(wxT("0")));
  connectsizer->Add(spin_port, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);
  connectsizer->Add(new wxButton(this, PORT_CONNECT, wxT("Connect")),
		 0,
		 wxALL | wxALIGN_CENTER_VERTICAL,
		 5);
  ctrlsizer->Add(connectsizer, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 5);
  
  topsizer->Add(ctrlsizer, 1, wxALL | wxALIGN_CENTER_VERTICAL, 5);

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

void MyFrame::OnStartLog(wxCommandEvent& event)
{
  wxLogMessage(wxT("Logging!"));
}

void MyFrame::OnStopLog(wxCommandEvent& event)
{
  wxLogMessage(wxT("Finished logging!"));
}

void MyFrame::OnGetData(wxCommandEvent& event)
{
  wxLogMessage(wxT("Getting data from module."));
}

void MyFrame::OnEraseData(wxCommandEvent& event)
{
  wxLogMessage(wxT("Erasing Data from module."));
}

void MyFrame::OnPortSelect(wxSpinEvent& event)
{
  wxString text;
  text.Printf(wxT("New spinctrl value %d"), event.GetPosition());
}

void MyFrame::OnPortConnect(wxCommandEvent& event)
{
  wxLogMessage(wxT("Attempting to connect to module"));
}
