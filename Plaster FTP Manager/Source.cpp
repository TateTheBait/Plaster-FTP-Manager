#define _CRT_SECURE_NO_WARNINGS
#include "uiManager.h"
#include <wx/app.h>

uiManager* ui = nullptr;

bool MyApp::OnInit() {
	SetAppearance(Appearance::Dark);
	uiManager* frame = new uiManager("Plaster FTP Manager");
	ui = frame;
	frame->Show(true);
	return true;
}


wxIMPLEMENT_APP(MyApp);