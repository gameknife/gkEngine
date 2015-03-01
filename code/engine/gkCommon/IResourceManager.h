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



//////////////////////////////////////////////////////////////////////
//	filename: 	IResourceManager.h
//
//	desc:		资源管理器接口
//
//	created:	2011/07/28
//
//	author:		Kaiming-Desktop
//
//////////////////////////////////////////////////////////////////////
#ifndef IResourceManager_h_434d73c0_a6ed_4b5e_afa3_6e7787165383
#define IResourceManager_h_434d73c0_a6ed_4b5e_afa3_6e7787165383
#include "gkPlatform.h"
#include "IResource.h"

enum GK_RESOURCE_MANAGER_TYPE
{
	GK_RESOURCE_MANAGER_TYPE_TEXTURE,
	GK_RESOURCE_MANAGER_TYPE_MATERIAL,
	GK_RESOURCE_MANAGER_TYPE_MESH,
	GK_RESOURCE_MANAGER_TYPE_SHADER,
};

#ifdef OS_WIN32
typedef stdext::hash_map< gkStdString, gkResourcePtr >			gkResourceMap;
#else
typedef std::map< gkStdString, gkResourcePtr >			gkResourceMap;
#endif
typedef std::map< gkResourceHandle, gkResourcePtr >			 	gkResourceHandleMap;
typedef std::vector< gkStdString >								gkReloadList;

class UNIQUE_IFACE IResourceManager
{
public:

//	typedef std::iterator< gkResourceHandleMap > 					gkResourceMapIterator;

protected:
	gkResourceHandleMap		m_mapResourcesByHandle;
	gkResourceMap			m_mapResources;
	gkResourceHandle		m_hNextHandle;

	gkStdString					m_wstrResourceType;			// 资源类型
	gkReloadList			m_reloadList;


public:
	IResourceManager():m_hNextHandle(0){}
	virtual ~IResourceManager(){removeAll();}

	bool cleanGarbage()
	{
		// clean from mapResource
		{
			gkResourceMap::iterator it = m_mapResources.begin();
			for ( ; it != m_mapResources.end(); )
			{
				if( !(it->second.isNull()) && it->second.useCount() <= 2 )
				{
					/*it = */m_mapResources.erase(it++);
				}
				else
				{
					++it;
				}
			}
		}


		{
			gkResourceHandleMap::iterator it = m_mapResourcesByHandle.begin();
			for ( ; it != m_mapResourcesByHandle.end(); )
			{
				if( !(it->second.isNull()) && it->second.useCount() <= 2 )
				{
					if (it->second->isLoaded())
					{
						it->second->unload();
					}					
					/*it = */m_mapResourcesByHandle.erase(it++);
				}
				else
				{
					++it;
				}
			}
		}

		return true;
	}

	//************************************
	// Name:    create
	// Desc: 给定名字和组别，创建对应的资源
	// Returns:   gkResource
	// Parameter: const wstring & name
	// Parameter: const wstring & group
	//************************************
	virtual gkResourcePtr create(const gkStdString& name, const gkStdString& group, gkNameValuePairList* params = NULL)
	{
		gkStdString uniqueName(name);
		gkNormalizeResName(uniqueName);

		gkResourcePtr ret = getByName( name );
		if ( ret.isNull() )
		{
			ret =  gkResourcePtr(createImpl(uniqueName, getNextHandle(), group, params));
			addImpl(ret);
		}
		else
		{
			gkLogError( _T("Resource[ %s ]recreating!"), name.c_str() );
		}


		return ret;
	}

	virtual bool prepare()
	{
		bool ret = false;
		gkReloadList::iterator it = m_reloadList.begin();
		for (; it != m_reloadList.end(); ++it)
		{
			reload(*it);
			ret = true;
		}

		m_reloadList.clear();

		return ret;
	}

	typedef std::pair<gkResourcePtr, bool> ResourceCreateOrRetrieveResult;
	//////////////////////////////////////
	// Name:  gkResourceManager::createOrRetrieve
	// Desc: 如果没有此资源，创建一个，如果有，使用已有资源
	// Returns:   gkResourceManager::ResourceCreateOrRetrieveResult
	// Parameter: const wstring & name
	// Parameter: const wstring & group
	//////////////////////////////////////
	virtual ResourceCreateOrRetrieveResult createOrRetrieve(const gkStdString& name, const gkStdString& group, gkNameValuePairList* params = NULL)
	{
		gkStdString uniqueName(name);
		gkNormalizeResName(uniqueName);

		gkResourcePtr res = getByName(uniqueName);
		bool created = false;
		if (res.isNull())
		{
			created = true;
			res = create(uniqueName, group, params);
		}

		return ResourceCreateOrRetrieveResult(res, created);
	}

	// 载入资源
	virtual gkResourcePtr load(const gkStdString& name, const gkStdString& group = _T("external"))
	{
		gkStdString uniqueName(name);
		gkNormalizeResName(uniqueName);

		gkResourcePtr r = createOrRetrieve(uniqueName,group).first;
		// ensure loaded
		if (r->load())
			return r;
		
		remove(uniqueName);
		r.setNull();
		return r;
	}

	virtual gkResourcePtr loadSync(const gkStdString& name, const gkStdString& group = _T("external"))
	{
		gkStdString uniqueName(name);
		gkNormalizeResName(uniqueName);

		gkResourcePtr r = createOrRetrieve(uniqueName,group).first;
		r->markSyncLoad(true);
		// ensure loaded
		if (r->load())
			return r;

		remove(uniqueName);
		r.setNull();
		return r;
	}

	// 删除资源
	virtual void remove(const gkStdString& name)
	{
		gkStdString uniqueName(name);
		gkNormalizeResName(uniqueName);

		gkResourcePtr res = getByName(uniqueName);

		if (!res.isNull())
		{
			removeImpl(res);
		}
	}
	virtual void remove(gkResourceHandle handle)
	{
		gkResourcePtr res = getByHandle(handle);

		if (!res.isNull())
		{
			removeImpl(res);
		}
	}
	virtual void removeAll(void)
	{
		m_mapResources.clear();
		m_mapResourcesByHandle.clear();
	}

	// 释放资源
	virtual void unload(const gkStdString& name)
	{
		gkStdString uniqueName(name);
		gkNormalizeResName(uniqueName);

		gkResourcePtr res = getByName(uniqueName);
		if (!res.isNull())
		{
			res->unload();
		}
	}

	virtual void unload(gkResourceHandle handle)
	{
		gkResourcePtr res = getByHandle(handle);
		if (!res.isNull())
		{
			res->unload();
		}
	}

	virtual void unloadAll(void)
	{
		gkResourceMap::iterator i, iend;
		i = m_mapResources.begin();
		iend = m_mapResources.end();
		for (; i != iend; ++i)
		{		
			i->second->unload();
		}
	}

	virtual void unloadAllByGroup(const gkStdString& group)
	{
		gkResourceMap::iterator i, iend;
		i = m_mapResources.begin();
		iend = m_mapResources.end();
		for (; i != iend; ++i)
		{
			if (i->second->getGroupName() == group)
				i->second->unload();
		}
	}

	// 载入资源
	virtual void reload(const gkStdString& name, const gkStdString& group = _T(""))
	{
		gkStdString uniqueName(name);
		gkNormalizeResName(uniqueName);

		gkResourceMap::iterator i, iend;
		i = m_mapResources.find(uniqueName);
		if (i != m_mapResources.end())
		{
			// ensure loaded
			i->second->reload();
		}
	}

	// 载入资源
	virtual void reload_s(const gkStdString& name, const gkStdString& group = _T(""))
	{
		m_reloadList.push_back(name);
	}

	void reloadAll( void )
	{
		gkResourceMap::iterator i, iend;
		i = m_mapResources.begin();
		iend = m_mapResources.end();
		for (; i != iend; ++i)
		{		
			i->second->reload();
		}
	}

	// 取得资源
	virtual gkResourcePtr getByName(const gkStdString& name)
	{
		gkStdString uniqueName(name);
		gkNormalizeResName(uniqueName);

		gkResourcePtr res;
		// 从名称资源树删除
		gkResourceMap::iterator nameIt = m_mapResources.find(uniqueName);
		if (nameIt != m_mapResources.end())
		{
			res = nameIt->second;
		}
		return res;
	}

	virtual gkResourcePtr getByHandle(gkResourceHandle handle)
	{
		gkResourcePtr res;
		// 从句柄资源树删除
		gkResourceHandleMap::iterator handleIt = m_mapResourcesByHandle.find(handle);
		if (handleIt != m_mapResourcesByHandle.end())
		{
			res = handleIt->second;
		}
		return res;
	}

	virtual gkResourceMap& getResourceMap() {return m_mapResources;}


	// 验证资源
	virtual bool resourceExists(const gkStdString& name)
	{
		return !(getByName(name).isNull());
	}

	virtual bool resourceExists(gkResourceHandle handle)
	{
		return !(getByHandle(handle).isNull());
	}

	// 加入，创建的实现
	virtual IResource* createImpl(const gkStdString& name, gkResourceHandle handle, 
		const gkStdString& group, gkNameValuePairList* params = NULL) = 0;
	/** Add a newly created resource to the manager (note weak reference) */
	virtual void addImpl( gkResourcePtr& res )
	{
		// 加入名称资源树
		std::pair<gkResourceMap::iterator, bool> result;
		result = m_mapResources.insert( gkResourceMap::value_type( res->getName(), res ) );

		// 加入句柄资源树
		std::pair<gkResourceHandleMap::iterator, bool> resultHandle = 
			m_mapResourcesByHandle.insert( gkResourceHandleMap::value_type( res->getHandle(), res ) );
		if (!resultHandle.second)
		{
			//OutputDebugString("++++gkResourceManager.cpp: a gkResouceHandle:\n%d has already exist!\n");
		}
	}

	/** Remove a resource from this manager; remove it from the lists. */
	virtual void removeImpl( gkResourcePtr& res )
	{
		// 从名称资源树删除
		gkResourceMap::iterator nameIt = m_mapResources.find(res->getName());
		if (nameIt != m_mapResources.end())
		{
			m_mapResources.erase(nameIt);
		}

		// 从句柄资源树删除
		gkResourceHandleMap::iterator handleIt = m_mapResourcesByHandle.find(res->getHandle());
		if (handleIt != m_mapResourcesByHandle.end())
		{
			if( !(handleIt->second.isNull()) )
			{
				handleIt->second->unload();
			}
			m_mapResourcesByHandle.erase(handleIt);
		}
	}

	gkResourceHandle getNextHandle()
	{
		return m_hNextHandle++;
	}

	const gkResourceMap& getResourceMap() const {return m_mapResources;}
	const gkResourceMap* getResourceMapPtr() const {return &m_mapResources;}

	virtual void saveAllMaterial() {}
};

#endif // gkResourceManager_h_434d73c0_a6ed_4b5e_afa3_6e7787165383
