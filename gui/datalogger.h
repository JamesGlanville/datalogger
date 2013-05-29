/*
File name: gui/datalogger.h
Description: header file for gui framework
Author: Andy Holt
Date: Mon 20 May 2013 16:52
*/

#ifndef datalogger_h
#define datalogger_h

#include <wx/wx.h>
#include <wx/spinctrl.h>
#include <wx/textctrl.h>
#include <iostream>
#include <fstream>

enum
  {
    PORT_SELECT = wxID_HIGHEST + 1,
    SAMPLE_SELECT,
    PORT_CONNECT,
    PORT_DISCONNECT,
    SAMPLE_SEND,
    LOG_START,
    LOG_STOP,
    DATA_GET,
    DATA_ERASE,
    SAVE_CSV,
    FIND_EVENTS,
    GRAPH_DATA,
  };

//class definition from wxApp;

class MyApp: public wxApp
{
public:
  bool read_loop_on;
  virtual bool OnInit();
  void onIdle(wxIdleEvent& evt);
};

// class definition from wxFrame

class MyFrame: public wxFrame
{
public:
  MyFrame (const wxString& title, const wxPoint& pos, const wxSize& size);
  wxSpinCtrl *spin_port;
  wxSpinCtrl *spin_sample;
  wxString currentdocpath;

  //private:
  void OnExit(wxCommandEvent& event);
  void OnAbout(wxCommandEvent& event);
  void OnPortSelect(wxSpinEvent& event);
  void OnPortConnect(wxCommandEvent& event);
  void OnPortDisconnect(wxCommandEvent& event);
  void OnSampleSelect(wxSpinEvent& event);
  void OnSampleSend(wxCommandEvent& event);
  void OnLogStart(wxCommandEvent& event);
  void OnLogStop(wxCommandEvent& event);
  void OnDataGet(wxCommandEvent& event);
  void OnDataErase(wxCommandEvent& event);
  void OnCSVWrite(wxCommandEvent& event);
  void OnFind_Events(wxCommandEvent& event);
  void OnGraph_Data(wxCommandEvent& event);
  DECLARE_EVENT_TABLE();

};

#endif	// datalogger_h
