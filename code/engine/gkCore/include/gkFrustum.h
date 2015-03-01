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
// Name:   	gkFrustum.h
// Desc:	视锥体类，视锥架构，能够辅助gkSceneManager进行可视判断
//			该类继承自movableobject和renderable，同时具有物体属性
// 描述:	
// 
// Author:  Kaiming-Desktop	 
// Date:	9/10/2010 	
// 
//////////////////////////////////////////////////////////////////////////

#ifndef gkFrustum_h_b44f3f3f_3609_459e_8846_2c15e3b64a8e
#define gkFrustum_h_b44f3f3f_3609_459e_8846_2c15e3b64a8e

#include "gkPrerequisites.h"

#include "gkMovableObject.h"
#include "gkRenderable.h"
#include "gk_Camera.h"

enum ProjectionType
{
	PT_ORTHOGRAPHIC,
	PT_PERSPECTIVE
};

/** Worldspace clipping planes.
*/
enum FrustumPlane
{
	FRUSTUM_PLANE_NEAR   = 0,
	FRUSTUM_PLANE_FAR    = 1,
	FRUSTUM_PLANE_LEFT   = 2,
	FRUSTUM_PLANE_RIGHT  = 3,
	FRUSTUM_PLANE_TOP    = 4,
	FRUSTUM_PLANE_BOTTOM = 5
};

class gkFrustum : public gkMovableObject 
{
protected:
	/// Orthographic or perspective?
	ProjectionType m_yProjType;
	/// y-direction field-of-view (default 45)
	float m_fFOV;
	/// Far clip distance - default 10000
	mutable float m_fFarDist;
	/// Near clip distance - default 100
	mutable float m_fNearDist;
	/// x/y viewport ratio - default 1.3333(4:3) | 1.7778(16:9)
	float m_fAspect;
	/// Ortho height size (world units)
	float m_fOrthoHeight;
	/// Focal length of frustum (for stereo rendering, defaults to 1.0)
	float m_fFocalLength;

	/// The 6 main clipping planes
	mutable Plane m_plFrustumPlanes[6];

	/// Stored versions of parent orientation / position
	mutable Quat m_qLastParentOrientation;
	mutable Vec3 m_vLastParentPosition;

	/// Shared class-level name for Movable type
	static gkStdString m_wstrMovableType;

	/// 更新标志
	/// 视锥的形体有变化？
	mutable bool m_bRecalcFrustum;
	/// 视锥的位置发生变化？
	mutable bool m_bRecalcView;
	/// 视锥面发生变化？
	mutable bool m_bRecalcFrustumPlanes;

	mutable CCamera m_testFrustum;

	/// 视锥更新函数
	/// Update frustum if out of date
	virtual void updateFrustum(void) const;

	/// Implementation of updateFrustum (called if out of date)
	virtual void updateFrustumImpl(void) const;
	/// Implementation of updateView (called if out of date)
	virtual void updateViewImpl(void) const;
	virtual void updateFrustumPlanes(void) const;
	/// Implementation of updateFrustumPlanes (called if out of date)
	virtual void updateFrustumPlanesImpl(void) const;

	virtual bool isViewOutOfDate(void) const;
	virtual bool isFrustumOutOfDate(void) const;
	/// 通知视锥体有变化
	virtual void invalidateFrustum(void) const;
	/// 通知视图矩阵有变化
	virtual void invalidateView(void) const;

public:
	gkFrustum();
	virtual ~gkFrustum();

	/// Update view if out of date
	virtual void updateView(void) const;

	virtual void setFOVy(float fovy);
	virtual float getFOVy(void) const;

	virtual void setNearClipDistance(float nearDist);
	virtual float getNearClipDistance(void) const;
	virtual void setFarClipDistance(float farDist);
	virtual float getFarClipDistance(void) const;
	inline virtual void setOrthoHeight(float height) { m_fOrthoHeight = height, invalidateView();}

	virtual void setAspect(float aspect);
	virtual float getAspect(void) const;

	virtual void setProjectionType(ProjectionType pt);
	virtual ProjectionType getProjectionType() const;

	/** Get the derived position of this frustum. */
	virtual const Vec3& getPositionForViewUpdate(void) const {return m_vLastParentPosition;}
	/** Get the derived orientation of this frustum. */
	virtual const Quat getOrientationForViewUpdate(void) const {return m_qLastParentOrientation;}

	virtual bool checkRenderable(const AABB& aabb);

	virtual const CCamera* getCCamera() const {return &m_testFrustum;}



	// override

	// 覆盖movableobject接口
	void _updateRenderSequence(IRenderSequence* queue);
	// 覆盖movable接口
	virtual const AABB& getAABB(void) const;

	// 覆盖renderable接口
	void getWorldTransforms( Matrix44* xform ) const;
	// 覆盖renderable接口
	const gkMaterialPtr& getMaterial(void) const;
	// 覆盖renderable接口
	void getRenderOperation(gkRenderOperation& op);
	// 覆盖renderable接口
	virtual bool getSkinnedMatrix(float** pStart, uint32& size) {return false;}
	// 覆盖renderable接口
	virtual float getSquaredViewDepth(const ICamera* cam) const ;



};

#endif // gkFrustum_h_b44f3f3f_3609_459e_8846_2c15e3b64a8e
