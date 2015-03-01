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
// Name:   	gkShaderParamDataSource.h
// Desc:	todo...
// 描述:	
// 
// Author:  Kaiming-Laptop
// Date:	2010/10/29
// 
//////////////////////////////////////////////////////////////////////////
#pragma once

#include "Prerequisites.h"
#include "ITimeOfDay.h"

class CRenderCamera;
class CCamera;

class gkShaderParamDataSource
{
public:
// 	mutable D3DXMATRIX mWorldMatrix[256];
// 	mutable size_t mWorldMatrixCount;
// 	mutable const D3DXMATRIX* m_matWorldMatrixArray;

// 常用矩阵
	mutable Matrix44 m_matWorldMatrix;
	mutable Matrix44 m_matViewMatrix;
	mutable Matrix44 m_matProjectionMatrix;
	mutable Matrix44 m_matWorldViewMatrix;
	mutable Matrix44 m_matViewProjMatrix;
	mutable Matrix44 m_matWorldViewProjMatrix;
	mutable Matrix44 m_matInverseWorldMatrix;
	mutable Matrix44 m_matInverseWorldViewMatrix;
	mutable Matrix44 m_matInverseViewMatrix;
	mutable Matrix44 m_matInverseTransposeWorldMatrix;
	mutable Matrix44 m_matInverseTransposeWorldViewMatrix;

	mutable Matrix44 m_matViewMatrix_ShadowCascade[3];
	mutable Matrix44 m_matProjectionMatrix_ShadowCascade[3];
	mutable Matrix44 m_matWorldViewMatrix_ShadowCascade[3];
	mutable Matrix44 m_matWorldViewProjMatrix_ShadowCascade[3];
	mutable Matrix44 m_matInverseViewMatrix_ShadowCascade[3];


// backup matrix
	mutable Matrix44 m_matBackupViewMatrix;
	mutable Matrix44 m_matBackupProjMatrix;
	mutable Matrix44 m_matBackupViewProjMatrix;
// 相机位置
	mutable Vec4 m_vCameraPosition;
	mutable Vec4 m_vCameraPositionObjectSpace;

	mutable Vec3 m_vLightDir;
	mutable Vec3 m_vLightDirInViewSpace;
	mutable Vec3 m_vCamDir;
	mutable Vec3 m_vCamPos;

// 过期标志
	mutable bool m_bIsWorldMatrixOutOfDate;
	mutable bool m_bIsViewMatrixOutOfDate;
	mutable bool m_bIsProjectionMatrixOutOfDate;
	mutable bool m_bIsWorldViewMatrixOutOfDate;
	mutable bool m_bIsViewProjMatrixOutOfDate;
	mutable bool m_bIsWorldViewProjMatrixOutOfDate;
	mutable bool m_bIsInverseWorldMatrixOutOfDate;
	mutable bool m_bIsInverseWorldViewMatrixOutOfDate;
	mutable bool m_bIsInverseViewMatrixOutOfDate;
	mutable bool m_bIsInverseTransposeWorldMatrixOutOfDate;
	mutable bool m_bIsInverseTransposeWorldViewMatrixOutOfDate;

	mutable bool m_bIsCameraPositionOutOfDate;
	mutable bool m_bIsCameraPositionObjectSpaceOutOfDate;

// 渲染使用数据
	const gkRenderable*  m_pCurrentRenderable;
	CRenderCamera* m_pCurrentCamera;
	CRenderCamera* m_pShadowsCamera[3];
	CRenderCamera* m_pCameras;

	bool				m_bCameraRelativeRendering;
	Vec3			m_vCameraRelativePosition;
	const CCamera*			m_pCam;

	STimeOfDayKey	m_sCurrTODKey;
    
    Vec3 m_ScreenFarVerts[4];
    Vec4 m_viewparam;
public:
	gkShaderParamDataSource();
	virtual ~gkShaderParamDataSource();
	/// 更新当前renderable
	virtual void setCurrentRenderable(const gkRenderable* rend);
	/// 设置世界矩阵
	virtual void setWorldMatrices(const Matrix44* m);
	/// 设置当前相机
	virtual void setMainCamera(const CCamera* cam);
	virtual void setShadowCascadeCamera(const CCamera* cam, uint8 index);

	void setCurrTodKey(const STimeOfDayKey& todkey) {m_sCurrTODKey = todkey;}
	STimeOfDayKey& getCurrTodKey() {return m_sCurrTODKey;}

    inline const Vec3& getCamFarVerts(uint32 index) {return m_ScreenFarVerts[index];}
    
	/// 设置当前的RT
	//virtual void setCurrentRenderTarget(const gkRenderTarget* target);
	void setLightDir(const Vec3& lightdir);
	inline const Vec3& getLightDir() {return m_vLightDir;}
	inline const Vec3& getLightDirViewSpace() {return m_vLightDirInViewSpace;}
	inline const Vec3& getCamDir() {return m_vCamDir;}
	inline const Vec3& getCamPos() {return m_vCamPos;}
    inline const CCamera* getMainCamera() const {return m_pCam;}
    inline const Vec4& getViewParam() {return m_viewparam;}
	inline void changeCurrentRT(uint32 width, uint32 height)
	{
		m_viewparam.x = width;
		m_viewparam.y = height;
	}
    
	/// 返回现有值
	virtual const Matrix44& getWorldMatrix(void) const;
	virtual const Matrix44& getViewMatrix(void) const;
	virtual const Matrix44& getProjectionMatrix(void) const;
 	virtual const Matrix44& getWorldViewProjMatrix(void) const;
 	virtual const Matrix44& getWorldViewMatrix(void) const;
 	virtual const Matrix44& getInverseWorldMatrix(void) const;
 	virtual const Matrix44& getInverseViewMatrix(void) const;
	virtual const Matrix44& getViewProjMatrix(void) const;

	virtual const Matrix44& getViewMatrix_ShadowCascade(uint8 index)  const;
	virtual const Matrix44& getProjectionMatrix_ShadowCascade(uint8 index)  const;
	virtual const Matrix44& getWorldViewProjMatrix_ShadowCascade(uint8 index)  const;
	virtual const Matrix44& getWorldViewMatrix_ShadowCascade(uint8 index)  const;
	virtual const Matrix44& getInverseViewMatrix_ShadowCascade(uint8 index)  const;

	virtual const Matrix44& getMainCamViewMatrix(void) const {return m_matBackupViewMatrix;}
	virtual const Matrix44& getMainCamProjectionMatrix(void) const {return m_matBackupProjMatrix;}
	virtual const Matrix44& getMainCamViewProjMatrix(void) const {return m_matBackupViewProjMatrix;}
	
	/// 返回加工值
};