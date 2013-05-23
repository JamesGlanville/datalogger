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

enum
  {
    ID_StartLog = wxID_HIGHEST + 1,
    ID_StopLog,
    ID_GetData,
    ID_EraseData,
    PORT_SELECT,
    PORT_CONNECT,
  };

//class definition from wxApp;

class MyApp: public wxApp
{
public:
  virtual bool OnInit();
};

// class definition from wxFrame

class MyFrame: public wxFrame
{
public:
  MyFrame (const wxString& title, const wxPoint& pos, const wxSize& size);
  wxSpinCtrl *spin_port;

  //private:
  void OnStartLog(wxCommandEvent& event);
  void OnStopLog(wxCommandEvent& event);
  void OnGetData(wxCommandEvent& event);
  void OnEraseData(wxCommandEvent& event);
  void OnExit(wxCommandEvent& event);
  void OnAbout(wxCommandEvent& event);
  void OnPortSelect(wxSpinEvent& event);
  void OnPortConnect(wxCommandEvent& event);
  DECLARE_EVENT_TABLE();
};

#endif	// datalogger_h
