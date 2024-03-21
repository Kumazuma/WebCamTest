#ifndef __WX_PLUGIN_CAMERA_H__
#define __WX_PLUGIN_CAMERA_H__

#include <wx/wx.h>

enum class wxCameraPixelFormat
{
	UNKNOWN = 0,
	YUV422_UYVY,
	YUV422_YUYV,
	YUV420_NV12,
	YUV420_NV21,
	RGB24,
	RGB32
};

enum class wxCameraColorSpaceKind
{
	UNKOWN,
};

struct wxCameraMode
{
	wxCameraPixelFormat format;
	wxSize resolution;
	wxUint32 frameRateNum;
	wxUint32 frameRateDen;
};

class wxCameraImpl : public wxEvtHandler
{
public:
	virtual ~wxCameraImpl() = default;
	virtual bool Open(const wxString& name) = 0;
	virtual void GetModes(wxVector<wxCameraMode>& modes) = 0;
	virtual bool IsOpened() const = 0;
	virtual bool StartCapture(wxEvtHandler* win, const wxCameraMode& mode, wxWindowID id) = 0;
	virtual void StopCapture() = 0;
	virtual wxCameraMode GetCurrentMode() = 0;
};

#if defined(__WINDOWS__)
#define wxCameraImplNative wxCameraImplMSW
#elif defined(__LINUX__)
#define wxCameraImplNative wxCameraImplLinux
#endif

class wxCamera final
{
public:
	wxCamera();
	wxCamera(const wxString& name);
	wxCamera(const wxCamera&) = delete;
	~wxCamera();
	bool Open(const wxString& name) { return m_impl->Open(name); }
	void GetModes(wxVector<wxCameraMode>& modes) { m_impl->GetModes(modes); }
	bool IsOpened() const { return m_impl->IsOpened(); }
	bool StartCapture(wxEvtHandler* win, const wxCameraMode& mode, wxWindowID id = wxID_ANY) { return m_impl->StartCapture(win, mode, id); }
	void StopCapture() { return m_impl->StopCapture(); }
	wxCameraMode GetCurrentMode() { return m_impl->GetCurrentMode(); }

public:
	static void GetDeviceNames(wxArrayString& names);

private:
	wxCameraImpl* m_impl;
};

class wxCameraEvent : public wxEvent
{
	struct Buffer;
public:
	wxCameraEvent(wxEventType eventType, int winId, wxByte* ptr, wxUint32 len);
	const wxByte* GetBufferPointer();
	wxUint32 GetBufferLength();
	wxEvent* Clone() const override;
private:
	std::shared_ptr<Buffer> m_buffer;
};

wxDECLARE_EVENT(wxEVT_CAMERA_SAMPLE_RECEIVED, wxCameraEvent);

#endif//__WX_PLUGIN_CAMERA_H__