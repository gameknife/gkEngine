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
// Name:   	RendererCVars.h
// Desc:		
// 	
// Author:  Kaiming
// Date:	2012/3/25
// Modify:	2012/3/25
// 
//////////////////////////////////////////////////////////////////////////

#ifndef _RendererCVars_h_
#define _RendererCVars_h_

	struct gkRendererCVars
	{
	public:
		int		r_aux;
		int		r_disinfo;
		int		r_defaultmat;
		int		r_MTRendering;


		double	r_SSAOScale;
		double	r_SSAOBias;
		double	r_SSAOAmount;
		double	r_SSAORadius;

		double	r_shadowSlopeBias;
		double  r_shadowmapsize;
		double	r_ssaodownscale;
		double	r_shadowmaskdownscale;

		int sw_HDRMode;
		double r_HDRLevel;
		double r_HDROffset;
		double r_HDRBrightThreshold;
		double r_HDRGrain;

		int r_HDRRendering;
		int r_dof;
		int r_ssrl;

		int r_ShadingMode;

		int r_Shadow;

		// Shadow Stuff
		double r_GSMShadowConstbia;
		double r_GSMShadowPenumbraLengh;
		double r_GSMShadowPenumbraStart;
		double r_FSMShadowPenumbra;

		// SSAO Stuff
		int r_SSAO;
		double r_SSAOConstract;

		int r_DebugDynTex;
		int r_DebugDynTexType;
		int r_ProfileGpu;

		// MSAA Stuff
		int r_PostMsaa;

		int r_sharpenpass;
		gkRendererCVars();
		~gkRendererCVars();
	};

	extern gkRendererCVars* g_pRendererCVars;



#endif