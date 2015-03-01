#include "InputStableHeader.h"
#include "gkKinect.h"

//////////////////////////////////////////////////////////////////////////
gkKinect::gkKinect( gkInputManager& input ):
gkInputDevice(input, _T("kinect"), GUID())
{
	m_deviceId = eDI_Kinect;
}

//////////////////////////////////////////////////////////////////////////
gkKinect::~gkKinect()
{
	kinectInterface.Nui_UnInit();
}

//////////////////////////////////////////////////////////////////////////
bool gkKinect::Init()
{
	// Clean state the class
	kinectInterface.Nui_Zero();

	// Bind application window handle
	kinectInterface.m_hWnd= GetInputManager().getHwnd();

	// Initialize and start NUI processing
	kinectInterface.Nui_Init();

	//throw std::exception("The method or operation is not implemented.");
	return true;
}
//////////////////////////////////////////////////////////////////////////
void gkKinect::Update( bool bFocus )
{
	//throw std::exception("The method or operation is not implemented.");
}
//////////////////////////////////////////////////////////////////////////
bool gkKinect::SetExclusiveMode( bool value )
{
	//throw std::exception("The method or operation is not implemented.");
	return true;
}
