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
// Name:   	gkGpuParticleProxy.h
// Desc:	
// 	
// 
// Author:  Kaiming
// Date:	2013/4/7	
// 
//////////////////////////////////////////////////////////////////////////

#ifndef gkGpuParticleProxy_h__
#define gkGpuParticleProxy_h__

#include "IParticle.h"

struct GPUUpdateContent
{
	gkTexturePtr g_prevPOSRT;
	gkTexturePtr g_currPOSRT;

	gkTexturePtr g_prevVELRT;
	gkTexturePtr g_currVELRT;

	void Bind( gkTexturePtr& pos0, gkTexturePtr& pos1, gkTexturePtr& vel0, gkTexturePtr& vel1 )
	{
		g_prevPOSRT = pos0;
		g_currPOSRT = pos1;

		g_prevVELRT = vel0;
		g_currVELRT = vel1;
	}

	void Swap()
	{
		gkTexturePtr tmp = g_prevPOSRT;
		g_prevPOSRT = g_currPOSRT;
		g_currPOSRT = tmp;

		tmp = g_prevVELRT;
		g_prevVELRT = g_currVELRT;
		g_currVELRT = tmp;
	}

	void SwapVelocity()
	{
		gkTexturePtr tmp = g_prevVELRT;
		g_prevVELRT = g_currVELRT;
		g_currVELRT = tmp;
	}

	void PushVelRT(int index = 0)
	{
		getRenderer()->FX_PushRenderTarget( index, g_currVELRT );
	}
	void PopVelRT(int index = 0)
	{
		getRenderer()->FX_PopRenderTarget( index );
	}

	void PushPosRT(int index = 0)
	{
		getRenderer()->FX_PushRenderTarget( index, g_currPOSRT );
	}
	void PopPosRT(int index = 0)
	{
		getRenderer()->FX_PopRenderTarget( index );
	}
};

class gkGpuParticleProxy : public IParticleProxy
{
public:
	virtual void Update();
	virtual void createFromXmlNode( CRapidXmlParseNode* node );
	virtual void ApplyGpuData();

	gkTexturePtr m_updatePOSRT0;
	gkTexturePtr m_updatePOSRT1;

	gkTexturePtr m_updateVELRT0;
	gkTexturePtr m_updateVELRT1;

	GPUUpdateContent m_content;

	float m_emitTimer;
	int m_emitted;
	int m_maxCount;
};

#endif // gkGpuParticleProxy_h__
