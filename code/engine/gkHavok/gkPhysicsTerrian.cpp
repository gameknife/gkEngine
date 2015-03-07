#include "StableHeader.h"
#include "gkPhysicsTerrian.h"
#include "ITexture.h"


gkPhysicsTerrian::gkPhysicsTerrian( const gkTexturePtr& heightmap, float maxHeight, float zeroOffset )
{
	if (heightmap.isNull())
	{
		gkLogError(_T("physics terrian create failed!"));
		return;
	}

	float* datas = hkAllocate<hkReal>((heightmap->getWidth() * heightmap->getHeight()), HK_MEMORY_CLASS_USER);
		

	uint8* rawData = heightmap->RawData();

	// basic l-r t-b loading
// 	for (uint32 i=0; i < heightmap->getWidth() * heightmap->getHeight(); ++i)
// 	{
// 		datas[i] = rawData[i] / 255.f * maxHeight;
// 	}

	// b-t l-r loading
	uint32 count = 0;
	for (uint32 x=0; x < heightmap->getWidth(); ++x)
	{
		for (uint32 y=heightmap->getHeight() - 1; y != -1; --y)
		{
			datas[count++] = rawData[ y * heightmap->getWidth() + x ] / 255.f * maxHeight - zeroOffset;
		}
	}

	hkpSampledHeightFieldBaseCinfo ci;
	ci.m_xRes = heightmap->getWidth();
	ci.m_zRes = heightmap->getHeight();
	ci.m_scale.set(1.0, 1.0, 1.0);

	m_proxy = new gkPhysicsTerrianProxy( ci , datas);

	//delete[] datas;
}

gkPhysicsTerrian::~gkPhysicsTerrian(void)
{
	//delete m_proxy;
}
