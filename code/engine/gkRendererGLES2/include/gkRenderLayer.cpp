#include "StableHeader.h"
#include "gkRenderLayer.h"

#include "gkMemoryLeakDetecter.h"


gkRenderLayer::gkRenderLayer( void )
{

}

gkRenderLayer::~gkRenderLayer( void )
{
	// 摧毁所有shader分类容器
	gkShaderGroupRenderableMap::iterator i, iend;
	iend = m_mapShaderGroupedMap.end();
	for (i = m_mapShaderGroupedMap.begin(); i != iend; ++i)
	{
		// 这时才真正的删除这个shaderGroup
		//SAFE_DELETE( i->second );
	}
	m_mapShaderGroupedMap.clear();
}

void gkRenderLayer::addRenderable( gkRenderable* pRend, IShader* pShader )
{
	gkShaderGroupRenderableMap::iterator i = m_mapShaderGroupedMap.find(pShader);
	if (i == m_mapShaderGroupedMap.end())
	{
		// 没找到，创建一个 [9/26/2010 Kaiming-Desktop]
		std::pair<gkShaderGroupRenderableMap::iterator, bool> retPair;
		// 这里一旦创建了这个SHADER类型的渲染列表，
		// 在程序结束之前一直都会存在
		//gkShaderGroup* tmpList = new gkShaderGroup;
		retPair = m_mapShaderGroupedMap.insert(
			gkShaderGroupRenderableMap::value_type(
			pShader, gkShaderGroup(pShader)));
		assert(retPair.second && "gkShaderGroupRenderableMap生成shader组时发生异常");
		i = retPair.first;
	}

	// 插入 renderable
	i->second.addRenderable(pRend, pRend->getMaterial());
}

//-----------------------------------------------------------------------
void gkRenderLayer::clear()
{
	// 摧毁所有shader分类容器
	gkShaderGroupRenderableMap::iterator i, iend;
	iend = m_mapShaderGroupedMap.end();
	for (i = m_mapShaderGroupedMap.begin(); i != iend; ++i)
	{
		i->second.clear();
	}
}

//-----------------------------------------------------------------------
gkRenderLayer::ShaderGroupMapIterator gkRenderLayer::_getShaderGroupIterator( void )
{
	return ShaderGroupMapIterator(m_mapShaderGroupedMap.begin(), m_mapShaderGroupedMap.end());
}

//-----------------------------------------------------------------------
gkRenderLayer::ConstShaderGroupMapIterator gkRenderLayer::_getShaderGroupIterator( void ) const
{
	return ConstShaderGroupMapIterator(m_mapShaderGroupedMap.begin(), m_mapShaderGroupedMap.end());
}


//////////////////////////////////////////////////////////////////////////
void gkShaderGroup::addRenderable( gkRenderable* pRend, IMaterial* pMaterial )
{
	gkMaterialGroupRenderableMap::iterator i = m_opaqueMaterialMap.find(pMaterial);
	if (i == m_opaqueMaterialMap.end())
	{
		// 没找到，创建一个 [9/26/2010 Kaiming-Desktop]
		std::pair<gkMaterialGroupRenderableMap::iterator, bool> retPair;
		// 这里一旦创建了这个SHADER类型的渲染列表，
		// 在程序结束之前一直都会存在
		//gkShaderGroup* tmpList = new gkShaderGroup;
		retPair = m_opaqueMaterialMap.insert(
			gkMaterialGroupRenderableMap::value_type(
			pMaterial, gkRenderableList()));
		assert(retPair.second && "gkShaderGroupRenderableMap生成shader组时发生异常");
		i = retPair.first;
	}

	// 插入 renderable
	uint32 tmp = 0;
	if (pRend->getSkinnedMatrix(0, tmp))
	{
		i->second.m_vecRenderable_Skinned.push_back(pRend);
	}
	else
	{
		i->second.m_vecRenderable.push_back(pRend);
	}
	
}
//////////////////////////////////////////////////////////////////////////
void gkShaderGroup::clear()
{
	gkMaterialGroupRenderableMap::iterator i, iend;
	iend = m_opaqueMaterialMap.end();
	for (i = m_opaqueMaterialMap.begin(); i != iend; ++i)
	{
		// Free the list associated with this pass
		i->second.m_vecRenderable.clear();
	}

	m_opaqueMaterialMap.clear();
}
