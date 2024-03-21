// WebCamTest.cpp : 애플리케이션의 진입점을 정의합니다.
//

#include "WebCamTest.h"
#include "MyFrameBase.h"
#include <wx/camera/camera.h>

wxIMPLEMENT_APP(WebCamTestApp);

bool WebCamTestApp::OnInit()
{
	if (!wxApp::OnInit())
		return false;
	
	m_frame = new MyFrameBase(nullptr, wxID_ANY, wxS("MyFrameBase"));
	wxArrayString deviceList;
	wxCamera::GetDeviceNames(deviceList);
	wxWindow* wnd = wxWindow::FindWindowById(MyFrameBase::ID_CHOICE_DEVICE, m_frame);
	wxASSERT(wnd != nullptr);
	wxChoice* choiceDevice = wxStaticCast(wnd, wxChoice);
	choiceDevice->Append(deviceList);

	m_frame->Show();
	return true;
}

void WebCamTestApp::OnChoiceDeviceSelectionChanged(wxCommandEvent& evt)
{

}
