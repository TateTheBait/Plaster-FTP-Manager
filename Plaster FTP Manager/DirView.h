#pragma once

#include <wx/wx.h>
#include <wx/listctrl.h>
#include <wx/artprov.h>
#include <wx/dir.h>
#include <wx/filename.h>


class DirView : public wxListCtrl {
public:
    DirView(wxWindow* parent, int type) : wxListCtrl(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLC_REPORT | wxLC_SINGLE_SEL | wxSUNKEN_BORDER) {
        viewType = type; // Local Device or FTP
        
        this->InsertColumn(0, "Name", wxLIST_FORMAT_LEFT, 200);
        this->InsertColumn(1, "Size", wxLIST_FORMAT_RIGHT, 100);
        this->InsertColumn(2, "Extension", wxLIST_FORMAT_LEFT, 100);
        this->InsertColumn(3, "Modified", wxLIST_FORMAT_LEFT, 150);

        // Icon List
        wxImageList* imageList = new wxImageList(16, 16, true);

        wxIcon fileIcon = wxArtProvider::GetIcon(wxART_NORMAL_FILE, wxART_OTHER, wxSize(16, 16));
        int fileIndex = imageList->Add(fileIcon);

        wxIcon folderIcon = wxArtProvider::GetIcon(wxART_FOLDER, wxART_OTHER, wxSize(16, 16));
        int folderIndex = imageList->Add(folderIcon);

        this->AssignImageList(imageList, wxIMAGE_LIST_SMALL);
		this->Bind(wxEVT_LIST_ITEM_ACTIVATED, &DirView::OnFileActivated, this);
    };

	void UpdateFileList(const wxString& path) {
		if (viewType == 0) {
			wxString currentLocalPath = path;
			this->DeleteAllItems(); // Destroy all list items from last directory

			if (!path.EndsWith("\\") || path != "C:\\") {
				long idx = this->InsertItem(0, "..");
				this->SetItem(idx, 2, "Directory");
			} // If an item is a directory, set the type to Directory


			wxDir dir(path); // Dir Created for easier listing and file handling
			if (!dir.IsOpened()) {
				return;
			}

			wxString fileName; // Init a variable for a current iterated file

			bool cont = dir.GetFirst(&fileName, wxEmptyString, wxDIR_FILES | wxDIR_DIRS); // Get first file in directory
			long index = (path != "C:\\") ? 1 : 0;


			while (cont) {
				wxFileName fn(path + "\\" + fileName);

				int iconIdx = fn.GetExt().length() > 0 ? 0 : 1; // If extension length is greater than 0, the icon should be a file, otherwise its a dir
				long itemIndex = this->InsertItem(this->GetItemCount(), fn.GetFullName(), iconIdx);

				if (fn.GetExt().length() > 0 && fileName != "..") { // if extension is file
					this->SetItem(itemIndex, 1, wxString::Format("%lld KB", (wxULongLong_t)(fn.GetSize().GetValue() / 1024))); // Set File Size!
					this->SetItem(itemIndex, 2, fn.GetExt()); // Set File Extension
				}
				else if (fileName == "") {
					this->SetItem(itemIndex, 2, "Parent Directory");
				} else {
					this->SetItem(itemIndex, 2, "Directory"); // If its a directory, set extension to Directory
				}

				if (!fn.Exists() || (!fn.IsDir() && !fn.IsOk())) // Permission bypass check
				{
					this->SetItem(itemIndex, 3, "Access Denied"); // Set Date to Denied
					cont = dir.GetNext(&fileName);
					continue;
				}

				wxDateTime modTime;
				if (fn.GetTimes(nullptr, &modTime, nullptr))  // returns false on access denied
				{
					this->SetItem(itemIndex, 3, modTime.FormatISODate() + " " + modTime.FormatISOTime());
				}
				else
				{
					this->SetItem(itemIndex, 3, "N/A");   // skip or mark as inaccessible
				}

				cont = dir.GetNext(&fileName); // Continue looping through files in directory
			}
		}
		else if (viewType == 1) {

		}
	}

private:
    int viewType = 0;
	wxString lastPath;

	void OnFileActivated(wxListEvent& event)
	{
		long idx = event.GetIndex();
		wxString name = this->GetItemText(idx);
		wxString type = this->GetItemText(idx, 2);

		wxString newPath;

		if (name == "" || type == "Parent Directory") {
			wxFileName fn(lastPath);
			fn.RemoveLastDir();
			newPath = fn.GetFullPath();
		}
		else if (type == "Directory") {
			newPath = lastPath + "\\" + name;
		}

		if (!newPath.IsEmpty()) {
			lastPath = newPath;
			UpdateFileList(newPath);
			return;
		}

		wxString fullPath = wxFileName(lastPath, name).GetFullPath();
		wxLaunchDefaultApplication(fullPath);
	}
};