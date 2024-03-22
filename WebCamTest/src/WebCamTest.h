// WebCamTest.h: 표준 시스템 포함 파일
// 또는 프로젝트 특정 포함 파일이 들어 있는 포함 파일입니다.

#pragma once

#include <wx/wx.h>
#include <wx/camera/camera.h>

class MyFrameBase;
struct wxCameraMode;
class WebCamTestApp: public wxApp {
	DECLARE_EVENT_TABLE();
public:
	bool OnInit() override;

protected:
	void OnChoiceDeviceSelectionChanged(wxCommandEvent& evt);
	void OnTglReadClicked(wxCommandEvent& evt);

	MyFrameBase* m_frame;
	wxCamera m_camera;
	wxVector<wxCameraMode> m_modes;
};

wxDECLARE_APP(WebCamTestApp);

// TODO: 여기서 프로그램에 필요한 추가 헤더를 참조합니다.
