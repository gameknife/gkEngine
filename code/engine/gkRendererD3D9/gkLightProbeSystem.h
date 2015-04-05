//////////////////////////////////////////////////////////////////////////
//
// yikaiming (C) 2014
// gkENGINE Source File 
//
// Name:   	gkLightProbeSystem.h
// Desc:	
// 	
// 
// Author:  gameKnife
// Date:	2015/4/5
// 
//////////////////////////////////////////////////////////////////////////

#ifndef _gkLightProbeSystem_h_
#define _gkLightProbeSystem_h_

class gkLightProbeSystem
{
public:
	gkLightProbeSystem(void);
	~gkLightProbeSystem(void);

	void Init();
	void Destroy();

	void Update();
	gkTexturePtr GetCubeMap(Vec3 position);
	void SavePositionCubeMap(Vec3 position, const TCHAR* texturename);

	gkTexturePtr m_tmp_cubemap;
	gkTexturePtr m_cubemap_filter_tmp;

	std::vector<gkTexturePtr> m_cubemap_grid;

	bool m_offline_gen_request;
	gkStdString m_offline_gen_name;
	Vec3 m_offline_gen_pos;
};

#endif
