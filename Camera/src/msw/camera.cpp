#include <wx/wx.h>
#include <wx/camera/camera.h>
#include <wx/camera/msw/camera.h>
#include <mfapi.h>
#include <mfcaptureengine.h>
#include <mfreadwrite.h>
#include <mfidl.h>

#pragma comment(lib, "Mfuuid.lib")
#pragma comment(lib, "Mf.lib")
#pragma comment(lib, "Mfplat.lib")
#pragma comment(lib, "Mfreadwrite.lib")

class wxCameraImplMSW::Listener : public IMFSourceReaderCallback {
	friend class wxCameraImplMSW;
public:
	Listener(wxCameraImplMSW* impl);
	// IMFSourceReaderCallback
	HRESULT QueryInterface(const IID& riid, void** ppvObject) override;
	ULONG AddRef() override;
	ULONG Release() override;
	HRESULT OnReadSample(HRESULT hrStatus, DWORD dwStreamIndex, DWORD dwStreamFlags, LONGLONG llTimestamp,
		IMFSample* pSample) override;
	HRESULT OnFlush(DWORD dwStreamIndex) override;
	HRESULT OnEvent(DWORD dwStreamIndex, IMFMediaEvent* pEvent) override;

private:
	wxCameraImplMSW* m_impl;
	std::atomic_ulong m_refCount;
	std::mutex m_mutex;
};

void wxCamera::GetDeviceNames(wxArrayString& names) {
	IMFAttributes* pAttributes = nullptr;
	HRESULT hr = MFCreateAttributes(&pAttributes, 2);
	if (FAILED(hr))
		return;

	hr = pAttributes->SetGUID(MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE, MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_VIDCAP_GUID);
	if (FAILED(hr))
		return;

	UINT32 count;
	IMFActivate** deviceList;
	hr = MFEnumDeviceSources(pAttributes, &deviceList, &count);
	pAttributes->Release();
	if (FAILED(hr))
		return;

	IMFActivate** it = deviceList;
	IMFActivate** const end = deviceList + count;
	for (; it != end; ++it)
	{
		wchar_t name[512];
		UINT32 length;
		(*it)->GetString(MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_VIDCAP_SYMBOLIC_LINK, name, 512, &length);
		names.Add(wxString{ name, length });
		(*it)->Release();
	}

	CoTaskMemFree(deviceList);
}

wxCameraImplMSW::wxCameraImplMSW()
	: m_pMediaSource(nullptr)
	, m_pReader()
	, m_win()
	, m_id(wxID_ANY)
	, m_listener() {
}

wxCameraImplMSW::~wxCameraImplMSW() {
	wxCameraImplMSW::StopCapture();
	m_pMediaSource = nullptr;
}

bool wxCameraImplMSW::Open(const wxString& name) {
	if (IsOpened())
		return false; // Already opened

	IMFAttributes* pAttributes{};
	HRESULT hr = MFCreateAttributes(&pAttributes, 2);
	if (FAILED(hr))
		return false;

	hr = pAttributes->SetGUID(MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE, MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_VIDCAP_GUID);
	if (FAILED(hr)) {
		pAttributes->Release();
		return false;
	}

	auto wstr = name.ToStdWstring();
	hr = pAttributes->SetString(MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_VIDCAP_SYMBOLIC_LINK, wstr.c_str());
	if (FAILED(hr)) {
		pAttributes->Release();
		return false;
	}

	hr = MFCreateDeviceSource(pAttributes, &m_pMediaSource);
	pAttributes->Release();
	if (FAILED(hr))
		return false;

	return true;
}

void wxCameraImplMSW::GetModes(wxVector<wxCameraMode>& modes) {
	IMFPresentationDescriptor* pPD = nullptr;
	IMFStreamDescriptor* pSD = nullptr;
	IMFMediaTypeHandler* pHandler = nullptr;
	IMFMediaType* pType = nullptr;
	do
	{
		HRESULT hr = m_pMediaSource->CreatePresentationDescriptor(&pPD);
		if (FAILED(hr))
			break;

		BOOL fSelected;
		hr = pPD->GetStreamDescriptorByIndex(0, &fSelected, &pSD);
		if (FAILED(hr))
			break;


		hr = pSD->GetMediaTypeHandler(&pHandler);
		if (FAILED(hr))
			break;

		DWORD cTypes = 0;
		
		hr = pHandler->GetMediaTypeCount(&cTypes);
		if (FAILED(hr))
			break;

		for (DWORD i = 0; i < cTypes; i++)
		{
			if (pType != nullptr)
				pType->Release();

			pType = nullptr;
			hr = pHandler->GetMediaTypeByIndex(i, &pType);
			if (FAILED(hr))
				break;

			GUID subType;
			hr = pType->GetGUID(MF_MT_SUBTYPE, &subType);
			if (FAILED(hr))
				break;

			wxCameraMode op{};
			if (subType == MFVideoFormat_NV12) {
				op.format = wxCameraPixelFormat::YUV420_NV12;
			}
			else if (subType == MFVideoFormat_NV21) {
				op.format = wxCameraPixelFormat::YUV420_NV21;
			}
			else {
				continue;
			}

			PROPVARIANT var;
			pType->GetItem(MF_MT_FRAME_SIZE, &var);
			UINT32 uHigh = 0, uLow = 0;
			Unpack2UINT32AsUINT64(var.uhVal.QuadPart, &uHigh, &uLow);
			op.resolution.Set(uHigh, uLow);

			UINT64 frameRate;
			pType->GetUINT64(MF_MT_FRAME_RATE, &frameRate);
			op.frameRateDen = LO32(frameRate);
			op.frameRateNum = HI32(frameRate);
			modes.push_back(op);
		}

	} while (false);

	if (pPD != nullptr)
		pPD->Release();

	if (pSD != nullptr)
		pSD->Release();

	if (pHandler != nullptr)
		pHandler->Release();

	if (pType != nullptr)
		pType->Release();
}

bool wxCameraImplMSW::IsOpened() const {
	return m_pMediaSource != nullptr;
}

bool wxCameraImplMSW::StartCapture(wxEvtHandler* win, const wxCameraMode& mode, wxWindowID id) {
	if (m_listener != nullptr) {
		m_listener->m_mutex.lock();
		m_listener->m_impl = nullptr;

		m_listener->m_mutex.unlock();

		m_pReader->Flush(MF_SOURCE_READER_ANY_STREAM);
		m_pReader->Release();
		m_pReader = nullptr;
		m_pMediaSource->Release();
		m_win = nullptr;
		m_listener->Release();
		m_listener = nullptr;
	}

	if (!SelectMode(mode))
		return false;

	IMFSourceReader* pNewReader;
	IMFAttributes* pAttributes;
	MFCreateAttributes(&pAttributes, 1);
	Listener* listener = new Listener(this);

	pAttributes->SetUnknown(MF_SOURCE_READER_ASYNC_CALLBACK, listener);
	MFCreateSourceReaderFromMediaSource(m_pMediaSource, pAttributes, &pNewReader);
	if (pNewReader == nullptr)
	{
		listener->Release();
		pAttributes->Release();
		return false;
	}

	m_win = win;
	pAttributes->Release();
	m_listener = listener;
	m_pReader = pNewReader;
	m_currentMode = mode;
	m_pReader->ReadSample(MF_SOURCE_READER_FIRST_VIDEO_STREAM, 0, nullptr, nullptr, nullptr, nullptr);
	return true;
}

void wxCameraImplMSW::StopCapture() {
	if (m_listener == nullptr)
		return;

	m_pReader->Flush(MF_SOURCE_READER_ANY_STREAM);
	m_pReader->Release();
	m_pReader = nullptr;
	m_pMediaSource->Release();
	
	m_listener->m_mutex.lock();
	m_listener->m_impl = nullptr;
	m_listener->m_mutex.unlock();

	m_win = nullptr;
	m_listener->Release();
	m_listener = nullptr;
}

wxCameraMode wxCameraImplMSW::GetCurrentMode()
{
	return m_currentMode;
}

bool wxCameraImplMSW::SelectMode(const wxCameraMode& mode)
{
	bool ret = false;
	IMFPresentationDescriptor* pPD = nullptr;
	IMFStreamDescriptor* pSD = nullptr;
	IMFMediaTypeHandler* pHandler = nullptr;
	IMFMediaType* pType = nullptr;
	do
	{
		HRESULT hr = m_pMediaSource->CreatePresentationDescriptor(&pPD);
		if (FAILED(hr))
			break;

		BOOL fSelected;
		hr = pPD->GetStreamDescriptorByIndex(0, &fSelected, &pSD);
		if (FAILED(hr))
			break;


		hr = pSD->GetMediaTypeHandler(&pHandler);
		if (FAILED(hr))
			break;

		DWORD cTypes;
		cTypes = 0;
		hr = pHandler->GetMediaTypeCount(&cTypes);
		if (FAILED(hr))
			break;

		for (DWORD i = 0; i < cTypes; i++)
		{
			if (pType != nullptr)
				pType->Release();

			pType = nullptr;
			hr = pHandler->GetMediaTypeByIndex(i, &pType);
			if (FAILED(hr))
				break;

			GUID subType;
			hr = pType->GetGUID(MF_MT_SUBTYPE, &subType);
			if (FAILED(hr))
				break;

			if ((subType == MFVideoFormat_NV12 && mode.format != wxCameraPixelFormat::YUV420_NV12)
				|| (subType == MFVideoFormat_NV21 && mode.format != wxCameraPixelFormat::YUV420_NV21)
				|| (subType == MFVideoFormat_YUY2 && mode.format != wxCameraPixelFormat::YUV422_YUYV)
				|| (subType == MFVideoFormat_RGB24 && mode.format != wxCameraPixelFormat::RGB24)
				|| (subType == MFVideoFormat_RGB32 && mode.format != wxCameraPixelFormat::RGB32))
			{
				continue;
			}

			PROPVARIANT var;
			pType->GetItem(MF_MT_FRAME_SIZE, &var);
			UINT32 uHigh = 0, uLow = 0;
			Unpack2UINT32AsUINT64(var.uhVal.QuadPart, &uHigh, &uLow);
			if (mode.resolution.GetWidth() != uHigh || mode.resolution.GetHeight() != uLow)
			{
				continue;
			}

			UINT64 frameRate;
			pType->GetUINT64(MF_MT_FRAME_RATE, &frameRate);
			if (LO32(frameRate) != mode.frameRateDen || HI32(frameRate) != mode.frameRateNum)
			{
				continue;
			}

			hr = pHandler->SetCurrentMediaType(pType);
			if (FAILED(hr))
				break;

			ret = true;
			break;
		}

	} while (false);

	if (pPD != nullptr)
		pPD->Release();

	if (pSD != nullptr)
		pSD->Release();

	if (pHandler != nullptr)
		pHandler->Release();

	if (pType != nullptr)
		pType->Release();

	return ret;
}

wxCameraImplMSW::Listener::Listener(wxCameraImplMSW* impl)
	: m_impl{ impl }
	, m_refCount{ 1 }
{
}

HRESULT wxCameraImplMSW::Listener::QueryInterface(const IID& riid, void** ppvObject)
{
	if (riid == __uuidof(IUnknown))
	{
		*ppvObject = static_cast<IUnknown*>(this);
	}
	else if (riid == __uuidof(IMFSourceReaderCallback))
	{
		*ppvObject = static_cast<IMFSourceReaderCallback*>(this);
	}
	else
	{
		return E_NOINTERFACE;
	}

	return S_OK;
}

ULONG wxCameraImplMSW::Listener::AddRef()
{
	ULONG refCount = m_refCount.fetch_add(1);
	return refCount + 1;
}

ULONG wxCameraImplMSW::Listener::Release()
{
	ULONG refCount = m_refCount.fetch_sub(1);
	if (refCount == 1)
	{
		delete this;
	}

	return refCount - 1;
}

HRESULT wxCameraImplMSW::Listener::OnReadSample(HRESULT hrStatus, DWORD dwStreamIndex, DWORD dwStreamFlags,
	LONGLONG llTimestamp, IMFSample* pSample)
{
	if (FAILED(hrStatus))
	{
		return S_OK;
	}

	std::lock_guard<std::mutex> guard{ m_mutex };
	if (m_impl == nullptr)
	{
		return S_OK;
	}

	if (m_impl->m_win == nullptr)
	{
		return S_OK;
	}

	if (m_impl->m_listener != this)
	{
		return S_OK;
	}

	if (pSample != nullptr)
	{
		IMFMediaBuffer* pBuffer;
		pSample->GetBufferByIndex(0, &pBuffer);
		BYTE* ptr;
		DWORD len;
		pBuffer->GetCurrentLength(&len);
		pBuffer->Lock(&ptr, nullptr, nullptr);
		wxCameraEvent event{ wxEVT_CAMERA_SAMPLE_RECEIVED, m_impl->m_id, ptr, len };
		pBuffer->Unlock();
		pBuffer->Release();
		m_impl->m_win->SafelyProcessEvent(event);
	}

	m_impl->m_pReader->ReadSample(MF_SOURCE_READER_FIRST_VIDEO_STREAM, 0, nullptr, nullptr, nullptr, nullptr);
	return S_OK;
}

HRESULT wxCameraImplMSW::Listener::OnFlush(DWORD dwStreamIndex)
{
	return S_OK;
}

HRESULT wxCameraImplMSW::Listener::OnEvent(DWORD dwStreamIndex, IMFMediaEvent* pEvent)
{
	// TODO: Not implmented yet: Event Handling
	return S_OK;
}
