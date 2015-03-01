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
// yikaiming (C) 2013
// gkENGINE Source File 
//
// Name:   	RenderCamera.h
// Desc:	
// 	
// 
// Author:  YiKaiming
// Date:	2013/6/21
// 
//////////////////////////////////////////////////////////////////////////

#ifndef _RenderCamera_h_
#define _RenderCamera_h_


//////////////////////////////////////////////////////////////////////////
// RenderCamera

class CRenderCamera
{
public:

	// X: Right
	// Y: Up
	// Z: Back (-Z = Forward)
	Vec3 X, Y, Z;            // Normalized camera coordinate-axis vectors.
	Vec3 Orig;               // Location of origin of camera system in world coords.
	float wL,wR,wB,wT;       // Window extents defined as a rect on NearPlane.
	float Near,Far;          // Distances to near and far planes (in Viewing dir).

	CRenderCamera();
	CRenderCamera(const CRenderCamera &Cam);
	void Copy(const CRenderCamera &Cam);

	void LookAt(const Vec3& Eye, const Vec3& ViewRefPt, const Vec3& ViewUp);
	void Perspective(float Yfov, float Aspect, float Ndist, float Fdist);
	void Frustum(float l, float r, float b, float t, float Ndist, float Fdist);

	//  void TightlyFitToSphere(const Vec3& Eye, const Vec3& ViewUp, const Vec3& Cntr, float RadX, float RadY);

	void GetLookAtParams(Vec3 *Eye, Vec3 *ViewRefPt, Vec3 *ViewUp) const;
	void GetPerspectiveParams(float *Yfov, float *Xfov, float *Aspect, float *Ndist, float *Fdist) const;
	void GetFrustumParams(float *l, float *r, float *b, float *t, float *Ndist, float *Fdist) const;
	const Vec3& wCOP() const; // WORLD COORDINATE CENTER-OF-PROJECTION (EYE)
	Vec3 ViewDir() const;     // VIEWING DIRECTION
	Vec3 ViewDirOffAxis() const;

	float* GetXform_Screen2Obj(float* M, int WW, int WH) const;
	float* GetXform_Obj2Screen(float* M, int WW, int WH) const;

	float* GetModelviewMatrix(float* M) const;
	float* GetProjectionMatrix(float* M) const;
	float* GetViewportMatrix(float* M, int WW, int WH) const;

	void SetModelviewMatrix(const float* M);

	//   float* GetInvModelviewMatrix(float* M) const;
	//   float* GetInvProjectionMatrix(float* M) const;
	//   float* GetInvViewportMatrix(float* M, int WW, int WH) const;

	Vec3 WorldToCam(const Vec3 &wP) const;
	float WorldToCamZ(const Vec3 &wP) const;
	Vec3 CamToWorld(const Vec3 &cP) const;

	void LoadIdentityXform();
	void Xform(const float M[16]);

	void Translate(const Vec3& trans);
	void Rotate(const float M[9]);

	void GetPixelRay(float sx, float sy, int ww, int wh, Vec3 *Start, Vec3 *Dir) const;

	void CalcVerts(Vec3 *V) const;    // CALCS EIGHT CORNERS OF VIEW-FRUSTUM

	void CalcTileVerts(Vec3 *V, f32 nPosX, f32 nPosY, f32 nGridSizeX, f32 nGridSizeY) const;    // CALCS EIGHT CORNERS FOR TILE OF VIEW-FRUSTUM 

	void CalcRegionVerts(Vec3 *V, const Vec2& vMin, const Vec2& vMax) const;

	void CalcTiledRegionVerts(Vec3 *V, Vec2& vMin, Vec2& vMax, f32 nPosX, f32 nPosY, f32 nGridSizeX, f32 nGridSizeY) const;

};

inline float* Frustum16fv(float* M, float l, float r, float b, float t, float n, float f)
{
	M[0]=(2*n)/(r-l); M[4]=0;           M[8]=(r+l)/(r-l);   M[12]=0;
	M[1]=0;           M[5]=(2*n)/(t-b); M[9]=(t+b)/(t-b);   M[13]=0;
	M[2]=0;           M[6]=0;           M[10]=-(f+n)/(f-n); M[14]=(-2*f*n)/(f-n);
	M[3]=0;           M[7]=0;           M[11]=-1;           M[15]=0;
	return M;
}

inline float* Viewing16fv(float* M, const Vec3 X, const Vec3 Y, const Vec3 Z, const Vec3 O)
{
	M[0]=X.x;  M[4]=X.y;  M[8]=X.z;  M[12]=-X|O;
	M[1]=Y.x;  M[5]=Y.y;  M[9]=Y.z;  M[13]=-Y|O;
	M[2]=Z.x;  M[6]=Z.y;  M[10]=Z.z; M[14]=-Z|O;
	M[3]=0;    M[7]=0;    M[11]=0;   M[15]=1;
	return M;
}


inline CRenderCamera::CRenderCamera()
{
	X.Set(1,0,0); Y.Set(0,1,0); Z.Set(0,0,1);
	Orig.Set(0,0,0);
	Near=1.4142f; Far=10; wL=-1; wR=1; wT=1; wB=-1;
}

inline CRenderCamera::CRenderCamera(const CRenderCamera &Cam)
{
	Copy(Cam);
}

inline void CRenderCamera::Copy(const CRenderCamera &Cam)
{
	X=Cam.X;  Y=Cam.Y;  Z=Cam.Z;  Orig=Cam.Orig;
	Near=Cam.Near;  Far=Cam.Far;
	wL=Cam.wL;  wR=Cam.wR;  wT=Cam.wT;  wB=Cam.wB;
}

inline void CRenderCamera::LookAt(const Vec3& Eye, const Vec3& ViewRefPt, const Vec3& ViewUp)
{
	Z = Eye-ViewRefPt;  Z.NormalizeSafe(); 
	X = ViewUp % Z;     X.NormalizeSafe();
	Y = Z % X;          Y.NormalizeSafe();
	Orig = Eye;
}

inline void CRenderCamera::Perspective(float Yfov, float Aspect, float Ndist, float Fdist)
{
	Near = Ndist;  Far=Fdist;
	wT = tanf(Yfov*0.5f)*Near;  wB=-wT;
	wR = wT*Aspect; wL=-wR;
}

inline void CRenderCamera::Frustum(float l, float r, float b, float t, float Ndist, float Fdist)
{
	Near=Ndist;  Far=Fdist;
	wR=r;  wL=l;  wB=b;  wT=t;
}


inline void CRenderCamera::GetLookAtParams(Vec3 *Eye, Vec3 *ViewRefPt, Vec3 *ViewUp) const
{
	*Eye = Orig;
	*ViewRefPt = Orig - Z;
	*ViewUp = Y;
}

inline void CRenderCamera::GetPerspectiveParams(float *Yfov, float *Xfov, float *Aspect, float *Ndist, float *Fdist) const
{
	*Yfov = atanf(wT/Near) * 57.29578f * 2.0f;
	*Xfov = atanf(wR/Near) * 57.29578f * 2.0f;
	*Aspect = wT/wR;
	*Ndist = Near;
	*Fdist = Far;
}

inline void CRenderCamera::GetFrustumParams(float *l, float *r, float *b, float *t, float *Ndist, float *Fdist) const
{
	*l = wL;
	*r = wR;
	*b = wB;
	*t = wT;
	*Ndist = Near;
	*Fdist = Far;
}

inline const Vec3& CRenderCamera::wCOP() const
{
	return( Orig );
}

inline Vec3 CRenderCamera::ViewDir() const
{
	return(-Z);
}

inline Vec3 CRenderCamera::ViewDirOffAxis() const
{
	float x=(wL+wR)*0.5f, y=(wT+wB)*0.5f;  // MIDPOINT ON VIEWPLANE WINDOW
	Vec3 ViewDir = X*x + Y*y - Z*Near;
	ViewDir.Normalize();
	return ViewDir;
}

inline Vec3 CRenderCamera::WorldToCam(const Vec3& wP) const
{
	Vec3 sP(wP-Orig);
	Vec3 cP(X|sP,Y|sP,Z|sP);
	return cP;
}

inline float CRenderCamera::WorldToCamZ(const Vec3& wP) const
{
	Vec3 sP(wP-Orig);
	float zdist = Z|sP;
	return zdist;
}

inline Vec3 CRenderCamera::CamToWorld(const Vec3& cP) const
{
	Vec3 wP(X*cP.x + Y*cP.y + Z*cP.z + Orig);
	return wP;
}

inline void CRenderCamera::LoadIdentityXform()
{
	X.Set(1,0,0);
	Y.Set(0,1,0);
	Z.Set(0,0,1);
	Orig.Set(0,0,0);
}

inline void CRenderCamera::Xform(const float M[16])
{
	X.Set( X.x*M[0] + X.y*M[4] + X.z*M[8],
		X.x*M[1] + X.y*M[5] + X.z*M[9],
		X.x*M[2] + X.y*M[6] + X.z*M[10] );
	Y.Set( Y.x*M[0] + Y.y*M[4] + Y.z*M[8],
		Y.x*M[1] + Y.y*M[5] + Y.z*M[9],
		Y.x*M[2] + Y.y*M[6] + Y.z*M[10] );
	Z.Set( Z.x*M[0] + Z.y*M[4] + Z.z*M[8],
		Z.x*M[1] + Z.y*M[5] + Z.z*M[9],
		Z.x*M[2] + Z.y*M[6] + Z.z*M[10] );
	Orig.Set( Orig.x*M[0] + Orig.y*M[4] + Orig.z*M[8] + M[12],
		Orig.x*M[1] + Orig.y*M[5] + Orig.z*M[9] + M[13],
		Orig.x*M[2] + Orig.y*M[6] + Orig.z*M[10] + M[14] );

	float Scale = X.GetLength();
	X /= Scale;
	Y /= Scale;
	Z /= Scale;

	wL*=Scale;
	wR*=Scale;
	wB*=Scale;
	wT*=Scale;
	Near*=Scale;
	Far*=Scale;
};

inline void CRenderCamera::Translate(const Vec3& trans)
{
	Orig += trans;
}

inline void CRenderCamera::Rotate(const float M[9])
{
	X.Set( X.x*M[0] + X.y*M[3] + X.z*M[6],
		X.x*M[1] + X.y*M[4] + X.z*M[7],
		X.x*M[2] + X.y*M[5] + X.z*M[8] );
	Y.Set( Y.x*M[0] + Y.y*M[3] + Y.z*M[6],
		Y.x*M[1] + Y.y*M[4] + Y.z*M[7],
		Y.x*M[2] + Y.y*M[5] + Y.z*M[8] );
	Z.Set( Z.x*M[0] + Z.y*M[3] + Z.z*M[6],
		Z.x*M[1] + Z.y*M[4] + Z.z*M[7],
		Z.x*M[2] + Z.y*M[5] + Z.z*M[8] );
}

inline float* CRenderCamera::GetModelviewMatrix(float* M) const
{
	Viewing16fv(M,X,Y,Z,Orig);
	return M;
}

inline float* CRenderCamera::GetProjectionMatrix(float* M) const
{
	Frustum16fv(M,wL,wR,wB,wT,Near,Far);
	return(M);  
}

inline void CRenderCamera::GetPixelRay(float sx, float sy, int ww, int wh, Vec3 *Start, Vec3 *Dir) const
{
	Vec3 wTL = Orig + (X*wL) + (Y*wT) - (Z*Near);		// FIND LOWER-LEFT
	Vec3 dX = (X*(wR-wL))/(float)ww;					// WORLD WIDTH OF PIXEL
	Vec3 dY = (Y*(wT-wB))/(float)wh;					// WORLD HEIGHT OF PIXEL
	wTL += (dX*sx - dY*sy);							// INCR TO WORLD PIXEL
	wTL += (dX*0.5f - dY*0.5f);                       // INCR TO PIXEL CNTR
	*Start = Orig;
	*Dir = wTL-Orig;
}

inline void CRenderCamera::CalcVerts(Vec3 *V)  const
{
	float NearZ = -Near;
	V[0].Set(wR,wT,NearZ);
	V[1].Set(wL,wT,NearZ);
	V[2].Set(wL,wB,NearZ);
	V[3].Set(wR,wB,NearZ);

	float FarZ=-Far, FN=Far/Near;
	float fwL=wL*FN, fwR=wR*FN, fwB=wB*FN, fwT=wT*FN;
	V[4].Set(fwR,fwT,FarZ);
	V[5].Set(fwL,fwT,FarZ);
	V[6].Set(fwL,fwB,FarZ);
	V[7].Set(fwR,fwB,FarZ);

	for (int i=0; i<8; i++)
		V[i] = CamToWorld(V[i]); 
}

inline void CRenderCamera::CalcTileVerts(Vec3 *V, f32 nPosX, f32 nPosY, f32 nGridSizeX, f32 nGridSizeY)  const
{
	float NearZ = -Near;

	float TileWidth = abs(wR - wL)/nGridSizeX ;
	float TileHeight = abs(wT - wB)/nGridSizeY;
	float TileL = wL + TileWidth * nPosX;
	float TileR = wL + TileWidth * (nPosX + 1);
	float TileB = wB + TileHeight * nPosY;
	float TileT = wB + TileHeight * (nPosY + 1);

	V[0].Set(TileR,TileT,NearZ);
	V[1].Set(TileL,TileT,NearZ);
	V[2].Set(TileL,TileB,NearZ);
	V[3].Set(TileR,TileB,NearZ);

	float FarZ=-Far, FN=Far/Near;
	float fwL=wL*FN, fwR=wR*FN, fwB=wB*FN, fwT=wT*FN;

	float TileFarWidth = abs(fwR - fwL)/nGridSizeX ;
	float TileFarHeight = abs(fwT - fwB)/nGridSizeY;
	float TileFarL = fwL + TileFarWidth * nPosX;
	float TileFarR = fwL + TileFarWidth * (nPosX + 1);
	float TileFarB = fwB + TileFarHeight * nPosY;
	float TileFarT = fwB + TileFarHeight * (nPosY + 1);

	V[4].Set(TileFarR,TileFarT,FarZ);
	V[5].Set(TileFarL,TileFarT,FarZ);
	V[6].Set(TileFarL,TileFarB,FarZ);
	V[7].Set(TileFarR,TileFarB,FarZ);

	for (int i=0; i<8; i++)
		V[i] = CamToWorld(V[i]); 
}

inline void CRenderCamera::CalcTiledRegionVerts(Vec3 *V, Vec2& vMin, Vec2& vMax, f32 nPosX, f32 nPosY, f32 nGridSizeX, f32 nGridSizeY) const
{
	float NearZ = -Near;

	Vec2 vTileMin, vTileMax;

	vMin.x = max(vMin.x, nPosX/nGridSizeX);
	vMax.x = min(vMax.x, (nPosX + 1)/nGridSizeX);

	vMin.y = max(vMin.y, nPosY/nGridSizeY);
	vMax.y = min(vMax.y, (nPosY + 1)/nGridSizeY);

	vTileMin.x = abs(wR - wL) * vMin.x;
	vTileMin.y = abs(wT - wB) * vMin.y;
	vTileMax.x = abs(wR - wL) * vMax.x;
	vTileMax.y = abs(wT - wB) * vMax.y;

	float TileWidth = abs(wR - wL)/nGridSizeX ;
	float TileHeight = abs(wT - wB)/nGridSizeY;

	float TileL = wL + vTileMin.x;
	float TileR = wL + vTileMax.x;
	float TileB = wB + vTileMin.y;
	float TileT = wB + vTileMax.y;

	V[0].Set(TileR,TileT,NearZ);
	V[1].Set(TileL,TileT,NearZ);
	V[2].Set(TileL,TileB,NearZ);
	V[3].Set(TileR,TileB,NearZ);

	float FarZ=-Far, FN=Far/Near;
	float fwL=wL*FN, fwR=wR*FN, fwB=wB*FN, fwT=wT*FN;

	Vec2 vTileFarMin, vTileFarMax;

	vTileFarMin.x = abs(fwR - fwL) * vMin.x;
	vTileFarMin.y = abs(fwT - fwB) * vMin.y;
	vTileFarMax.x = abs(fwR - fwL) * vMax.x;
	vTileFarMax.y = abs(fwT - fwB) * vMax.y;

	float TileFarWidth = abs(fwR - fwL)/nGridSizeX ;
	float TileFarHeight = abs(fwT - fwB)/nGridSizeY;

	float TileFarL = fwL + vTileFarMin.x;
	float TileFarR = fwL + vTileFarMax.x;
	float TileFarB = fwB + vTileFarMin.y;
	float TileFarT = fwB + vTileFarMax.y;

	V[4].Set(TileFarR,TileFarT,FarZ);
	V[5].Set(TileFarL,TileFarT,FarZ);
	V[6].Set(TileFarL,TileFarB,FarZ);
	V[7].Set(TileFarR,TileFarB,FarZ);

	for (int i=0; i<8; i++)
		V[i] = CamToWorld(V[i]); 
}


inline void CRenderCamera::CalcRegionVerts(Vec3 *V, const Vec2& vMin, const Vec2& vMax) const
{
	float NearZ = -Near;

	Vec2 vTileMin, vTileMax;

	vTileMin.x = abs(wR - wL) * vMin.x;
	vTileMin.y = abs(wT - wB) * vMin.y;
	vTileMax.x = abs(wR - wL) * vMax.x;
	vTileMax.y = abs(wT - wB) * vMax.y;

	float TileL = wL + vTileMin.x;
	float TileR = wL + vTileMax.x;
	float TileB = wB + vTileMin.y;
	float TileT = wB + vTileMax.y;

	V[0].Set(TileR,TileT,NearZ);
	V[1].Set(TileL,TileT,NearZ);
	V[2].Set(TileL,TileB,NearZ);
	V[3].Set(TileR,TileB,NearZ);

	float FarZ=-Far, FN=Far/Near;
	float fwL=wL*FN, fwR=wR*FN, fwB=wB*FN, fwT=wT*FN;

	Vec2 vTileFarMin, vTileFarMax;

	vTileFarMin.x = abs(fwR - fwL) * vMin.x;
	vTileFarMin.y = abs(fwT - fwB) * vMin.y;
	vTileFarMax.x = abs(fwR - fwL) * vMax.x;
	vTileFarMax.y = abs(fwT - fwB) * vMax.y;

	float TileFarL = fwL + vTileFarMin.x;
	float TileFarR = fwL + vTileFarMax.x;
	float TileFarB = fwB + vTileFarMin.y;
	float TileFarT = fwB + vTileFarMax.y;

	V[4].Set(TileFarR,TileFarT,FarZ);
	V[5].Set(TileFarL,TileFarT,FarZ);
	V[6].Set(TileFarL,TileFarB,FarZ);
	V[7].Set(TileFarR,TileFarB,FarZ);

	for (int i=0; i<8; i++)
		V[i] = CamToWorld(V[i]); 
}

#endif


