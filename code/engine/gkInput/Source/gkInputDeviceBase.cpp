#include "InputStableHeader.h"
#include "gkInputDeviceBase.h"

static BOOL CALLBACK EnumDeviceObjectsCallback(LPCDIDEVICEOBJECTINSTANCE lpddoi, LPVOID pvRef)
{
	std::vector<DIOBJECTDATAFORMAT>*	customFormat = (std::vector<DIOBJECTDATAFORMAT>*) pvRef;

	DIOBJECTDATAFORMAT format;

	format.pguid = 0;
	format.dwOfs = lpddoi->dwOfs;
	format.dwType = lpddoi->dwType;
	format.dwFlags = 0;

	customFormat->push_back(format);

	return DIENUM_CONTINUE;
}


gkInputDevice::gkInputDevice(gkInputManager& input, const TCHAR* deviceName, const GUID& guid):
m_input(input), m_pDevice(0), m_guid(guid), m_pDataFormat(0), m_dwCoopLevel(0),
m_bNeedsPoll(false)
{
	m_enabled = true;
}

gkInputDevice::~gkInputDevice()
{
	TDevSpecIdToSymbolMap::iterator it = m_devSpecIdToSymbol.begin();
	for (; it != m_devSpecIdToSymbol.end(); ++it)
	{
		delete (it->second);
	}
	

	if (m_pDevice)
	{
		Unacquire();
		m_pDevice->Release();
		m_pDevice = 0;
	}
}

LPDIRECTINPUTDEVICE8 gkInputDevice::GetDirectInputDevice() const
{
	return m_pDevice;
}


bool gkInputDevice::Acquire()
{
	if (!m_pDevice) return false;

	HRESULT hr = m_pDevice->Acquire();

	unsigned char maxAcquire = 10;

	while (hr == DIERR_INPUTLOST && maxAcquire > 0)
	{
		hr = m_pDevice->Acquire();
		--maxAcquire;
	}

	if (FAILED(hr) || maxAcquire == 0) return false;

	return true;
}

///////////////////////////////////////////
bool gkInputDevice::Unacquire()
{
	return (m_pDevice && SUCCEEDED(m_pDevice->Unacquire()));
}

bool gkInputDevice::CreateDirectInputDevice(LPCDIDATAFORMAT dataFormat, DWORD coopLevel, DWORD bufSize)
{
	HRESULT hr = GetInputManager().getDirectInput()->CreateDevice(m_guid, &m_pDevice, 0);

	if (FAILED(hr))
	{
		return false;
	}

	// get capabilities
	DIDEVCAPS caps;
	caps.dwSize = sizeof(DIDEVCAPS);
	m_pDevice->GetCapabilities(&caps);

	if (caps.dwFlags & DIDC_POLLEDDEVICE)
	{
		gkLogWarning(_T("Device need poll."));
		m_bNeedsPoll = true;
	}

	if (!dataFormat)
	{
		// build a custom one, here
	}

	hr = m_pDevice->SetDataFormat(dataFormat);
	if (FAILED(hr)) 
	{
		return false;
	}				

	m_pDataFormat	= dataFormat;
	m_dwCoopLevel	= coopLevel;

	hr = m_pDevice->SetCooperativeLevel(GetInputManager().getHwnd(), m_dwCoopLevel);
	if (FAILED(hr)) 
	{
		return false;
	}						

	DIPROPDWORD dipdw = {{sizeof(DIPROPDWORD), sizeof(DIPROPHEADER), 0, DIPH_DEVICE}, bufSize};
	hr = m_pDevice->SetProperty(DIPROP_BUFFERSIZE, &dipdw.diph);	
	if (FAILED(hr)) 
	{
		//gEnv->p("Cannot Set Di Buffer Size\n");
		return false;
	}

	return true;
}


void gkInputDevice::Update(bool bFocus)
{
}

void gkInputDevice::ClearKeyState()
{
	for (TIdToSymbolMap::iterator i = m_idToInfo.begin(); i != m_idToInfo.end(); ++i)
	{
		SInputSymbol* pSymbol = (*i).second;
		if (pSymbol && pSymbol->value > 0.0)
		{
			SInputEvent event;
			event.deviceId = m_deviceId;
			//event.keyName = pSymbol->name;
			event.keyId = pSymbol->keyId;
			event.state = eIS_Released;
			event.value = 0.0f;
			event.pSymbol = pSymbol;
			pSymbol->value = 0.0f;
			pSymbol->state = eIS_Released;
			//m_input.PostInputEvent(event);
		}
	}
}

void gkInputDevice::ClearAnalogKeyState()
{

}

SInputSymbol* gkInputDevice::IdToSymbol(EKeyId id) const
{
	TIdToSymbolMap::const_iterator i = m_idToInfo.find(id);
	if (i != m_idToInfo.end())
		return (*i).second;
	else
		return 0;
}

SInputSymbol* gkInputDevice::DevSpecIdToSymbol(uint32 devSpecId) const
{
	TDevSpecIdToSymbolMap::const_iterator i = m_devSpecIdToSymbol.find(devSpecId);
	if (i != m_devSpecIdToSymbol.end())
		return (*i).second;
	else
		return 0;
}

SInputSymbol* gkInputDevice::MapSymbol(uint32 deviceSpecificId, EKeyId keyId, SInputSymbol::EType type, uint32 user)
{
	SInputSymbol* pSymbol = new SInputSymbol(deviceSpecificId, keyId, type);
	pSymbol->user = user;
	pSymbol->deviceId = m_deviceId;
	m_idToInfo[keyId] = pSymbol;
	m_devSpecIdToSymbol[deviceSpecificId] = pSymbol;

	return pSymbol;
}

//////////////////////////////////////////////////////////////////////////
SInputSymbol* gkInputDevice::LookupSymbol(EKeyId id) const
{
	return IdToSymbol(id);
}

void gkInputDevice::Enable(bool enable)
{
	m_enabled = enable;
}

const char* gkInputDevice::GetKeyName( const SInputEvent& event, bool bGUI/*=0*/ )
{
	return "";
}

void gkInputDevice::SetHwnd(HWND hwnd)
{
	if(m_pDevice)
	{
		m_pDevice->Unacquire();
		m_pDevice->SetCooperativeLevel(hwnd, m_dwCoopLevel);
		m_pDevice->Acquire();
	}
	
}

