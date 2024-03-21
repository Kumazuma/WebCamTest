#include "wx/camera/camera.h"
#if defined(__WINDOWS__)
#include "wx/camera/msw/camera.h"
#elif defined(__WINDOWS__)
#include "../linux/camera.h"
#endif

wxDEFINE_EVENT(wxEVT_CAMERA_SAMPLE_RECEIVED, wxCameraEvent);

struct wxCameraEvent::Buffer
{
	Buffer(wxByte* ptr, wxUint32 len)
		: ptr(ptr)
		, len(len)
	{

	}
	~Buffer()
	{
		delete[] ptr;
	}

	wxByte* ptr;
	wxUint32 len;
};

wxCamera::wxCamera()
	: m_impl(new wxCameraImplNative())
{

}

wxCamera::wxCamera(const wxString& name)
	: m_impl(new wxCameraImplNative())
{
	m_impl->Open(name);
}

wxCamera::~wxCamera()
{
	delete m_impl;
	m_impl = nullptr;
}

wxCameraEvent::wxCameraEvent(wxEventType eventType, int winId, wxByte* ptr, wxUint32 len) : wxEvent(winId, eventType)
, m_buffer(std::make_shared<Buffer>(new wxByte[((len + 15) / 16) * 16], len))
{
	memcpy(m_buffer->ptr, ptr, len);
}

const wxByte* wxCameraEvent::GetBufferPointer()
{
	return m_buffer->ptr;
}

wxUint32 wxCameraEvent::GetBufferLength()
{
	return m_buffer->len;
}

wxEvent* wxCameraEvent::Clone() const
{
	return new wxCameraEvent(*this);
}
