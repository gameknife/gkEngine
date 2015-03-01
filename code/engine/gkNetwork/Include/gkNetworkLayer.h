//////////////////////////////////////////////////////////////////////////
/*
Copyright (c) 2011-2015 Kaiming Yi
	
	Permission is hereby granted, free of charge, to any person obtaining a copy
	of this software and associated documentation files (the "Software"), to deal
	in the Software without restriction, including without limitation the rights
	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
	copies of the Software, and to permit persons to whom the Software is
	furnished to do so, subject to the following conditions:
	
	The above copyright notice and this permission notice shall be included in
	all copies or substantial portions of the Software.
	
	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
	THE SOFTWARE.
	
*/
//////////////////////////////////////////////////////////////////////////



//////////////////////////////////////////////////////////////////////////
//
// Name:   	gkNetLayer.h
// Desc:	派生自network.dll的一个网络层
// 描述:	
// 
// Author:  Kaiming-Desktop	 
// Date:	11/21/2010 	
// 
//////////////////////////////////////////////////////////////////////////

#pragma once
#include "INetworkLayer.h"
#include "SocketClient.h"

class GAMEKNIFENETWORK_API gkNetworkLayer : public INetworkLayer, public CSocketClient
{
private:
	bool m_bIsLogin;
	LPGKCALLBACKONMSGFROMSERVER m_funcOnMsgFromServer;

public:
	gkNetworkLayer();
	virtual ~gkNetworkLayer();
	bool InitSocket();
	bool ClearSocket();
	bool ConnectServer(gkStdString wstrIPAddress, int nPort);

	//template<typename T>
	//bool SendMsgToServer( WORD wMsgID, T& msgInfo);

	virtual bool SendMsgToServer( const void* lpBuf, int nBuflen);

	bool OnMsgFromServer(WORD wMessageID, CSerializer& serializer);

	// OnMsgFromServer Func [7/21/2011 Kaiming-Desktop]
	void SetMsgCallBack(LPGKCALLBACKONMSGFROMSERVER pCallback);
};
