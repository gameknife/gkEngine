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
// Name:   	IHavok.h
// Desc:		
// 	
// Author:  Kaiming
// Date:	2012/3/6
// Modify:	2012/3/6
// 
//////////////////////////////////////////////////////////////////////////

#ifndef _IHAVOK_H_
#define _IHAVOK_H_

struct IGameObjectAnimLayer;
struct IGameObjectPhysicLayer;
struct IMeshLoader;

	/**
	 @ingroup CoreModuleAPI
	 @brief 物理模块
	 @remark 物理模块提供物理层对象的申请和删除接口
	 \n物理模块内部负责维护各物理对象的模拟和更新
	 \n目前物理模块有两个实现，基于HAVOK实现和基于PHYSX实现

	 @sa IGameObjectPhysicLayer 物理层对象
	*/
struct IPhysics
{
public:
	virtual ~IPhysics(void) {}

	/**
	 @brief 初始化物理模块
	 @return 
	 @remark 
	*/
	virtual void InitPhysics() =0;
	/**
	 @brief 销毁物理模块
	 @return 
	 @remark 
	*/
	virtual void DestroyPhysics() =0;

	/**
	 @brief 创建一个物理层
	 @return 
	 @remark 
	*/
	virtual IGameObjectPhysicLayer* CreatePhysicLayer() =0;
	/**
	 @brief 销毁一个物理层
	 @return 
	 @param IGameObjectPhysicLayer * target
	 @remark 
	*/
	virtual void DestroyPhysicLayer(IGameObjectPhysicLayer* target) =0;
	/**
	 @brief 销毁所有物理层
	 @return 
	 @remark 
	*/
	virtual void DestroyAllPhysicLayer() =0;


	/**
	 @brief 更新所有物理对象
	 @return 
	 @param float fElapsedTime
	 @remark 
	*/
	virtual void UpdatePhysics(float fElapsedTime) =0;
	/**
	 @brief 物理模块后更新
	 @return 
	 @remark 
	*/
	virtual void PostUpdatePhysics() {}

	/**
	 @brief HAVOK线程的HINT
	 @return 
	 @remark 目前HACK在物理接口中，其他物理实现不必理会
	*/
	virtual void HavokThreadInit() {}
	virtual void HavokThreadQuit() {}
};


#endif