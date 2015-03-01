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
// yikaiming (c) 2013
// GameKnife ENGINE Source File
//
// Name:   	gk_Camera.h
// Desc:	
// 	
// 
// Author:  yikaiming
// Date:	2013/6/1	
// 
//////////////////////////////////////////////////////////////////////////

#ifndef gk_Camera_h__
#define gk_Camera_h__

//DOC-IGNORE-BEGIN
#include "gk_Math.h"
#include "gk_Geo.h"

inline void mathVec4Transform(f32 out[4], const f32 m[16], const f32 in[4])
{
#define M(row,col)  m[col*4+row]
	out[0] =	M(0, 0) * in[0] + M(0, 1) * in[1] + M(0, 2) * in[2] + M(0, 3) * in[3];
	out[1] =	M(1, 0) * in[0] + M(1, 1) * in[1] + M(1, 2) * in[2] + M(1, 3) * in[3];
	out[2] =	M(2, 0) * in[0] + M(2, 1) * in[1] + M(2, 2) * in[2] + M(2, 3) * in[3];
	out[3] =	M(3, 0) * in[0] + M(3, 1) * in[1] + M(3, 2) * in[2] + M(3, 3) * in[3];
#undef M
}

inline void mathMatrixLookAt(Matrix44A* pMatr, const Vec3& Eye, const Vec3& At, const Vec3& Up)
{
	Vec3 vLightDir = (Eye - At);
	Vec3 zaxis = vLightDir.GetNormalized();
	Vec3 xaxis = (Up.Cross(zaxis)).GetNormalized();
	Vec3 yaxis =zaxis.Cross(xaxis);

	(*pMatr)(0,0) = xaxis.x;		(*pMatr)(0,1) = yaxis.x;	(*pMatr)(0,2) = zaxis.x;	(*pMatr)(0,3) = 0; 
	(*pMatr)(1,0) = xaxis.y;		(*pMatr)(1,1) = yaxis.y;	(*pMatr)(1,2) = zaxis.y;	(*pMatr)(1,3) = 0; 
	(*pMatr)(2,0) = xaxis.z;		(*pMatr)(2,1) = yaxis.z;	(*pMatr)(2,2) = zaxis.z;	(*pMatr)(2,3) = 0; 
	(*pMatr)(3,0) = -xaxis.Dot(Eye);	(*pMatr)(3,1) = -yaxis.Dot(Eye);	(*pMatr)(3,2) = -zaxis.Dot(Eye);	(*pMatr)(3,3) = 1; 
}

inline void mathMatrixPerspectiveFov(Matrix44A* pMatr, f32 fovY, f32 Aspect, f32 zn, f32 zf)
{
	f32 yScale = 1.0f/gk_tanf(fovY/2.0f);
	f32 xScale = yScale / Aspect;

	(*pMatr)(0,0) = xScale;		(*pMatr)(0,1) = 0;			(*pMatr)(0,2) = 0;						(*pMatr)(0,3) = 0; 
	(*pMatr)(1,0) = 0;				(*pMatr)(1,1) = yScale; (*pMatr)(1,2) = 0;						(*pMatr)(1,3) = 0; 
	(*pMatr)(2,0) = 0;				(*pMatr)(2,1) = 0;			(*pMatr)(2,2) = zf/(zn-zf);		(*pMatr)(2,3) = -1.0f; 
	(*pMatr)(3,0) = 0;				(*pMatr)(3,1) = 0;			(*pMatr)(3,2) = zn*zf/(zn-zf); (*pMatr)(3,3) = 0; 
}

//DOC-IGNORE-END

//////////////////////////////////////////////////////////////////////
#define DEFAULT_NEAR	0.25f
#define DEFAULT_FAR		1024.0f
#define DEFAULT_FOV		(75.0f*gf_PI/180.0f)

//////////////////////////////////////////////////////////////////////

enum {
	FR_PLANE_NEAR,
	FR_PLANE_FAR,
	FR_PLANE_RIGHT,
	FR_PLANE_LEFT,
	FR_PLANE_TOP,
	FR_PLANE_BOTTOM,
	FRUSTUM_PLANES
};

//////////////////////////////////////////////////////////////////////

enum cull {
	CULL_EXCLUSION,		// The whole object is outside of frustum.
	CULL_OVERLAP,			// The object &  frustum overlap.
	CULL_INCLUSION		// The whole object is inside frustum.
};

class CCamera
{
public:
	ILINE static Matrix33 CreateOrientationYPR( const Ang3& ypr );
	ILINE static Ang3 CreateAnglesYPR( const Matrix33& m );
	ILINE static Ang3 CreateAnglesYPR( const Vec3& vdir, f32 r=0 );
	ILINE static Vec3 CreateViewdir( const Ang3& ypr );
	ILINE static Vec3 CreateViewdir( const Matrix33& m ) { return m.GetColumn1(); };

	ILINE void SetMatrix( const Matrix34& mat ) { assert(mat.IsOrthonormal()); m_Matrix=mat; UpdateFrustum(); };
	ILINE const Matrix34& GetMatrix() const { return m_Matrix; };
	ILINE Vec3 GetViewdir() const { return m_Matrix.GetColumn1(); };

	ILINE Matrix34 GetViewMatrix() const { return m_Matrix.GetInverted(); };
	ILINE Vec3 GetPosition() const { return m_Matrix.GetTranslation(); }
	ILINE void	SetPosition(const Vec3& p)	{ m_Matrix.SetTranslation(p); UpdateFrustum(); }
	ILINE bool Project(const Vec3& p, Vec3& result, Vec2i topLeft = Vec2i(0,0), Vec2i widthHeight = Vec2i(0,0)) const;

	//------------------------------------------------------------

	void SetFrustum(int nWidth,int nHeight,  f32 FOV=DEFAULT_FOV,f32 nearplane=DEFAULT_NEAR,f32 farpane=DEFAULT_FAR, f32 fPixelAspectRatio = 1.0f);
	ILINE void SetAsymmetry( float l, float r, float b, float t ) { m_asymL = l; m_asymR = r; m_asymB = b; m_asymT = t; UpdateFrustum(); }

	ILINE int GetViewSurfaceX() const { return m_Width; }	
	ILINE int GetViewSurfaceZ() const { return m_Height; }	
	ILINE f32 GetFov() const { return m_fov; }	
	ILINE float GetHorizontalFov() const;
	ILINE f32 GetNearPlane() const { return m_edge_nlt.y; }
	ILINE f32 GetFarPlane() const { return m_edge_flt.y; }
	ILINE f32 GetProjRatio() const { return(m_ProjectionRatio); }
	ILINE f32 GetAngularResolution() const { return m_Height / m_fov; }

	ILINE Vec3 GetEdgeP() const { return m_edge_plt; }
	ILINE Vec3 GetEdgeN() const { return m_edge_nlt; }
	ILINE Vec3 GetEdgeF() const { return m_edge_flt; }

	ILINE f32 GetAsymL() const { return m_asymL; }
	ILINE f32 GetAsymR() const { return m_asymR; }
	ILINE f32 GetAsymB() const { return m_asymB; }
	ILINE f32 GetAsymT() const { return m_asymT; }
	
	ILINE const Vec3& GetNPVertex(int nId) const; //get near-plane vertices
	ILINE const Vec3& GetFPVertex(int nId) const; //get far-plane vertices
	ILINE const Vec3& GetPPVertex(int nId) const; //get projection-plane vertices

	ILINE const Plane* GetFrustumPlane(int numplane)	const		{ return &m_fp[numplane]; }

	//////////////////////////////////////////////////////////////////////////
	// Z-Buffer ranges.
	// This values are defining near/far clipping plane, it only used to specify z-buffer range.
	// Use it only when you want to override default z-buffer range.
	// Valid values for are: 0 <= zrange <= 1
	//////////////////////////////////////////////////////////////////////////
	ILINE void SetZRange( float zmin,float zmax )
	{
		// Clamp to 0-1 range.
		m_zrangeMin = min(1.0f,max(0.0f,zmin));
		m_zrangeMax = min(1.0f,max(0.0f,zmax));
	};
	ILINE float GetZRangeMin() const { return m_zrangeMin; }
	ILINE float GetZRangeMax() const { return m_zrangeMax; }
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------------------
	//--------                Frustum-Culling                ----------------------------
	//-----------------------------------------------------------------------------------

	//Check if a point lies within camera's frustum
	bool IsPointVisible(const Vec3& p) const;

	//sphere-frustum test
	bool IsSphereVisible_F( const Sphere &s ) const;
	uint8 IsSphereVisible_FH( const Sphere &s ) const; //this is going to be the exact version of sphere-culling

	// AABB-frustum test 
  // Fast
	bool IsAABBVisible_F( const AABB& aabb ) const;
	uint8 IsAABBVisible_FH( const AABB& aabb, bool *pAllInside ) const;
  uint8 IsAABBVisible_FH( const AABB& aabb ) const;

  // Exact
	bool IsAABBVisible_E(const AABB& aabb) const;
  uint8 IsAABBVisible_EH( const AABB& aabb, bool *pAllInside ) const;
  uint8 IsAABBVisible_EH( const AABB& aabb ) const;

  // Multi-camera
  bool IsAABBVisible_EHM( const AABB& aabb, bool *pAllInside ) const;
  bool IsAABBVisible_EM( const AABB& aabb ) const;
  bool IsAABBVisible_FM( const AABB& aabb ) const;

	//OBB-frustum test 
	bool IsOBBVisible_F( const Vec3& wpos, const OBB& obb ) const;
	uint8 IsOBBVisible_FH( const Vec3& wpos,  const OBB& obb) const;
	bool IsOBBVisible_E( const Vec3& wpos,  const OBB& obb, f32 uscale ) const;
	uint8 IsOBBVisible_EH( const Vec3& wpos,  const OBB& obb, f32 uscale ) const;

	//## constructor/destructor
  CCamera()	{	m_Matrix.SetIdentity();	SetFrustum(640,480); m_zrangeMin = 0.0f; m_zrangeMax = 1.0f;	/*m_pMultiCamera = NULL;*/ m_pPortal = NULL; m_JustActivated = 0; m_nPosX=m_nPosY=m_nSizeX=m_nSizeY=0; m_asymR=0; m_asymL=0; m_asymB=0; m_asymT=0; }
	~CCamera() {}

	void GetFrustumVertices(Vec3 * pVerts) const;
	void GetFrustumVerticesCam(Vec3 * pVerts) const;

	void SetJustActivated(const bool justActivated) {m_JustActivated = (int)justActivated;}
	bool IsJustActivated() const {return m_JustActivated!=0;}

  void SetViewPort(int nPosX, int nPosY, int nSizeX, int nSizeY)
  {
    m_nPosX = nPosX;
    m_nPosY = nPosY;
    m_nSizeX = nSizeX;
    m_nSizeY = nSizeY;
  }

  void GetViewPort(int & nPosX, int & nPosY, int & nSizeX, int & nSizeY) const
  {
    nPosX = m_nPosX;
    nPosY = m_nPosY;
    nSizeX = m_nSizeX;
    nSizeY = m_nSizeY;
  }

  void UpdateFrustum();
private:		
  bool AdditionalCheck( const AABB& aabb ) const; 
	bool AdditionalCheck( const Vec3& wpos, const OBB& obb, f32 uscale ) const; 

	Matrix34 m_Matrix;				// world space-matrix

	f32		m_fov;							// vertical fov in radiants [0..1*PI[
	int		m_Width;						// surface width-resolution
	int		m_Height;						// surface height-resolution
	f32		m_ProjectionRatio;	// ratio between width and height of view-surface

	Vec3	m_edge_nlt;					// this is the left/upper vertex of the near-plane
	Vec3	m_edge_plt;					// this is the left/upper vertex of the projection-plane 
	Vec3	m_edge_flt;					// this is the left/upper vertex of the far-clip-plane

	f32		m_asymL, m_asymR, m_asymB, m_asymT;   // Shift to create asymmetric frustum (not used for culling atm)

	//usually we update these values every frame (they depend on m_Matrix)
	Vec3	m_cltp,m_crtp,m_clbp,m_crbp;		//this are the 4 vertices of the projection-plane in cam-space
	Vec3	m_cltn,m_crtn,m_clbn,m_crbn;		//this are the 4 vertices of the near-plane in cam-space
	Vec3	m_cltf,m_crtf,m_clbf,m_crbf;		//this are the 4 vertices of the farclip-plane in cam-space

	//usually we update these values every frame (they depend on m_Matrix)

	Plane	m_fp [FRUSTUM_PLANES]; //
	uint32	m_idx1[FRUSTUM_PLANES],m_idy1[FRUSTUM_PLANES],m_idz1[FRUSTUM_PLANES]; //
	uint32	m_idx2[FRUSTUM_PLANES],m_idy2[FRUSTUM_PLANES],m_idz2[FRUSTUM_PLANES]; //

	// Near Far range of the z-buffer to use for this camera.
	float m_zrangeMin;
	float m_zrangeMax;

  int m_nPosX, m_nPosY, m_nSizeX, m_nSizeY;

	//------------------------------------------------------------------------
	//---   OLD STUFF
	//------------------------------------------------------------------------
	public:
	
  void SetFrustumVertices(Vec3 * arrvVerts)	{
  m_clbp = arrvVerts[0];
	  m_cltp = arrvVerts[1];
	  m_crtp = arrvVerts[2];
	  m_crbp = arrvVerts[3];
  }
  void SetPPVertex(int nId, const Vec3 & vVert)	{
	  switch(nId)	{
		  case 0:	m_clbp=vVert;	break;
		  case 1:	m_cltp=vVert;	break;
		  case 2:	m_crtp=vVert;	break;
		  case 3:	m_crbp=vVert;	break;
	  }	assert(0);
  }
  inline void SetFrustumPlane(int i, const Plane	&plane) { 
	  m_fp[i] = plane; 
		//do not break strict aliasing rules, use union instead of reinterpret_casts
		union f32_u
		{
			float floatVal;
			uint32 uintVal;
		};

		{
			f32_u ux; ux.floatVal = m_fp[i].n.x;
			f32_u uy; uy.floatVal = m_fp[i].n.y;
			f32_u uz; uz.floatVal = m_fp[i].n.z;
			uint32 bitX	=	ux.uintVal>>31;
			uint32 bitY	=	uy.uintVal>>31;
			uint32 bitZ	=	uz.uintVal>>31;
			m_idx1[i]=bitX*3+0; m_idx2[i]=(1-bitX)*3+0;
			m_idy1[i]=bitY*3+1;	m_idy2[i]=(1-bitY)*3+1;
			m_idz1[i]=bitZ*3+2;	m_idz2[i]=(1-bitZ)*3+2;
		}









































  }

  inline Ang3 GetAngles()	const { return CreateAnglesYPR( Matrix33(m_Matrix) ); }	
  void SetAngles( const Ang3 &angles )	{ SetMatrix( Matrix34::CreateRotationXYZ(angles) ); }	


  struct IVisArea* m_pPortal; // pointer to portal used to create this camera
  struct ScissorInfo{
	  ScissorInfo() { x1=y1=x2=y2=0; }
	  uint16 x1,y1,x2,y2;
  };
  ScissorInfo m_ScissorInfo;
  //class std::vector<CCamera> m_pMultiCamera; // maybe used for culling instead of this camera

	Vec3	m_OccPosition;		//Position for calculate occlusions (needed for portals rendering)
	inline const Vec3& GetOccPos() const { return(m_OccPosition); }

	int m_JustActivated;		//Camera activated in this frame, used for disabling motion blur effect at camera changes in movies
};




#define CAM_NO_INLINE NO_INLINE inline // elf/exe will have multiply defined symbol error without inline


inline float CCamera::GetHorizontalFov() const
{
	float fFractionVert = tanf(m_fov*0.5f);
	float fFractionHoriz = fFractionVert*GetProjRatio();
	float fHorizFov = atanf(fFractionHoriz)*2;

	return fHorizFov;
}


// Description
//  This function builds a 3x3 orientation matrix using YPR-angles
//  Rotation order for the orientation-matrix is Z-X-Y. (Zaxis=YAW / Xaxis=PITCH / Yaxis=ROLL)
//
// <PRE>
//  COORDINATE-SYSTEM       
//
//  z-axis                                 
//    ^                               
//    |                               
//    |  y-axis                   
//    |  /                         
//    | /                           
//    |/                             
//    +--------------->   x-axis     
// </PRE>
//
//  Example:
//		Matrix33 orientation=CCamera::CreateOrientationYPR( Ang3(1,2,3) );
inline Matrix33 CCamera::CreateOrientationYPR( const Ang3& ypr )
{
	f32 sz,cz; sincos_tpl(ypr.x, &sz,&cz);	//Zaxis = YAW
	f32 sx,cx; sincos_tpl(ypr.y, &sx,&cx);	//Xaxis = PITCH
	f32 sy,cy; sincos_tpl(ypr.z, &sy,&cy);	//Yaxis = ROLL
	Matrix33 c;
	c.m00=cy*cz-sy*sz*sx;		c.m01=-sz*cx;		c.m02=sy*cz+cy*sz*sx;
	c.m10=cy*sz+sy*sx*cz;		c.m11=cz*cx;		c.m12=sy*sz-cy*sx*cz;
	c.m20=-sy*cx;						c.m21=sx;				c.m22=cy*cx;					
	return c;
}

// Description
//   <PRE>
//   x-YAW
//   y-PITCH (negative=looking down / positive=looking up)
//   z-ROLL 
//   </PRE>
// Note: If we are looking along the z-axis, its not possible to specify the x and z-angle
inline Ang3 CCamera::CreateAnglesYPR( const Matrix33& m )
{
	assert(m.IsOrthonormal());
	float l = Vec3(m.m01,m.m11,0.0f).GetLength(); 
	if (l>0.0001) 
		return Ang3(atan2f(-m.m01/l,m.m11/l),atan2f(m.m21,l),atan2f(-m.m20/l,m.m22/l)); 
	else 
		return Ang3(0,atan2f(m.m21,l),0); 
}

// Description
//   <PRE>
//x-YAW
//y-PITCH (negative=looking down / positive=looking up)
//z-ROLL (its not possile to extract a "roll" from a view-vector)
// </PRE>
// Note: if we are looking along the z-axis, its not possible to specify the rotation about the z-axis
ILINE Ang3 CCamera::CreateAnglesYPR( const Vec3& vdir, f32 r )	
{
	assert((fabs_tpl(1-(vdir|vdir)))<0.001);		//check if unit-vector
	f32 l = Vec3(vdir.x,vdir.y,0.0f).GetLength(); //check if not zero
	if (l>0.0001) 
		return Ang3(atan2f(-vdir.x/l,vdir.y/l),atan2f(vdir.z,l),r);
	else 
		return Ang3(0,atan2f(vdir.z,l),r);
}

// Description
//   <PRE>
//x=yaw
//y=pitch
//z=roll (we ignore this element, since its not possible to convert the roll-component into a vector)
// </PRE>
ILINE Vec3 CCamera::CreateViewdir( const Ang3& ypr ){
	assert(ypr.IsInRangePI()); //all angles need to be in range between -pi and +pi
	f32 sz,cz; sincos_tpl(ypr.x,&sz,&cz); //YAW
	f32 sx,cx; sincos_tpl(ypr.y,&sx,&cx); //PITCH
	return Vec3(-sz*cx,cz*cx,sx); //calculate the view-direction
}

// Description
//   <PRE>
//p=world space position
//result=spreen space pos
//retval=is visible on screen
// </PRE>
ILINE bool CCamera::Project(const Vec3& p, Vec3& result, Vec2i topLeft, Vec2i widthHeight ) const
{
	Matrix44A mProj, mView;
	Vec4 in, transformed, projected;

	mathMatrixPerspectiveFov(&mProj, GetFov(), GetProjRatio(), GetNearPlane(), GetFarPlane());
	mathMatrixLookAt(&mView, GetPosition(), GetPosition()+GetViewdir(), Vec3(0, 0, 1));

	int pViewport[4] = {0, 0, GetViewSurfaceX(), GetViewSurfaceZ()};

	if( !topLeft.IsZero() || !widthHeight.IsZero() )
	{
		pViewport[0] = topLeft.x;
		pViewport[1] = topLeft.y;
		pViewport[2] = widthHeight.x;
		pViewport[3] = widthHeight.y;
	}

	in.x = p.x;
	in.y = p.y;
	in.z = p.z;
	in.w = 1.0f;
	mathVec4Transform((f32*)&transformed, (f32*)&mView, (f32*)&in);

	bool visible = transformed.z < 0.0f;

	mathVec4Transform((f32*)&projected, (f32*)&mProj, (f32*)&transformed);

	if (projected.w == 0.0f)
	{
		result = Vec3(0.f,0.f,0.f);
		return false;
	}

	projected.x /= projected.w;
	projected.y /= projected.w;
	projected.z /= projected.w;

	visible = visible && (gk_fabsf(projected.x) <= 1.0f) && (gk_fabsf(projected.y) <= 1.0f);

	//output coords
	result.x = pViewport[0] + (1 + projected.x) * pViewport[2] / 2;
	result.y = pViewport[1] + (1 - projected.y) * pViewport[3] / 2;  //flip coords for y axis
	result.z = projected.z;

	return visible;
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
inline void	CCamera::SetFrustum(int nWidth,int nHeight, f32 FOV, f32 nearplane,f32 farpane, f32 fPixelAspectRatio)
{	
	assert (nearplane>0.001f);		//check if near-plane is valid
	assert (farpane>0.1f);				//check if far-plane is valid
	assert (farpane>nearplane);		//check if far-plane bigger then near-plane
	assert (FOV>=0.000001f && FOV<gf_PI);		  //check if specified FOV is valid

	m_fov			= FOV;
	m_Width		= (int)(((float)nWidth)/fPixelAspectRatio);		//surface x-resolution
	m_Height	= nHeight;	//surface z-resolution
	m_ProjectionRatio = (f32)m_Width/(f32)m_Height;    // projection ratio (1.0 for square pixels)

	//-------------------------------------------------------------------------
	//---  calculate the Left/Top edge of the Projection-Plane in EYE-SPACE ---
	//-------------------------------------------------------------------------
	m_edge_plt.x    =-m_Width*0.5f; 
	m_edge_plt.y    = (f32)(1.0f/tanf(m_fov*0.5f) * (m_Height*0.5f));
	m_edge_plt.z    = m_Height*0.5f;
	assert( fabs(acos_tpl(Vec3r(0,m_edge_plt.y,m_edge_plt.z).GetNormalized().y)*2-m_fov)<0.001 );

	m_edge_nlt.x	= nearplane*m_edge_plt.x/m_edge_plt.y;	 
	m_edge_nlt.y	=	nearplane; 
	m_edge_nlt.z	= nearplane*m_edge_plt.z/m_edge_plt.y;  

	//calculate the left/upper edge of the far-plane (=not rotated) 
	m_edge_flt.x	= m_edge_plt.x  * (farpane/m_edge_plt.y);	 
	m_edge_flt.y	=	farpane; 
	m_edge_flt.z	= m_edge_plt.z  * (farpane/m_edge_plt.y);  
	UpdateFrustum();
}

/*!
 *
 *  Updates all parameters required by the render-engine:
 *
 *	3d-view-frustum and all matrices
 *
 */
inline void CCamera::UpdateFrustum()
{
	//-------------------------------------------------------------------
	//--- calculate frustum-edges of projection-plane in CAMERA-SPACE ---
	//-------------------------------------------------------------------
	Matrix33 m33=Matrix33(m_Matrix);
	m_cltp=m33*Vec3(+m_edge_plt.x,+m_edge_plt.y,+m_edge_plt.z);
	m_crtp=m33*Vec3(-m_edge_plt.x,+m_edge_plt.y,+m_edge_plt.z);
	m_clbp=m33*Vec3(+m_edge_plt.x,+m_edge_plt.y,-m_edge_plt.z);
	m_crbp=m33*Vec3(-m_edge_plt.x,+m_edge_plt.y,-m_edge_plt.z);

	m_cltn=m33*Vec3(+m_edge_nlt.x,+m_edge_nlt.y,+m_edge_nlt.z);
	m_crtn=m33*Vec3(-m_edge_nlt.x,+m_edge_nlt.y,+m_edge_nlt.z);
	m_clbn=m33*Vec3(+m_edge_nlt.x,+m_edge_nlt.y,-m_edge_nlt.z);
	m_crbn=m33*Vec3(-m_edge_nlt.x,+m_edge_nlt.y,-m_edge_nlt.z);

	m_cltf=m33*Vec3(+m_edge_flt.x,+m_edge_flt.y,+m_edge_flt.z);
	m_crtf=m33*Vec3(-m_edge_flt.x,+m_edge_flt.y,+m_edge_flt.z);
	m_clbf=m33*Vec3(+m_edge_flt.x,+m_edge_flt.y,-m_edge_flt.z);
	m_crbf=m33*Vec3(-m_edge_flt.x,+m_edge_flt.y,-m_edge_flt.z);

	//-------------------------------------------------------------------------------
	//---  calculate the six frustum-planes using the fustum edges in world-space ---
	//-------------------------------------------------------------------------------
	m_fp[FR_PLANE_NEAR  ]	=	Plane::CreatePlane( m_crtn+GetPosition(),m_cltn+GetPosition(),m_crbn+GetPosition() );
	m_fp[FR_PLANE_RIGHT ]	=	Plane::CreatePlane( m_crbf+GetPosition(),m_crtf+GetPosition(),GetPosition() );
	m_fp[FR_PLANE_LEFT  ]	=	Plane::CreatePlane( m_cltf+GetPosition(),m_clbf+GetPosition(),GetPosition() );
	m_fp[FR_PLANE_TOP   ]	=	Plane::CreatePlane( m_crtf+GetPosition(),m_cltf+GetPosition(),GetPosition() );
	m_fp[FR_PLANE_BOTTOM]	=	Plane::CreatePlane( m_clbf+GetPosition(),m_crbf+GetPosition(),GetPosition() );
	m_fp[FR_PLANE_FAR   ]	=	Plane::CreatePlane( m_crtf+GetPosition(),m_crbf+GetPosition(),m_cltf+GetPosition() );  //clip-plane

	uint32 rh = m_Matrix.IsOrthonormalRH();
	if (rh==0)
	{
		m_fp[FR_PLANE_NEAR  ]=-m_fp[FR_PLANE_NEAR  ];
		m_fp[FR_PLANE_RIGHT ]=-m_fp[FR_PLANE_RIGHT ];
		m_fp[FR_PLANE_LEFT  ]=-m_fp[FR_PLANE_LEFT  ];
		m_fp[FR_PLANE_TOP   ]=-m_fp[FR_PLANE_TOP   ];
		m_fp[FR_PLANE_BOTTOM]=-m_fp[FR_PLANE_BOTTOM];
		m_fp[FR_PLANE_FAR   ]=-m_fp[FR_PLANE_FAR   ];  //clip-plane
	}

	union f32_u
	{
		float floatVal;
		uint32 uintVal;
	};

	for (int i=0;i<FRUSTUM_PLANES;i++) {

		f32_u ux; ux.floatVal = m_fp[i].n.x;
		f32_u uy; uy.floatVal = m_fp[i].n.y;
		f32_u uz; uz.floatVal = m_fp[i].n.z;
		uint32 bitX	=	ux.uintVal>>31;
		uint32 bitY	=	uy.uintVal>>31;
		uint32 bitZ	=	uz.uintVal>>31;
		m_idx1[i]=bitX*3+0; m_idx2[i]=(1-bitX)*3+0;
		m_idy1[i]=bitY*3+1;	m_idy2[i]=(1-bitY)*3+1;
		m_idz1[i]=bitZ*3+2;	m_idz2[i]=(1-bitZ)*3+2;
	}

 m_OccPosition=GetPosition();
}

// Summary
//   Return frustum vertices in world space
// Description
//   Takes pointer to array of 8 elements
inline void CCamera::GetFrustumVertices(Vec3 * pVerts) const
{
	Matrix33 m33=Matrix33(m_Matrix);

	int i=0;

	pVerts[i++] = m33*Vec3(+m_edge_flt.x,+m_edge_flt.y,+m_edge_flt.z)+GetPosition();
	pVerts[i++] = m33*Vec3(+m_edge_flt.x,+m_edge_flt.y,-m_edge_flt.z)+GetPosition();
	pVerts[i++] = m33*Vec3(-m_edge_flt.x,+m_edge_flt.y,-m_edge_flt.z)+GetPosition();
	pVerts[i++] = m33*Vec3(-m_edge_flt.x,+m_edge_flt.y,+m_edge_flt.z)+GetPosition();

	pVerts[i++] = m33*Vec3(+m_edge_nlt.x,+m_edge_nlt.y,+m_edge_nlt.z)+GetPosition();
	pVerts[i++] = m33*Vec3(+m_edge_nlt.x,+m_edge_nlt.y,-m_edge_nlt.z)+GetPosition();
	pVerts[i++] = m33*Vec3(-m_edge_nlt.x,+m_edge_nlt.y,-m_edge_nlt.z)+GetPosition();
	pVerts[i++] = m33*Vec3(-m_edge_nlt.x,+m_edge_nlt.y,+m_edge_nlt.z)+GetPosition();
}

inline void CCamera::GetFrustumVerticesCam(Vec3 * pVerts) const
{
	int i=0;

	//near plane
	pVerts[i++] = Vec3(+m_edge_nlt.x,+m_edge_nlt.y,+m_edge_nlt.z);
	pVerts[i++] = Vec3(+m_edge_nlt.x,+m_edge_nlt.y,-m_edge_nlt.z);
	pVerts[i++] = Vec3(-m_edge_nlt.x,+m_edge_nlt.y,-m_edge_nlt.z);
	pVerts[i++] = Vec3(-m_edge_nlt.x,+m_edge_nlt.y,+m_edge_nlt.z);

	//far plane
	pVerts[i++] = Vec3(+m_edge_flt.x,+m_edge_flt.y,+m_edge_flt.z);
	pVerts[i++] = Vec3(+m_edge_flt.x,+m_edge_flt.y,-m_edge_flt.z);
	pVerts[i++] = Vec3(-m_edge_flt.x,+m_edge_flt.y,-m_edge_flt.z);
	pVerts[i++] = Vec3(-m_edge_flt.x,+m_edge_flt.y,+m_edge_flt.z);
}

//get near-plane vertices
ILINE const Vec3& CCamera::GetNPVertex(int nId) const	{
	switch(nId)	{
			case 0:	return m_clbn;
			case 1:	return m_cltn;
			case 2:	return m_crtn;
			case 3:	return m_crbn;
	}	assert(0); return m_clbn;
}
//get far-plane vertices
ILINE const Vec3& CCamera::GetFPVertex(int nId) const	{
	switch(nId)	{
			case 0:	return m_clbf;
			case 1:	return m_cltf;
			case 2:	return m_crtf;
			case 3:	return m_crbf;
	}	assert(0); return m_clbf;
}
ILINE const Vec3& CCamera::GetPPVertex(int nId) const	{
	switch(nId)	{
			case 0:	return m_clbp;
			case 1:	return m_cltp;
			case 2:	return m_crtp;
			case 3:	return m_crbp;
	}	assert(0); return m_clbp;
}

// Description
//   Check if a point lies within camera's frustum
//
// Example
//   u8 InOut=camera.IsPointVisible(point);
//
// return values
//   CULL_EXCLUSION = point outside of frustum      
//   CULL_INTERSECT = point inside of frustum
inline bool	CCamera::IsPointVisible(const Vec3 &p) const {
	if ((m_fp[FR_PLANE_NEAR  ]|p) > 0) return CULL_EXCLUSION;
	if ((m_fp[FR_PLANE_RIGHT ]|p) > 0) return CULL_EXCLUSION;
	if ((m_fp[FR_PLANE_LEFT  ]|p) > 0) return CULL_EXCLUSION;
	if ((m_fp[FR_PLANE_TOP   ]|p) > 0) return CULL_EXCLUSION;
	if ((m_fp[FR_PLANE_BOTTOM]|p) > 0) return CULL_EXCLUSION;
	if ((m_fp[FR_PLANE_FAR   ]|p) > 0) return CULL_EXCLUSION;
	return CULL_OVERLAP;
}

// Description
//   Conventional method to check if a sphere and the camera-frustum overlap 
//   The center of the sphere is assumed to be in world-space.  
//
// Example
//   u8 InOut=camera.IsSphereVisible_F(sphere);
//
// return values
//   CULL_EXCLUSION = sphere outside of frustum (very fast rejection-test)      
//   CULL_INTERSECT = sphere and frustum intersects or sphere in completely inside frustum
inline bool CCamera::IsSphereVisible_F( const Sphere &s ) const {
	if ((m_fp[0]|s.center) > s.radius) return CULL_EXCLUSION;
	if ((m_fp[1]|s.center) > s.radius) return CULL_EXCLUSION;
	if ((m_fp[2]|s.center) > s.radius) return CULL_EXCLUSION;
	if ((m_fp[3]|s.center) > s.radius) return CULL_EXCLUSION;
	if ((m_fp[4]|s.center) > s.radius) return CULL_EXCLUSION;
	if ((m_fp[5]|s.center) > s.radius) return CULL_EXCLUSION;
	return CULL_OVERLAP; 
}

// Description
//   Conventional method to check if a sphere and the camera-frustum overlap, or 
//   if the sphere is completely inside the camera-frustum. The center of the 
//   sphere is assumed to be in world-space.
//
// Example
//   u8 InOut=camera.IsSphereVisible_FH(sphere);
//
// return values
//   CULL_EXCLUSION   = sphere outside of frustum (very fast rejection-test)      
//   CULL_INTERSECT   = sphere intersects the borders of the frustum, further checks necessary
//   CULL_INCLUSION   = sphere is complete inside the frustum, no further checks necessary
inline uint8 CCamera::IsSphereVisible_FH( const Sphere &s) const {
	f32 nc,rc,lc,tc,bc,cc;
	if ((nc=m_fp[0]|s.center)>s.radius) return CULL_EXCLUSION;
	if ((rc=m_fp[1]|s.center)>s.radius) return CULL_EXCLUSION;
	if ((lc=m_fp[2]|s.center)>s.radius) return CULL_EXCLUSION;
	if ((tc=m_fp[3]|s.center)>s.radius) return CULL_EXCLUSION;
	if ((bc=m_fp[4]|s.center)>s.radius) return CULL_EXCLUSION;
	if ((cc=m_fp[5]|s.center)>s.radius) return CULL_EXCLUSION;

	//now we have to check if it is completely in frustum
	f32 r=-s.radius;
	if (nc>r) return CULL_OVERLAP;
	if (lc>r) return CULL_OVERLAP;
	if (rc>r) return CULL_OVERLAP;
	if (tc>r) return CULL_OVERLAP;
	if (bc>r) return CULL_OVERLAP;
	if (cc>r) return CULL_OVERLAP;
	return CULL_INCLUSION;

}
// Description
//   Simple approach to check if an AABB and the camera-frustum overlap. The AABB 
//   is assumed to be in world-space. This is a very fast method, just one single 
//   dot-product is necessary to check an AABB against a plane. Actually there 
//   is no significant speed-different between culling a sphere or an AABB.
//
// Example
//   bool InOut=camera.IsAABBVisible_F(aabb);
//
// return values
//   CULL_EXCLUSION   = AABB outside of frustum (very fast rejection-test)      
//   CULL_OVERLAP     = AABB either intersects the borders of the frustum or is totally inside
inline bool CCamera::IsAABBVisible_F( const AABB& aabb ) const
{ 
	const f32* p=&aabb.min.x; uint32 x,y,z;
	x=m_idx1[0]; y=m_idy1[0]; z=m_idz1[0]; if ( (m_fp[0]|Vec3(p[x],p[y],p[z])) > 0) return CULL_EXCLUSION;	
	x=m_idx1[1]; y=m_idy1[1]; z=m_idz1[1]; if ( (m_fp[1]|Vec3(p[x],p[y],p[z])) > 0) return CULL_EXCLUSION;	
	x=m_idx1[2]; y=m_idy1[2]; z=m_idz1[2]; if ( (m_fp[2]|Vec3(p[x],p[y],p[z])) > 0) return CULL_EXCLUSION;	
	x=m_idx1[3]; y=m_idy1[3]; z=m_idz1[3]; if ( (m_fp[3]|Vec3(p[x],p[y],p[z])) > 0) return CULL_EXCLUSION;	
	x=m_idx1[4]; y=m_idy1[4]; z=m_idz1[4]; if ( (m_fp[4]|Vec3(p[x],p[y],p[z])) > 0) return CULL_EXCLUSION;	
	x=m_idx1[5]; y=m_idy1[5]; z=m_idz1[5]; if ( (m_fp[5]|Vec3(p[x],p[y],p[z])) > 0) return CULL_EXCLUSION;	
	return CULL_OVERLAP;	  
}
// Description
//   Hierarchical approach to check if an AABB and the camera-frustum overlap, or if the AABB 
//   is totally inside the camera-frustum. The AABB is assumed to be in world-space.  
//
// Example
//   int InOut=camera.IsAABBVisible_FH(aabb);
//
// return values
//   CULL_EXCLUSION   = AABB outside of frustum (very fast rejection-test)      
//   CULL_OVERLAP     = AABB intersects the borders of the frustum, further checks necessary
//   CULL_INCLUSION   = AABB is complete inside the frustum, no further checks necessary
inline uint8 CCamera::IsAABBVisible_FH( const AABB& aabb, bool *pAllInside ) const
{
  assert(pAllInside && *pAllInside==false);
  if (IsAABBVisible_F(aabb)==CULL_EXCLUSION) { return CULL_EXCLUSION; }
	const f32* p=&aabb.min.x; uint32 x,y,z;
	x=m_idx2[0]; y=m_idy2[0]; z=m_idz2[0];	if ( (m_fp[0]|Vec3(p[x],p[y],p[z])) >0) return CULL_OVERLAP;	
	x=m_idx2[1]; y=m_idy2[1]; z=m_idz2[1];	if ( (m_fp[1]|Vec3(p[x],p[y],p[z])) >0) return CULL_OVERLAP;	
	x=m_idx2[2]; y=m_idy2[2]; z=m_idz2[2];	if ( (m_fp[2]|Vec3(p[x],p[y],p[z])) >0) return CULL_OVERLAP;	
	x=m_idx2[3]; y=m_idy2[3]; z=m_idz2[3];	if ( (m_fp[3]|Vec3(p[x],p[y],p[z])) >0) return CULL_OVERLAP;	
	x=m_idx2[4]; y=m_idy2[4]; z=m_idz2[4];	if ( (m_fp[4]|Vec3(p[x],p[y],p[z])) >0) return CULL_OVERLAP;	
	x=m_idx2[5]; y=m_idy2[5]; z=m_idz2[5];	if ( (m_fp[5]|Vec3(p[x],p[y],p[z])) >0) return CULL_OVERLAP;	
  *pAllInside=true;	
	return CULL_INCLUSION;
}
// Description
//   Hierarchical approach to check if an AABB and the camera-frustum overlap, or if the AABB 
//   is totally inside the camera-frustum. The AABB is assumed to be in world-space.  
//
// Example
//   int InOut=camera.IsAABBVisible_FH(aabb);
//
// return values
//   CULL_EXCLUSION   = AABB outside of frustum (very fast rejection-test)      
//   CULL_OVERLAP     = AABB intersects the borders of the frustum, further checks necessary
//   CULL_INCLUSION   = AABB is complete inside the frustum, no further checks necessary
inline uint8 CCamera::IsAABBVisible_FH( const AABB& aabb ) const
{
  if (IsAABBVisible_F(aabb)==CULL_EXCLUSION) return CULL_EXCLUSION;
  const f32* p=&aabb.min.x; uint32 x,y,z;
  x=m_idx2[0]; y=m_idy2[0]; z=m_idz2[0];	if ( (m_fp[0]|Vec3(p[x],p[y],p[z])) >0) return CULL_OVERLAP;	
  x=m_idx2[1]; y=m_idy2[1]; z=m_idz2[1];	if ( (m_fp[1]|Vec3(p[x],p[y],p[z])) >0) return CULL_OVERLAP;	
  x=m_idx2[2]; y=m_idy2[2]; z=m_idz2[2];	if ( (m_fp[2]|Vec3(p[x],p[y],p[z])) >0) return CULL_OVERLAP;	
  x=m_idx2[3]; y=m_idy2[3]; z=m_idz2[3];	if ( (m_fp[3]|Vec3(p[x],p[y],p[z])) >0) return CULL_OVERLAP;	
  x=m_idx2[4]; y=m_idy2[4]; z=m_idz2[4];	if ( (m_fp[4]|Vec3(p[x],p[y],p[z])) >0) return CULL_OVERLAP;	
  x=m_idx2[5]; y=m_idy2[5]; z=m_idz2[5];	if ( (m_fp[5]|Vec3(p[x],p[y],p[z])) >0) return CULL_OVERLAP;	
  return CULL_INCLUSION;
}


// Description
//  This function checks if an AABB and the camera-frustum overlap. The AABB is assumed to be in world-space. 
//  This test can reject even such AABBs that overlap a frustum-plane far outside the view-frustum. 
// IMPORTANT: 
//  This function is only useful if you really need exact-culling. 
//  It's about 30% slower then "IsAABBVisible_F(aabb)"   
//
// Example:
//  int InOut=camera.IsAABBVisible_E(aabb);
//
// return values:
//  CULL_EXCLUSION   = AABB outside of frustum (very fast rejection-test)      
//  CULL_OVERLAP     = AABB either intersects the borders of the frustum or is totally inside
inline bool CCamera::IsAABBVisible_E(const AABB& aabb) const
{
	uint8 o=IsAABBVisible_FH(aabb);
	if (o==CULL_EXCLUSION) return CULL_EXCLUSION;
	if (o==CULL_INCLUSION) return CULL_OVERLAP;
	return AdditionalCheck(aabb); //result is either "exclusion" or "overlap"
}

// Description:
//   Improved approach to check if an AABB and the camera-frustum overlap, or if the AABB 
//   is totally inside the camera-frustum. The AABB is assumed to be in world-space. This 
//   test can reject even such AABBs that overlap a frustum-plane far outside the view-frustum. 
// IMPORTANT: 
//   This function is only useful if you really need exact-culling. 
//   It's about 30% slower then "IsAABBVisible_FH(aabb)"   
//
// Example:
//   int InOut=camera.IsAABBVisible_EH(aabb);
//
// return values:
//   CULL_EXCLUSION   = AABB outside of frustum (very fast rejection-test)      
//   CULL_OVERLAP     = AABB intersects the borders of the frustum, further checks necessary
//   CULL_INCLUSION   = AABB is complete inside the frustum, no further checks necessary
inline uint8 CCamera::IsAABBVisible_EH(const AABB& aabb, bool* pAllInside ) const
{
	uint8 o=IsAABBVisible_FH(aabb,pAllInside);
	if (o==CULL_EXCLUSION) return CULL_EXCLUSION;
	if (o==CULL_INCLUSION) return CULL_INCLUSION;
	return AdditionalCheck(aabb); //result is either "exclusion" or "overlap"
}

// Description:
//   Improved approach to check if an AABB and the camera-frustum overlap, or if the AABB 
//   is totally inside the camera-frustum. The AABB is assumed to be in world-space. This 
//   test can reject even such AABBs that overlap a frustum-plane far outside the view-frustum. 
// IMPORTANT: 
//   This function is only useful if you really need exact-culling. 
//   It's about 30% slower then "IsAABBVisible_FH(aabb)"   
//
// Example:
//   int InOut=camera.IsAABBVisible_EH(aabb);
//
// return values:
//   CULL_EXCLUSION   = AABB outside of frustum (very fast rejection-test)      
//   CULL_OVERLAP     = AABB intersects the borders of the frustum, further checks necessary
//   CULL_INCLUSION   = AABB is complete inside the frustum, no further checks necessary
inline uint8 CCamera::IsAABBVisible_EH(const AABB& aabb) const
{
  uint8 o=IsAABBVisible_FH(aabb);
  if (o==CULL_EXCLUSION) return CULL_EXCLUSION;
  if (o==CULL_INCLUSION) return CULL_INCLUSION;
  return AdditionalCheck(aabb); //result is either "exclusion" or "overlap"
}

// Description:
//  Makes culling taking into account presence of m_pMultiCamera
//  If m_pMultiCamera exists - object is visible if at least one of cameras see's it
//
// return values:
//  true - box visible
//  true - not visible
inline bool CCamera::IsAABBVisible_EHM( const AABB& aabb, bool *pAllInside ) const
{
  assert(pAllInside && *pAllInside==false);
  bool bVisible = false;
  return bVisible;
}

// Description:
//  Makes culling taking into account presence of m_pMultiCamera
//  If m_pMultiCamera exists - object is visible if at least one of cameras see's it
//
// return values:
//  true - box visible
//  true - not visible
inline bool CCamera::IsAABBVisible_EM( const AABB& aabb ) const
{
//   if(!m_pMultiCamera) // use main camera
//     return IsAABBVisible_E(aabb) != CULL_EXCLUSION;
// 
//   // check several parallel cameras - object is visible if at least one camera see's it
// 
//   for(int i=0; i<m_pMultiCamera->Count(); i++)
//     if( m_pMultiCamera->GetAt(i).IsAABBVisible_E(aabb) )
//       return true;

  return false;
}


// Description:
//  Makes culling taking into account presence of m_pMultiCamera
//  If m_pMultiCamera exists - object is visible if at least one of cameras see's it
//
// return values:
//  true - box visible
//  true - not visible
inline bool CCamera::IsAABBVisible_FM( const AABB& aabb ) const
{
//   if(!m_pMultiCamera) // use main camera
//     return IsAABBVisible_F(aabb) != CULL_EXCLUSION;
// 
//   // check several parallel cameras - object is visible if at least one camera see's it
// 
//   for(int i=0; i<m_pMultiCamera.size(); i++)
//     if( m_pMultiCamera[i].IsAABBVisible_F(aabb) )
//       return true;

  return false;
}




// Description
//   Fast check if an OBB and the camera-frustum overlap, using the separating-axis-theorem (SAT) 
//   The center of the OOBB is assumed to be in world-space.
// NOTE: even if the OBB is totally inside the frustum, this function returns CULL_OVERLAP 
//   For hierarchical frustum-culling this function is not perfect.
//
// Example:
//   bool InOut=camera.IsOBBVisibleFast(obb);
//
// return values:
//   CULL_EXCLUSION = OBB outside of frustum (very fast rejection-test)      
//   CULL_OVERLAP   = OBB and frustum intersects or OBB in totally inside frustum
inline bool CCamera::IsOBBVisible_F( const Vec3& wpos, const OBB& obb ) const {
	assert( obb.m33.IsOrthonormalRH(0.001f) );	

	//transform the obb-center into world-space
	Vec3 p	=	obb.m33*obb.c + wpos;

	//extract the orientation-vectors from the columns of the 3x3 matrix
	//and scale them by the half-lengths
	Vec3 ax	= obb.m33.GetColumn0()*obb.h.x;
	Vec3 ay	= obb.m33.GetColumn1()*obb.h.y;
	Vec3 az	= obb.m33.GetColumn2()*obb.h.z;

	//we project the axes of the OBB onto the normal of each of the 6 planes.
	//If the absolute value of the distance from the center of the OBB to the plane 
	//is larger then the "radius" of the OBB, then the OBB is outside the frustum.
	f32 t;
	if ((t=m_fp[0]|p)>0.0f)	if ( t > (fabsf(m_fp[0].n|ax)+fabsf(m_fp[0].n|ay)+fabsf(m_fp[0].n|az))) return CULL_EXCLUSION;
	if ((t=m_fp[1]|p)>0.0f)	if ( t > (fabsf(m_fp[1].n|ax)+fabsf(m_fp[1].n|ay)+fabsf(m_fp[1].n|az))) return CULL_EXCLUSION;
	if ((t=m_fp[2]|p)>0.0f)	if ( t > (fabsf(m_fp[2].n|ax)+fabsf(m_fp[2].n|ay)+fabsf(m_fp[2].n|az))) return CULL_EXCLUSION;
	if ((t=m_fp[3]|p)>0.0f)	if ( t > (fabsf(m_fp[3].n|ax)+fabsf(m_fp[3].n|ay)+fabsf(m_fp[3].n|az))) return CULL_EXCLUSION;
	if ((t=m_fp[4]|p)>0.0f)	if ( t > (fabsf(m_fp[4].n|ax)+fabsf(m_fp[4].n|ay)+fabsf(m_fp[4].n|az))) return CULL_EXCLUSION;
	if ((t=m_fp[5]|p)>0.0f)	if ( t > (fabsf(m_fp[5].n|ax)+fabsf(m_fp[5].n|ay)+fabsf(m_fp[5].n|az))) return CULL_EXCLUSION;

	//probably the OBB is visible! 
	//With this test we can't be sure if the OBB partially visible or totally included or  
	//totally outside the frustum but still intersecting one of the 6 planes (=worst case)
	return CULL_OVERLAP; 

}

ILINE uint8 CCamera::IsOBBVisible_FH( const Vec3& wpos, const OBB& obb ) const 
{
	//not implemented yet
	return CULL_EXCLUSION; 
}

// Description:
//   This function checks if an OBB and the camera-frustum overlap. 
//   This test can reject even such OBBs that overlap a frustum-plane 
//   far outside the view-frustum. 
// IMPORTANT: It is about 10% slower then "IsOBBVisibleFast(obb)"   
//
// Example:
//   int InOut=camera.IsOBBVisible_E(OBB);
//
// return values:
//   CULL_EXCLUSION   = OBB outside of frustum (very fast rejection-test)      
//   CULL_OVERLAP     = OBB intersects the borders of the frustum or is totally inside
inline bool CCamera::IsOBBVisible_E( const Vec3& wpos, const OBB& obb, f32 uscale=1.0f ) const {
	assert( obb.m33.IsOrthonormalRH(0.001f) );	

	//transform the obb-center into world-space
	Vec3 p	=	obb.m33*obb.c*uscale + wpos;

	//extract the orientation-vectors from the columns of the 3x3 matrix
	//and scale them by the half-lengths
	Vec3 ax	= obb.m33.GetColumn0()*obb.h.x*uscale;
	Vec3 ay	= obb.m33.GetColumn1()*obb.h.y*uscale;
	Vec3 az	= obb.m33.GetColumn2()*obb.h.z*uscale;

	//we project the axes of the OBB onto the normal of each of the 6 planes.
	//If the absolute value of the distance from the center of the OBB to the plane 
	//is larger then the "radius" of the OBB, then the OBB is outside the frustum.
	f32 t0,t1,t2,t3,t4,t5;
	bool mt0,mt1,mt2,mt3,mt4,mt5;
	if (mt0=(t0=m_fp[0]|p)>0.0f)	if ( t0 > (fabsf(m_fp[0].n|ax)+fabsf(m_fp[0].n|ay)+fabsf(m_fp[0].n|az))) return CULL_EXCLUSION;
	if (mt1=(t1=m_fp[1]|p)>0.0f)	if ( t1 > (fabsf(m_fp[1].n|ax)+fabsf(m_fp[1].n|ay)+fabsf(m_fp[1].n|az))) return CULL_EXCLUSION;
	if (mt2=(t2=m_fp[2]|p)>0.0f)	if ( t2 > (fabsf(m_fp[2].n|ax)+fabsf(m_fp[2].n|ay)+fabsf(m_fp[2].n|az))) return CULL_EXCLUSION;
	if (mt3=(t3=m_fp[3]|p)>0.0f)	if ( t3 > (fabsf(m_fp[3].n|ax)+fabsf(m_fp[3].n|ay)+fabsf(m_fp[3].n|az))) return CULL_EXCLUSION;
	if (mt4=(t4=m_fp[4]|p)>0.0f)	if ( t4 > (fabsf(m_fp[4].n|ax)+fabsf(m_fp[4].n|ay)+fabsf(m_fp[4].n|az))) return CULL_EXCLUSION;
	if (mt5=(t5=m_fp[5]|p)>0.0f)	if ( t5 > (fabsf(m_fp[5].n|ax)+fabsf(m_fp[5].n|ay)+fabsf(m_fp[5].n|az))) return CULL_EXCLUSION;

	//if obb-center is in view-frustum, then stop further calculation
	if (!(mt0|mt1|mt2|mt3|mt4|mt5)) return CULL_OVERLAP;

	return AdditionalCheck( wpos,obb,uscale );
}


// Description:
//   Improved approach to check if an OBB and the camera-frustum intersect, or if the OBB 
//   is totally inside the camera-frustum. The bounding-box of the OBB is assumed to be 
//   in world-space. This test can reject even such OBBs that intersect a frustum-plane far 
//   outside the view-frustum
//
// Example:
//   int InOut=camera.IsOBBVisible_EH(obb);
//
// return values:
//   CULL_EXCLUSION   = OBB outside of frustum (very fast rejection-test)      
//   CULL_OVERLAP     = OBB intersects the borders of the frustum, further checks necessary
//   CULL_INCLUSION   = OBB is complete inside the frustum, no further checks necessary
inline uint8 CCamera::IsOBBVisible_EH( const Vec3& wpos, const OBB& obb, f32 uscale=1.0f ) const {
	assert( obb.m33.IsOrthonormalRH(0.001f) );	
	//transform the obb-center into world-space
	Vec3 p	=	obb.m33*obb.c*uscale + wpos;

	//extract the orientation-vectors from the columns of the 3x3 matrix
	//and scale them by the half-lengths
	Vec3 ax	= obb.m33.GetColumn0()*obb.h.x*uscale;
	Vec3 ay	= obb.m33.GetColumn1()*obb.h.y*uscale;
	Vec3 az	= obb.m33.GetColumn2()*obb.h.z*uscale;

	//we project the axes of the OBB onto the normal of each of the 6 planes.
	//If the absolute value of the distance from the center of the OBB to the plane 
	//is larger then the "radius" of the OBB, then the OBB is outside the frustum.
	f32 t0,t1,t2,t3,t4,t5;
	bool mt0,mt1,mt2,mt3,mt4,mt5;
	if (mt0=(t0=m_fp[0]|p)>0.0f)	if ( t0 > (fabsf(m_fp[0].n|ax)+fabsf(m_fp[0].n|ay)+fabsf(m_fp[0].n|az))) return CULL_EXCLUSION;
	if (mt1=(t1=m_fp[1]|p)>0.0f)	if ( t1 > (fabsf(m_fp[1].n|ax)+fabsf(m_fp[1].n|ay)+fabsf(m_fp[1].n|az))) return CULL_EXCLUSION;
	if (mt2=(t2=m_fp[2]|p)>0.0f)	if ( t2 > (fabsf(m_fp[2].n|ax)+fabsf(m_fp[2].n|ay)+fabsf(m_fp[2].n|az))) return CULL_EXCLUSION;
	if (mt3=(t3=m_fp[3]|p)>0.0f)	if ( t3 > (fabsf(m_fp[3].n|ax)+fabsf(m_fp[3].n|ay)+fabsf(m_fp[3].n|az))) return CULL_EXCLUSION;
	if (mt4=(t4=m_fp[4]|p)>0.0f)	if ( t4 > (fabsf(m_fp[4].n|ax)+fabsf(m_fp[4].n|ay)+fabsf(m_fp[4].n|az))) return CULL_EXCLUSION;
	if (mt5=(t5=m_fp[5]|p)>0.0f)	if ( t5 > (fabsf(m_fp[5].n|ax)+fabsf(m_fp[5].n|ay)+fabsf(m_fp[5].n|az))) return CULL_EXCLUSION;

	//check if obb-center is in view-frustum
	if (!(mt0|mt1|mt2|mt3|mt4|mt5)) {
		//yes, it is! 
		//and now check if OBB is totally included
		if ( -t0 < (fabsf(m_fp[0].n|ax)+fabsf(m_fp[0].n|ay)+fabsf(m_fp[0].n|az))) return CULL_OVERLAP;
		if ( -t1 < (fabsf(m_fp[1].n|ax)+fabsf(m_fp[1].n|ay)+fabsf(m_fp[1].n|az))) return CULL_OVERLAP;
		if ( -t2 < (fabsf(m_fp[2].n|ax)+fabsf(m_fp[2].n|ay)+fabsf(m_fp[2].n|az))) return CULL_OVERLAP;
		if ( -t3 < (fabsf(m_fp[3].n|ax)+fabsf(m_fp[3].n|ay)+fabsf(m_fp[3].n|az))) return CULL_OVERLAP;
		if ( -t4 < (fabsf(m_fp[4].n|ax)+fabsf(m_fp[4].n|ay)+fabsf(m_fp[4].n|az))) return CULL_OVERLAP;
		if ( -t5 < (fabsf(m_fp[5].n|ax)+fabsf(m_fp[5].n|ay)+fabsf(m_fp[5].n|az))) return CULL_OVERLAP;
		return CULL_INCLUSION; 
	}
	return AdditionalCheck( wpos,obb,uscale );
}

//------------------------------------------------------------------------------
//---                         ADDITIONAL-TEST                                ---
//------------------------------------------------------------------------------

extern uint8 BoxSides[];

// Description:
//   A box can easily straddle one of the view-frustum planes far
//   outside the view-frustum and in this case the previous test would
//   return CULL_OVERLAP.
//
// Note:  With this check, we make sure the AABB is really not visble
inline bool CCamera::AdditionalCheck( const AABB& aabb ) const
{
	Vec3_f64 m = (aabb.min+aabb.max)*0.5;	
	uint32 o=1; //will be reset to 0 if center is outside
	o&=isneg(m_fp[0]|m);	o&=isneg(m_fp[2]|m);	o&=isneg(m_fp[3]|m);
	o&=isneg(m_fp[4]|m);	o&=isneg(m_fp[5]|m);	o&=isneg(m_fp[1]|m);
	if (o) return CULL_OVERLAP; //if obb-center is in view-frustum, then stop further calculation

	Vec3_f64 vmin(aabb.min-GetPosition());  //AABB in camera-space
	Vec3_f64 vmax(aabb.max-GetPosition());  //AABB in camera-space

	uint32 frontx8=0; // make the flags using the fact that the upper bit in f32 is its sign

	union f64_u
	{
		f64 floatVal;
		int64 intVal;
	};
	f64_u uminx, uminy, uminz, umaxx, umaxy, umaxz;
	uminx.floatVal = vmin.x;	uminy.floatVal = vmin.y;	uminz.floatVal = vmin.z;
	umaxx.floatVal = vmax.x;	umaxy.floatVal = vmax.y;	umaxz.floatVal = vmax.z;

	frontx8 |= (-uminx.intVal>>0x3f) & 0x008; //if (AABB.min.x>0.0f)  frontx8|=0x008;
	frontx8 |= ( umaxx.intVal>>0x3f) & 0x010; //if (AABB.max.x<0.0f)  frontx8|=0x010;
	frontx8 |= (-uminy.intVal>>0x3f) & 0x020; //if (AABB.min.y>0.0f)  frontx8|=0x020;
	frontx8 |= ( umaxy.intVal>>0x3f) & 0x040; //if (AABB.max.y<0.0f)  frontx8|=0x040;
	frontx8 |= (-uminz.intVal>>0x3f) & 0x080; //if (AABB.min.z>0.0f)  frontx8|=0x080;
	frontx8 |= ( umaxz.intVal>>0x3f) & 0x100; //if (AABB.max.z<0.0f)  frontx8|=0x100;

	//check if camera is inside the aabb
	if (frontx8==0)	return CULL_OVERLAP; //AABB is patially visible

	Vec3_f64 v[8] = {
		Vec3_f64(vmin.x,vmin.y,vmin.z),
			Vec3_f64(vmax.x,vmin.y,vmin.z),
			Vec3_f64(vmin.x,vmax.y,vmin.z),
			Vec3_f64(vmax.x,vmax.y,vmin.z),
			Vec3_f64(vmin.x,vmin.y,vmax.z),
			Vec3_f64(vmax.x,vmin.y,vmax.z),
			Vec3_f64(vmin.x,vmax.y,vmax.z),
			Vec3_f64(vmax.x,vmax.y,vmax.z)
	};

	//---------------------------------------------------------------------
	//---            find the silhouette-vertices of the AABB            ---
	//---------------------------------------------------------------------
	uint32 p0	=	BoxSides[frontx8+0];
	uint32 p1	=	BoxSides[frontx8+1];
	uint32 p2	=	BoxSides[frontx8+2];
	uint32 p3	=	BoxSides[frontx8+3];
	uint32 p4	=	BoxSides[frontx8+4];
	uint32 p5	=	BoxSides[frontx8+5];
	uint32 sideamount=BoxSides[frontx8+7];

	if(sideamount==4) {
		//--------------------------------------------------------------------------
		//---     we take the 4 vertices of projection-plane in cam-space,       ---
		//-----  and clip them against the 4 side-frustum-planes of the AABB        -
		//--------------------------------------------------------------------------
		Vec3_f64 s0=v[p0]%v[p1];	
		if (  (s0|m_cltp)>0 && (s0|m_crtp)>0 && (s0|m_crbp)>0 && (s0|m_clbp)>0  ) return CULL_EXCLUSION;
		Vec3_f64 s1=v[p1]%v[p2];	
		if (  (s1|m_cltp)>0 && (s1|m_crtp)>0 && (s1|m_crbp)>0 && (s1|m_clbp)>0  ) return CULL_EXCLUSION;
		Vec3_f64 s2=v[p2]%v[p3];	
		if (  (s2|m_cltp)>0 && (s2|m_crtp)>0 && (s2|m_crbp)>0 && (s2|m_clbp)>0  ) return CULL_EXCLUSION;
		Vec3_f64 s3=v[p3]%v[p0];	
		if (  (s3|m_cltp)>0 && (s3|m_crtp)>0 && (s3|m_crbp)>0 && (s3|m_clbp)>0  ) return CULL_EXCLUSION;
	}

	if(sideamount==6) {
		//--------------------------------------------------------------------------
		//---     we take the 4 vertices of projection-plane in cam-space,       ---
		//---    and clip them against the 6 side-frustum-planes of the AABB      ---
		//--------------------------------------------------------------------------
		Vec3_f64 s0=v[p0]%v[p1];	
		if (  (s0|m_cltp)>0 && (s0|m_crtp)>0 && (s0|m_crbp)>0 && (s0|m_clbp)>0  ) return CULL_EXCLUSION;
		Vec3_f64 s1=v[p1]%v[p2];	
		if (  (s1|m_cltp)>0 && (s1|m_crtp)>0 && (s1|m_crbp)>0 && (s1|m_clbp)>0  ) return CULL_EXCLUSION;
		Vec3_f64 s2=v[p2]%v[p3];	
		if (  (s2|m_cltp)>0 && (s2|m_crtp)>0 && (s2|m_crbp)>0 && (s2|m_clbp)>0  ) return CULL_EXCLUSION;
		Vec3_f64 s3=v[p3]%v[p4];	
		if (  (s3|m_cltp)>0 && (s3|m_crtp)>0 && (s3|m_crbp)>0 && (s3|m_clbp)>0  ) return CULL_EXCLUSION;
		Vec3_f64 s4=v[p4]%v[p5];	
		if (  (s4|m_cltp)>0 && (s4|m_crtp)>0 && (s4|m_crbp)>0 && (s4|m_clbp)>0  ) return CULL_EXCLUSION;
		Vec3_f64 s5=v[p5]%v[p0];	
		if (  (s5|m_cltp)>0 && (s5|m_crtp)>0 && (s5|m_crbp)>0 && (s5|m_clbp)>0  ) return CULL_EXCLUSION;
	}

	return CULL_OVERLAP; //AABB is patially visible
}

//------------------------------------------------------------------------------
//---                         ADDITIONAL-TEST                                ---
//------------------------------------------------------------------------------

// Description:
//   A box can easily straddle one of the view-frustum planes far
//   outside the view-frustum and in this case the previous test would
//   return CULL_OVERLAP.
// Note:
//   With this check, we make sure the OBB is really not visible
inline bool CCamera::AdditionalCheck( const Vec3& wpos, const OBB& obb, f32 uscale ) const
{
	Vec3 CamInOBBSpace	= wpos-GetPosition();
	Vec3 iCamPos = -CamInOBBSpace*obb.m33;
	uint32 front8=0;
	AABB aabb=AABB((obb.c-obb.h)*uscale,(obb.c+obb.h)*uscale);
	if (iCamPos.x<aabb.min.x)  front8|=0x008;
	if (iCamPos.x>aabb.max.x)  front8|=0x010;
	if (iCamPos.y<aabb.min.y)  front8|=0x020;
	if (iCamPos.y>aabb.max.y)  front8|=0x040;
	if (iCamPos.z<aabb.min.z)  front8|=0x080;
	if (iCamPos.z>aabb.max.z)  front8|=0x100;

	if (front8==0) return CULL_OVERLAP;

	//the transformed OBB-vertices in cam-space
	Vec3 v[8] = { 
		obb.m33*Vec3(aabb.min.x,aabb.min.y,aabb.min.z)+CamInOBBSpace,
			obb.m33*Vec3(aabb.max.x,aabb.min.y,aabb.min.z)+CamInOBBSpace,
			obb.m33*Vec3(aabb.min.x,aabb.max.y,aabb.min.z)+CamInOBBSpace,
			obb.m33*Vec3(aabb.max.x,aabb.max.y,aabb.min.z)+CamInOBBSpace,
			obb.m33*Vec3(aabb.min.x,aabb.min.y,aabb.max.z)+CamInOBBSpace,
			obb.m33*Vec3(aabb.max.x,aabb.min.y,aabb.max.z)+CamInOBBSpace,
			obb.m33*Vec3(aabb.min.x,aabb.max.y,aabb.max.z)+CamInOBBSpace,
			obb.m33*Vec3(aabb.max.x,aabb.max.y,aabb.max.z)+CamInOBBSpace
	};

	//---------------------------------------------------------------------
	//---            find the silhouette-vertices of the OBB            ---
	//---------------------------------------------------------------------
	uint32 p0	=	BoxSides[front8+0];
	uint32 p1	=	BoxSides[front8+1];
	uint32 p2	=	BoxSides[front8+2];
	uint32 p3	=	BoxSides[front8+3];
	uint32 p4	=	BoxSides[front8+4];
	uint32 p5	=	BoxSides[front8+5];
	uint32 sideamount=BoxSides[front8+7];

	if(sideamount==4) {
		//--------------------------------------------------------------------------
		//---     we take the 4 vertices of projection-plane in cam-space,       ---
		//-----  and clip them against the 4 side-frustum-planes of the OBB        -
		//--------------------------------------------------------------------------
		Vec3 s0=v[p0]%v[p1];	
		if (  ((s0|m_cltp)>=0) && ((s0|m_crtp)>=0) && ((s0|m_crbp)>=0) && ((s0|m_clbp)>=0)  ) return CULL_EXCLUSION;
		Vec3 s1=v[p1]%v[p2];	
		if (  ((s1|m_cltp)>=0) && ((s1|m_crtp)>=0) && ((s1|m_crbp)>=0) && ((s1|m_clbp)>=0)  ) return CULL_EXCLUSION;
		Vec3 s2=v[p2]%v[p3];	
		if (  ((s2|m_cltp)>=0) && ((s2|m_crtp)>=0) && ((s2|m_crbp)>=0) && ((s2|m_clbp)>=0)  ) return CULL_EXCLUSION;
		Vec3 s3=v[p3]%v[p0];	
		if (  ((s3|m_cltp)>=0) && ((s3|m_crtp)>=0) && ((s3|m_crbp)>=0) && ((s3|m_clbp)>=0)  ) return CULL_EXCLUSION;
	}

	if(sideamount==6) {
		//--------------------------------------------------------------------------
		//---     we take the 4 vertices of projection-plane in cam-space,       ---
		//---    and clip them against the 6 side-frustum-planes of the OBB      ---
		//--------------------------------------------------------------------------
		Vec3 s0=v[p0]%v[p1];	
		if (  ((s0|m_cltp)>=0) && ((s0|m_crtp)>=0) && ((s0|m_crbp)>=0) && ((s0|m_clbp)>=0)  ) return CULL_EXCLUSION;
		Vec3 s1=v[p1]%v[p2];	
		if (  ((s1|m_cltp)>=0) && ((s1|m_crtp)>=0) && ((s1|m_crbp)>=0) && ((s1|m_clbp)>=0)  ) return CULL_EXCLUSION;
		Vec3 s2=v[p2]%v[p3];	
		if (  ((s2|m_cltp)>=0) && ((s2|m_crtp)>=0) && ((s2|m_crbp)>=0) && ((s2|m_clbp)>=0)  ) return CULL_EXCLUSION;
		Vec3 s3=v[p3]%v[p4];	
		if (  ((s3|m_cltp)>=0) && ((s3|m_crtp)>=0) && ((s3|m_crbp)>=0) && ((s3|m_clbp)>=0)  ) return CULL_EXCLUSION;
		Vec3 s4=v[p4]%v[p5];	
		if (  ((s4|m_cltp)>=0) && ((s4|m_crtp)>=0) && ((s4|m_crbp)>=0) && ((s4|m_clbp)>=0)  ) return CULL_EXCLUSION;
		Vec3 s5=v[p5]%v[p0];	
		if (  ((s5|m_cltp)>=0) && ((s5|m_crtp)>=0) && ((s5|m_crbp)>=0) && ((s5|m_clbp)>=0)  ) return CULL_EXCLUSION;
	}
	//now we are 100% sure that the OBB is visible on the screen
	return CULL_OVERLAP; 
}

#undef CAM_NO_INLINE // Not affecting other files at the moment

#endif
