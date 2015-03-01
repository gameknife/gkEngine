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
// Name:    gkCamera.h
// Desc:	摄像机类，包含了摄像机的3D信息，用于选择渲染scene中的
//			物体。
//			以后各种与摄像机交互的帮助函数都在此实现。
// 描述:	
// 
// Author:  Kaiming-Desktop	 
// Date:	9/10/2010 	
// 
//////////////////////////////////////////////////////////////////////////


#ifndef gkCamera_h_fd8eabf3_09a1_48d8_9308_cd847f705df5
#define gkCamera_h_fd8eabf3_09a1_48d8_9308_cd847f705df5

#include "gkPrerequisites.h"

#include "gkFrustum.h"
#include "ICamera.h"


class gkCamera : public gkFrustum ,public ICamera
{
protected:
	/// Camera name
	gkStdString m_wstrName;

	/// Camera orientation, quaternion style
	Quat m_qOrientation;

	/// Camera position - default (0,0,0)
	Vec3 m_vPosition;

	/// Derived orientation/position of the camera, including reflection
	mutable Quat m_qDerivedOrientation;
	mutable Vec3 m_vDerivedPosition;

	/// Real world orientation/position of the camera
	mutable Quat m_qRealOrientation;
	mutable Vec3 m_vRealPosition;

	/// Whether to yaw around a fixed axis.
	bool m_bYawFixed;
	/// Fixed axis to yaw around
	Vec3 m_vYawFixedAxis;

	/// Shared class-level name for Movable type
	static gkStdString m_wstrMovableType;

	/// 单独的裁剪视锥体，以达到一些特殊效果。
	gkFrustum *m_pCullFrustum;

	bool m_bIsLightCamera;

	BYTE m_yHideMask;

	// Internal functions for calcs
	bool isViewOutOfDate(void) const;

public:
	/** Standard constructor.
	*/
	gkCamera( const gkStdString& name );

	/** Standard destructor.
	*/
	virtual ~gkCamera();

	/// 摄像机名字
	virtual const gkStdString& getName(void) const;


	/** 摄像机操作函数。
	操作摄像机可以使用的所有函数
	以后可扩充
	*/

	/// 设置位置（传送型）
	void setPosition(float x, float y, float z);

	/// 设置位置（传送型）
	void setPosition(const Vec3& vec);

	/// 取得位置
	const Vec3& getPosition(void) const;

	/// 移动（世界坐标系）
	void move(const Vec3& vec);

	/// 移动（自身坐标系）
	void moveLocal(const Vec3& vec);

	/** Sets the camera's direction vector.
	@remarks
	Note that the 'up' vector for the camera will automatically be recalculated based on the
	current 'up' vector (i.e. the roll will remain the same).
	*/
	void setDirection(float x, float y, float z);

	/** Sets the camera's direction vector.
	*/
	void setDirection(const Vec3& vec);

	/* Gets the camera's direction.
	*/
	Vec3 getDirection(void) const;

	/// 锁定一个Yaw轴
	void setFixedYawAxis( bool useFixed, const Vec3& fixedAxis = Vec3(0,0,1) );


	/// Returns the camera's current orientation.
	const Quat& getOrientation(void) const;

	/// Sets the camera's orientation.
	void setOrientation(const Quat& q);

	/** Gets the camera's up vector.
	*/
	Vec3 getUp(void) const;

	/** Gets the camera's right vector.
	*/
	Vec3 getRight(void) const;

	/** Gets the derived orientation of the camera, including any
	rotation inherited from a node attachment and reflection matrix. */
	const Quat& getDerivedOrientation(void) const;
	/** Gets the derived position of the camera, including any
	translation inherited from a node attachment and reflection matrix. */
	const Vec3& getDerivedPosition(void) const;
	/** Gets the derived direction vector of the camera, including any
	rotation inherited from a node attachment and reflection matrix. */
	Vec3 getDerivedDirection(void) const;
	/** Gets the derived up vector of the camera, including any
	rotation inherited from a node attachment and reflection matrix. */
	Vec3 getDerivedUp(void) const;
	/** Gets the derived right vector of the camera, including any
	rotation inherited from a node attachment and reflection matrix. */
	Vec3 getDerivedRight(void) const;

	float getFOVy() const {return gkFrustum::getFOVy();}
	void setFOVy(float fov) {gkFrustum::setFOVy(fov);}


	///
	void lookAt( const Vec3& targetPoint );
	void lookAt(float x, float y, float z);

	/** 暂不实现了，时间紧迫
	/// Rolls the camera anticlockwise, around its local z axis.
	void roll(const float angle);

	/// Rotates the camera anticlockwise around it's local y axis.
	void yaw(const float angle);

	/// Pitches the camera up/down anticlockwise around it's local z axis.
	void pitch(const float angle);

	/// Rotate the camera around an arbitrary axis.
	void rotate(const D3DXVECTOR3& axis, const float angle);

	/// Rotate the camera around an arbitrary axis using a Quaternion.
	void rotate(const D3DXQUATERNION& q);

	*/

	/// 通知scenemanager渲染此摄像机看到的图像
	// remark: 通过摄像机调用，是一个迅捷面向对象的方式
	void _renderScene();


// 	/** 取得真实的绝对世界坐标系朝向，位置信息 */
// 	const D3DXQUATERNION& getRealOrientation(void) const;
// 	/** Gets the real world position of the camera, including any
// 	translation inherited from a node attachment. */
// 	const D3DXVECTOR3& getRealPosition(void) const;
// 	/** Gets the real world direction vector of the camera, including any
// 	rotation inherited from a node attachment. */
// 	D3DXVECTOR3 getRealDirection(void) const;
// 	/** Gets the real world up vector of the camera, including any
// 	rotation inherited from a node attachment. */
// 	D3DXVECTOR3 getRealUp(void) const;
// 	/** Gets the real world right vector of the camera, including any
// 	rotation inherited from a node attachment. */
// 	D3DXVECTOR3 getRealRight(void) const;

	/** Overridden from MovableObject */
	const gkStdString& getMovableType(void) const;
   
	/// 使用单独的裁剪视锥体
	// remark: 如此，可以让视域和裁剪分开，
	// 以达到一些特殊的效果，或者更大限度的提高效能
	void setCullingFrustum(gkFrustum* frustum) { m_pCullFrustum = frustum; }
	// 返回单独的裁剪视锥体
	gkFrustum* getCullingFrustum(void) const { return m_pCullFrustum; }

	/// 同gkFrustum::getNearClipDistance
	float getNearClipDistance(void) const;
	/// 同gkFrustum::getFarClipDistance
	float getFarClipDistance(void) const;

	/** Get the derived position of this frustum. */
	virtual const Vec3& getPositionForViewUpdate(void) const {return m_vRealPosition;}
	/** Get the derived orientation of this frustum. */
	virtual const Quat getOrientationForViewUpdate(void) const {return m_qRealOrientation * m_stereoOrientation;}

	/** 与其他摄像机同步基本设置数据。 
	@remarks
	拷贝位置，朝向，FOV，aspect等基础属性。
	*/
	virtual void synchroniseBaseSettingsWith(const gkCamera* cam);
	virtual void synchroniseBaseSettingsWith( ICamera* cam );

	virtual bool isLightCamera()
	{
		return m_bIsLightCamera;
	}

	virtual void setLightCamera(bool bIsLightCam)
	{
		m_bIsLightCamera = bIsLightCam;
	}

	virtual void setHideMask(BYTE hideMASK) {m_yHideMask = hideMASK;}
	virtual BYTE getHideMask() {return m_yHideMask;}

	virtual void setAspect(float fAspect) {gkFrustum::setAspect(fAspect);}

	CCamera* getCCam() {return &m_testFrustum;}

	virtual void setNearPlane( float zNear );

	virtual void setFarPlane( float zFar );

	virtual float getNearPlane();

	virtual float getFarPlane();

	virtual void setStereoOffset( Vec3 leftEye, Vec3 rightEye);
	virtual void getStereoOffset( Vec3& leftEye, Vec3& rightEye);

	virtual void setAdditionalOrientation(Quat orien);

	virtual void setStereoProjMatrix(Matrix44 leftMat, Matrix44 rightMat);
	virtual void getStereoProjMatrix(Matrix44& leftMat, Matrix44& rightMat);

	Vec3 m_stereoOffset[2];
	Quat m_stereoOrientation;

	Matrix44 m_stereoProjMatrix[2];
};

#endif // gkCamera_h_fd8eabf3_09a1_48d8_9308_cd847f705df5
