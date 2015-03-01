#include "NetWorkStableHeader.h"
#include "gkNetworkLayer.h"
#include "Memorybuffer.h"
#include "serializer.h"
#include <iostream>  
#include <sstream> 
#include <time.inl>

#include "gkMemoryLeakDetecter.h"
//-----------------------------------------------------------------------
gkNetworkLayer::gkNetworkLayer():m_bIsLogin(false)
	,m_funcOnMsgFromServer(NULL)
{

}
//-----------------------------------------------------------------------
gkNetworkLayer::~gkNetworkLayer()
{

}
//-----------------------------------------------------------------------
bool gkNetworkLayer::InitSocket()
{
	// 网络层初始化
	if (!AfxWinInit(::GetModuleHandle(NULL), NULL, ::GetCommandLine(), 0)) {
		OutputDebugString(_T("Fatal Error: MFC initialization failed"));
	}

	if   (!AfxSocketInit(NULL))
	{
		OutputDebugString(_T("初始化socket出错\n"));
		return   FALSE;
	}
	return TRUE; // 除非设置了控件的焦点，否则返回 TRUE
}
//-----------------------------------------------------------------------
bool gkNetworkLayer::ClearSocket()
{
	return TRUE;
}

//-----------------------------------------------------------------------
bool gkNetworkLayer::OnMsgFromServer( WORD wMessageID, CSerializer& serializer )
{
	bool bResult = false;

	if(m_funcOnMsgFromServer)
		m_funcOnMsgFromServer(wMessageID, serializer);

	return bResult;
}
//-----------------------------------------------------------------------
bool gkNetworkLayer::ConnectServer( gkStdString wstrIPAddress, int nPort )
{
	if( IsConnected() )
		return FALSE;

	if (!Create())
	{
		Close();
		MessageBox(NULL, _T("gkNetLayer: 创建Socket出错。"),NULL,MB_OK);
		OutputDebugString(_T("gkNetLayer:网络创建错误\n"));
		return FALSE;
	}

	if (!Connect(wstrIPAddress.c_str(),nPort))
	{
		Close();
		MessageBox(NULL, _T("gkNetLayer: 连接不到服务器，请检查NetConfig.cfg文件。"),NULL,MB_OK);
		OutputDebugString(_T("gkNetLayer:网络连接错误\n"));
		return FALSE;
	}

	m_bConnected = true;
	m_wstrName = _T("LocalClient");
	return TRUE;
}
//-----------------------------------------------------------------------
// template<typename T>
// bool gkNetworkLayer::SendMsgToServer( WORD wMsgID, T& msgInfo )
// {
// 	if (IsConnected())
// 	{
// 		CMemoryBuffer memoryBuffer;
// 		CSerializer serializer;
// 		serializer.BeginWriting(&memoryBuffer, false);
// 
// 		serializer << wMsgID;
// 		msgInfo.Serialize(serializer);
// 
// 		serializer.EndWriting();
// 		return Send(reinterpret_cast<const BYTE*>(memoryBuffer.GetData()), memoryBuffer.GetLength());
// 	}
// 	return FALSE;
//}

// bool gkNetworkLayer::SendMsgToServer( WORD wMsgID, void* msgInfo )
// {
// 	CMemoryBuffer memoryBuffer;
// 	CSerializer serializer;
// 	serializer.BeginWriting(&memoryBuffer, false);
// 
// 	serializer << wMsgID;
// 	msgInfo.Serialize(serializer);
// 
// 	serializer.EndWriting();
// 	return Send(reinterpret_cast<const BYTE*>(memoryBuffer.GetData()), memoryBuffer.GetLength());
// }

bool gkNetworkLayer::SendMsgToServer( const void* lpBuf, int nBuflen )
{
	return Send(lpBuf, nBuflen);
}
void gkNetworkLayer::SetMsgCallBack( LPGKCALLBACKONMSGFROMSERVER pCallback )
{
	m_funcOnMsgFromServer = pCallback;
}