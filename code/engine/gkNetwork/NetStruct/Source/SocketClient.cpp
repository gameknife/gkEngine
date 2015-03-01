#include "NetWorkStableHeader.h"
#include "SocketClient.h"
#include "serializer.h"
#include "MemoryBuffer.h"

/////////////////////////////////////////////////////////////////////////////
// CSocketClient

CSocketClient::CSocketClient()
{
	m_hSocket = INVALID_SOCKET;
	m_wstrName = _T("");
	m_nLength=0;
	memset(m_yBuffer,0,sizeof(m_yBuffer));
	m_bConnected=FALSE;
}

CSocketClient::~CSocketClient()
{
	if(m_hSocket!=INVALID_SOCKET)
		Close();
}

/////////////////////////////////////////////////////////////////////////////
// CSocketClient member functions

void CSocketClient::OnReceive(int nErrorCode) 
{
	// TODO: Add your specialized code here and/or call the base class
	// TODO： 通知客户端接到消息了

	m_nLength=Receive(m_yBuffer,sizeof(m_yBuffer),0);
	
	
	// 解析数据包
	CMemoryBuffer memoryBuffer(m_yBuffer, sizeof(m_yBuffer));
	CSerializer arc;
	if(!arc.BeginReading(&memoryBuffer, false))
	{
		OutputDebugString(_T("ClientSocket: 网络包解析失败！"));
		return;
	}
	WORD wMsgID = -1;

	// 循环取信息
	while(1)
	{
		arc >> wMsgID;	
		// 通知网络层收到消息 [11/22/2010 Kaiming-Laptop]
		if( !this->OnMsgFromServer(wMsgID, arc) )
			break;
	}


	// 收到信息以后就清空
	m_nLength=0;
	memset(m_yBuffer,0,sizeof(m_yBuffer));
	//AsyncSelect(FD_READ);
	CSocket::OnReceive(nErrorCode);
}

void CSocketClient::OnSend(int nErrorCode) {
	Send(m_yBuffer,m_nLength,0); 
	
	m_nLength=0;
	memset(m_yBuffer,0,sizeof(m_yBuffer));
	//继续提请一个“读”的网络事件，接收Server消息AsyncSelect(FD_READ);
	//AsyncSelect(FD_READ);
	CSocket::OnSend(nErrorCode);
}

void CSocketClient::OnConnect(int nErrorCode) {
	if (nErrorCode==0) {
		m_bConnected=TRUE;
// 		CCSockClientApp* pApp=(CCSockClientApp*)AfxGetApp();
// 		CCSockClientDlg* pDlg=(CCSockClientDlg*)pApp- >m_pMainWnd;
// 		memcpy(m_szBuffer,"Connected to ",13);
// 		strncat(m_szBuffer,pDlg- >m_szServerAdr,
// 			sizeof(pDlg- >m_szServerAdr));
// 		pDlg- >m_MSGS.InsertString(0,m_szBuffer);
		//AsyncSelect(FD_READ);
		////提请一个“读”的网络事件，准备接收 }
		CSocket::OnConnect(nErrorCode);
	}
}
