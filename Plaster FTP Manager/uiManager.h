#pragma once

//Basics
#include <vector>
#include <string>
#include <iostream>


#include <wx/wx.h>
#include <wx/utils.h>

// local files
#include "Toolbar.h"
#include "DirView.h"
#include "FTPManager.h"

//Widgets
#include <wx/splitter.h>
#include <wx/dirctrl.h>
#include <wx/msw/darkmode.h>

class uiManager : public wxFrame {
public: 
	uiManager(const wxString& title) : wxFrame(nullptr, wxID_ANY, title, wxDefaultPosition, wxSize(800, 600))
	{
		Toolbar* tb = new Toolbar();
		SetMenuBar(tb);

		// Provide the parent window for dialogs
		tb->getParentWindow = [this]() { return this; };

		// Handle new FTPManager* connections
		tb->onConnectClicked = [this](FTPManager* connection) {
			addConnection(connection);

			if (connection->isConnected())
				wxMessageBox("Connected successfully!", "Status", wxOK | wxICON_INFORMATION, this);
			else
				wxMessageBox("Connection failed: " + connection->getLastError(),
					"Status", wxOK | wxICON_ERROR, this);
			};

		wxSplitterWindow* mainSplitter = new wxSplitterWindow(this, wxID_ANY);

		// Init HIDDEN Local Directory Control
		m_dirCtrl = new wxGenericDirCtrl(mainSplitter, wxID_ANY, wxEmptyString,
			wxDefaultPosition, wxSize(200, -1),
			wxDIRCTRL_DIR_ONLY);

		m_dirCtrl->SetPath(currentLocalPath);
		m_dirCtrl->Hide();


		// Init Directory List Views
		listCtrl = new DirView(mainSplitter, 0);
		ftpListCtrl = new DirView(mainSplitter, 1);

		mainSplitter->SplitVertically(listCtrl, ftpListCtrl); // Divide between the two parts
		mainSplitter->SetSashGravity(0.5); // The initial position of the Sash

		listCtrl->UpdateFileList(currentLocalPath);
	}

	void addConnection(FTPManager* connection) {
		SFTPConnections.push_back(connection);
	}

private:
	wxGenericDirCtrl* m_dirCtrl; // Hidden Directory Controls

	DirView* listCtrl; // Local File Browser
	DirView* ftpListCtrl; // FTP Browser

	// ENV variables
	wxString currentLocalPath = "C:\\";
	wxLogNull noLog; // Disable Error Logging by wxWidget
	std::vector<FTPManager*> SFTPConnections;


	void onDirSelectionChanged(wxTreeEvent& event) {
		wxString selectedPath = m_dirCtrl->GetPath();
		(static_cast<DirView*>(event.GetEventObject()))->UpdateFileList(selectedPath);
	}

	void OnClose(wxCloseEvent& event) {

	}
};

class MyApp : public wxApp {
public:
	bool OnInit() override;
};