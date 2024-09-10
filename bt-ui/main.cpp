#include <wx/wx.h>

const std::string APP_NAME = "Bittorrent Client";

class BtGui : public wxApp {
 public:
  bool OnInit() override;
};

wxIMPLEMENT_APP(BtGui);

class MainFrame : public wxFrame {
 public:
  MainFrame();

 private:
  void OnHello(wxCommandEvent& event);
  void OnExit(wxCommandEvent& event);
  void OnAbout(wxCommandEvent& event);
};

enum { ID_Hello = 1 };

bool BtGui::OnInit() {
  MainFrame* frame = new MainFrame();
  frame->SetMinSize(wxSize(600, 400));
  frame->Show(true);
  return true;
}

MainFrame::MainFrame() : wxFrame(nullptr, wxID_ANY, APP_NAME) {
  wxMenu* menuFile = new wxMenu;
  menuFile->Append(ID_Hello, "&Hello...\tCtrl-H",
                   "Help string shown in status bar for this menu item");
  menuFile->AppendSeparator();
  menuFile->Append(wxID_EXIT);

  wxMenu* menuHelp = new wxMenu;
  menuHelp->Append(wxID_ABOUT);

  wxMenuBar* menuBar = new wxMenuBar;
  menuBar->Append(menuFile, "&File");
  menuBar->Append(menuHelp, "&Help");

  SetMenuBar(menuBar);

  CreateStatusBar();
  SetStatusText("Welcome to " + APP_NAME + "!");

  Bind(wxEVT_MENU, &MainFrame::OnHello, this, ID_Hello);
  Bind(wxEVT_MENU, &MainFrame::OnAbout, this, wxID_ABOUT);
  Bind(wxEVT_MENU, &MainFrame::OnExit, this, wxID_EXIT);
}

void MainFrame::OnExit(wxCommandEvent& event) { Close(true); }

void MainFrame::OnAbout(wxCommandEvent& event) {
  wxMessageBox("This is a wxWidgets Hello World example", "About " + APP_NAME,
               wxOK | wxICON_INFORMATION);
}

void MainFrame::OnHello(wxCommandEvent& event) {
  wxLogMessage("Hello world from wxWidgets!");
}