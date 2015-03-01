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
// Name:   	ClientSocket.h
// Desc:	客户端socket封装，继承MFC的CSOCKET结构
// 描述:	虽然网上诟病这个CSOCKET很多，但现在的MFC看样子好像
//			CSOCKET是线程安全的了，客户端先用到这个，出了问题再想办法
// 
// Author:  Kaiming-Desktop	 
// Date:	11/20/2010 	
// 
//////////////////////////////////////////////////////////////////////////
#pragma once
#include "NetWorkPrerequisites.h"
#include <afxsock.h>		// MFC socket extensions
#include <string>

class CSerializer;
/////////////////////////////////////////////////////////////////////////////
// CSocketClient command target
class GAMEKNIFENETWORK_API CSocketClient : public CSocket
{
	// Attributes
public:
	gkStdString m_wstrName;
	UINT m_nLength;			//消息长度
	BYTE m_yBuffer[10240]; 
	bool m_bConnected;

	// Operations
public:
	CSocketClient();
	virtual ~CSocketClient();

	// Overrides
public:
	virtual void OnReceive(int nErrorCode);
	virtual void OnSend(int nErrorCode);
	virtual void OnConnect(int nErrorCode);
	// Implementation
public:
	// 获得链接信息
	virtual bool IsConnected() {return m_bConnected;}

	// 纯虚一个，继承出来的网络层必须要实现一个
	virtual bool OnMsgFromServer( WORD wMessageID, CSerializer& serializer) = 0;
protected:
};
