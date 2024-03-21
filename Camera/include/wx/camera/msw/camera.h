#pragma once
#include "wx/camera/camera.h"
#include <mfidl.h>
#include <mfreadwrite.h>
#include <atomic>
#include <mutex>

class wxCameraImplMSW final
	: public wxCameraImpl {
	class Listener;
public:
	wxCameraImplMSW();
	~wxCameraImplMSW() override;
	bool Open(const wxString& name) override;
	void GetModes(wxVector<wxCameraMode>& modes) override;
	bool IsOpened() const override;
	bool StartCapture(wxEvtHandler* win, const wxCameraMode& mode, wxWindowID id) override;
	void StopCapture() override;
	wxCameraMode GetCurrentMode() override;
	bool SelectMode(const wxCameraMode& mode);
private:
	wxWindowID m_id;
	wxCameraMode m_currentMode;
	IMFMediaSource* m_pMediaSource;
	IMFSourceReader* m_pReader;
	Listener* m_listener;
	wxEvtHandler* m_win;
};
