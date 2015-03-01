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
// K&K Studio GameKnife ENGINE Source File
//
// Name:   	IGameFramework.h
// Desc:		
// 	
// Author:  Kaiming
// Date:	2012/4/22
// Modify:	2012/4/22
// 
//////////////////////////////////////////////////////////////////////////

#ifndef _IGameFramework_h_
#define _IGameFramework_h_

struct SSystemGlobalEnvironment;
struct ISystemInitInfo;

	/**
	 @brief 游戏逻辑框架的抽象
	 @remark 
	*/
struct IGameFramework
{
	virtual ~IGameFramework(void) {}

	/**
	 @brief 初始化框架
	 @return 
	 @param ISystemInitInfo & sii 描述初始化参数
	 @remark 
	*/
	virtual bool Init(ISystemInitInfo& sii) =0;
	virtual bool PostInit( HWND hWnd, ISystemInitInfo& sii ) =0;
	// run
	virtual void Run() =0;
	// destroy
	virtual bool Destroy() =0;
	// updating
	virtual bool Update() =0;



	// GameDll Initializing & Destroying
	virtual bool InitGame(const TCHAR* dllname) =0;
	virtual bool DestroyGame(bool deferred = true) =0;


	// LevelLoading
	virtual void LoadLevel(const TCHAR* filename) =0;
	virtual void OnLevelLoaded() =0;
    
    virtual void markClose() =0;


	virtual SSystemGlobalEnvironment* getENV() =0;
};

#endif
