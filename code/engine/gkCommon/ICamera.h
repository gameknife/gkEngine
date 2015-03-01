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
// Name:   	ICamera.h
// Desc:		
// 
// Author:  Kaiming-Desktop
// Date:	2011 /8/4
// Modify:	2011 /8/4
// 
//////////////////////////////////////////////////////////////////////////


#ifndef ICamera_h_fd8eabf3_09a1_48d8_9308_cd847f705df5
#define ICamera_h_fd8eabf3_09a1_48d8_9308_cd847f705df5

#include "gk_Math.h"

class CCamera;

struct UNIQUE_IFACE ICamera
{
public:
	virtual ~ICamera() {}

	/// 其他模块调用3DEngine Camera的接口， 目前仅一个
	virtual void _renderScene() =0;

	// Movement
	/// 设置位置（传送型）
	virtual void setPosition(float x, float y, float z) =0;

	/// 设置位置（传送型）
	virtual void setPosition(const Vec3& vec) =0;

	/// 取得位置
	virtual const Vec3& getPosition(void) const =0;

	/// 移动（世界坐标系）
	virtual void move(const Vec3& vec) =0;

	/// 移动（自身坐标系）
	virtual void moveLocal(const Vec3& vec) =0;

	virtual void setDirection(float x, float y, float z) =0;

	virtual void setDirection(const Vec3& vec) =0;

	virtual Vec3 getDirection(void) const =0;

	/// 锁定一个Yaw轴
	virtual void setFixedYawAxis( bool useFixed, const Vec3& fixedAxis = Vec3(0,1,0) ) =0;

	virtual const Quat& getOrientation(void) const =0;

	virtual void setOrientation(const Quat& q) =0;

	virtual Vec3 getUp(void) const =0;

	virtual Vec3 getRight(void) const =0;

	/** Get the derived position of this frustum. */
	virtual const Vec3& getPositionForViewUpdate(void) const =0;
	/** Get the derived orientation of this frustum. */
	virtual const Quat getOrientationForViewUpdate(void) const =0;

	virtual const Quat& getDerivedOrientation(void) const =0;
	virtual const Vec3& getDerivedPosition(void) const =0;
	virtual Vec3 getDerivedDirection(void) const =0;
	virtual Vec3 getDerivedUp(void) const =0;
	virtual Vec3 getDerivedRight(void) const =0;

	virtual float getFOVy() const =0;
	virtual void setFOVy(float fov) =0;
	///
	virtual void lookAt( const Vec3& targetPoint ) =0;
	virtual void lookAt(float x, float y, float z) =0;

	virtual void setAspect(float fAspect) =0;

	virtual CCamera* getCCam() =0;

	virtual void synchroniseBaseSettingsWith( ICamera* cam ) =0;

	virtual void setNearPlane(float zNear) =0;
	virtual void setFarPlane(float zFar) = 0;

	virtual float getNearPlane() =0;
	virtual float getFarPlane() =0;

	virtual void setAdditionalOrientation(Quat orien) {}
	//virtual void getStereoOrientation(Quat& orien) {}

	virtual void setStereoOffset( Vec3 leftEye, Vec3 rightEye) {}
	virtual void getStereoOffset( Vec3& leftEye, Vec3& rightEye) {}

	virtual void setStereoProjMatrix(Matrix44 leftMat, Matrix44 rightMat) {}
	virtual void getStereoProjMatrix(Matrix44& leftMat, Matrix44& rightMat) {}
};

#endif // gkCamera_h_fd8eabf3_09a1_48d8_9308_cd847f705df5
