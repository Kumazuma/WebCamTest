// WebCamTest.cpp : 애플리케이션의 진입점을 정의합니다.
//

#include "WebCamTest.h"
#include "MyFrameBase.h"
#include <wx/camera/camera.h>

wxIMPLEMENT_APP(WebCamTestApp);

BEGIN_EVENT_TABLE(WebCamTestApp, wxApp)
	EVT_CHOICE(MyFrameBase::ID_CHOICE_DEVICE, WebCamTestApp::OnChoiceDeviceSelectionChanged)
	EVT_TOGGLEBUTTON(MyFrameBase::ID_TGL_READ, WebCamTestApp::OnTglReadClicked)
END_EVENT_TABLE()

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
	m_camera.Open(evt.GetString());
	wxVector<wxCameraMode> modes;
	m_camera.GetModes(modes);
	wxWindow* wnd = wxWindow::FindWindowById(MyFrameBase::ID_CHOICE_MODE, m_frame);
	wxASSERT(wnd != nullptr);
	wxChoice* choiceMode = wxStaticCast(wnd, wxChoice);
	choiceMode->Clear();
	for(auto& mode: modes)
	{
		wxString modeStr;
		modeStr << mode.resolution.GetWidth() << wxS("x") << mode.resolution.GetHeight() << wxS(" ");
		switch(mode.format)
		{
		case wxCameraPixelFormat::RGB24:
			modeStr << wxS("RGB24");
			break;

		case wxCameraPixelFormat::RGB32:
			modeStr << wxS("RGB32");
			break;

		case wxCameraPixelFormat::YUV420_NV12:
			modeStr << wxS("NV12");
			break;

		case wxCameraPixelFormat::YUV420_NV21:
			modeStr << wxS("NV21");
			break;

		case wxCameraPixelFormat::YUV422_UYVY:
			modeStr << wxS("UYVY");
			break;

		case wxCameraPixelFormat::YUV422_YUYV:
			modeStr << wxS("YUYV");
			break;
		}

		modeStr << " " << (mode.frameRateNum / mode.frameRateDen) << wxS(" Hz");
		choiceMode->AppendString(modeStr);
	}

	m_modes = modes;

}

void WebCamTestApp::OnTglReadClicked(wxCommandEvent& evt)
{
	if(evt.IsChecked())
	{
		if(m_camera.IsOpened())
		{
			wxWindow* wnd = wxWindow::FindWindowById(MyFrameBase::ID_CHOICE_MODE, m_frame);
			wxASSERT(wnd != nullptr);
			wxChoice* choiceMode = wxStaticCast(wnd, wxChoice);
			wxWindow* wnd2 = wxWindow::FindWindowById(MyFrameBase::ID_TGL_READ, m_frame);
			wxToggleButton* tglBtn = wxStaticCast(wnd2, wxToggleButton);
			if(!m_camera.StartCapture(this, m_modes[choiceMode->GetSelection()]))
			{
				tglBtn->SetValue(false);
			}
		}
	}
	else
	{
		m_camera.StopCapture();
	}
}
