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
// Name:   	IGame.h
// Desc:	Real Gamecode write from this Dll module.
//			The GameFrameworkSystem will load the specific name GameDll Module during the runtime
//			
// 	
// Author:  Kaiming
// Date:	2012/4/22
// Modify:	2012/4/22
// 
//////////////////////////////////////////////////////////////////////////

#ifndef _IGame_h_
#define _IGame_h_
	/**
	 @brief 游戏逻辑的对象抽象
	 @remark 
	*/
struct IGame
{
	virtual ~IGame() {}

	/**
	 @brief 初始化
	 @return 
	 @remark 
	*/
	virtual bool OnInit() =0;
	
	/**
	 @brief 销毁
	 @return 
	 @remark 
	*/
	virtual bool OnDestroy() =0;
	
	/**
	 @brief 每帧逻辑更新
	 @return 
	 @remark 
	*/
	virtual bool OnUpdate() =0;

	/**
	 @brief 关卡加载完成时
	 @return 
	 @remark 
	*/
	virtual bool OnLevelLoaded() =0;
};

#endif
