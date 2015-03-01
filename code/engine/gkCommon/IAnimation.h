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
// yikaiming (C) 2013
// gkENGINE Source File 
//
// Name:   	IAnimation.h
// Desc:	
// 	
// 
// Author:  YiKaiming
// Date:	2012/3/6
// 
//////////////////////////////////////////////////////////////////////////

#ifndef _IAnimation_h_
#define _IAnimation_h_


struct IGameObjectAnimLayer;
struct IGameObjectPhysicLayer;
struct IMeshLoader;
	/**
	 @ingroup CoreModuleAPI
	 @brief 动画模块
	 @remark 负责游戏中动画的驱动
	 \n角色动画
	 \netc
	*/
struct IAnimation
{
public:
	virtual ~IAnimation(void) {}

	/**
	 @brief 初始化动画模块
	 @return 
	 @remark 
	*/
	virtual void InitAnimation() =0;
	/**
	 @brief 销毁动画模块
	 @return 
	 @remark 
	*/
	virtual void DestroyAnimation() =0;

	/**
	 @brief 创建一个anim layer
	 @return 
	 @param const gkStdString & name anim layer的名字
	 @remark 这里使用name作为key来管理anim layer
	 @sa IGameObjectAnimLayer
	*/
	virtual IGameObjectAnimLayer* CreateAnimLayer(const gkStdString& name) =0;
	/**
	 @brief 销毁一个anim layer
	 @return 
	 @param const gkStdString & name
	 @remark 
	*/
	virtual void DestroyAnimLayer(const gkStdString& name) =0;
	/**
	 @brief 销毁所有anim layer
	 @return 
	 @remark 
	*/
	virtual void DestroyAllAnimLayer() =0;
	/**
	 @brief 获得动画模块的HKX mesh加载器
	 @return 
	 @remark 对于HKX mesh，需要从havok加载，因此需要实现在动画模块中
	*/
	virtual IMeshLoader* getHKXMeshLoader() =0;
	/**
	 @brief 动画模块的更新
	 @return 
	 @param float fElapsedTime
	 @remark 每帧由system调度
	*/
	virtual void _updateAnimation(float fElapsedTime) =0;
};


#endif