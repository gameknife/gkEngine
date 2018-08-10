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
// Name:   	IResource.h
// Desc:	
// 	
// 
// Author:  YiKaiming
// Date:	2013/6/17
// 
//////////////////////////////////////////////////////////////////////////

/** 
	@defgroup ResourceAPI Resource Interface
	@brief 抽象资源对象
	@remark 
*/

#ifndef _IResource_h_
#define _IResource_h_

#include "IThread.h"
#include "ITimer.h"

struct ITask;
class IResourceManager;
typedef unsigned long long int gkResourceHandle;

enum eResourceType
{
	eRT_Texture,
	eRT_Mesh,
	eRT_Shader,
	eRT_Material,
};

struct IResourceResetCallback
{
	virtual void OnResetCallBack() =0;
};

/**
	@ingroup ResourceAPI
	@brief 资源接口
	@remark 纹理，模型，材质，shader的基类，实现资源的基本行为。
*/
class IResource
{
protected:
	gkStdString m_wstrFileName;						//! 资源文件名，非文件资源的资源名
	gkStdString m_wstrGroupName;					//! 资源组名
	gkStdString m_wstrShortName;					//! 资源短名，没有目录结构

	gkResourceHandle m_hHandle;						//! 数字句柄
	uint32 m_nSize;									//! 资源的内存占用
	bool m_bIsLoaded;								//! 资源是否已经载入
	IResourceManager* m_pCreator;					//! 资源的管理器指针
	eResourceType m_type;							//! 资源内部类型

	IResourceResetCallback* m_pResetCallback;		//! 重设的回调函数，主要用于DX9的重设处理
	gkScopedLock<gkMutexLock>* m_resLock;			//! 多线程加载的资源锁，基于mutex实现
	std::vector<ITask*> m_loadingtasks;				//! 加载此资源的loading任务

	bool m_syncLoad;								//! 是否同步加载的标志，某些系统资源需要立即加载，不走多线程加载

protected:

	/**
	 @brief 用于加载资源的接口
	 @remark 
	*/
	virtual bool loadImpl(void) =0;

	/**
	 @brief 用于卸载资源的接口
	 @remark 
	*/
	virtual bool unloadImpl(void) =0;


public:
	IResource( eResourceType type, IResourceManager* creator, const gkStdString& name, gkResourceHandle handle,
		const gkStdString& group = _T("none"))
	{

		m_type = type;
		m_pCreator = creator;
		m_wstrFileName = name;
		m_hHandle = handle;
		m_wstrGroupName = group;
		m_nSize = 0;							
		m_bIsLoaded = false;
		m_pResetCallback = NULL;
		m_resLock = NULL;
		m_syncLoad = false;
#if defined( __x86_64__ ) || defined( __arm64__ )
        m_resLock = new gkScopedLock<gkMutexLock>( eLGID_Resource, (uint64)this );
#else
		m_resLock = new gkScopedLock<gkMutexLock>( eLGID_Resource, (uint32)this );
#endif



		// 取得短名, 分割文件的|,路径\\,/
		int p = m_wstrFileName.rfind('|');
		if (p >= 0)
		{
			m_wstrShortName = m_wstrFileName.substr(p + 1);
		}
		else
		{
			p = m_wstrFileName.rfind('\\');
			if (p >= 0)
			{
				m_wstrShortName = m_wstrFileName.substr(p + 1);
			}
			else
			{
				p = m_wstrFileName.rfind('/');
				if (p >= 0)
				{
					m_wstrShortName = m_wstrFileName.substr(p + 1);
				}
				else
				{
					m_wstrShortName = m_wstrFileName;
				}
			}
		}

		//finally, cut the .xxx
		p = m_wstrShortName.rfind('.');
		if (p >= 0)
		{
			m_wstrShortName = m_wstrShortName.substr(0, p);
		}

		// 多维子材质，没有creator, 主动成功
		if (!m_pCreator)
		{
			m_bIsLoaded = true;
		}
	};

	virtual ~IResource(void) {
		delete m_resLock;	
	}


public:
	/**
	 @brief 基本load行为
	 @remark 根据loadImpl的返回值，来判断是否load成功，并置标志
	*/
	virtual bool load()
	{
		if(m_bIsLoaded)
		{
			return true;
		}

		if (!loadImpl())
		{
			return false;
		}

		m_bIsLoaded = true;
		return true;

	}
	/**
	 @brief 基本reload行为
	 @remark 
	*/
	virtual void reload()
	{
		unload();
		load();
	}
	/**
	 @brief 基本unload行为
	 @remark 
	*/
	virtual void unload()
	{
		if(!m_bIsLoaded)
		{
			return;
		}
		unloadImpl();
		m_bIsLoaded = false;
	}

	/**
	 @brief 加锁，用于多线程加载
	 @remark 
	*/
	void lockres()
	{
		m_resLock->Lock();
	}

	/**
	 @brief 开锁
	 @remark 
	*/
	void unlockres()
	{
		m_resLock->UnLock();
	}

	/**
	 @brief 绑定加载任务
	 @remark 用于取消时取消任务
	*/
	void bindtask(ITask* task)
	{
		m_loadingtasks.push_back( task );
	}

	/**
	 @brief 得到资源锁
	 @remark 
	*/
	gkScopedLock<gkMutexLock>* getLock()
	{
		return m_resLock;
	}

	/**
	 @brief 在需要使用资源时touch，确保加载
	 @remark 
	*/
	virtual void touch()
	{
		if (!m_bIsLoaded)
			load();
	}

	/**
	 @brief 取得资源名
	 @remark 
	*/
	virtual const gkStdString& getName(void) const
	{ 
		return m_wstrFileName; 
	}

	/**
	 @brief 取得资源短名
	 @remark 
	*/
	virtual const gkStdString& getShortName(void) const
	{ 
		return m_wstrShortName;
	}

	/**
	 @brief 
	 @remark 取得资源组名
	*/
	virtual const gkStdString& getGroupName(void) const
	{ 
		return m_wstrGroupName; 
	}

	
	/**
	 @brief 取得资源数字句柄
	 @remark 
	*/
	virtual const gkResourceHandle getHandle(void) const
	{
		return m_hHandle;
	}

	/**
	 @brief 资源是否加载
	 @remark 
	*/
	virtual bool isLoaded(void) const 
	{ 
		return m_bIsLoaded; 
	}

	/**
	 @brief 获取资源类型
	 @remark 
	*/
	virtual eResourceType getType() const {return m_type;}

	/**
	 @brief 设置重设回调
	 @remark 
	*/
	void bindResetCallback( IResourceResetCallback* callback)
	{
		m_pResetCallback = callback;
	}

	/**
	 @brief 标志是否同步加载
	 @remark 
	*/
	void markSyncLoad(bool sync)
	{
		m_syncLoad = sync;
	}

	uint32 getMemUsage()
	{
		return m_nSize;
	}
};

#include "gkSharedPtr.h"
typedef SharedPtr<IResource> gkResourcePtr;

#endif // IResource_h_434d73c0_a6ed_4b5e_afa3_6e3387165383
