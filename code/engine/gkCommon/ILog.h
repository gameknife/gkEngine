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
// Name:   	ILog.h
// Desc:	gkLog系统	
// 
// Author:  Kaiming-Desktop
// Date:	2011 /8/18
// Modify:	2011 /8/18
// 
//////////////////////////////////////////////////////////////////////////

#ifndef ILOG_H_
#define ILOG_H_

#include "gkPlatform.h"

	/**
	 @brief 日志回调
	 @remark 需要输出日志继承此类，会受到log的回调
	*/
struct ILogCallback
{
	virtual void OnWriteToConsole( const TCHAR *sText,bool bNewLine ) = 0;
	virtual void OnWriteToFile( const TCHAR *sText,bool bNewLine ) = 0;
};

	/**
	 @brief 日志模块
	 @remark 继承了详细的日志记录功能
	*/
UNIQUE_IFACE struct ILog
{
	enum ELogType
	{
		eMessage,					// 消息
		eWarning,					// 警告，默认级别从此开始
		eError,						// 错误
		eAlways,					// 不屏蔽日志 v
		eWarningAlways,
		eErrorAlways,
		eInput,						// 输入操作
		eInputResponse,				// 输入反馈
		eComment					// 注释
	};

	/**
	 @brief 设置log文件名
	 @return 
	 @param const gkStdString & command
	 @remark 如果不设置，系统使用日期和时间生成默认文件名生成日志。如果设置，则使用设置的log路径
	*/
	virtual bool	SetFileName(const gkStdString& command = NULL) = 0;
	/**
	 @brief 取得log文件名
	 @return 
	 @remark 
	*/
	virtual const gkStdString&	GetFileName() = 0;

	/**
	 @brief 标准log实现
	 @return 
	 @param const ELogType nType
	 @param const TCHAR * szFormat
	 @param va_list args
	 @remark 
	*/
	virtual void LogV (const ELogType nType, const TCHAR* szFormat, va_list args) = 0;

	/**
	 @brief 设置log级别
	 @return 
	 @param int verbosity
	 @remark 
	*/
	virtual void SetVerbosity( int verbosity ) = 0;
	/**
	 @brief 取得log级别
	 @return 
	 @remark 
	*/
	virtual int	 GetVerbosityLevel()=0;

	/**
	 @brief 添加Log回调
	 @return 
	 @param ILogCallback * pCallback
	 @remark 
	*/
	virtual void AddCallback( ILogCallback *pCallback ) = 0;
	/**
	 @brief 清楚Log回调
	 @return 
	 @param ILogCallback * pCallback
	 @remark 
	*/
	virtual void RemoveCallback( ILogCallback *pCallback ) = 0;
};


#endif
