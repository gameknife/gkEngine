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
// Name:   	gk_Matrix.h
// Desc:	
// 	
// 
// Author:  yikaiming
// Date:	2013/6/1	
// 
//////////////////////////////////////////////////////////////////////////

#ifndef gk_Matrix_h__
#define gk_Matrix_h__

template<typename F> struct Matrix33_tpl;
template<typename F, class A> struct Matrix34_tpl;
template<typename F, class A> struct Matrix44_tpl;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// struct Diag33_tpl
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

template<typename F> struct Diag33_tpl {

	F x,y,z;

#if defined(_DEBUG) && !defined(__SPU__)
	ILINE Diag33_tpl() 
	{
		if (sizeof(F)==4)
		{
			uint32* p=(uint32*)&x;	p[0]=F32NAN;	p[1]=F32NAN;	p[2]=F32NAN;
		}
		if (sizeof(F)==8)
		{
			uint64* p=(uint64*)&x;	p[0]=F64NAN;	p[1]=F64NAN;	p[2]=F64NAN;
		}
	}
#else
	ILINE Diag33_tpl()	{};
#endif

	Diag33_tpl(F dx,F dy,F dz) { x=dx; y=dy; z=dz; }
	Diag33_tpl(const Vec3_tpl<F> &v) {	x=v.x; y=v.y; z=v.z;	} 
	template<class F1> const Diag33_tpl& operator=(const Vec3_tpl<F1>& v) { x=v.x; y=v.y; z = v.z; return *this; 	}
	Diag33_tpl& operator=(const Diag33_tpl<F> &diag) { x=diag.x; y=diag.y; z=diag.z; return *this;	}
	template<class F1> Diag33_tpl& operator=(const Diag33_tpl<F1>& diag) { x=diag.x; y=diag.y; z=diag.z; return *this; }

	const void SetIdentity() { x=y=z=1;	}
	Diag33_tpl(type_identity) { x=y=z=1; }

	const Diag33_tpl& zero() {	x=y=z=0; return *this; }

	Diag33_tpl& fabs() {	x=fabs_tpl(x); y=fabs_tpl(y); z=fabs_tpl(z); return *this;	}

	Diag33_tpl& invert() { // in-place inversion
		F det = determinant();
		if (det==0) return *this;	
		det = (F)1.0/det;
		F oldata[3]; oldata[0]=x; oldata[1]=y; oldata[2]=z;
		x = oldata[1]*oldata[2]*det; 
		y = oldata[0]*oldata[2]*det; 
		z = oldata[0]*oldata[1]*det;
		return *this;
	}

	/*!
	* Linear-Interpolation between Diag33(lerp)
	* 
	* Example:
	*  Diag33 r=Diag33::CreateLerp( p, q, 0.345f );
	*/
	ILINE void SetLerp( const Diag33_tpl<F> &p, const Diag33_tpl<F> &q, F t ) 
	{	
		const float lerp = (1.0f-t);
		x = p.x*lerp + q.x*t;
		y = p.y*lerp + q.y*t;
		z = p.z*lerp + q.z*t;
	}
	static ILINE Diag33_tpl<F> CreateLerp( const Diag33_tpl<F> &p, const Diag33_tpl<F> &q, F t ) 
	{	
		Diag33_tpl<F>  d;
		const float lerp = (1.0f-t);
		d.x = p.x*lerp + q.x*t;
		d.y = p.y*lerp + q.y*t;
		d.z = p.z*lerp + q.z*t;
		return d;
	}

	F determinant() const {	return x*y*z; }

	bool IsValid() const
	{
		if (!NumberValid(x)) return false;
		if (!NumberValid(y)) return false;
		if (!NumberValid(z)) return false;
		return true;
	}

};

///////////////////////////////////////////////////////////////////////////////
// Typedefs                                                                  //
///////////////////////////////////////////////////////////////////////////////


typedef Diag33_tpl<f32> Diag33;
typedef Diag33_tpl<f64> Diag33_f64;
typedef Diag33_tpl<real> Diag33r;


template<class F1, class F2> 
Diag33_tpl<F1> operator*(const Diag33_tpl<F1> &l, const Diag33_tpl<F2> &r) {
	return Diag33_tpl<F1>(	l.x*r.x, l.y*r.y,	l.z*r.z	);
}

template<class F1, class F2> 
Matrix33_tpl<F2> operator*(const Diag33_tpl<F1> &l, const Matrix33_tpl<F2> &r) {
	Matrix33_tpl<F2> res;
	res.m00 = r.m00*l.x;	res.m01 = r.m01*l.x;		res.m02 = r.m02*l.x;
	res.m10 = r.m10*l.y;	res.m11 = r.m11*l.y;		res.m12 = r.m12*l.y;
	res.m20 = r.m20*l.z;	res.m21 = r.m21*l.z;		res.m22 = r.m22*l.z;
	return res;
}
template<class F1, class F2, class B, class C> 
Matrix34_tpl<F2, B> operator*(const Diag33_tpl<F1> &l, const Matrix34_tpl<F2, C> &r) {
	Matrix34_tpl<F2, B> m;
	m.m00=l.x*r.m00;	m.m01=l.x*r.m01;	m.m02=l.x*r.m02;	m.m03=l.x*r.m03;
	m.m10=l.y*r.m10;	m.m11=l.y*r.m11;	m.m12=l.y*r.m12;	m.m13=l.y*r.m13;
	m.m20=l.z*r.m20;	m.m21=l.z*r.m21;	m.m22=l.z*r.m22;	m.m23=l.z*r.m23;
	return m;
}

template<class F1,class F2>
Vec3_tpl<F2> operator *(const Diag33_tpl<F1> &mtx, const Vec3_tpl<F2> &vec) {
	return Vec3_tpl<F2>(mtx.x*vec.x, mtx.y*vec.y, mtx.z*vec.z);
}

template<class F1,class F2>
Vec3_tpl<F1> operator *(const Vec3_tpl<F1> &vec, const Diag33_tpl<F2> &mtx) {
	return Vec3_tpl<F1>(mtx.x*vec.x, mtx.y*vec.y, mtx.z*vec.z);
}








///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// struct Matrix33_tpl
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

template<typename F> struct Matrix33_tpl
{
#ifndef XENON_INTRINSICS
	F m00,m01,m02;
	F m10,m11,m12;
	F m20,m21,m22;
#endif

	//---------------------------------------------------------------------------------

#if defined(_DEBUG) && !defined(__SPU__)
	ILINE Matrix33_tpl() 
	{
		if (sizeof(F)==4)
		{
			uint32* p=(uint32*)&m00;
			p[0]=F32NAN;	p[1]=F32NAN;	p[2]=F32NAN;
			p[3]=F32NAN;	p[4]=F32NAN;	p[5]=F32NAN;
			p[6]=F32NAN;	p[7]=F32NAN;	p[8]=F32NAN;
		}
		if (sizeof(F)==8)
		{
			uint64* p=(uint64*)&m00;
			p[0]=F64NAN;	p[1]=F64NAN;	p[2]=F64NAN;
			p[3]=F64NAN;	p[4]=F64NAN;	p[5]=F64NAN;
			p[6]=F64NAN;	p[7]=F64NAN;	p[8]=F64NAN;
		}
	}
#else
	ILINE Matrix33_tpl(){};
#endif

	//assignment operator is needed since default operator = has precedence over assignment constructor
	ILINE Matrix33_tpl& operator = (const Matrix33_tpl<F> &m) { 
		assert(m.IsValid());
		m00=m.m00;	m01=m.m01;	m02=m.m02; 
		m10=m.m10;	m11=m.m11;	m12=m.m12;
		m20=m.m20;	m21=m.m21;	m22=m.m22; 
		return *this;
	}

	template<class F1> ILINE Matrix33_tpl(const Diag33_tpl<F1> &d) {
		assert(d.IsValid());
		m00=F(d.x); m01=0;			m02=0; 
		m10=0;			m11=F(d.y);	m12=0;
		m20=0;			m21=0;			m22=F(d.z); 
	}
	template<class F1> ILINE Matrix33_tpl(const Matrix33_tpl<F1>& m) {
		assert(m.IsValid());
		m00=F(m.m00);	m01=F(m.m01);	m02=F(m.m02); 
		m10=F(m.m10);	m11=F(m.m11);	m12=F(m.m12);
		m20=F(m.m20);	m21=F(m.m21);	m22=F(m.m22); 
	}
	template<class F1, class B> ILINE explicit Matrix33_tpl(const Matrix34_tpl<F1, B>& m )	{
		assert(m.IsValid());
		m00=F(m.m00);		m01=F(m.m01);		m02=F(m.m02);	
		m10=F(m.m10);		m11=F(m.m11);		m12=F(m.m12);
		m20=F(m.m20);		m21=F(m.m21);		m22=F(m.m22);
	}
	template<class F1, class B> ILINE explicit Matrix33_tpl(const Matrix44_tpl<F1, B>& m ) {
		assert(m.IsValid());
		m00=F(m.m00);	m01=F(m.m01);	m02=F(m.m02); 
		m10=F(m.m10);	m11=F(m.m11);	m12=F(m.m12);
		m20=F(m.m20);	m21=F(m.m21);	m22=F(m.m22); 
	}
  template<class F1> explicit ILINE Matrix33_tpl(const Vec3_tpl<F1>& vx, const Vec3_tpl<F1>& vy, const Vec3_tpl<F1>& vz)	{
    m00=F(vx.x);		m01=F(vy.x);		m02=F(vz.x);
    m10=F(vx.y);		m11=F(vy.y);		m12=F(vz.y);
    m20=F(vx.z);		m21=F(vy.z);		m22=F(vz.z);
  }

	//Convert unit quaternion to matrix (23-flops).
	template<class F1> explicit Matrix33_tpl( const Quat_tpl<F1>& q ) {
		assert(q.IsValid(0.05f));
		Vec3r v2=q.v+q.v;
		f64 xx=1-v2.x*q.v.x;	f64 yy=v2.y*q.v.y;		f64 xw=v2.x*q.w;
		f64 xy=v2.y*q.v.x;		f64 yz=v2.z*q.v.y;		f64 yw=v2.y*q.w;
		f64 xz=v2.z*q.v.x;		f64 zz=v2.z*q.v.z;		f64 zw=v2.z*q.w;
		m00=F(1-yy-zz);				m01=F(xy-zw);					m02=F(xz+yw);
		m10=F(xy+zw);					m11=F(xx-zz);					m12=F(yz-xw);
		m20=F(xz-yw);					m21=F(yz+xw);					m22=F(xx-yy);
	}

	ILINE explicit Matrix33_tpl( const Ang3_tpl<F>& rad ) 
	{
		assert(rad.IsValid());
		SetRotationXYZ(rad);
	}

	template<class F1> ILINE Matrix33_tpl& operator = (const Vec3_tpl<F1> &v) {
		m00=F(v.x);	m01=0;			m02=0; 
		m10=0;			m11=F(v.y);	m12=0;
		m20=0;			m21=0;			m22=F(v.z); 
		return *this;
	}

	//---------------------------------------------------------------------------------------

	ILINE void SetIdentity(void) {
		m00=1;	m01=0;	m02=0; 
		m10=0;	m11=1;	m12=0;
		m20=0;	m21=0;	m22=1; 
	}
	ILINE static Matrix33_tpl<F> CreateIdentity() {	Matrix33_tpl<F> m33;	m33.SetIdentity(); return m33; }
	ILINE Matrix33_tpl(type_identity) { SetIdentity(); }

	ILINE void SetZero() { 
		m00=0;	m01=0;	m02=0; 
		m10=0;	m11=0;	m12=0;
		m20=0;	m21=0;	m22=0; 
	}


	/*!
	*  Create a rotation matrix around an arbitrary axis (Eulers Theorem).  
	*  The axis is specified as a normalized Vec3. The angle is assumed to be in radians.  
	*
	*  Example:
	*		Matrix34 m34;
	*		Vec3 axis=GetNormalized( Vec3(-1.0f,-0.3f,0.0f) );
	*		m34.SetRotationAA( rad, axis );
	*/
	void SetRotationAA(F angle, Vec3_tpl<F> const& axis) {	
		f64 s,c; sincos_tpl(angle,&s,&c);	f64	mc=1.0-c;	
		f64 mcx=mc*axis.x;				f64 mcy=mc*axis.y;					f64 mcz=mc*axis.z; 
		f64 tcx=axis.x*s;					f64 tcy=axis.y*s;						f64 tcz=axis.z*s;
		m00=F(mcx*axis.x+c);			m01=F(mcx*axis.y-tcz);			m02=F(mcx*axis.z+tcy);	
		m10=F(mcy*axis.x+tcz);		m11=F(mcy*axis.y+c);				m12=F(mcy*axis.z-tcx);	
		m20=F(mcz*axis.x-tcy);		m21=F(mcz*axis.y+tcx);			m22=F(mcz*axis.z+c);					
	}
	ILINE static Matrix33_tpl<F> CreateRotationAA( const F rad, Vec3_tpl<F> const& axis ) {	
		Matrix33_tpl<F> m33;	m33.SetRotationAA(rad,axis); return m33;	
	}


	void SetRotationAA(F c, F s, Vec3_tpl<F> const& axis) { 
		assert(axis.IsUnit(0.001f));
		F	mc	=	1-c;
		m00=mc*axis.x*axis.x + c;					m01=mc*axis.x*axis.y - axis.z*s;	m02=mc*axis.x*axis.z + axis.y*s;	
		m10=mc*axis.y*axis.x + axis.z*s;	m11=mc*axis.y*axis.y + c;					m12=mc*axis.y*axis.z - axis.x*s;	
		m20=mc*axis.z*axis.x - axis.y*s;	m21=mc*axis.z*axis.y + axis.x*s;	m22=mc*axis.z*axis.z + c;					
	}
	ILINE static Matrix33_tpl<F> CreateRotationAA(F c, F s, Vec3_tpl<F> const& axis) {	
		Matrix33_tpl<F> m33;	m33.SetRotationAA(c,s,axis); return m33;	
	}

	ILINE void SetRotationAA(Vec3_tpl<F> const& rot) { 
		F angle = rot.GetLength();
		if (angle == F(0))
			SetIdentity();
		else
			SetRotationAA(angle, rot / angle);
	}
	ILINE static Matrix33_tpl<F> CreateRotationAA(Vec3_tpl<F> const& rot) {
		Matrix33_tpl<F> m33;	m33.SetRotationAA(rot); return m33;	
	}

	/*!
	*
	* Create rotation-matrix about X axis using an angle.
	* The angle is assumed to be in radians. 
	*
	*  Example:
	*		Matrix m33;
	*		m33.SetRotationX(0.5f);
	*/
	void SetRotationX(const f32 rad )	{
		F s,c; sincos_tpl(rad,&s,&c);
		m00=1.0f;		m01=0.0f;		m02=	0.0f;		
		m10=0.0f;		m11=c;	m12=-s;
		m20=0.0f;		m21=s;	m22= c;
	}
	ILINE static Matrix33_tpl<F> CreateRotationX(const f32 rad ) {	
		Matrix33_tpl<F> m33; m33.SetRotationX(rad); return m33;	
	}

	void SetRotationY(const f32 rad ) {
		F s,c; sincos_tpl(rad,&s,&c);
		m00	=	c;		m01	=	0;		m02	=	s;
		m10	=	0;		m11	=	1;		m12	=	0;			
		m20	=-s;		m21	=	0;		m22	= c;	
	}
	ILINE static Matrix33_tpl<F> CreateRotationY(const f32 rad ) { 
		Matrix33_tpl<F> m33; m33.SetRotationY(rad);	return m33;	
	}

	void SetRotationZ( const f32 rad ) {
		F s,c; sincos_tpl(rad,&s,&c);
		m00	=	c;			m01	=-s;			m02	=	0.0f;	
		m10	=	s;			m11	=	c;			m12	=	0.0f;	
		m20	=	0.0f;		m21	=	0.0f;		m22	= 1.0f;
	}
	ILINE static Matrix33_tpl<F> CreateRotationZ(const f32 rad) {	
		Matrix33_tpl<F> m33;	m33.SetRotationZ(rad);	return m33;	
	}

	void SetRotationXYZ( const Ang3_tpl<F>& rad ) 
	{
		assert(rad.IsValid());
		F sx,cx;  sincos_tpl(rad.x,&sx,&cx);
		F sy,cy;  sincos_tpl(rad.y,&sy,&cy);
		F sz,cz;  sincos_tpl(rad.z,&sz,&cz);
		F sycz  =(sy*cz), sysz  =(sy*sz);
		m00=cy*cz;	m01=sycz*sx-cx*sz;	m02=sycz*cx+sx*sz;
		m10=cy*sz;	m11=sysz*sx+cx*cz;	m12=sysz*cx-sx*cz;
		m20=-sy;		m21=cy*sx;					m22=cy*cx;				
	}
	ILINE static Matrix33_tpl<F> CreateRotationXYZ( const Ang3_tpl<F>& rad ) 
	{	
		assert(rad.IsValid());
		Matrix33_tpl<F> m33;	m33.SetRotationXYZ(rad); return m33;	
	}



	/*!
	* Creates a rotation matrix that rotates the vector "v0" into "v1". 
	*
	*	a) If both vectors are exactly parallel it returns an identity-matrix 
	*	b) CAUTION: If both vectors are exactly diametrical it returns a matrix that rotates 
	*    pi-radians about a "random" axis that is orthogonal to v0.  
	*	c) CAUTION: If both vectors are almost diametrical we have to normalize 
	*    a very small vector and the result is inaccurate. It is recommended to use this 
	*    function with 64-bit precision. 
	*/
	void SetRotationV0V1( const Vec3_tpl<F>& v0, const Vec3_tpl<F>& v1 ) {
		assert((fabs_tpl(1-(v0|v0)))<0.01); //check if unit-vector
		assert((fabs_tpl(1-(v1|v1)))<0.01); //check if unit-vector
		f64 dot = v0|v1;
		if ( dot < -0.9999)	{
			Vec3r axis = v0.GetOrthogonal().GetNormalized(); 
			m00=F(2*axis.x*axis.x-1);	m01=F(2*axis.x*axis.y);		m02=F(2*axis.x*axis.z);	
			m10=F(2*axis.y*axis.x);		m11=F(2*axis.y*axis.y-1);	m12=F(2*axis.y*axis.z);	
			m20=F(2*axis.z*axis.x);		m21=F(2*axis.z*axis.y);		m22=F(2*axis.z*axis.z-1);					
		}	else	{
			Vec3r v = v0%v1;	f64 h = 1/(1 + dot); 
			m00=F(dot+h*v.x*v.x);		m01=F(h*v.x*v.y-v.z);		m02=F(h*v.x*v.z+v.y);
			m10=F(h*v.x*v.y+v.z);		m11=F(dot+h*v.y*v.y);		m12=F(h*v.y*v.z-v.x);
			m20=F(h*v.x*v.z-v.y);		m21=F(h*v.y*v.z+v.x);		m22=F(dot+h*v.z*v.z);
		}
	}
	ILINE static Matrix33_tpl<F> CreateRotationV0V1( const Vec3_tpl<F>& v0, const Vec3_tpl<F>& v1 ) {	Matrix33_tpl<F> m33;	m33.SetRotationV0V1(v0,v1); return m33;	}







	/*!
	*
	* \param vdir  normalized view direction.
	* \param roll  radiant to rotate about Y-axis.
	*
	*  Given a view-direction and a radiant to rotate about Y-axis, this function builds a 3x3 look-at matrix 
	*  using only simple vector arithmetic. This function is always using the implicit up-vector Vec3(0,0,1). 
	*  The view-direction is always stored in column(1).
	*  IMPORTANT: The view-vector is assumed to be normalized, because all trig-values for the orientation are beeing 
	*  extracted  directly out of the vector. This function must NOT be called with a view-direction 
	*  that is close to Vec3(0,0,1) or Vec3(0,0,-1). If one of these rules is broken, the function returns a matrix 
	*  with an undifined rotation about the Z-axis.
	*
	*  Rotation order for the look-at-matrix is Z-X-Y. (Zaxis=YAW / Xaxis=PITCH / Yaxis=ROLL)
	*
	*  COORDINATE-SYSTEM       
	*                                   
	*  z-axis                                 
	*    ^                               
	*    |                               
	*    |  y-axis                   
	*    |  /                         
	*    | /                           
	*    |/                             
	*    +--------------->   x-axis     
	*                                  
	*  Example:
	*		Matrix33 orientation=Matrix33::CreateRotationVDir( Vec3(0,1,0), 0 );
	*/
	void SetRotationVDir( const Vec3_tpl<F>& vdir ) {
		assert((fabs_tpl(1-(vdir|vdir)))<0.01);		//check if unit-vector
		//set default initialisation for up-vector	
		m00=1;	m01=0;			m02=0;
		m10=0;	m11=0;			m12=-vdir.z;
		m20=0;	m21=vdir.z;	m22=0;
		//calculate look-at matrix
		f64 l = sqrt(vdir.x*vdir.x+vdir.y*vdir.y);
		if (l>0.0001)	
		{
			f64 xl=-vdir.x/l; f64 yl=vdir.y/l;
			m00=F(yl);	m01=F(vdir.x);		m02=F(xl*vdir.z);
			m10=F(xl);	m11=F(vdir.y);		m12=F(-vdir.z*yl);
			m20=0;			m21=F(vdir.z);		m22=F(l);
		}
	}
	ILINE static Matrix33_tpl<F> CreateRotationVDir( const Vec3_tpl<F>& vdir ) {	
		Matrix33_tpl<F> m33;	m33.SetRotationVDir(vdir); return m33;	
	}


	//look-at matrix with roll
	void SetRotationVDir( const Vec3_tpl<F>& vdir, F roll ) {
		SetRotationVDir( vdir );
		F s,c; sincos_tpl(roll,&s,&c);
		F x00=m00,x10=m10;
		m00=m00*c-m02*s;	m02=x00*s+m02*c;
		m10=m10*c-m12*s;	m12=x10*s+m12*c;
		m20=-m22*s;				m22=m22*c;
	}
	ILINE static Matrix33_tpl<F> CreateRotationVDir( const Vec3_tpl<F>& vdir, F roll  ) {	
		Matrix33_tpl<F> m33;	m33.SetRotationVDir(vdir,roll ); return m33;	
	}

	//////////////////////////////////////////////////////////////////////////
	static Matrix33_tpl<F> CreateOrientation( const Vec3_tpl<F> &dir,const Vec3_tpl<F> &up,float rollAngle )
	{
		// LookAt transform.
		Vec3 xAxis,yAxis,zAxis;
		Vec3 upVector = up;
		if (dir.IsZero())
		{
			Matrix33_tpl<F> tm;
			tm.SetIdentity();
			return tm;
		}
		yAxis = dir.GetNormalized();

		if (yAxis.x == 0 && yAxis.y == 0 && up.IsEquivalent(Vec3_tpl<F>(0,0,1.0f)))
			upVector.Set( -yAxis.z,0,0 );

		xAxis = (upVector % yAxis).GetNormalized();
		zAxis = (xAxis % yAxis).GetNormalized();

		Matrix33_tpl<F> tm;
		tm.SetFromVectors( xAxis,yAxis,zAxis );

		if (rollAngle != 0)
		{
			Matrix33_tpl<F> RollMtx;
			RollMtx.SetRotationY(rollAngle);
			tm = tm * RollMtx;
		}
		return tm;
	}


	/*!
	*  Direct-Matrix-Slerp: for the sake of completeness, I have included the following expression 
	*  for Spherical-Linear-Interpolation without using quaternions. This is much faster then converting 
	*  both matrices into quaternions in order to do a quaternion slerp and then converting the slerped 
	*  quaternion back into a matrix.
	*  This is a high-precision calculation. Given two orthonormal 3x3 matrices this function calculates 
	*  the shortest possible interpolation-path between the two rotations. The interpolation curve forms 
	*  a great arc on the rotation sphere (geodesic). Not only does Slerp follow a great arc it follows 
	*  the shortest great arc.	Furthermore Slerp has constant angular velocity. All in all Slerp is the 
	*  optimal interpolation curve between two rotations. 
	*
	*  STABILITY PROBLEM: There are two singularities at angle=0 and angle=PI. At 0 the interpolation-axis 
	*  is arbitrary, which means any axis will produce the same result because we have no rotation. Thats 
	*  why I'm using (1,0,0). At PI the rotations point away from each other and the interpolation-axis 
	*  is unpredictable. In this case I'm also using the axis (1,0,0). If the angle is ~0 or ~PI, then we 
	*  have to normalize a very small vector and this can cause numerical instability. The quaternion-slerp 
	*  has exactly the same problems. 
	*                                                                                                  Ivo
	*  Example:
	*		Matrix33 slerp=Matrix33::CreateSlerp( m,n,0.333f );
	*/
	ILINE static Matrix33_tpl<F> CreateSlerp( const Matrix33_tpl<F>& m, const Matrix33_tpl<F>& n, F t) {	Matrix33_tpl<F> m33;	m33.SetSlerp(m,n,t); return m33;	}
	void SetSlerp( const Matrix33_tpl<F>& m, const Matrix33_tpl<F>& n, F t)
	{
		assert(m.IsValid());
		assert(n.IsValid());
		//calculate delta-rotation between m and n (=39 flops)
		Matrix33_tpl<real> d,i;
		d.m00=m.m00*n.m00+m.m10*n.m10+m.m20*n.m20;	d.m01=m.m00*n.m01+m.m10*n.m11+m.m20*n.m21;	d.m02=m.m00*n.m02+m.m10*n.m12+m.m20*n.m22;
		d.m10=m.m01*n.m00+m.m11*n.m10+m.m21*n.m20;	d.m11=m.m01*n.m01+m.m11*n.m11+m.m21*n.m21;	d.m12=m.m01*n.m02+m.m11*n.m12+m.m21*n.m22;
		d.m20=d.m01*d.m12-d.m02*d.m11;							d.m21=d.m02*d.m10-d.m00*d.m12;							d.m22=d.m00*d.m11-d.m01*d.m10;
		assert( d.IsOrthonormalRH(0.0001f) );

		//extract angle and axis
		f64 cosine = clamp((d.m00+d.m11+d.m22-1.0)*0.5,-1.0,+1.0);
		f64 angle = atan2(sqrt(1.0-cosine*cosine),cosine);
		Vec3r axis(d.m21-d.m12,d.m02-d.m20,d.m10-d.m01);
		f64 l = sqrt(axis|axis);	if (l>0.00001) axis/=l; else axis(1,0,0); 
		i.SetRotationAA(angle*t,axis); //angle interpolation and calculation of new delta-matrix (=26 flops) 

		//final concatenation (=39 flops)
		m00=F(m.m00*i.m00+m.m01*i.m10+m.m02*i.m20);	m01=F(m.m00*i.m01+m.m01*i.m11+m.m02*i.m21);	m02=F(m.m00*i.m02+m.m01*i.m12+m.m02*i.m22);
		m10=F(m.m10*i.m00+m.m11*i.m10+m.m12*i.m20);	m11=F(m.m10*i.m01+m.m11*i.m11+m.m12*i.m21);	m12=F(m.m10*i.m02+m.m11*i.m12+m.m12*i.m22);
		m20=m01*m12-m02*m11;												m21=m02*m10-m00*m12;												m22=m00*m11-m01*m10;
		assert( this->IsOrthonormalRH(0.0001f) );
	}


	//! calculate 2 vector that form a orthogonal base with a given input vector (by M.M.)
	//! /param invDirection input direction (has to be normalized)
	//! /param outvA first output vector that is perpendicular to the input direction
	//! /param outvB second output vector that is perpendicular the input vector and the first output vector
	static Matrix33_tpl<F> CreateOrthogonalBase( const Vec3& invDirection )
	{
		Vec3 outvA; 
		if(invDirection.z<-0.5f || invDirection.z>0.5f)	outvA=Vec3(invDirection.z,invDirection.y,-invDirection.x);
		else outvA=Vec3(invDirection.y,-invDirection.x,invDirection.z);
		Vec3 outvB = (invDirection % outvA).GetNormalized();
		outvA = (invDirection % outvB).GetNormalized();
		return CreateFromVectors(invDirection,outvA,outvB);
	}

	ILINE void SetScale( const Vec3_tpl<F> &s ) {
		m00=s.x;		m01=0;			m02=0;
		m10=0;			m11=s.y;		m12=0;
		m20=0;			m21=0;			m22=s.z;
	}
	ILINE static Matrix33_tpl<F> CreateScale( const Vec3_tpl<F> &s  ) { Matrix33_tpl<F> m; m.SetScale(s);	return m;	}


	//NOTE: all vectors are stored in columns
	ILINE void SetFromVectors(const Vec3_tpl<F>& vx, const Vec3_tpl<F>& vy, const Vec3_tpl<F>& vz)	{
		m00=vx.x;		m01=vy.x;		m02=vz.x;
		m10=vx.y;		m11=vy.y;		m12=vz.y;
		m20=vx.z;		m21=vy.z;		m22=vz.z;
	}
	ILINE static Matrix33_tpl<F> CreateFromVectors( const Vec3_tpl<F>& vx, const Vec3_tpl<F>& vy, const Vec3_tpl<F>& vz ) {	Matrix33_tpl<F> dst; dst.SetFromVectors(vx,vy,vz); return dst;	}




	ILINE void Transpose() { // in-place transposition
		F t; 
		t=m01; m01=m10; m10=t;
		t=m02; m02=m20; m20=t;
		t=m12; m12=m21; m21=t;
	}
	ILINE Matrix33_tpl<F> GetTransposed() const {
		Matrix33_tpl<F> dst;
		dst.m00=m00;			dst.m01=m10;			dst.m02=m20;
		dst.m10=m01;			dst.m11=m11;			dst.m12=m21;
		dst.m20=m02;			dst.m21=m12;			dst.m22=m22;
		return dst;
	}
	ILINE Matrix33_tpl<F> T() const { return GetTransposed(); }

	Matrix33_tpl& Fabs() {
		m00=fabs_tpl(m00); m01=fabs_tpl(m01);	m02=fabs_tpl(m02); 
		m10=fabs_tpl(m10); m11=fabs_tpl(m11); m12=fabs_tpl(m12);
		m20=fabs_tpl(m20); m21=fabs_tpl(m21);	m22=fabs_tpl(m22); 
		return *this;
	}
	ILINE Matrix33_tpl<F> GetFabs() const {	Matrix33_tpl<F> m=*this; m.Fabs();	return m;	}


	void Adjoint( void ) {  
		//rescue members
		Matrix33_tpl<F> m=*this;
		//calculate the adjoint-matrix
		m00=m.m11*m.m22-m.m12*m.m21;	m01=m.m12*m.m20-m.m10*m.m22;	m02=m.m10*m.m21-m.m11*m.m20;
		m10=m.m21*m.m02-m.m22*m.m01;	m11=m.m22*m.m00-m.m20*m.m02;	m12=m.m20*m.m01-m.m21*m.m00;
		m20=m.m01*m.m12-m.m02*m.m11;	m21=m.m02*m.m10-m.m00*m.m12;	m22=m.m00*m.m11-m.m01*m.m10;
	}
	ILINE Matrix33_tpl<F> GetAdjoint() const {	Matrix33_tpl<F> dst=*this; dst.Adjoint(); return dst;	}



	/*!
	*
	* calculate a real inversion of a Matrix33.
	* an inverse-matrix is an UnDo-matrix for all kind of transformations 
	* NOTE: if the return value of Invert33() is zero, then the inversion failed! 
	* 
	*  Example 1:
	*		Matrix33 im33;
	*		bool st=i33.Invert();
	*   assert(st);  
	*
	*  Example 2:
	*   matrix33 im33=Matrix33::GetInverted(m33);
	*/
	bool Invert( void ) {  
		//rescue members
		Matrix33_tpl<F>	m=*this;
		//calculate the cofactor-matrix (=transposed adjoint-matrix)
		m00=m.m22*m.m11-m.m12*m.m21;	m01=m.m02*m.m21-m.m22*m.m01;	m02=m.m12*m.m01-m.m02*m.m11;
		m10=m.m12*m.m20-m.m22*m.m10;	m11=m.m22*m.m00-m.m02*m.m20;	m12=m.m02*m.m10-m.m12*m.m00;
		m20=m.m10*m.m21-m.m20*m.m11;	m21=m.m20*m.m01-m.m00*m.m21;	m22=m.m00*m.m11-m.m10*m.m01;
		// calculate determinant
		F det=(m.m00*m00 + m.m10*m01 + m.m20*m02);
		if (fabs_tpl(det)<1E-20f) 
			return 0;	
		//devide the cofactor-matrix by the determinat
		F idet=(F)1.0/det;
		m00*=idet; m01*=idet;	m02*=idet;
		m10*=idet; m11*=idet;	m12*=idet;
		m20*=idet; m21*=idet;	m22*=idet;
		return 1;
	}
	ILINE Matrix33_tpl<F> GetInverted() const {	Matrix33_tpl<F> dst=*this; dst.Invert(); return dst;	}

	ILINE Vec3_tpl<F> TransformVector(const Vec3_tpl<F> &v) const  {
		return Vec3_tpl<F>(m00*v.x+m01*v.y+m02*v.z, m10*v.x+m11*v.y+m12*v.z, m20*v.x+m21*v.y+m22*v.z);
	}



	//! make a right-handed orthonormal matrix.
	void OrthonormalizeFast()
	{
		Vec3 x = Vec3(m00,m10,m20).GetNormalized();
		Vec3 y = (Vec3(m02,m12,m22)%x).GetNormalized();
		Vec3 z = (x%y);
		m00=x.x;	m01=y.x;	m02=z.x;
		m10=x.y;	m11=y.y;	m12=z.y;
		m20=x.z;	m21=y.z;	m22=z.z;
	}

	ILINE f32 Determinant() const		{
		return (m00*m11*m22) + (m01*m12*m20) + (m02*m10*m21) - (m02*m11*m20) - (m00*m12*m21) - (m01*m10*m22);
	}

	//--------------------------------------------------------------------------------
	//----                  helper functions to access matrix-members     ------------
	//--------------------------------------------------------------------------------
	F *GetData() { return &m00; }
	const F *GetData() const { return &m00; }

	ILINE F operator () (uint32 i, uint32 j) const {	assert ((i<3) && (j<3));	F* p_data=(F*)(&m00); 	return p_data[i*3+j];	}
	ILINE F& operator () (uint32 i, uint32 j)	{	assert ((i<3) && (j<3));	F* p_data=(F*)(&m00);		return p_data[i*3+j];	}

	ILINE void SetRow(int i, const Vec3_tpl<F> &v)	{	assert(i<3);	F* p=(F*)(&m00);	p[0+3*i]=v.x;	p[1+3*i]=v.y;	p[2+3*i]=v.z;		}
	ILINE Vec3_tpl<F> GetRow(int i) const	{	assert(i<3);	F* p=(F*)(&m00);	return Vec3_tpl<F>(p[0+3*i],p[1+3*i],p[2+3*i]);	}

	ILINE void SetColumn(int i, const Vec3_tpl<F> &v)	{	assert(i<3);	F* p=(F*)(&m00);	p[i+3*0]=v.x;	p[i+3*1]=v.y;	p[i+3*2]=v.z;		}
	ILINE Vec3_tpl<F> GetColumn(int i) const	{	assert(i<3);	F* p=(F*)(&m00);	return Vec3_tpl<F>(p[i+3*0],p[i+3*1],p[i+3*2]);	}
	ILINE Vec3_tpl<F> GetColumn0() const	{ return Vec3_tpl<F> (m00,m10,m20);	}
	ILINE Vec3_tpl<F> GetColumn1() const	{ return Vec3_tpl<F> (m01,m11,m21);	}
	ILINE Vec3_tpl<F> GetColumn2() const	{ return Vec3_tpl<F> (m02,m12,m22);	}


	ILINE Matrix33_tpl<F>& operator *= (F op) {
		m00*=op; m01*=op;	m02*=op;
		m10*=op; m11*=op;	m12*=op;
		m20*=op; m21*=op;	m22*=op;
		return *this;
	}

	ILINE Matrix33_tpl<F>& operator /= (F op) {
		F iop=(F)1.0/op;
		m00*=iop; m01*=iop;	m02*=iop;
		m10*=iop; m11*=iop;	m12*=iop;
		m20*=iop; m21*=iop;	m22*=iop;
		return *this;
	}

	bool IsEquivalent( const Matrix33_tpl<F>& m, F e=VEC_EPSILON) const {
		return  ( 
			(fabs_tpl(m00-m.m00)<=e) && (fabs_tpl(m01-m.m01)<=e) && (fabs_tpl(m02-m.m02)<=e) && 
			(fabs_tpl(m10-m.m10)<=e) && (fabs_tpl(m11-m.m11)<=e) && (fabs_tpl(m12-m.m12)<=e) && 
			(fabs_tpl(m20-m.m20)<=e) && (fabs_tpl(m21-m.m21)<=e) && (fabs_tpl(m22-m.m22)<=e) 
			);	
	}

	bool IsIdentity() const	{
		return 0 == (fabs_tpl((F)1-m00) + fabs_tpl(m01) + fabs_tpl(m02)+ fabs_tpl(m10) + fabs_tpl((F)1-m11) + fabs_tpl(m12)+ fabs_tpl(m20) + fabs_tpl(m21) + fabs_tpl((F)1-m22));
	}
	int IsZero() const {
		return 0 == (fabs_tpl(m00)+fabs_tpl(m01)+fabs_tpl(m02)+fabs_tpl(m10)+fabs_tpl(m11)+fabs_tpl(m12)+fabs_tpl(m20)+fabs_tpl(m21)+fabs_tpl(m22));
	}

	//check if we have an orthonormal-base (general case, works even with reflection matrices)
	int IsOrthonormal(F threshold=0.001) const { 
		f32 d0=fabs_tpl(GetColumn0()|GetColumn1());	if 	(d0>threshold) return 0;	
		f32 d1=fabs_tpl(GetColumn0()|GetColumn2());	if 	(d1>threshold) return 0;	
		f32 d2=fabs_tpl(GetColumn1()|GetColumn2());	if 	(d2>threshold) return 0;	
		int a=(fabs_tpl(1-(GetColumn0()|GetColumn0())))<threshold;
		int b=(fabs_tpl(1-(GetColumn1()|GetColumn1())))<threshold;
		int c=(fabs_tpl(1-(GetColumn2()|GetColumn2())))<threshold;
		return a&b&c;
	}

	//check if we have an orthonormal-base (assuming we are using a right-handed coordinate system)
	int IsOrthonormalRH(F threshold=0.002) const { 
		Vec3_tpl<F> x=GetColumn0();
		Vec3_tpl<F> y=GetColumn1();
		Vec3_tpl<F> z=GetColumn2();
		bool a=x.IsEquivalent(y%z,threshold);
		bool b=y.IsEquivalent(z%x,threshold);
		bool c=z.IsEquivalent(x%y,threshold);
		a&=x.IsUnit(0.01f);
		b&=y.IsUnit(0.01f);
		c&=z.IsUnit(0.01f);
		return a&b&c;

	}

	//////////////////////////////////////////////////////////////////////////
	// Remove uniform scale from matrix.
	ILINE void NoScale()
	{
		*this /= GetColumn(0).GetLength();
	}

	bool IsValid() const
	{
		if (!NumberValid(m00)) return false;	if (!NumberValid(m01)) return false;	if (!NumberValid(m02)) return false;
		if (!NumberValid(m10)) return false;	if (!NumberValid(m11)) return false;	if (!NumberValid(m12)) return false;
		if (!NumberValid(m20)) return false;	if (!NumberValid(m21)) return false;	if (!NumberValid(m22)) return false;
		return true;
	}

	// Gram Schmitt Orthogonalization
	// - favors the x axis over the y axis over the z axis (property of the algorithm but favoring can be changed)
	// - parity (left handed or right handed) remains from input
	// - degenerated matrix input (point,line,plane) results in degenerated output
	template<class F1, class F2> void Orthogonalization_GramSchmidt();


};

///////////////////////////////////////////////////////////////////////////////
// Typedefs                                                                  //
///////////////////////////////////////////////////////////////////////////////

typedef Matrix33_tpl<f32> Matrix33;
typedef Matrix33_tpl<f64> Matrix33_f64;
typedef Matrix33_tpl<real> Matrix33r;

//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
//-------------       implementation of Matrix33      ------------------------------
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------

template<class F1, class F2> 
Matrix33_tpl<F1> operator*(const Matrix33_tpl<F1> &l, const Diag33_tpl<F2> &r) {
	assert(l.IsValid());
	assert(r.IsValid());
	Matrix33_tpl<F1> res;
	res.m00 = l.m00*r.x;		res.m01 = l.m01*r.y;		res.m02 = l.m02*r.z;
	res.m10 = l.m10*r.x;		res.m11 = l.m11*r.y;		res.m12 = l.m12*r.z;
	res.m20 = l.m20*r.x;		res.m21 = l.m21*r.y;		res.m22 = l.m22*r.z;
	return res;
}
template<class F1, class F2> 
Matrix33_tpl<F1>& operator *= (Matrix33_tpl<F1> &l, const Diag33_tpl<F2> &r) {
	assert(l.IsValid());
	assert(r.IsValid());
	l.m00*=r.x;	l.m01*=r.y;	l.m02*=r.z; 
	l.m10*=r.x;	l.m11*=r.y;	l.m12*=r.z;
	l.m20*=r.x;	l.m21*=r.y;	l.m22*=r.z;
	return l;
}

//Matrix33 operations with another Matrix33
template<class F1, class F2> 
Matrix33_tpl<F1> operator*(const Matrix33_tpl<F1> &l, const Matrix33_tpl<F2> &r) {
	assert(l.IsValid());
	assert(r.IsValid());
	Matrix33_tpl<F1> m;
	m.m00 = l.m00*r.m00 +	l.m01*r.m10 +	l.m02*r.m20;
	m.m01 = l.m00*r.m01 +	l.m01*r.m11 +	l.m02*r.m21;
	m.m02 = l.m00*r.m02 +	l.m01*r.m12 +	l.m02*r.m22;
	m.m10 = l.m10*r.m00 +	l.m11*r.m10 +	l.m12*r.m20;
	m.m11 = l.m10*r.m01 +	l.m11*r.m11 +	l.m12*r.m21;
	m.m12 = l.m10*r.m02 +	l.m11*r.m12 +	l.m12*r.m22;
	m.m20 = l.m20*r.m00 +	l.m21*r.m10 + l.m22*r.m20;
	m.m21 = l.m20*r.m01 +	l.m21*r.m11 +	l.m22*r.m21;
	m.m22 = l.m20*r.m02 +	l.m21*r.m12 +	l.m22*r.m22;
	return m;
}


/*!
*
*  Implements the multiplication operator: Matrix34=Matrix33*Matrix34
*
*  Matrix33 and Matrix34 are specified in collumn order for a right-handed coordinate-system.        
*  AxB = operation B followed by operation A.  
*  A multiplication takes 36 muls and 24 adds. 
*
*  Example:
*   Matrix33 m33=Matrix33::CreateRotationX(1.94192f);;
*   Matrix34 m34=Matrix34::CreateRotationZ(3.14192f);
*	  Matrix34 result=m33*m34;
*
*/
template<class F1, class F2, class B> 
Matrix34_tpl<F2, B> operator * (const Matrix33_tpl<F1>& l, const Matrix34_tpl<F2, B>& r) {
	assert(l.IsValid());
	assert(r.IsValid());
	Matrix34_tpl<F2, B> m;
	m.m00 = l.m00*r.m00 + l.m01*r.m10 + l.m02*r.m20;
	m.m10 = l.m10*r.m00 + l.m11*r.m10 + l.m12*r.m20;
	m.m20 = l.m20*r.m00 + l.m21*r.m10 + l.m22*r.m20;
	m.m01 = l.m00*r.m01 + l.m01*r.m11 + l.m02*r.m21;
	m.m11 = l.m10*r.m01 + l.m11*r.m11 + l.m12*r.m21;
	m.m21 = l.m20*r.m01 + l.m21*r.m11 + l.m22*r.m21;
	m.m02 = l.m00*r.m02 + l.m01*r.m12 + l.m02*r.m22;
	m.m12 = l.m10*r.m02 + l.m11*r.m12 + l.m12*r.m22;
	m.m22 = l.m20*r.m02 + l.m21*r.m12 + l.m22*r.m22;
	m.m03 = l.m00*r.m03 + l.m01*r.m13 + l.m02*r.m23;
	m.m13 = l.m10*r.m03 + l.m11*r.m13 + l.m12*r.m23;
	m.m23 = l.m20*r.m03 + l.m21*r.m13 + l.m22*r.m23;
	return m;
}


/*!
*
*  Implements the multiplication operator: Matrix44=Matrix33*Matrix44
*
*  Matrix33 and Matrix44 are specified in collumn order for a right-handed coordinate-system.        
*  AxB = operation B followed by operation A.  
*  A multiplication takes 36 muls and 24 adds. 
*
*  Example:
*   Matrix33 m33=Matrix33::CreateRotationX(1.94192f);;
*   Matrix44 m44=Matrix33::CreateRotationZ(3.14192f);
*	  Matrix44 result=m33*m44;
*
*/
template<class F1, class F2, class B, class C> 
Matrix44_tpl<F2, C> operator * (const Matrix33_tpl<F1>& l, const Matrix44_tpl<F2, B>& r) {
	assert(l.IsValid());
	assert(r.IsValid());
	Matrix44_tpl<F2, C> m;
	m.m00 = l.m00*r.m00 + l.m01*r.m10 + l.m02*r.m20;
	m.m10 = l.m10*r.m00 + l.m11*r.m10 + l.m12*r.m20;
	m.m20 = l.m20*r.m00 + l.m21*r.m10 + l.m22*r.m20;
	m.m30 = r.m30;
	m.m01 = l.m00*r.m01 + l.m01*r.m11 + l.m02*r.m21;
	m.m11 = l.m10*r.m01 + l.m11*r.m11 + l.m12*r.m21;
	m.m21 = l.m20*r.m01 + l.m21*r.m11 + l.m22*r.m21;
	m.m31 = r.m31;
	m.m02 = l.m00*r.m02 + l.m01*r.m12 + l.m02*r.m22;
	m.m12 = l.m10*r.m02 + l.m11*r.m12 + l.m12*r.m22;
	m.m22 = l.m20*r.m02 + l.m21*r.m12 + l.m22*r.m22;
	m.m32 = r.m32;
	m.m03 = l.m00*r.m03 + l.m01*r.m13 + l.m02*r.m23;
	m.m13 = l.m10*r.m03 + l.m11*r.m13 + l.m12*r.m23;
	m.m23 = l.m20*r.m03 + l.m21*r.m13 + l.m22*r.m23;
	m.m33 = r.m33;
	return m;
}

template<class F1, class F2> 
Matrix33_tpl<F1>& operator *= (Matrix33_tpl<F1> &l,	const Matrix33_tpl<F2> &r) 
{
	assert(l.IsValid());
	assert(r.IsValid());
	F1 oldata[9]; 
	oldata[0]=l.m00; oldata[1]=l.m01; oldata[2]=l.m02;
	oldata[3]=l.m10; oldata[4]=l.m11; oldata[5]=l.m12; 
	oldata[6]=l.m20; oldata[7]=l.m21; oldata[8]=l.m22; 
	l.m00 = oldata[0]*r.m00 + oldata[1]*r.m10 +		oldata[2]*r.m20;
	l.m01 = oldata[0]*r.m01 + oldata[1]*r.m11 + 	oldata[2]*r.m21;
	l.m02 = oldata[0]*r.m02 +	oldata[1]*r.m12 + 	oldata[2]*r.m22;
	l.m10 = oldata[3]*r.m00 +	oldata[4]*r.m10 + 	oldata[5]*r.m20;
	l.m11 = oldata[3]*r.m01 + oldata[4]*r.m11 + 	oldata[5]*r.m21;
	l.m12 = oldata[3]*r.m02 + oldata[4]*r.m12 + 	oldata[5]*r.m22;
	l.m20 = oldata[6]*r.m00 +	oldata[7]*r.m10 + 	oldata[8]*r.m20;
	l.m21 = oldata[6]*r.m01 +	oldata[7]*r.m11 + 	oldata[8]*r.m21;
	l.m22 = oldata[6]*r.m02 + oldata[7]*r.m12 + 	oldata[8]*r.m22;
	return l;
}


template<class F1, class F2> 
ILINE Matrix33_tpl<F1> operator+(const Matrix33_tpl<F1> &l, const Matrix33_tpl<F2> &r) {
	assert(l.IsValid());
	assert(r.IsValid());
	Matrix33_tpl<F1> res; 
	res.m00=l.m00+r.m00;  res.m01=l.m01+r.m01;	res.m02=l.m02+r.m02; 
	
	res.m10=l.m10+r.m10;  res.m11=l.m11+r.m11;  res.m12=l.m12+r.m12;
	res.m20=l.m20+r.m20;  res.m21=l.m21+r.m21;  res.m22=l.m22+r.m22; 
	return res;
}
template<class F1, class F2> 
ILINE Matrix33_tpl<F1>& operator+=(Matrix33_tpl<F1> &l,	const Matrix33_tpl<F2> &r) {
	assert(l.IsValid());
	assert(r.IsValid());
	l.m00+=r.m00; l.m01+=r.m01;	l.m02+=r.m02; 
	l.m10+=r.m10;	l.m11+=r.m11; l.m12+=r.m12; 
	l.m20+=r.m20; l.m21+=r.m21;	l.m22+=r.m22; 
	return l;
}




template<class F1, class F2> 
ILINE Matrix33_tpl<F1> operator - (const Matrix33_tpl<F1> &l, const Matrix33_tpl<F2> &r) {
	assert(l.IsValid());
	assert(r.IsValid());
	Matrix33_tpl<F1> res;
	res.m00 = l.m00-r.m00;	res.m01 = l.m01-r.m01;	res.m02 = l.m02-r.m02; 
	res.m10 = l.m10-r.m10; 	res.m11 = l.m11-r.m11; 	res.m12 = l.m12-r.m12;
	res.m20 = l.m20-r.m20; 	res.m21 = l.m21-r.m21; 	res.m22 = l.m22-r.m22; 
	return res;
}
template<class F1, class F2> 
ILINE Matrix33_tpl<F1>& operator-=(Matrix33_tpl<F1> &l, const Matrix33_tpl<F2> &r) 
{
	assert(l.IsValid());
	assert(r.IsValid());
	l.m00-=r.m00; l.m01-=r.m01;	l.m02-=r.m02;	
	l.m10-=r.m10;	l.m11-=r.m11; l.m12-=r.m12; 
	l.m20-=r.m20; l.m21-=r.m21;	l.m22-=r.m22; 
	return l;
}




template<class F>
ILINE Matrix33_tpl<F> operator*(const Matrix33_tpl<F> &m, F op) {
	assert(m.IsValid());
	Matrix33_tpl<F> res;
	res.m00=m.m00*op; res.m01=m.m01*op;	res.m02=m.m02*op; 
	res.m10=m.m10*op; res.m11=m.m11*op; res.m12=m.m12*op;
	res.m20=m.m20*op; res.m21=m.m21*op;	res.m22=m.m22*op;
	return res;
}
template<class F>
ILINE Matrix33_tpl<F> operator/(const Matrix33_tpl<F> &src, F op) { return src*((F)1.0/op); }







//post-multiply
template<class F1, class F2>
Vec3_tpl<F1> operator*(const Matrix33_tpl<F2> &m, const Vec3_tpl<F1> &v) {
	assert(m.IsValid());
	assert(v.IsValid());
	return Vec3_tpl<F1>(v.x*m.m00 + v.y*m.m01 + v.z*m.m02,
		v.x*m.m10 + v.y*m.m11 + v.z*m.m12,
		v.x*m.m20 + v.y*m.m21 + v.z*m.m22);
}

//pre-multiply
template<class F1, class F2>
Vec3_tpl<F1> operator*(const Vec3_tpl<F1> &v, const Matrix33_tpl<F2> &m) {
	assert(m.IsValid());
	assert(v.IsValid());
	return Vec3_tpl<F1>(v.x*m.m00 + v.y*m.m10 + v.z*m.m20,
		v.x*m.m01 + v.y*m.m11 + v.z*m.m21,
		v.x*m.m02 + v.y*m.m12 + v.z*m.m22);
}

//post-multiply
template<class F1, class F2>
ILINE Vec2_tpl<F1> operator*(const Matrix33_tpl<F2> &m, const Vec2_tpl<F1> &v) {
	assert(m.IsValid());
	assert(v.IsValid());
	return Vec2_tpl<F1>(v.x*m.m00+v.y*m.m01, 	v.x*m.m10 + v.y*m.m11);
}

//pre-multiply
template<class F1, class F2>
ILINE Vec2_tpl<F1> operator*(const Vec2_tpl<F1> &v, const Matrix33_tpl<F2> &m) {
	assert(m.IsValid());
	assert(v.IsValid());
	return Vec2_tpl<F1>(v.x*m.m00 + v.y*m.m10, v.x*m.m01 + v.y*m.m11);
}

template<class F1> 
Matrix33_tpl<F1>& crossproduct_matrix(const Vec3_tpl<F1> &v, Matrix33_tpl<F1> &m) {
	m.m00=0;			m.m01=-v.z;		m.m02=v.y;
	m.m10=v.z;    m.m11=0;			m.m12=-v.x;
	m.m20=-v.y;	  m.m21=v.x;		m.m22=0;
	return m;
}

template<class F1> 
Matrix33_tpl<F1>& dotproduct_matrix(const Vec3_tpl<F1> &v, const Vec3_tpl<F1> &op, Matrix33_tpl<F1> &m) {
	m.m00=v.x*op.x;		m.m10=v.y*op.x;		m.m20=v.z*op.x;
	m.m01=v.x*op.y;		m.m11=v.y*op.y;		m.m21=v.z*op.y;
	m.m02=v.x*op.z;		m.m12=v.y*op.z;		m.m22=v.z*op.z;
	return m;
}











///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// struct Matrix34_tpl
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
template <typename F, class A=XMVec4A> struct Matrix34_tpl
{
#ifndef XENON_INTRINSICS
	F m00,m01,m02,m03;
	F m10,m11,m12,m13;
	F m20,m21,m22,m23;











#endif

	//default constructor
#if defined(_DEBUG) && !defined(__SPU__)
	ILINE Matrix34_tpl() 
	{
		if (sizeof(F)==4)
		{
			uint32* p=(uint32*)&m00;
			p[ 0]=F32NAN;	p[ 1]=F32NAN;	p[ 2]=F32NAN;	p[ 3]=F32NAN;
			p[ 4]=F32NAN;	p[ 5]=F32NAN;	p[ 6]=F32NAN;	p[ 7]=F32NAN;
			p[ 8]=F32NAN;	p[ 9]=F32NAN;	p[10]=F32NAN;	p[11]=F32NAN;
		}
		if (sizeof(F)==8)
		{
			uint64* p=(uint64*)&m00;
			p[ 0]=F64NAN;	p[ 1]=F64NAN;	p[ 2]=F64NAN;	p[ 3]=F64NAN;
			p[ 4]=F64NAN;	p[ 5]=F64NAN;	p[ 6]=F64NAN;	p[ 7]=F64NAN;
			p[ 8]=F64NAN;	p[ 9]=F64NAN;	p[10]=F64NAN;	p[11]=F64NAN;
		}
	}
#else
	ILINE Matrix34_tpl(){};
#endif


	template<class F1> ILINE Matrix34_tpl(const Matrix33_tpl<F1>& m)
  {
		assert(m.IsValid());
#ifndef XENON_INTRINSICS
		m00=F(m.m00);		m01=F(m.m01);		m02=F(m.m02);		m03=0;	
		m10=F(m.m10);		m11=F(m.m11);		m12=F(m.m12);		m13=0;
		m20=F(m.m20);		m21=F(m.m21);		m22=F(m.m22);		m23=0;








#endif
	}
	template<class B> ILINE Matrix34_tpl( const Matrix34_tpl<F, B>& m )
  {
		assert(m.IsValid());
#ifndef XENON_INTRINSICS
		m00=m.m00;		m01=m.m01;		m02=m.m02;	m03=m.m03;
		m10=m.m10;		m11=m.m11;		m12=m.m12;	m13=m.m13;
		m20=m.m20;		m21=m.m21;		m22=m.m22;	m23=m.m23;







#endif
	}
	template<class F1, class B> ILINE explicit Matrix34_tpl( const Matrix34_tpl<F1, B>& m )
  {
		assert(m.IsValid());
#ifndef XENON_INTRINSICS
		m00=F(m.m00);		m01=F(m.m01);		m02=F(m.m02);	m03=F(m.m03);	
		m10=F(m.m10);		m11=F(m.m11);		m12=F(m.m12);	m13=F(m.m13);
		m20=F(m.m20);		m21=F(m.m21);		m22=F(m.m22);	m23=F(m.m23);







#endif
	}
	template<class F1, class B> ILINE explicit Matrix34_tpl(const Matrix44_tpl<F1, B>& m )
  {
		assert(m.IsValid());
#ifndef XENON_INTRINSICS
		m00=F(m.m00);		m01=F(m.m01);		m02=F(m.m02);		m03=F(m.m03);	
		m10=F(m.m10);		m11=F(m.m11);		m12=F(m.m12);		m13=F(m.m13);
		m20=F(m.m20);		m21=F(m.m21);		m22=F(m.m22);		m23=F(m.m23);







#endif
	}
	template<class F1> ILINE Matrix34_tpl(const Matrix33_tpl<F1>& m, const Vec3_tpl<F1>& t)
  {
		assert(m.IsValid());
		assert(t.IsValid());
#ifndef XENON_INTRINSICS
		m00=F(m.m00);	m01=F(m.m01);	m02=F(m.m02);	m03=F(t.x);	
		m10=F(m.m10);	m11=F(m.m11);	m12=F(m.m12);	m13=F(t.y);
		m20=F(m.m20);	m21=F(m.m21);	m22=F(m.m22);	m23=F(t.z);








#endif
	}

	explicit ILINE Matrix34_tpl( const Quat_tpl<F>& q ) { *this=Matrix33(q); }

	//Convert unit quaternion to matrix.
	template<class F1> explicit Matrix34_tpl( const QuatT_tpl<F1>& q ) {
		assert(q.q.IsValid());
		Vec3_tpl<F1> v2=q.q.v+q.q.v;
		F1 xx=1-v2.x*q.q.v.x;		F1 yy=v2.y*q.q.v.y;		F1 xw=v2.x*q.q.w;
		F1 xy=v2.y*q.q.v.x;			F1 yz=v2.z*q.q.v.y;		F1 yw=v2.y*q.q.w;
		F1 xz=v2.z*q.q.v.x;			F1 zz=v2.z*q.q.v.z;		F1 zw=v2.z*q.q.w;
		m00=F(1-yy-zz);					m01=F(xy-zw);					m02=F(xz+yw);			m03=F(q.t.x);
		m10=F(xy+zw);						m11=F(xx-zz);					m12=F(yz-xw);			m13=F(q.t.y);
		m20=F(xz-yw);						m21=F(yz+xw);					m22=F(xx-yy);			m23=F(q.t.z);
	}

	template<class F1> explicit Matrix34_tpl( const QuatTS_tpl<F1>& q ) {
		assert(q.q.IsValid());
		Vec3_tpl<F1> v2=q.q.v+q.q.v;
		F1 xx=1-v2.x*q.q.v.x;		F1 yy=v2.y*q.q.v.y;		F1 xw=v2.x*q.q.w;
		F1 xy=v2.y*q.q.v.x;			F1 yz=v2.z*q.q.v.y;		F1 yw=v2.y*q.q.w;
		F1 xz=v2.z*q.q.v.x;			F1 zz=v2.z*q.q.v.z;		F1 zw=v2.z*q.q.w;
		m00=F((1-yy-zz)*q.s);		m01=F((xy-zw)*q.s);		m02=F((xz+yw)*q.s); m03=F(q.t.x);
		m10=F((xy+zw)*q.s);			m11=F((xx-zz)*q.s);		m12=F((yz-xw)*q.s);	m13=F(q.t.y);
		m20=F((xz-yw)*q.s);			m21=F((yz+xw)*q.s);		m22=F((xx-yy)*q.s);	m23=F(q.t.z);
	}

	template<class F1> explicit Matrix34_tpl( const QuatD_tpl<F1>& q ) 
	{
		assert((fabs_tpl(1-(q.nq|q.nq)))<0.01); //check if unit-quaternion

		Vec3 t=(q.nq.w*q.dq.v - q.dq.w*q.nq.v + q.nq.v%q.dq.v); //perfect for HLSL

		Vec3_tpl<F1> v2=q.nq.v+q.nq.v;
		F1 xx=1-v2.x*q.nq.v.x;	F1 yy=v2.y*q.nq.v.y;	F1 xw=v2.x*q.nq.w;
		F1 xy=v2.y*q.nq.v.x;		F1 yz=v2.z*q.nq.v.y;	F1 yw=v2.y*q.nq.w;
		F1 xz=v2.z*q.nq.v.x;		F1 zz=v2.z*q.nq.v.z;	F1 zw=v2.z*q.nq.w;
		m00=F(1-yy-zz);					m01=F(xy-zw);					m02=F(xz+yw);			m03=F(t.x+t.x);
		m10=F(xy+zw);						m11=F(xx-zz);					m12=F(yz-xw);			m13=F(t.y+t.y);
		m20=F(xz-yw);						m21=F(yz+xw);					m22=F(xx-yy);			m23=F(t.z+t.z);
	}

	//! build a matrix from 12 f32s
	ILINE Matrix34_tpl(	f32 v00, f32 v01, f32 v02, f32 v03,	f32 v10, f32 v11, f32 v12, f32 v13,	f32 v20, f32 v21, f32 v22, f32 v23) :
		m00(v00), m01(v01), m02(v02), m03(v03), 
		m10(v10), m11(v11), m12(v12), m13(v13), 
		m20(v20), m21(v21), m22(v22), m23(v23) 
	{ 		
	}

	Matrix34_tpl( const Vec3_tpl<F>& s, const Quat_tpl<F>& q, const Vec3_tpl<F>& t=Vec3(ZERO) ) {	Set(s, q, t); }

	//apply scaling to the columns of the matrix.
	ILINE void ScaleColumn( const Vec3_tpl<F>& s)
  {
#ifndef XENON_INTRINSICS
    m00*=s.x;		m01*=s.y;		m02*=s.z;
    m10*=s.x;		m11*=s.y;		m12*=s.z;
    m20*=s.x;		m21*=s.y;		m22*=s.z;








#endif
	}
	//apply scaling to matrix.
	ILINE void Scale( const Vec3_tpl<F>& s )
	{
#ifndef XENON_INTRINSICS
		m00*=s.x; m01*=s.y; m02*=s.z;
		m10*=s.x; m11*=s.y; m12*=s.z;
		m20*=s.x; m21*=s.y; m22*=s.z;








#endif
	}



	/*!
	*
	*  Initializes the Matrix34 with the identity.
	*
	*/
	void SetIdentity( void )
  {
#ifndef XENON_INTRINSICS
		m00=1.0f;	m01=0.0f;	m02=0.0f;	m03=0.0f;
		m10=0.0f;	m11=1.0f;	m12=0.0f;	m13=0.0f;
		m20=0.0f;	m21=0.0f; m22=1.0f;	m23=0.0f;




#endif
	}
	ILINE static Matrix34_tpl<F> CreateIdentity( void ) {	
		Matrix34_tpl<F> m34; m34.SetIdentity();	return m34;	
	}
	ILINE Matrix34_tpl(type_identity) { SetIdentity(); }




	/*!
	*  Create a rotation matrix around an arbitrary axis (Eulers Theorem).  
	*  The axis is specified as an normalized Vec3. The angle is assumed to be in radians.  
	*  This function also assumes a translation-vector and stores it in the right column.  
	*
	*  Example:
	*		Matrix34 m34;
	*		Vec3 axis=GetNormalized( Vec3(-1.0f,-0.3f,0.0f) );
	*		m34.SetRotationAA( 3.14314f, axis, Vec3(5,5,5) );
	*/
	ILINE void SetRotationAA(const F rad, const Vec3_tpl<F>& axis, const Vec3_tpl<F>& t=Vec3(ZERO) )	{
		*this=Matrix33_tpl<F>::CreateRotationAA(rad,axis); this->SetTranslation(t);
	}
	ILINE static Matrix34_tpl<F> CreateRotationAA( const F rad, const Vec3_tpl<F>& axis, const Vec3_tpl<F>& t=Vec3(ZERO) )	{	
		Matrix34_tpl<F> m34;  m34.SetRotationAA(rad,axis,t);	return m34;	
	}

	ILINE void SetRotationAA(const Vec3_tpl<F>& rot, const Vec3_tpl<F>& t=Vec3(ZERO) ) {
		*this=Matrix33_tpl<F>::CreateRotationAA(rot); this->SetTranslation(t);
	}
	ILINE static Matrix34_tpl<F> CreateRotationAA( const Vec3_tpl<F>& rot, const Vec3_tpl<F>& t=Vec3(ZERO) )	{	
		Matrix34_tpl<F> m34;  m34.SetRotationAA(rot,t);	return m34;	
	}


	/*!
	* Create rotation-matrix about X axis using an angle.
	* The angle is assumed to be in radians. 
	* The translation-vector is set to zero.  
	*
	*  Example:
	*		Matrix34 m34;
	*		m34.SetRotationX(0.5f);
	*/
	ILINE void SetRotationX(const f32 rad, const Vec3_tpl<F>& t=Vec3(ZERO) )	{
		*this=Matrix33_tpl<F>::CreateRotationX(rad); this->SetTranslation(t);
	}
	ILINE static Matrix34_tpl<F> CreateRotationX( const f32 rad, const Vec3_tpl<F>& t=Vec3(ZERO) ) {
		Matrix34_tpl<F> m34;  m34.SetRotationX(rad,t);	return m34;	
	}

	/*!
	* Create rotation-matrix about Y axis using an angle.
	* The angle is assumed to be in radians. 
	* The translation-vector is set to zero.  
	*
	*  Example:
	*		Matrix34 m34;
	*		m34.SetRotationY(0.5f);
	*/
	ILINE void SetRotationY(const f32 rad, const Vec3_tpl<F>& t=Vec3(ZERO) )	{
		*this=Matrix33_tpl<F>::CreateRotationY(rad);	this->SetTranslation(t);
	}
	ILINE static Matrix34_tpl<F> CreateRotationY( const f32 rad, const Vec3_tpl<F>& t=Vec3(ZERO) )	{	
		Matrix34_tpl<F> m34;  m34.SetRotationY(rad,t);	return m34;	
	}

	/*!
	* Create rotation-matrix about Z axis using an angle.
	* The angle is assumed to be in radians. 
	* The translation-vector is set to zero.  
	*
	*  Example:
	*		Matrix34 m34;
	*		m34.SetRotationZ(0.5f);
	*/
	ILINE void SetRotationZ(const f32 rad, const Vec3_tpl<F>& t=Vec3(ZERO) )	{
		*this=Matrix33_tpl<F>::CreateRotationZ(rad);  this->SetTranslation(t);
	}
	ILINE static Matrix34_tpl<F> CreateRotationZ( const f32 rad, const Vec3_tpl<F>& t=Vec3(ZERO) )	{	
		Matrix34_tpl<F> m34;  m34.SetRotationZ(rad,t);	return m34;	
	}


	/*!
	*
	* Convert three Euler angle to mat33 (rotation order:XYZ)
	* The Euler angles are assumed to be in radians. 
	* The translation-vector is set to zero.  
	*
	*  Example 1:
	*		Matrix34 m34;
	*		m34.SetRotationXYZ( Ang3(0.5f,0.2f,0.9f), translation );
	*
	*  Example 2:
	*		Matrix34 m34=Matrix34::CreateRotationXYZ( Ang3(0.5f,0.2f,0.9f), translation );
	*/
	ILINE void SetRotationXYZ( const Ang3_tpl<F>& rad, const Vec3_tpl<F>& t=Vec3(ZERO) ) 
	{
		assert(rad.IsValid());
		assert(t.IsValid());
		*this=Matrix33_tpl<F>::CreateRotationXYZ(rad); this->SetTranslation(t);
	}
	ILINE static Matrix34_tpl<F> CreateRotationXYZ( const Ang3_tpl<F>& rad, const Vec3_tpl<F>& t=Vec3(ZERO) ) {	
		assert(rad.IsValid());
		assert(t.IsValid());
		Matrix34_tpl<F> m34;  m34.SetRotationXYZ(rad,t);	return m34;	
	}


	ILINE void SetRotationAA(F c, F s, Vec3_tpl<F> axis, const Vec3_tpl<F>& t=Vec3(ZERO) ) { 
		assert(axis.IsValid());
		assert(t.IsValid());
		*this=Matrix33_tpl<F>::CreateRotationAA(c, s, axis);
		m03=t.x; m13=t.y; m23=t.z;
	}
	ILINE static Matrix34_tpl<F> CreateRotationAA(F c, F s, Vec3_tpl<F> axis, const Vec3_tpl<F>& t=Vec3(ZERO) ) {	
		Matrix34_tpl<F> m34;	m34.SetRotationAA(c,s,axis,t); return m34;	
	}

	void Set( const Vec3_tpl<F>& s, const Quat_tpl<F>& q, const Vec3_tpl<F>& t=Vec3(ZERO)  );
	static Matrix34_tpl<F> Create(  const Vec3_tpl<F>& s, const Quat_tpl<F>& q, const Vec3_tpl<F>& t=Vec3(ZERO) );

	void SetScale( const Vec3_tpl<F> &s, const Vec3_tpl<F>& t=Vec3(ZERO) );
	static Matrix34_tpl<F> CreateScale(  const Vec3_tpl<F> &s, const Vec3_tpl<F>& t=Vec3(ZERO)  );

	ILINE void SetTranslationMat(  const Vec3_tpl<F>& v  ) {
		m00=1.0f;	m01=0.0f;	m02=0.0f;	m03=v.x;
		m10=0.0f;	m11=1.0f;	m12=0.0f;	m13=v.y;
		m20=0.0f;	m21=0.0f;	m22=1.0f;	m23=v.z;
	}
	ILINE static Matrix34_tpl<F, A> CreateTranslationMat(  const Vec3_tpl<F>& v  ) {	Matrix34_tpl<F, A> m34; m34.SetTranslationMat(v); return m34; 	}


	//NOTE: all vectors are stored in columns
	ILINE void SetFromVectors(const Vec3& vx, const Vec3& vy, const Vec3& vz, const Vec3& pos)	{
		m00=vx.x;		m01=vy.x;		m02=vz.x;		m03 = pos.x;
		m10=vx.y;		m11=vy.y;		m12=vz.y;		m13 = pos.y;
		m20=vx.z;		m21=vy.z;		m22=vz.z;		m23 = pos.z;
	}
	ILINE static Matrix34_tpl<F> CreateFromVectors(const Vec3_tpl<F>& vx, const Vec3_tpl<F>& vy, const Vec3_tpl<F>& vz, const Vec3_tpl<F>& pos) {
		Matrix34_tpl<F> m; m.SetFromVectors(vx,vy,vz,pos); return m;
	}

	void InvertFast() { // in-place transposition
		assert(IsOrthonormal());
		F t; Vec3 v(m03,m13,m23);
		t=m01; m01=m10; m10=t; m03=-v.x*m00-v.y*m01-v.z*m20;
		t=m02; m02=m20; m20=t; m13=-v.x*m10-v.y*m11-v.z*m21;
		t=m12; m12=m21; m21=t; m23=-v.x*m20-v.y*m21-v.z*m22;
	}

	Matrix34_tpl<F> GetInvertedFast() const {
		assert(IsOrthonormal());
		Matrix34_tpl<F> dst;
		dst.m00=m00;	dst.m01=m10;	dst.m02=m20;	dst.m03=-m03*m00-m13*m10-m23*m20;
		dst.m10=m01;	dst.m11=m11;	dst.m12=m21;	dst.m13=-m03*m01-m13*m11-m23*m21;
		dst.m20=m02;	dst.m21=m12;	dst.m22=m22;	dst.m23=-m03*m02-m13*m12-m23*m22;
		return dst;
	}

	void Invert( void );
	Matrix34_tpl<F> GetInverted() const;

	//! transforms a vector. the translation is not beeing considered
	ILINE Vec3_tpl<F> TransformVector(const Vec3_tpl<F> &v) const  {
		assert(v.IsValid());
		return Vec3_tpl<F>(m00*v.x+m01*v.y+m02*v.z, m10*v.x+m11*v.y+m12*v.z, m20*v.x+m21*v.y+m22*v.z);
	}
	//! transforms a point and add translation vector
	ILINE Vec3_tpl<F> TransformPoint(const Vec3_tpl<F> &p) const  {
		assert(p.IsValid());
		return Vec3_tpl<F>( m00*p.x+m01*p.y+m02*p.z+m03, m10*p.x+m11*p.y+m12*p.z+m13, m20*p.x+m21*p.y+m22*p.z+m23);
	}

	//! Remove scale from matrix.
	void OrthonormalizeFast()
	{
		Vec3 x( m00,m10,m20 );
		Vec3 y( m01,m11,m21 );
		Vec3 z;
		x = x.GetNormalized();
		z = (x % y).GetNormalized();
		y = (z % x).GetNormalized();
		m00=x.x;		m10=x.y;		m20=x.z;
		m01=y.x;		m11=y.y;		m21=y.z;
		m02=z.x;		m12=z.y;		m22=z.z;
	}


	//determinant is ambiguous: only the upper-left-submatrix's determinant is calculated
	ILINE f32 Determinant() const		{
		return (m00*m11*m22) + (m01*m12*m20) + (m02*m10*m21) - (m02*m11*m20) - (m00*m12*m21) - (m01*m10*m22);
	}

	/*!
	*  Direct-Matrix-Slerp: for the sake of completeness, I have included the following expression 
	*  for Spherical-Linear-Interpolation without using quaternions. This is much faster then converting 
	*  both matrices into quaternions in order to do a quaternion slerp and then converting the slerped 
	*  quaternion back into a matrix.
	*  This is a high-precision calculation. Given two orthonormal 3x3 matrices this function calculates 
	*  the shortest possible interpolation-path between the two rotations. The interpolation curve forms 
	*  a great arc on the rotation sphere (geodesic). Not only does Slerp follow a great arc it follows 
	*  the shortest great arc.	Furthermore Slerp has constant angular velocity. All in all Slerp is the 
	*  optimal interpolation curve between two rotations. 
	*
	*  STABILITY PROBLEM: There are two singularities at angle=0 and angle=PI. At 0 the interpolation-axis 
	*  is arbitrary, which means any axis will produce the same result because we have no rotation. Thats 
	*  why I'm using (1,0,0). At PI the rotations point away from each other and the interpolation-axis 
	*  is unpredictable. In this case I'm also using the axis (1,0,0). If the angle is ~0 or ~PI, then we 
	*  have to normalize a very small vector and this can cause numerical instability. The quaternion-slerp 
	*  has exactly the same problems. 
	*                                                                                                  Ivo
	*  Example:
	*		Matrix33 slerp=Matrix33::CreateSlerp( m,n,0.333f );
	*/
	ILINE static Matrix34_tpl<F> CreateSlerp( const Matrix34_tpl<F>& m, const Matrix34_tpl<F>& n, F t) {	Matrix34_tpl<F> m33;	m33.SetSlerp(m,n,t); return m33;	}
	void SetSlerp( const Matrix34_tpl<F>& m, const Matrix34_tpl<F>& n, F t)
	{
		//calculate delta-rotation between m and n (=39 flops)
		Matrix33_tpl<real> d,i;
		d.m00=m.m00*n.m00+m.m10*n.m10+m.m20*n.m20;	d.m01=m.m00*n.m01+m.m10*n.m11+m.m20*n.m21;	d.m02=m.m00*n.m02+m.m10*n.m12+m.m20*n.m22;
		d.m10=m.m01*n.m00+m.m11*n.m10+m.m21*n.m20;	d.m11=m.m01*n.m01+m.m11*n.m11+m.m21*n.m21;	d.m12=m.m01*n.m02+m.m11*n.m12+m.m21*n.m22;
		d.m20=d.m01*d.m12-d.m02*d.m11;							d.m21=d.m02*d.m10-d.m00*d.m12;							d.m22=d.m00*d.m11-d.m01*d.m10;
		assert( d.IsOrthonormalRH(0.0001f) );

		//extract angle and axis
		f64 cosine = clamp_tpl((d.m00+d.m11+d.m22-1.0)*0.5,-1.0,+1.0);
		f64 angle = atan2(sqrt(1.0-cosine*cosine),cosine);
		Vec3r axis(d.m21-d.m12,d.m02-d.m20,d.m10-d.m01);
		f64 l = sqrt(axis|axis);	if (l>0.00001) axis/=l; else axis(1,0,0); 
		i.SetRotationAA(angle*t,axis); //angle interpolation and calculation of new delta-matrix (=26 flops) 

		//final concatenation (=39 flops)
		m00=F(m.m00*i.m00+m.m01*i.m10+m.m02*i.m20);	m01=F(m.m00*i.m01+m.m01*i.m11+m.m02*i.m21);	m02=F(m.m00*i.m02+m.m01*i.m12+m.m02*i.m22);
		m10=F(m.m10*i.m00+m.m11*i.m10+m.m12*i.m20);	m11=F(m.m10*i.m01+m.m11*i.m11+m.m12*i.m21);	m12=F(m.m10*i.m02+m.m11*i.m12+m.m12*i.m22);
		m20=m01*m12-m02*m11;												m21=m02*m10-m00*m12;												m22=m00*m11-m01*m10;
		assert( this->IsOrthonormalRH(0.0001f) );

		m03=m.m03*(1-t) + n.m03*t;
		m13=m.m13*(1-t) + n.m13*t;
		m23=m.m23*(1-t) + n.m23*t;
	}

	//--------------------------------------------------------------------------------
	//----                  helper functions to access matrix-members     ------------
	//--------------------------------------------------------------------------------

	F *GetData() { return &m00; }
	const F *GetData() const { return &m00; }

	ILINE F operator () (uint32 i, uint32 j) const {	assert ((i<3) && (j<4));	F* p_data=(F*)(&m00); 	return p_data[i*4+j];	}
	ILINE F& operator () (uint32 i, uint32 j)	{	assert ((i<3) && (j<4));	F* p_data=(F*)(&m00);		return p_data[i*4+j];	}

	ILINE void SetRow(int i, const Vec3_tpl<F> &v)	{	assert(i<3);	F* p=(F*)(&m00);	p[0+4*i]=v.x;	p[1+4*i]=v.y;	p[2+4*i]=v.z;		}
	ILINE Vec3_tpl<F> GetRow(int i) const	{	assert(i<3);	F* p=(F*)(&m00);	return Vec3(p[0+4*i],p[1+4*i],p[2+4*i]);	}
  ILINE Vec4_tpl<F> GetRow4(int i) const	{	assert(i<3);	F* p=(F*)(&m00);	return Vec4(p[0+4*i],p[1+4*i],p[2+4*i],p[3+4*i]);	}

	ILINE void SetColumn(int i, const Vec3_tpl<F> &v)	{	assert(i<4);	F* p=(F*)(&m00);	p[i+4*0]=v.x;	p[i+4*1]=v.y;	p[i+4*2]=v.z;		}
	ILINE Vec3_tpl<F> GetColumn(int i) const	{	assert(i<4);	F* p=(F*)(&m00);	return Vec3(p[i+4*0],p[i+4*1],p[i+4*2]);	}
	ILINE Vec3_tpl<F> GetColumn0() const	{ return Vec3_tpl<F> (m00,m10,m20);	}
	ILINE Vec3_tpl<F> GetColumn1() const	{ return Vec3_tpl<F> (m01,m11,m21);	}
	ILINE Vec3_tpl<F> GetColumn2() const	{ return Vec3_tpl<F> (m02,m12,m22);	}
	ILINE Vec3_tpl<F> GetColumn3() const	{ return Vec3_tpl<F> (m03,m13,m23);	}


	ILINE void SetTranslation( const Vec3_tpl<F>& t ) { m03=t.x;	m13=t.y; m23=t.z;	}
	ILINE Vec3_tpl<F> GetTranslation() const { return Vec3_tpl<F>(m03,m13,m23); }
	ILINE void ScaleTranslation (F s)			            {	m03*=s;		m13*=s;		m23*=s;		}
	ILINE Matrix34_tpl<F> AddTranslation( const Vec3_tpl<F>& t )	{	m03+=t.x;	m13+=t.y;	m23+=t.z;	return *this; }

	ILINE void SetRotation33( const Matrix33_tpl<F>& m33 ) { 
		m00=m33.m00;	m01=m33.m01; m02=m33.m02;	
		m10=m33.m10;	m11=m33.m11; m12=m33.m12;	
		m20=m33.m20;	m21=m33.m21; m22=m33.m22;	
	}

	//check if we have an orthonormal-base (general case, works even with reflection matrices)
	int IsOrthonormal(F threshold=0.001) const { 
		f32 d0=fabs_tpl(GetColumn0()|GetColumn1());	if 	(d0>threshold) return 0;	
		f32 d1=fabs_tpl(GetColumn0()|GetColumn2());	if 	(d1>threshold) return 0;	
		f32 d2=fabs_tpl(GetColumn1()|GetColumn2());	if 	(d2>threshold) return 0;	
		int a=(fabs_tpl(1-(GetColumn0()|GetColumn0())))<threshold;
		int b=(fabs_tpl(1-(GetColumn1()|GetColumn1())))<threshold;
		int c=(fabs_tpl(1-(GetColumn2()|GetColumn2())))<threshold;
		return a&b&c;
	}

	//check if we have an orthonormal-base (assuming we are using a right-handed coordinate system)
	int IsOrthonormalRH(F threshold=0.001) const { 
		int a=GetColumn0().IsEquivalent(GetColumn1()%GetColumn2(),threshold);
		int b=GetColumn1().IsEquivalent(GetColumn2()%GetColumn0(),threshold);
		int c=GetColumn2().IsEquivalent(GetColumn0()%GetColumn1(),threshold);
		return a&b&c;
	}
	bool IsEquivalent( const Matrix34_tpl<F>& m, F e=VEC_EPSILON) const {
		return  ( 
			(fabs_tpl(m00-m.m00)<=e) && (fabs_tpl(m01-m.m01)<=e) && (fabs_tpl(m02-m.m02)<=e) && (fabs_tpl(m03-m.m03)<=e) && 
			(fabs_tpl(m10-m.m10)<=e) && (fabs_tpl(m11-m.m11)<=e) && (fabs_tpl(m12-m.m12)<=e) && (fabs_tpl(m13-m.m13)<=e) && 
			(fabs_tpl(m20-m.m20)<=e) && (fabs_tpl(m21-m.m21)<=e) && (fabs_tpl(m22-m.m22)<=e) && (fabs_tpl(m23-m.m23)<=e) 
			);	
	}

	bool IsValid() const
	{
		if (!NumberValid(m00)) return false;	if (!NumberValid(m01)) return false;	if (!NumberValid(m02)) return false;	if (!NumberValid(m03)) return false;
		if (!NumberValid(m10)) return false;	if (!NumberValid(m11)) return false;	if (!NumberValid(m12)) return false;	if (!NumberValid(m13)) return false;
		if (!NumberValid(m20)) return false;	if (!NumberValid(m21)) return false;	if (!NumberValid(m22)) return false;	if (!NumberValid(m23)) return false;
		return true;
	}


};

///////////////////////////////////////////////////////////////////////////////
// Typedefs                                                                  //
///////////////////////////////////////////////////////////////////////////////

typedef Matrix34_tpl<f32, XMVec4> Matrix34;
typedef Matrix34_tpl<f32, XMVec4A> Matrix34A;
typedef Matrix34_tpl<f64, XMVec4> Matrix34_f64;
typedef Matrix34_tpl<real, XMVec4> Matrix34r;


//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
//-------------       implementation of Matrix34      ------------------------------
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------

/*!
*  multiplication of Matrix34 by a (column) Vec3.
*  This function transforms the given input Vec3
*  into the coordinate system defined by this matrix.
*
*  Example:
*   Matrix34 m34;
*   Vec3 vector(44,55,66);
*   Vec3 result = m34*vector;
*/
template<class F, class B> 
ILINE Vec3_tpl<F> operator * (const Matrix34_tpl<F, B>& m, const Vec3_tpl<F> &p) {
	assert(m.IsValid());
	assert(p.IsValid());
	Vec3_tpl<F> tp;
	tp.x	=	m.m00*p.x + m.m01*p.y + m.m02*p.z + m.m03;
	tp.y	=	m.m10*p.x + m.m11*p.y + m.m12*p.z + m.m13;
	tp.z	=	m.m20*p.x + m.m21*p.y + m.m22*p.z + m.m23;
	return	tp;
}


template<class F1, class F2, class B> 
ILINE Matrix34_tpl<F1> operator*(const Matrix34_tpl<F1, B> &l, const Diag33_tpl<F2> &r) {
	assert(l.IsValid());
	assert(r.IsValid());
	Matrix34_tpl<F1> m;
	m.m00=l.m00*r.x;	m.m01=l.m01*r.y;	m.m02=l.m02*r.z;	m.m03=l.m03;
	m.m10=l.m10*r.x;	m.m11=l.m11*r.y;	m.m12=l.m12*r.z;	m.m13=l.m13;
	m.m20=l.m20*r.x;	m.m21=l.m21*r.y;	m.m22=l.m22*r.z;	m.m23=l.m23;
	return m;
}
template<class F1, class F2, class B> 
ILINE Matrix34_tpl<F1>& operator *= (Matrix34_tpl<F1, B>& l, const Diag33_tpl<F2>& r) {
	assert(l.IsValid());
	assert(r.IsValid());
	l.m00*=r.x;	l.m01*=r.y;	l.m02*=r.z; 
	l.m10*=r.x;	l.m11*=r.y;	l.m12*=r.z;
	l.m20*=r.x;	l.m21*=r.y;	l.m22*=r.z;
	return l;
}

/*!
*  Implements the multiplication operator: Matrix34=Matrix34*Matrix33
*
*  Matrix33 and Matrix44 are specified in collumn order for a right-handed coordinate-system.        
*  AxB = operation B followed by operation A.  
*  A multiplication takes 27 muls and 24 adds. 
*
*  Example:
*   Matrix34 m34=Matrix33::CreateRotationX(1.94192f);;
*   Matrix33 m33=Matrix34::CreateRotationZ(3.14192f);
*	  Matrix34 result=m34*m33;
*/
template<class F, class B> 
Matrix34_tpl<F> operator * (const Matrix34_tpl<F, B>& l, const Matrix33_tpl<F>& r) {
	assert(l.IsValid());
	assert(r.IsValid());
	Matrix34_tpl<F> m;
	m.m00 = l.m00*r.m00 + l.m01*r.m10 + l.m02*r.m20;
	m.m10 = l.m10*r.m00 + l.m11*r.m10 + l.m12*r.m20;
	m.m20 = l.m20*r.m00 + l.m21*r.m10 + l.m22*r.m20;
	m.m01 = l.m00*r.m01 + l.m01*r.m11 + l.m02*r.m21;
	m.m11 = l.m10*r.m01 + l.m11*r.m11 + l.m12*r.m21;
	m.m21 = l.m20*r.m01 + l.m21*r.m11 + l.m22*r.m21;
	m.m02 = l.m00*r.m02 + l.m01*r.m12 + l.m02*r.m22;
	m.m12 = l.m10*r.m02 + l.m11*r.m12 + l.m12*r.m22;
	m.m22 = l.m20*r.m02 + l.m21*r.m12 + l.m22*r.m22;
	m.m03 = l.m03;
	m.m13 = l.m13;
	m.m23 = l.m23;
	return m;
}



/*!
*  Implements the multiplication operator: Matrix34=Matrix34*Matrix34
*
*  Matrix34 is specified in collumn order.        
*  AxB = rotation B followed by rotation A.  
*  This operation takes 36 mults and 27 adds. 
*
*  Example:
*   Matrix34 m34=Matrix34::CreateRotationX(1.94192f, Vec3(11,22,33));;
*   Matrix34 m34=Matrix33::CreateRotationZ(3.14192f);
*	  Matrix34 result=m34*m34;
*/
template<class F, class B, class C> 
Matrix34_tpl<F> operator * (const Matrix34_tpl<F, B>& l, const Matrix34_tpl<F, C>& r) {
	assert(l.IsValid());
	assert(r.IsValid());
	Matrix34_tpl<F> m;
	m.m00 = l.m00*r.m00 + l.m01*r.m10 + l.m02*r.m20;
	m.m10 = l.m10*r.m00 + l.m11*r.m10 + l.m12*r.m20;
	m.m20 = l.m20*r.m00 + l.m21*r.m10 + l.m22*r.m20;
	m.m01 = l.m00*r.m01 + l.m01*r.m11 + l.m02*r.m21;
	m.m11 = l.m10*r.m01 + l.m11*r.m11 + l.m12*r.m21;
	m.m21 = l.m20*r.m01 + l.m21*r.m11 + l.m22*r.m21;
	m.m02 = l.m00*r.m02 + l.m01*r.m12 + l.m02*r.m22;
	m.m12 = l.m10*r.m02 + l.m11*r.m12 + l.m12*r.m22;
	m.m22 = l.m20*r.m02 + l.m21*r.m12 + l.m22*r.m22;
	m.m03 = l.m00*r.m03 + l.m01*r.m13 + l.m02*r.m23 + l.m03;
	m.m13 = l.m10*r.m03 + l.m11*r.m13 + l.m12*r.m23 + l.m13;
	m.m23 = l.m20*r.m03 + l.m21*r.m13 + l.m22*r.m23 + l.m23;
	return m;
}


/*!
*  Implements the multiplication operator: Matrix44=Matrix34*Matrix44
*
*  Matrix44 and Matrix34 are specified in collumn order.         
*	 AxB = rotation B followed by rotation A.  
*  This operation takes 48 mults and 36 adds.  
*
*  Example:
*   Matrix34 m34=Matrix33::CreateRotationX(1.94192f);;
*   Matrix44 m44=Matrix33::CreateRotationZ(3.14192f);
*	  Matrix44 result=m34*m44;
*/
template<class F, class B, class C> 
Matrix44_tpl<F, XMVec4A> operator * (const Matrix34_tpl<F, B>& l, const Matrix44_tpl<F, C>& r) {
	assert(l.IsValid());
	assert(r.IsValid());
	Matrix44_tpl<F, XMVec4A> m;
	m.m00 = l.m00*r.m00 + l.m01*r.m10 + l.m02*r.m20 + l.m03*r.m30;
	m.m10 = l.m10*r.m00 + l.m11*r.m10 + l.m12*r.m20 + l.m13*r.m30;
	m.m20 = l.m20*r.m00 + l.m21*r.m10 + l.m22*r.m20 + l.m23*r.m30;
	m.m30 = r.m30;
	m.m01 = l.m00*r.m01 + l.m01*r.m11 + l.m02*r.m21 + l.m03*r.m31;
	m.m11 = l.m10*r.m01 + l.m11*r.m11 + l.m12*r.m21 + l.m13*r.m31;
	m.m21 = l.m20*r.m01 + l.m21*r.m11 + l.m22*r.m21 + l.m23*r.m31;
	m.m31 = r.m31;
	m.m02 = l.m00*r.m02 + l.m01*r.m12 + l.m02*r.m22 + l.m03*r.m32;
	m.m12 = l.m10*r.m02 + l.m11*r.m12 + l.m12*r.m22 + l.m13*r.m32;
	m.m22 = l.m20*r.m02 + l.m21*r.m12 + l.m22*r.m22 + l.m23*r.m32;
	m.m32 = r.m32;
	m.m03 = l.m00*r.m03 + l.m01*r.m13 + l.m02*r.m23 + l.m03*r.m33;
	m.m13 = l.m10*r.m03 + l.m11*r.m13 + l.m12*r.m23 + l.m13*r.m33;
	m.m23 = l.m20*r.m03 + l.m21*r.m13 + l.m22*r.m23 + l.m23*r.m33;
	m.m33 = r.m33;
	return m;
}


 
/*!
* Create a matrix with SCALING, ROTATION and TRANSLATION (in this order).
*
*  Example 1:
*		Matrix m34;
*		m34.SetMatrix( Vec3(1,2,3), quat, Vec3(11,22,33)  );
*/
template<class F, class A> 
void Matrix34_tpl<F, A>::Set( const Vec3_tpl<F>& s, const Quat_tpl<F>& q, const Vec3_tpl<F>& t ) {
	assert(s.IsValid());
	assert(q.IsUnit(0.1f));
	assert(t.IsValid());
	F vxvx=q.v.x*q.v.x;		F vzvz=q.v.z*q.v.z;		F vyvy=q.v.y*q.v.y; 
	F vxvy=q.v.x*q.v.y;		F vxvz=q.v.x*q.v.z;		F vyvz=q.v.y*q.v.z; 
	F svx=q.w*q.v.x;			F svy=q.w*q.v.y;			F svz=q.w*q.v.z;
	m00=(1-(vyvy+vzvz)*2)*s.x;	m01=(vxvy-svz)*2*s.y;				m02=(vxvz+svy)*2*s.z;				m03=t.x;
	m10=(vxvy+svz)*2*s.x;				m11=(1-(vxvx+vzvz)*2)*s.y;	m12=(vyvz-svx)*2*s.z;				m13=t.y;
	m20=(vxvz-svy)*2*s.x;				m21=(vyvz+svx)*2*s.y;				m22=(1-(vxvx+vyvy)*2)*s.z;	m23=t.z;
}
template<class F, class A> 
ILINE Matrix34_tpl<F> Matrix34_tpl<F, A>::Create(  const Vec3_tpl<F>& s, const Quat_tpl<F>& q, const Vec3_tpl<F>& t  ) { Matrix34_tpl<F, A> m34;	m34.Set(s,q,t); return m34;	}


/*!
* Create scaling-matrix.
* The translation-vector is set to zero.  
*
*  Example 1:
*		Matrix m34;
*		m34.SetScale( Vec3(0.5f, 1.0f, 2.0f) );
*
*  Example 2:
*		Matrix34 m34 = Matrix34::CreateScale( Vec3(0.5f, 1.0f, 2.0f) );
*/
template<class F, class B> 
ILINE void Matrix34_tpl<F, B>::SetScale( const Vec3_tpl<F> &s, const Vec3_tpl<F>& t ) {
	*this=Matrix33::CreateScale(s); this->SetTranslation(t);
}
template<class F, class A> 
ILINE Matrix34_tpl<F> Matrix34_tpl<F, A>::CreateScale(  const Vec3_tpl<F> &s, const Vec3_tpl<F>& t  )	{ 	Matrix34_tpl<F, A> m34;  m34.SetScale(s,t);	return m34;	}


/*!
* calculate a real inversion of a Matrix34
* an inverse-matrix is an UnDo-matrix for all kind of transformations 
* 
*  Example 1:
*		Matrix34 im34;
*		im34.Invert();
*
*  Example 2:
*   Matrix34 im34=m34.GetInverted();
*/
template<class F, class B> 
void Matrix34_tpl<F, B>::Invert( void ) {
	//rescue members	
	Matrix34_tpl<F>	m=*this;
	// calculate 12 cofactors
	m00= m.m22*m.m11-m.m12*m.m21;
	m10= m.m12*m.m20-m.m22*m.m10;
	m20= m.m10*m.m21-m.m20*m.m11;
	m01= m.m02*m.m21-m.m22*m.m01;
	m11= m.m22*m.m00-m.m02*m.m20;
	m21= m.m20*m.m01-m.m00*m.m21;
	m02= m.m12*m.m01-m.m02*m.m11;
	m12= m.m02*m.m10-m.m12*m.m00;
	m22= m.m00*m.m11-m.m10*m.m01;
	m03= (m.m22*m.m13*m.m01 + m.m02*m.m23*m.m11 + m.m12*m.m03*m.m21) - (m.m12*m.m23*m.m01 + m.m22*m.m03*m.m11 + m.m02*m.m13*m.m21);
	m13= (m.m12*m.m23*m.m00 + m.m22*m.m03*m.m10 + m.m02*m.m13*m.m20) - (m.m22*m.m13*m.m00 + m.m02*m.m23*m.m10 + m.m12*m.m03*m.m20);
	m23= (m.m20*m.m11*m.m03 + m.m00*m.m21*m.m13 + m.m10*m.m01*m.m23) - (m.m10*m.m21*m.m03 + m.m20*m.m01*m.m13 + m.m00*m.m11*m.m23);
	// calculate determinant
	F det=1.0f/(m.m00*m00+m.m10*m01+m.m20*m02);
	assert( fabs_tpl(det)>(F)0.00000001 );
	// calculate matrix inverse/
	m00*=det; m01*=det; m02*=det; m03*=det;
	m10*=det; m11*=det; m12*=det; m13*=det;
	m20*=det; m21*=det; m22*=det; m23*=det;
}
template<class F, class A> 
ILINE Matrix34_tpl<F> Matrix34_tpl<F, A>::GetInverted() const {	Matrix34_tpl<F> dst=*this; dst.Invert(); return dst; }

/*!
*  Name:             ReflectMat34
*  Description:      reflect a rotation matrix with respect to a plane.
*
*  Example:
*		Vec3 normal( 0.0f,-1.0f, 0.0f);
*		Vec3 pos(0,1000,0);
*		Matrix34 m34=CreateReflectionMat( pos, normal );
*/
static Matrix34 CreateReflectionMat ( const Vec3& p, const Vec3& n )
{
	Matrix34 m;	
	f32 vxy   = -2.0f * n.x * n.y;
	f32 vxz   = -2.0f * n.x * n.z;
	f32 vyz   = -2.0f * n.y * n.z;
	f32 pdotn = 2.0f * (p|n);
	m.m00=1.0f-2.0f*n.x*n.x;	m.m01=vxy;    						m.m02=vxz;    						m.m03=pdotn*n.x;
	m.m10=vxy;  							m.m11=1.0f-2.0f*n.y*n.y; 	m.m12=vyz;    						m.m13=pdotn*n.y;
	m.m20=vxz;  							m.m21=vyz;   							m.m22=1.0f-2.0f*n.z*n.z; 	m.m23=pdotn*n.z;
	return m;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// struct Matrix44_tpl
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
template<typename F, class A=XMVec4A> struct Matrix44_tpl
{
#ifndef XENON_INTRINSICS
  F m00,m01,m02,m03;
  F m10,m11,m12,m13;
  F m20,m21,m22,m23;
  F m30,m31,m32,m33;















#endif

	//---------------------------------------------------------------------------------

#if defined(_DEBUG) && !defined(__SPU__)
	ILINE Matrix44_tpl() 
	{
		if (sizeof(F)==4)
		{
			uint32* p=(uint32*)&m00;
			p[ 0]=F32NAN;	p[ 1]=F32NAN;	p[ 2]=F32NAN;	p[ 3]=F32NAN;
			p[ 4]=F32NAN;	p[ 5]=F32NAN;	p[ 6]=F32NAN;	p[ 7]=F32NAN;
			p[ 8]=F32NAN;	p[ 9]=F32NAN;	p[10]=F32NAN;	p[11]=F32NAN;
			p[12]=F32NAN;	p[13]=F32NAN;	p[14]=F32NAN;	p[15]=F32NAN;
		}
		if (sizeof(F)==8)
		{
			uint64* p=(uint64*)&m00;
			p[ 0]=F64NAN;	p[ 1]=F64NAN;	p[ 2]=F64NAN;	p[ 3]=F64NAN;
			p[ 4]=F64NAN;	p[ 5]=F64NAN;	p[ 6]=F64NAN;	p[ 7]=F64NAN;
			p[ 8]=F64NAN;	p[ 9]=F64NAN;	p[10]=F64NAN;	p[11]=F64NAN;
			p[12]=F64NAN;	p[13]=F64NAN;	p[14]=F64NAN;	p[15]=F64NAN;
		}
	}
#else
	ILINE Matrix44_tpl(){};
#endif

	template<class F1> ILINE Matrix44_tpl(const Matrix33_tpl<F1>& m ) {
		assert(m.IsValid());
		m00=(F)m.m00;		m01=(F)m.m01;		m02=(F)m.m02;		m03=0;	
		m10=(F)m.m10;		m11=(F)m.m11;		m12=(F)m.m12;		m13=0;
		m20=(F)m.m20;		m21=(F)m.m21;		m22=(F)m.m22;		m23=0;
		m30=0;			m31=0;			m32=0;			m33=1;
	}
	template<class F1, class B> explicit ILINE Matrix44_tpl(const Matrix34_tpl<F1, B>& m ) {
		assert(m.IsValid());
#ifndef XENON_INTRINSICS
		m00=(F)m.m00;		m01=(F)m.m01;		m02=(F)m.m02;		m03=(F)m.m03;	
		m10=(F)m.m10;		m11=(F)m.m11;		m12=(F)m.m12;		m13=(F)m.m13;
		m20=(F)m.m20;		m21=(F)m.m21;		m22=(F)m.m22;		m23=(F)m.m23;
		m30=0;				m31=0;				m32=0;				m33=1;








#endif
	}
	template<class F1, class B> ILINE Matrix44_tpl(const Matrix44_tpl<F1, B>& m ) {
		assert(m.IsValid());
#ifndef XENON_INTRINSICS
		m00=(F)m.m00;		m01=(F)m.m01;		m02=(F)m.m02;	m03=(F)m.m03;	
		m10=(F)m.m10;		m11=(F)m.m11;		m12=(F)m.m12;	m13=(F)m.m13;
		m20=(F)m.m20;		m21=(F)m.m21;		m22=(F)m.m22;	m23=(F)m.m23;
		m30=(F)m.m30;   m31=(F)m.m31;	  m32=(F)m.m32;	m33=(F)m.m33;









#endif
	}

	//---------------------------------------------------------------------------------

	//! build a matrix from 16 f32s
	Matrix44_tpl(	f32 v00, f32 v01, f32 v02, f32 v03,
								f32 v10, f32 v11, f32 v12, f32 v13,
								f32 v20, f32 v21, f32 v22, f32 v23,
								f32 v30, f32 v31, f32 v32, f32 v33) :
		m00(v00), m01(v01), m02(v02), m03(v03), 
		m10(v10), m11(v11), m12(v12), m13(v13), 
		m20(v20), m21(v21), m22(v22), m23(v23), 
		m30(v30), m31(v31), m32(v32), m33(v33) 
	{ 	
	}

	//---------------------------------------------------------------------

	//! multiply all m1 matrix's values by f and return the matrix
	friend	Matrix44_tpl<F> operator * (const Matrix44_tpl<F, A>& m, const f32 f)	{ 
		assert(m.IsValid());
    Matrix44_tpl<F> r;
#ifndef XENON_INTRINSICS
		r.m00=m.m00*f;	r.m01=m.m01*f;	r.m02=m.m02*f;	r.m03=m.m03*f; 
		r.m10=m.m10*f;	r.m11=m.m11*f;	r.m12=m.m12*f;	r.m13=m.m13*f;
		r.m20=m.m20*f;	r.m21=m.m21*f;	r.m22=m.m22*f;	r.m23=m.m23*f;
		r.m30=m.m30*f;	r.m31=m.m31*f;	r.m32=m.m32*f;	r.m33=m.m33*f;










#endif
		return r;
	}	

  template<class B> ILINE Matrix44_tpl& operator = (const Matrix44_tpl<F, B> &m) { 
    assert(m.IsValid());
#ifndef XENON_INTRINSICS
    m00=m.m00;	m01=m.m01;	m02=m.m02; m03=m.m03; 
    m10=m.m10;	m11=m.m11;	m12=m.m12; m13=m.m13; 
    m20=m.m20;	m21=m.m21;	m22=m.m22; m23=m.m23; 
    m30=m.m30;	m31=m.m31;	m32=m.m32; m33=m.m33; 









#endif
    return *this;
  }

  template<class F1, class B> ILINE Matrix44_tpl& operator = (const Matrix34_tpl<F1, B> &m) { 
    assert(m.IsValid());
#ifndef XENON_INTRINSICS
    m00=m.m00;	m01=m.m01;	m02=m.m02; m03=m.m03; 
    m10=m.m10;	m11=m.m11;	m12=m.m12; m13=m.m13; 
    m20=m.m20;	m21=m.m21;	m22=m.m22; m23=m.m23; 
    m30=0;	m31=0;	m32=0; m33=1; 









#endif
    return *this;
  }

  //! add all m matrix's values and return the matrix
  template<class B> friend	Matrix44_tpl<F> operator + (const Matrix44_tpl<F, A>& mm0, const Matrix44_tpl<F, B>& mm1)	{ 
		assert(mm0.IsValid());
		assert(mm1.IsValid());
    Matrix44_tpl<F> r;
    r.m00=mm0.m00 + mm1.m00;	r.m01=mm0.m01 + mm1.m01;	r.m02=mm0.m02 + mm1.m02;	r.m03=mm0.m03 + mm1.m03; 
    r.m10=mm0.m10 + mm1.m10;	r.m11=mm0.m11 + mm1.m11;	r.m12=mm0.m12 + mm1.m12;	r.m13=mm0.m13 + mm1.m13;
    r.m20=mm0.m20 + mm1.m20;	r.m21=mm0.m21 + mm1.m21;	r.m22=mm0.m22 + mm1.m22;	r.m23=mm0.m23 + mm1.m23;
    r.m30=mm0.m30 + mm1.m30;	r.m31=mm0.m31 + mm1.m31;	r.m32=mm0.m32 + mm1.m32;	r.m33=mm0.m33 + mm1.m33;
    return r;
  }	

	ILINE void SetIdentity()	{
		m00=1;		m01=0;		m02=0;	m03=0;
		m10=0;		m11=1;		m12=0;	m13=0;
		m20=0;		m21=0;		m22=1;	m23=0;
		m30=0;		m31=0;		m32=0;	m33=1;
	}
	ILINE Matrix44_tpl(type_identity) { SetIdentity(); }


	_inline void Transpose()
  {
    Transpose(*this);
	}
  template <class B> _inline void Transpose(const Matrix44_tpl<float, B>& M)
  {
    Matrix44_tpl<F> m;
    m.m00=M.m00;		m.m01=M.m10;		m.m02=M.m20; 	m.m03=M.m30;
    m.m10=M.m01;		m.m11=M.m11;		m.m12=M.m21; 	m.m13=M.m31;
    m.m20=M.m02;		m.m21=M.m12;		m.m22=M.m22; 	m.m23=M.m32;
    m.m30=M.m03;		m.m31=M.m13;		m.m32=M.m23; 	m.m33=M.m33;
    m00=m.m00;		m01=m.m01;		m02=m.m02;	m03=m.m03;
    m10=m.m10;		m11=m.m11;		m12=m.m12;	m13=m.m13;
    m20=m.m20;		m21=m.m21;		m22=m.m22;	m23=m.m23;
    m30=m.m30;		m31=m.m31;		m32=m.m32;	m33=m.m33;
  }
  template <class B> _inline void Transpose(const Matrix34_tpl<float, B>& M)
  {
    Matrix44_tpl<F> m;
    m.m00=M.m00;		m.m01=M.m10;		m.m02=M.m20;
    m.m10=M.m01;		m.m11=M.m11;		m.m12=M.m21;
    m.m20=M.m02;		m.m21=M.m12;		m.m22=M.m22;
    m.m30=M.m03;		m.m31=M.m13;		m.m32=M.m23;
    m00=m.m00;		m01=m.m01;		m02=m.m02;	m03=0;
    m10=m.m10;		m11=m.m11;		m12=m.m12;	m13=0;
    m20=m.m20;		m21=m.m21;		m22=m.m22;	m23=0;
    m30=m.m30;		m31=m.m31;		m32=m.m32;	m33=1;
  }
	friend Matrix44_tpl<F, XMVec4A> GetTransposed44( const Matrix44_tpl<F, A>& m ) {
    Matrix44_tpl<F, XMVec4A> dst;
		dst.m00=m.m00;	dst.m01=m.m10;	dst.m02=m.m20;	dst.m03=m.m30;
		dst.m10=m.m01;	dst.m11=m.m11;	dst.m12=m.m21;	dst.m13=m.m31;
		dst.m20=m.m02;	dst.m21=m.m12;	dst.m22=m.m22;	dst.m23=m.m32;
		dst.m30=m.m03;	dst.m31=m.m13;	dst.m32=m.m23;	dst.m33=m.m33;
		return dst;
	}

	/*!
	*
	* calculate a real inversion of a Matrix44.
	* an inverse-matrix is an UnDo-matrix for all kind of transformations 
	* 
	*  Example 1:
	*		Matrix44 im44;
	*		im44.Invert();
	*
	*  Example 2:
	*   Matrix44 im44=m33.GetInverted();
	*/
	void Invert( void ) {
		F	tmp[12];
		Matrix44_tpl<F>	m=*this;

		/* calculate pairs for first 8 elements (cofactors) */
		tmp[0] = m.m22 * m.m33;
		tmp[1] = m.m32 * m.m23;
		tmp[2] = m.m12 * m.m33;
		tmp[3] = m.m32 * m.m13;
		tmp[4] = m.m12 * m.m23;
		tmp[5] = m.m22 * m.m13;
		tmp[6] = m.m02 * m.m33;
		tmp[7] = m.m32 * m.m03;
		tmp[8] = m.m02 * m.m23;
		tmp[9] = m.m22 * m.m03;
		tmp[10]= m.m02 * m.m13;
		tmp[11]= m.m12 * m.m03;

		/* calculate first 8 elements (cofactors) */
		m00 = tmp[0]*m.m11 + tmp[3]*m.m21 + tmp[ 4]*m.m31;
		m00-= tmp[1]*m.m11 + tmp[2]*m.m21 + tmp[ 5]*m.m31;
		m01 = tmp[1]*m.m01 + tmp[6]*m.m21 + tmp[ 9]*m.m31;
		m01-= tmp[0]*m.m01 + tmp[7]*m.m21 + tmp[ 8]*m.m31;
		m02 = tmp[2]*m.m01 + tmp[7]*m.m11 + tmp[10]*m.m31;
		m02-= tmp[3]*m.m01 + tmp[6]*m.m11 + tmp[11]*m.m31;
		m03 = tmp[5]*m.m01 + tmp[8]*m.m11 + tmp[11]*m.m21;
		m03-= tmp[4]*m.m01 + tmp[9]*m.m11 + tmp[10]*m.m21;
		m10 = tmp[1]*m.m10 + tmp[2]*m.m20 + tmp[ 5]*m.m30;
		m10-= tmp[0]*m.m10 + tmp[3]*m.m20 + tmp[ 4]*m.m30;
		m11 = tmp[0]*m.m00 + tmp[7]*m.m20 + tmp[ 8]*m.m30;
		m11-= tmp[1]*m.m00 + tmp[6]*m.m20 + tmp[ 9]*m.m30;
		m12 = tmp[3]*m.m00 + tmp[6]*m.m10 + tmp[11]*m.m30;
		m12-= tmp[2]*m.m00 + tmp[7]*m.m10 + tmp[10]*m.m30;
		m13 = tmp[4]*m.m00 + tmp[9]*m.m10 + tmp[10]*m.m20;
		m13-= tmp[5]*m.m00 + tmp[8]*m.m10 + tmp[11]*m.m20;

		/* calculate pairs for second 8 elements (cofactors) */
		tmp[ 0] = m.m20*m.m31;
		tmp[ 1] = m.m30*m.m21;
		tmp[ 2] = m.m10*m.m31;
		tmp[ 3] = m.m30*m.m11;
		tmp[ 4] = m.m10*m.m21;
		tmp[ 5] = m.m20*m.m11;
		tmp[ 6] = m.m00*m.m31;
		tmp[ 7] = m.m30*m.m01;
		tmp[ 8] = m.m00*m.m21;
		tmp[ 9] = m.m20*m.m01;
		tmp[10] = m.m00*m.m11;
		tmp[11] = m.m10*m.m01;

		/* calculate second 8 elements (cofactors) */
		m20 = tmp[ 0]*m.m13 + tmp[ 3]*m.m23 + tmp[ 4]*m.m33;
		m20-= tmp[ 1]*m.m13 + tmp[ 2]*m.m23 + tmp[ 5]*m.m33;
		m21 = tmp[ 1]*m.m03 + tmp[ 6]*m.m23 + tmp[ 9]*m.m33;
		m21-= tmp[ 0]*m.m03 + tmp[ 7]*m.m23 + tmp[ 8]*m.m33;
		m22 = tmp[ 2]*m.m03 + tmp[ 7]*m.m13 + tmp[10]*m.m33;
		m22-= tmp[ 3]*m.m03 + tmp[ 6]*m.m13 + tmp[11]*m.m33;
		m23 = tmp[ 5]*m.m03 + tmp[ 8]*m.m13 + tmp[11]*m.m23;
		m23-= tmp[ 4]*m.m03 + tmp[ 9]*m.m13 + tmp[10]*m.m23;
		m30 = tmp[ 2]*m.m22 + tmp[ 5]*m.m32 + tmp[ 1]*m.m12;
		m30-= tmp[ 4]*m.m32 + tmp[ 0]*m.m12 + tmp[ 3]*m.m22;
		m31 = tmp[ 8]*m.m32 + tmp[ 0]*m.m02 + tmp[ 7]*m.m22;
		m31-= tmp[ 6]*m.m22 + tmp[ 9]*m.m32 + tmp[ 1]*m.m02;
		m32 = tmp[ 6]*m.m12 + tmp[11]*m.m32 + tmp[ 3]*m.m02;
		m32-= tmp[10]*m.m32 + tmp[ 2]*m.m02 + tmp[ 7]*m.m12;
		m33 = tmp[10]*m.m22 + tmp[ 4]*m.m02 + tmp[ 9]*m.m12;
		m33-= tmp[ 8]*m.m12 + tmp[11]*m.m22 + tmp[ 5]*m.m02;

		/* calculate determinant */
		F det=(m.m00*m00+m.m10*m01+m.m20*m02+m.m30*m03);
		//if (fabs_tpl(det)<0.0001f) assert(0);	

		//devide the cofactor-matrix by the determinat
		F idet=(F)1.0/det;
		m00*=idet; m01*=idet; m02*=idet; m03*=idet;
		m10*=idet; m11*=idet; m12*=idet; m13*=idet;
		m20*=idet; m21*=idet; m22*=idet; m23*=idet;
		m30*=idet; m31*=idet; m32*=idet; m33*=idet;
	}
	ILINE Matrix44_tpl<F> GetInverted() const {	Matrix44_tpl<F> dst=*this; dst.Invert(); return dst; }

  template <class B> _inline void Invert(const Matrix44_tpl<float, B>& M)
  {
    *this = M.GetInverted();
  }

  template <class F1, class F2, class B, class C> _inline void Multiply(const Matrix44_tpl<F1, B>& l, const Matrix44_tpl<F2, C>& r)
  {
    Matrix44_tpl<F> res;
    res.m00 = l.m00*r.m00 + l.m01*r.m10 + l.m02*r.m20 + l.m03*r.m30;
    res.m10 = l.m10*r.m00 + l.m11*r.m10 + l.m12*r.m20 + l.m13*r.m30;
    res.m20 = l.m20*r.m00 + l.m21*r.m10 + l.m22*r.m20 + l.m23*r.m30;
    res.m30 = l.m30*r.m00 + l.m31*r.m10 + l.m32*r.m20 + l.m33*r.m30;
    res.m01 = l.m00*r.m01 + l.m01*r.m11 + l.m02*r.m21 + l.m03*r.m31;
    res.m11 = l.m10*r.m01 + l.m11*r.m11 + l.m12*r.m21 + l.m13*r.m31;
    res.m21 = l.m20*r.m01 + l.m21*r.m11 + l.m22*r.m21 + l.m23*r.m31;
    res.m31 = l.m30*r.m01 + l.m31*r.m11 + l.m32*r.m21 + l.m33*r.m31;
    res.m02 = l.m00*r.m02 + l.m01*r.m12 + l.m02*r.m22 + l.m03*r.m32;
    res.m12 = l.m10*r.m02 + l.m11*r.m12 + l.m12*r.m22 + l.m13*r.m32;
    res.m22 = l.m20*r.m02 + l.m21*r.m12 + l.m22*r.m22 + l.m23*r.m32;
    res.m32 = l.m30*r.m02 + l.m31*r.m12 + l.m32*r.m22 + l.m33*r.m32;
    res.m03 = l.m00*r.m03 + l.m01*r.m13 + l.m02*r.m23 + l.m03*r.m33;
    res.m13 = l.m10*r.m03 + l.m11*r.m13 + l.m12*r.m23 + l.m13*r.m33;
    res.m23 = l.m20*r.m03 + l.m21*r.m13 + l.m22*r.m23 + l.m23*r.m33;
    res.m33 = l.m30*r.m03 + l.m31*r.m13 + l.m32*r.m23 + l.m33*r.m33;
    *this = res;
  }



	ILINE f32 Determinant() const		{
		//determinant is ambiguous: only the upper-left-submatrix's determinant is calculated
		return (m00*m11*m22) + (m01*m12*m20) + (m02*m10*m21) - (m02*m11*m20) - (m00*m12*m21) - (m01*m10*m22);
	}

	//! transform a vector
	Vec3 TransformVector(const Vec3 &b) const	{
		assert(b.IsValid());
		Vec3 v;
		v.x = m00*b.x + m01*b.y + m02*b.z;
		v.y = m10*b.x + m11*b.y + m12*b.z;
		v.z = m20*b.x + m21*b.y + m22*b.z;
		return v;
	}
	//! transform a point
	Vec3 TransformPoint(const Vec3 &b) const {
		assert(b.IsValid());
		Vec3 v;
		v.x = m00*b.x + m01*b.y + m02* b.z + m03;
		v.y = m10*b.x + m11*b.y + m12* b.z + m13;
		v.z = m20*b.x + m21*b.y + m22* b.z + m23;
		return v;
	}


	//--------------------------------------------------------------------------------
	//----                  helper functions to access matrix-members     ------------
	//--------------------------------------------------------------------------------
	F *GetData() { return &m00; }
	const F *GetData() const { return &m00; }

	ILINE F operator ()  (uint32 i, uint32 j) const {	assert ((i<4) && (j<4));	F* p_data=(F*)(&m00); 	return p_data[i*4+j];	}
	ILINE F& operator () (uint32 i, uint32 j)	{	assert ((i<4) && (j<4));	F* p_data=(F*)(&m00);		return p_data[i*4+j];	}

	ILINE void SetRow(int i, const Vec3_tpl<F> &v)	{	assert(i<4);	F* p=(F*)(&m00);	p[0+4*i]=v.x;	p[1+4*i]=v.y;	p[2+4*i]=v.z;		}
	ILINE Vec3_tpl<F> GetRow(int i) const	{	assert(i<4);	F* p=(F*)(&m00);	return Vec3(p[0+4*i],p[1+4*i],p[2+4*i]);	}

	ILINE void SetColumn(int i, const Vec3_tpl<F> &v)	{	assert(i<4);	F* p=(F*)(&m00);	p[i+4*0]=v.x;	p[i+4*1]=v.y;	p[i+4*2]=v.z;		}
	ILINE Vec3_tpl<F> GetColumn(int i) const	{	assert(i<4);	F* p=(F*)(&m00);	return Vec3(p[i+4*0],p[i+4*1],p[i+4*2]);	}
  ILINE Vec4_tpl<F> GetColumn4(int i) const	{	assert(i<4);	F* p=(F*)(&m00);	return Vec4(p[i+4*0],p[i+4*1],p[i+4*2],p[i+4*3]);	}

	ILINE Vec3 GetTranslation() const {	return Vec3( m03, m13, m23 );	}
	ILINE void SetTranslation( const Vec3& t )	{	m03=t.x; m13=t.y; m23=t.z; }

	bool IsValid() const
	{
		if (!NumberValid(m00)) return false;	if (!NumberValid(m01)) return false;	if (!NumberValid(m02)) return false;	if (!NumberValid(m03)) return false;
		if (!NumberValid(m10)) return false;	if (!NumberValid(m11)) return false;	if (!NumberValid(m12)) return false;	if (!NumberValid(m13)) return false;
		if (!NumberValid(m20)) return false;	if (!NumberValid(m21)) return false;	if (!NumberValid(m22)) return false;	if (!NumberValid(m23)) return false;
		if (!NumberValid(m30)) return false;	if (!NumberValid(m31)) return false;	if (!NumberValid(m32)) return false;	if (!NumberValid(m33)) return false;
		return true;
	}

  // new operators
  inline void* operator new   ( size_t s ) { return A::New(s); }
  inline void* operator new[] ( size_t s ) { return A::New(s); }
  inline void* operator new   ( size_t s, void *pMem ) { return pMem; };
  inline void* operator new[]( size_t s, void *pMem ) { return pMem; }

  // delete operators
  inline void operator delete   ( void* p ) { A::Del(p); }
  inline void operator delete[] ( void* p ) { A::Del(p); }
  inline void operator delete   ( void* p, void* p1 ) { }
  inline void operator delete[] ( void* p, void* p1 ) { }

};

template<class F1, class B>
_inline Matrix44_tpl<float, XMVec4A> GetTransposed44A( const Matrix34_tpl<F1, B>& m ) 
{
  Matrix44_tpl<float, XMVec4A> dst;
  dst.Transpose(m);

  return dst;
}

///////////////////////////////////////////////////////////////////////////////
// Typedefs                                                                  //
///////////////////////////////////////////////////////////////////////////////

typedef Matrix44_tpl<f32, XMVec4> Matrix44;
typedef Matrix44_tpl<f64, XMVec4> Matrix44_f64;
typedef Matrix44_tpl<real, XMVec4> Matrix44r;

typedef Matrix44_tpl<f32, XMVec4A> Matrix44A;



//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
//-------------       implementation of Matrix44      ------------------------------
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------

/*!
*  Implements the multiplication operator: Matrix44=Matrix44*Matrix33diag
*
*  Matrix44 and Matrix33diag are specified in collumn order.         
*  AxB = operation B followed by operation A.  
*  This operation takes 12 mults. 
*
*  Example:
*   Matrix33diag diag(1,2,3);
*   Matrix44 m44=CreateRotationZ33(3.14192f);
*	  Matrix44 result=m44*diag;
*/
template<class F1,  class F2, class B> 
Matrix44_tpl<F1> operator * (const Matrix44_tpl<F1>& l, const Diag33_tpl<F2>& r) {
	assert(l.IsValid());
	assert(r.IsValid());
	Matrix44_tpl<F1> m;
	m.m00=l.m00*r.x;	m.m01=l.m01*r.y;	m.m02=l.m02*r.z;	m.m03=l.m03;
	m.m10=l.m10*r.x;	m.m11=l.m11*r.y;	m.m12=l.m12*r.z;	m.m13=l.m13;
	m.m20=l.m20*r.x;	m.m21=l.m21*r.y;	m.m22=l.m22*r.z;	m.m23=l.m23;
	m.m30=l.m30*r.x;	m.m31=l.m31*r.y;	m.m32=l.m32*r.z;	m.m33=l.m33;
	return m;
}
template<class F1, class F2, class B> 
Matrix44_tpl<F1>& operator *= (Matrix44_tpl<F1, B>& l, const Diag33_tpl<F2>& r) {
	assert(l.IsValid());
	assert(r.IsValid());
	l.m00*=r.x;	l.m01*=r.y;	l.m02*=r.z; 
	l.m10*=r.x;	l.m11*=r.y;	l.m12*=r.z;
	l.m20*=r.x;	l.m21*=r.y;	l.m22*=r.z;
	l.m30*=r.x;	l.m31*=r.y;	l.m32*=r.z;
	return l;
}


/*!
*  Implements the multiplication operator: Matrix44=Matrix44*Matrix33
*
*  Matrix44 and Matrix33 are specified in collumn order.         
*  AxB = rotation B followed by rotation A.  
*  This operation takes 48 mults and 24 adds. 
*
*  Example:
*   Matrix33 m34=CreateRotationX33(1.94192f);;
*   Matrix44 m44=CreateRotationZ33(3.14192f);
*	  Matrix44 result=m44*m33;
*/
template<class F1,  class F2, class B> 
Matrix44_tpl<F1> operator * (const Matrix44_tpl<F1, B>& l, const Matrix33_tpl<F2>& r) {
	assert(l.IsValid());
	assert(r.IsValid());
	Matrix44_tpl<F1> m;
  m.m00 =l.m00*r.m00 + l.m01*r.m10 + l.m02*r.m20;
  m.m10 =l.m10*r.m00 + l.m11*r.m10 + l.m12*r.m20;
  m.m20 =l.m20*r.m00 + l.m21*r.m10 + l.m22*r.m20;
  m.m30 =l.m30*r.m00 + l.m31*r.m10 + l.m32*r.m20;
  m.m01 =l.m00*r.m01 + l.m01*r.m11 + l.m02*r.m21;
  m.m11 =l.m10*r.m01 + l.m11*r.m11 + l.m12*r.m21;
  m.m21 =l.m20*r.m01 + l.m21*r.m11 + l.m22*r.m21;
  m.m31 =l.m30*r.m01 + l.m31*r.m11 + l.m32*r.m21;
  m.m02 =l.m00*r.m02 + l.m01*r.m12 + l.m02*r.m22;
  m.m12 =l.m10*r.m02 + l.m11*r.m12 + l.m12*r.m22;
  m.m22 =l.m20*r.m02 + l.m21*r.m12 + l.m22*r.m22;
  m.m32 =l.m30*r.m02 + l.m31*r.m12 + l.m32*r.m22;
  m.m03 =l.m03;
  m.m13 =l.m13;
  m.m23 =l.m23;
  m.m33 =l.m33;
	return m;
}


/*!
*  Implements the multiplication operator: Matrix44=Matrix44*Matrix34
*
*  Matrix44 and Matrix34 are specified in collumn order.         
*  AxB = rotation B followed by rotation A.  
*  This operation takes 48 mults and 36 adds. 
*
*  Example:
*   Matrix34 m34=CreateRotationX33(1.94192f);;
*   Matrix44 m44=CreateRotationZ33(3.14192f);
*	  Matrix44 result=m44*m34;
*/
template<class F, class B, class C> 
Matrix44_tpl<F> operator * (const Matrix44_tpl<F, B>& l, const Matrix34_tpl<F, C>& r) {
	assert(l.IsValid());
	assert(r.IsValid());
	Matrix44_tpl<F> m;
	m.m00 = l.m00*r.m00 + l.m01*r.m10 + l.m02*r.m20;
	m.m10 = l.m10*r.m00 + l.m11*r.m10 + l.m12*r.m20;
	m.m20 = l.m20*r.m00 + l.m21*r.m10 + l.m22*r.m20;
	m.m30 = l.m30*r.m00 + l.m31*r.m10 + l.m32*r.m20;
	m.m01 = l.m00*r.m01 + l.m01*r.m11 + l.m02*r.m21;
	m.m11 = l.m10*r.m01 + l.m11*r.m11 + l.m12*r.m21;
	m.m21 = l.m20*r.m01 + l.m21*r.m11 + l.m22*r.m21;
	m.m31 = l.m30*r.m01 + l.m31*r.m11 + l.m32*r.m21;
	m.m02 = l.m00*r.m02 + l.m01*r.m12 + l.m02*r.m22;
	m.m12 = l.m10*r.m02 + l.m11*r.m12 + l.m12*r.m22;
	m.m22 = l.m20*r.m02 + l.m21*r.m12 + l.m22*r.m22;
	m.m32 = l.m30*r.m02 + l.m31*r.m12 + l.m32*r.m22;
	m.m03 = l.m00*r.m03 + l.m01*r.m13 + l.m02*r.m23 + l.m03;
	m.m13 = l.m10*r.m03 + l.m11*r.m13 + l.m12*r.m23 + l.m13;
	m.m23 = l.m20*r.m03 + l.m21*r.m13 + l.m22*r.m23 + l.m23;
	m.m33 = l.m30*r.m03 + l.m31*r.m13 + l.m32*r.m23 + l.m33;
	return m;
}	


/*!
*  Implements the multiplication operator: Matrix44=Matrix44*Matrix44
*
*  Matrix44 and Matrix34 are specified in collumn order.         
*	 AxB = rotation B followed by rotation A.  
*  This operation takes 48 mults and 36 adds.  
*
*  Example:
*   Matrix44 m44=CreateRotationX33(1.94192f);;
*   Matrix44 m44=CreateRotationZ33(3.14192f);
*	  Matrix44 result=m44*m44;
*/
template<class F1, class F2, class B, class C> 
ILINE Matrix44_tpl<F1> operator * (const Matrix44_tpl<F1, B>& l, const Matrix44_tpl<F2, C>& r)
{
	assert(l.IsValid());
	assert(r.IsValid());
	Matrix44_tpl<F1> m;
  m.Multiply(l, r);
	return m;
}

//post-multiply
template<class F1, class F2, class B>
ILINE Vec4_tpl<F1> operator*(const Matrix44_tpl<F2, B> &m, const Vec4_tpl<F1> &v) 
{
	//CHECK_SIMD_ALIGNMENT_P(&v);
	assert(m.IsValid());
	assert(v.IsValid());
	return Vec4_tpl<F1>(v.x*m.m00 + v.y*m.m01 + v.z*m.m02 + v.w*m.m03,
                      v.x*m.m10 + v.y*m.m11 + v.z*m.m12 + v.w*m.m13,
                      v.x*m.m20 + v.y*m.m21 + v.z*m.m22 + v.w*m.m23,
                      v.x*m.m30 + v.y*m.m31 + v.z*m.m32 + v.w*m.m33);
}

//pre-multiply
template<class F1, class F2, class B>
ILINE Vec4_tpl<F1> operator*(const Vec4_tpl<F1> &v, const Matrix44_tpl<F2, B> &m) 
{
	//CHECK_SIMD_ALIGNMENT_P(&v);
	assert(m.IsValid());
	assert(v.IsValid());
	return Vec4_tpl<F1>(v.x*m.m00 + v.y*m.m10 + v.z*m.m20 + v.w*m.m30,
                      v.x*m.m01 + v.y*m.m11 + v.z*m.m21 + v.w*m.m31,
                      v.x*m.m02 + v.y*m.m12 + v.z*m.m22 + v.w*m.m32,
                      v.x*m.m03 + v.y*m.m13 + v.z*m.m23 + v.w*m.m33);
}

#endif

