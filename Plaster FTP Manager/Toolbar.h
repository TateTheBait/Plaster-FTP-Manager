#pragma once
#include <wx/wx.h>
#include <wx/spinctrl.h>

#include "FTPManager.h"


class Toolbar : public wxMenuBar {
public:
	Toolbar() {
		wxMenu* fileMenu = new wxMenu();
		fileMenu->Append(wxID_NEW, "&New\tCtrl-N");
		fileMenu->Append(wxID_OPEN, "&Open\tCtrl-O");
		fileMenu->Append(wxID_SAVE, "&Save\tCtrl-S");
		fileMenu->AppendSeparator();
		fileMenu->Append(wxID_EXIT, "E&xit\tAlt-X");
		Append(fileMenu, "&File");

		// Edit Menu
		wxMenu* editMenu = new wxMenu();
		editMenu->Append(wxID_CUT, "Cu&t\tCtrl-X");
		editMenu->Append(wxID_COPY, "&Copy\tCtrl-C");
		editMenu->Append(wxID_PASTE, "&Paste\tCtrl-V");
		Append(editMenu, "&Edit");

		// Connect Menu (example)
		wxMenu* connectMenu = new wxMenu();
		connectMenu->Append(1001, "Connect to Server");
		connectMenu->Append(1002, "Disconnect");
		Append(connectMenu, "&Connect");

		Bind(wxEVT_MENU, &Toolbar::openConnectionManager, this, 1001);
	}

	std::function<void(FTPManager*)> onConnectClicked;
	std::function<wxWindow* ()> getParentWindow;

private:
	FTPManager* addConnection(wxWindow* parent, const wxString& ip, int port, const wxString& user, const wxString& pass)
	{
		FTPManager* connection = new FTPManager(ip, port, user, pass); // Start FTP connection

		//wxString msg = connection->getLastError();
		//wxMessageBox(msg, "Connection Status", wxOK | wxICON_INFORMATION, parent);

		return connection;
	}

	void openConnectionManager(wxCommandEvent& event) {
		wxWindow* parent = nullptr;
		if (getParentWindow)
			parent = getParentWindow();

		wxDialog dlg(parent, wxID_ANY, "Connect", wxDefaultPosition, wxSize(320, 200));
		wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);

		// IP
		wxBoxSizer* ipSizer = new wxBoxSizer(wxHORIZONTAL);
		ipSizer->Add(new wxStaticText(&dlg, wxID_ANY, "HOST:"), 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 5);
		wxTextCtrl* ipCtrl = new wxTextCtrl(&dlg, wxID_ANY);
		ipSizer->Add(ipCtrl, 1, wxEXPAND);
		mainSizer->Add(ipSizer, 0, wxALL | wxEXPAND, 10);

		// Port
		wxBoxSizer* portSizer = new wxBoxSizer(wxHORIZONTAL);
		portSizer->Add(new wxStaticText(&dlg, wxID_ANY, "Port:"), 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 5);
		wxSpinCtrl* portCtrl = new wxSpinCtrl(&dlg, wxID_ANY);
		portCtrl->SetRange(1, 65535);
		portCtrl->SetValue(80); // default port
		portSizer->Add(portCtrl, 1, wxEXPAND);
		mainSizer->Add(portSizer, 0, wxALL | wxEXPAND, 10);

		// Username
		wxBoxSizer* userSizer = new wxBoxSizer(wxHORIZONTAL);
		userSizer->Add(new wxStaticText(&dlg, wxID_ANY, "Username:"), 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 5);
		wxTextCtrl* userCtrl = new wxTextCtrl(&dlg, wxID_ANY);
		userSizer->Add(userCtrl, 1, wxEXPAND);
		mainSizer->Add(userSizer, 0, wxALL | wxEXPAND, 10);

		// Password
		wxBoxSizer* passSizer = new wxBoxSizer(wxHORIZONTAL);
		passSizer->Add(new wxStaticText(&dlg, wxID_ANY, "Password:"), 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 5);
		wxTextCtrl* passCtrl = new wxTextCtrl(&dlg, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_PASSWORD);
		passSizer->Add(passCtrl, 1, wxEXPAND);
		mainSizer->Add(passSizer, 0, wxALL | wxEXPAND, 10);

		// OK / Cancel buttons
		mainSizer->Add(dlg.CreateButtonSizer(wxOK | wxCANCEL), 0, wxALL | wxALIGN_CENTER, 10);

		dlg.SetSizerAndFit(mainSizer);

		// FTP Dialog
		if (dlg.ShowModal() == wxID_OK)
		{
			wxString ip = ipCtrl->GetValue();
			int port = portCtrl->GetValue();
			wxString user = userCtrl->GetValue();
			wxString pass = passCtrl->GetValue();

			FTPManager* connection = addConnection(parent, ip, port, user, pass);

			if (!connection->isConnected() && parent) {
				wxMessageBox("Connection failed: " + connection->getLastError(),
					"Connection Status", wxOK | wxICON_ERROR, parent);
			}

			if (onConnectClicked)
				onConnectClicked(connection);
		}
	}
};