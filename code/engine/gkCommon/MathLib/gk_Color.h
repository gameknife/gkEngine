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
// Name:   	gk_Color.h
// Desc:	
// 	
// 
// Author:  yikaiming
// Date:	2013/6/1	
// 
//////////////////////////////////////////////////////////////////////////

#ifndef gk_Color_h__
#define gk_Color_h__

_inline float FClamp( float X, float Min, float Max ) {
	return X<Min ? Min : X<Max ? X : Max;
}

template <class T> struct Color_tpl;

typedef Color_tpl<uint8> ColorB; // [ 0,  255]
typedef Color_tpl<float> ColorF; // [0.0, 1.0]

//////////////////////////////////////////////////////////////////////////////////////////////
// RGBA Color structure.
//////////////////////////////////////////////////////////////////////////////////////////////
template <class T> struct Color_tpl
{
	T	r,g,b,a;

	inline Color_tpl() {};

	Color_tpl(T _x, T _y, T _z, T _w);
	Color_tpl(T _x, T _y, T _z);

	/*	inline Color_tpl(const Color_tpl<T> & v)	{
	r = v.r; g = v.g; b = v.b; a = v.a;
	}*/

	// works together with pack_abgr8888
	Color_tpl(const unsigned int abgr );
	Color_tpl(const f32 c );
	Color_tpl(const ColorF& c);
	Color_tpl(const ColorF& c, float fAlpha);
	Color_tpl(const Vec3& c, float fAlpha);

	void Clamp(float Min = 0, float Max = 1.0f)  
	{
		r = ::FClamp(r, Min, Max);
		g = ::FClamp(g, Min, Max);
		b = ::FClamp(b, Min, Max);
		a = ::FClamp(a, Min, Max);
	}
	void ScaleCol (float f)	
	{
		r *= f; g *= f; b *= f;
	}

	float Luminance() const	
	{
		return r*0.33f + g*0.59f + b*0.11f;
	}

	float Max() const
	{
		float max;

		max = r;
		if (g > max)
			max = g;
		if (b > max)
			max = b;
		return max;
	}

	float NormalizeCol (ColorF& out) const
	{
		float max = Max();

		if (max == 0)
			return 0;

		out = *this / max;

		return max;
	}

	Color_tpl(const Vec3 & vVec)
	{
		r = (T)vVec.x;
		g = (T)vVec.y;
		b = (T)vVec.z;
		a = (T)1.f;
	}

	inline Color_tpl& operator = (const Vec3 &v) { r=(T)v.x; g=(T)v.y; b=(T)v.z; a=(T)1.0f; return *this; }

	inline T &operator [] (int index)		  { assert(index>=0 && index<=3); return ((T*)this)[index]; }
	inline T operator [] (int index) const { assert(index>=0 && index<=3); return ((T*)this)[index]; }

	void	Set(float fR,float fG,float fB,float fA=1.0f)	{
		r=static_cast<T>(fR);g=static_cast<T>(fG);b=static_cast<T>(fB);a=static_cast<T>(fA);
	}

	inline void set(T _x, T _y = 0, T _z = 0, T _w = 0)
	{
		r = _x; g = _y; b = _z; a = _w;
	}
	inline void set(T _x, T _y = 0, T _z = 0)
	{
		r = _x; g = _y; b = _z; a = 1;
	}
	inline Color_tpl operator + () const
	{
		return *this;
	}
	inline Color_tpl operator - () const
	{
		return Color_tpl<T>(-r, -g, -b, -a);
	}

	inline Color_tpl & operator += (const Color_tpl & v)
	{
		r += v.r; g += v.g; b += v.b; a += v.a;
		return *this;
	}
	inline Color_tpl & operator -= (const Color_tpl & v)
	{
		r -= v.r; g -= v.g; b -= v.b; a -= v.a;
		return *this;
	}
	inline Color_tpl & operator *= (const Color_tpl & v)
	{
		r *= v.r; g *= v.g; b *= v.b; a *= v.a;
		return *this;
	}
	inline Color_tpl & operator /= (const Color_tpl & v)
	{
		r /= v.r; g /= v.g; b /= v.b; a /= v.a;
		return *this;
	}
	inline Color_tpl & operator *= (T s)
	{
		r *= s; g *= s; b *= s; a *= s;
		return *this;
	}
	inline Color_tpl & operator /= (T s)
	{
		s = 1.0f / s;
		r *= s; g *= s; b *= s; a *= s;
		return *this;
	}

	inline Color_tpl operator + (const Color_tpl & v) const
	{
		return Color_tpl(r + v.r, g + v.g, b + v.b, a + v.a);
	}
	inline Color_tpl operator - (const Color_tpl & v) const
	{
		return Color_tpl(r - v.r, g - v.g, b - v.b, a - v.a);
	}
	inline Color_tpl operator * (const Color_tpl & v) const
	{
		return Color_tpl(r * v.r, g * v.g, b * v.b, a * v.a);
	}
	inline Color_tpl operator / (const Color_tpl & v) const
	{
		return Color_tpl(r / v.r, g / v.g, b / v.b, a / v.a);
	}
	inline Color_tpl operator * (T s) const
	{
		return Color_tpl(r * s, g * s, b * s, a * s);
	}
	inline Color_tpl operator / (T s) const
	{
		s = 1.0f / s;
		return Color_tpl(r * s, g * s, b * s, a * s);
	}

	inline bool operator == (const Color_tpl & v) const
	{
		return (r == v.r) && (g == v.g) && (b == v.b) && (a == v.a);
	}
	inline bool operator != (const Color_tpl & v) const
	{
		return (r != v.r) || (g != v.g) || (b != v.b) || (a != v.a);
	}

	inline unsigned char pack_rgb332() const;
	inline unsigned short pack_argb4444() const;
	inline unsigned short pack_rgb555() const;
	inline unsigned short pack_rgb565() const;
	inline unsigned int pack_bgr888() const;
	inline unsigned int pack_rgb888() const;
	inline unsigned int pack_abgr8888() const;
	inline unsigned int pack_argb8888() const;
  inline Vec4 toVec4() const { return Vec4(r,g,b,a); }
  inline Vec3 toVec3() const { return Vec3(r, g, b); }

	inline void toHSV(f32 &h, f32 &s, f32&v) const;
	inline void fromHSV(f32 h, f32 s, f32 v);

	inline void clamp(T bottom = 0.0f, T top = 1.0f);

	inline void maximum(const Color_tpl<T> &ca, const Color_tpl<T> &cb);
	inline void minimum(const Color_tpl<T> &ca, const Color_tpl<T> &cb);
	inline void abs();

	inline void adjust_contrast(T c);
	inline void adjust_saturation(T s);

	inline void lerpFloat(const Color_tpl<T> &ca, const Color_tpl<T> &cb, float s);
	inline void negative(const Color_tpl<T> &c);
	inline void grey(const Color_tpl<T> &c);

	// helper function - maybe we can improve the integration
	static inline uint32 ComputeAvgCol_Fast( const uint32 dwCol0, const uint32 dwCol1 )
	{
		uint32 dwHalfCol0 = (dwCol0/2) & 0x7f7f7f7f;			// each component /2
		uint32 dwHalfCol1 = (dwCol1/2) & 0x7f7f7f7f;			// each component /2

		return dwHalfCol0+dwHalfCol1;
	}

	// mCIE: adjusted to compensate problems of DXT compression (extra bit in green channel causes green/purple artifacts)
	Color_tpl<T> RGB2mCIE() const
	{
		Color_tpl<T> in=*this;

		// to get grey chrominance for dark colors
		in.r+=0.000001f;
		in.g+=0.000001f;
		in.b+=0.000001f;

		float RGBSum = in.r+in.g+in.b;		

		float fInv = 1.0f / RGBSum;

		float RNorm = 3*10.0f/31.0f*in.r * fInv;
		float GNorm = 3*21.0f/63.0f*in.g * fInv;
		float Scale = RGBSum/3.0f;

		// test grey
		//	out.r = 10.0f/31.0f;		// 1/3 = 10/30      Red range     0..30, 31 unused
		//	out.g = 21.0f/63.0f;		// 1/3 = 21/63      Green range   0..63

		if(RNorm<0)RNorm=0;				if(RNorm>1)RNorm=1;
		if(GNorm<0)GNorm=0;				if(GNorm>1)GNorm=1;

		return Color_tpl<T>(RNorm,GNorm,Scale);
	}

	// mCIE: adjusted to compensate problems of DXT compression (extra bit in green channel causes green/purple artefacts)
	Color_tpl<T> mCIE2RGB() const
	{
		Color_tpl<T> out = *this;

		float fScale = out.b;				//                  Blue range   0..31

		// test grey
		//	out.r = 10.0f/31.0f;		// 1/3 = 10/30      Red range     0..30, 31 unused
		//	out.g = 21.0f/63.0f;		// 1/3 = 21/63      Green range   0..63

		out.r *= 31.0f/30.0f;		// 
		out.g *= 63.0f/63.0f;		//
		out.b = 0.999f-out.r-out.g;

		float s = 3.0f*fScale;

		out.r*=s; out.g*=s; out.b*=s;

		out.Clamp();

		return out;
	}

  void rgb2srgb()
  {
    for(int i=0; i<3; i++)
    {
      T & c = (*this)[i];

      if( c < 0.0031308f ) 
      { 
        c = 12.92f * c; 
      } 
      else 
      { 
        c = 1.005f * pow( c, 1.0f/2.4f ) - 0.055f; 
      } 
    }
  }

  void srgb2rgb()
  {
    for(int i=0; i<3; i++)
    {
      T & c = (*this)[i];

      if( c <= 0.03928f ) 
      { 
        c = c/12.92f; 
      } 
      else 
      { 
        c = pow( ( c + 0.055f) / 1.055f, 2.4f ); 
      } 
    }
  }

};


//////////////////////////////////////////////////////////////////////////////////////////////
// template specialization 
///////////////////////////////////////////////

template<>
inline Color_tpl<f32>::Color_tpl(f32 _x, f32 _y, f32 _z, f32 _w)	
{
	r = _x; g = _y; b = _z; a = _w;
}

template<>
inline Color_tpl<f32>::Color_tpl(f32 _x, f32 _y, f32 _z)
{
	r = _x; g = _y; b = _z; a = 1.f;
}

template<>
inline Color_tpl<uint8>::Color_tpl(uint8 _x, uint8 _y, uint8 _z, uint8 _w)
{
	r = _x; g = _y; b = _z; a = _w;
}

template<>
inline Color_tpl<uint8>::Color_tpl(uint8 _x, uint8 _y, uint8 _z)
{
	r = _x; g = _y; b = _z; a = 255;
}

//-----------------------------------------------------------------------------

template<>
inline Color_tpl<f32>::Color_tpl(const unsigned int abgr)	
{
	r = (abgr & 0xff) / 255.0f;
	g = ((abgr>>8) & 0xff) / 255.0f;
	b = ((abgr>>16) & 0xff) / 255.0f;
	a = ((abgr>>24) & 0xff) / 255.0f;
}

template<>
inline Color_tpl<uint8>::Color_tpl(const unsigned int c)	{	*(unsigned int*)(&r)=c; } //use this with RGBA8 macro!

//-----------------------------------------------------------------------------

template<>
inline Color_tpl<f32>::Color_tpl(const float c)	
{
	r=c;	g=c;	b=c;	a=c;
}
template<>
inline Color_tpl<uint8>::Color_tpl(const float c)	
{
	r = (uint8)(c*255);	g = (uint8)(c*255);	b = (uint8)(c*255);	a = (uint8)(c*255);
}
//-----------------------------------------------------------------------------

template<>
inline Color_tpl<f32>::Color_tpl(const ColorF& c)	
{
	r=c.r;	g=c.g;	b=c.b;	a=c.a;
}
template<>
inline Color_tpl<uint8>::Color_tpl(const ColorF& c)	{
	r = (uint8)(c.r*255);	g = (uint8)(c.g*255);	b = (uint8)(c.b*255);	a = (uint8)(c.a*255);
}

template<>
inline Color_tpl<f32>::Color_tpl(const ColorF& c, float fAlpha)
{
	r=c.r;	g=c.g;	b=c.b;	a=fAlpha;
}

template<>
inline Color_tpl<f32>::Color_tpl(const Vec3& c, float fAlpha)
{
	r=c.x;	g=c.y;	b=c.z;	a=fAlpha;
}

template<>
inline Color_tpl<uint8>::Color_tpl(const ColorF& c, float fAlpha)
{
	r = (uint8)(c.r*255);	g = (uint8)(c.g*255);	b = (uint8)(c.b*255);	a = (uint8)(fAlpha*255);
}
template<>
inline Color_tpl<uint8>::Color_tpl(const Vec3& c, float fAlpha)	
{
	r = (uint8)(c.x*255);	g = (uint8)(c.y*255);	b = (uint8)(c.z*255);	a = (uint8)(fAlpha*255);
}


//////////////////////////////////////////////////////////////////////////////////////////////
// functions implementation
///////////////////////////////////////////////

///////////////////////////////////////////////
/*template <class T>
inline Color_tpl<T>::Color_tpl(const T *p_elts)
{
r = p_elts[0]; g = p_elts[1]; b = p_elts[2]; a = p_elts[3];
}*/

///////////////////////////////////////////////
///////////////////////////////////////////////
template <class T>
inline Color_tpl<T> operator * (T s, const Color_tpl<T> & v)
{
	return Color_tpl<T>(v.r * s, v.g * s, v.b * s, v.a * s);
}

///////////////////////////////////////////////
template <class T>
inline unsigned char Color_tpl<T>::pack_rgb332() const
{
	unsigned char cr;
	unsigned char cg;
	unsigned char cb;
	if(sizeof(r) == 1) // char and unsigned char
	{
		cr = (unsigned char)r;
		cg = (unsigned char)g;
		cb = (unsigned char)b;
	}
	else if(sizeof(r) == 2) // short and unsigned short
	{
		cr = (unsigned short)(r)>>8;
		cg = (unsigned short)(g)>>8;
		cb = (unsigned short)(b)>>8;
	}
	else // float or double
	{
		cr = (unsigned char)(r * 255.0f);
		cg = (unsigned char)(g * 255.0f);
		cb = (unsigned char)(b * 255.0f);
	}
	return ((cr >> 5) << 5) | ((cg >> 5) << 2) | (cb >> 5);
}

///////////////////////////////////////////////
template <class T>
inline unsigned short Color_tpl<T>::pack_argb4444() const
{
	unsigned char cr;
	unsigned char cg;
	unsigned char cb;
	unsigned char ca;
	if(sizeof(r) == 1) // char and unsigned char
	{
		cr = (unsigned char)r;
		cg = (unsigned char)g;
		cb = (unsigned char)b;
		ca = (unsigned char)a;
	}
	else if(sizeof(r) == 2) // short and unsigned short
	{
		cr = (unsigned short)(r)>>8;
		cg = (unsigned short)(g)>>8;
		cb = (unsigned short)(b)>>8;
		ca = (unsigned short)(a)>>8;
	}
	else // float or double
	{
		cr = (unsigned char)(r * 255.0f);
		cg = (unsigned char)(g * 255.0f);
		cb = (unsigned char)(b * 255.0f);
		ca = (unsigned char)(a * 255.0f);
	}
	return ((ca >> 4) << 12) | ((cr >> 4) << 8) | ((cg >> 4) << 4) | (cb >> 4);
}

///////////////////////////////////////////////
template <class T>
inline unsigned short Color_tpl<T>::pack_rgb555() const
{
	unsigned char cr;
	unsigned char cg;
	unsigned char cb;
	if(sizeof(r) == 1) // char and unsigned char
	{
		cr = (unsigned char)r;
		cg = (unsigned char)g;
		cb = (unsigned char)b;
	}
	else if(sizeof(r) == 2) // short and unsigned short
	{
		cr = (unsigned short)(r)>>8;
		cg = (unsigned short)(g)>>8;
		cb = (unsigned short)(b)>>8;
	}
	else // float or double
	{
		cr = (unsigned char)(r * 255.0f);
		cg = (unsigned char)(g * 255.0f);
		cb = (unsigned char)(b * 255.0f);
	}
	return ((cr >> 3) << 10) | ((cg >> 3) << 5) | (cb >> 3);
}

///////////////////////////////////////////////
template <class T>
inline unsigned short Color_tpl<T>::pack_rgb565() const
{
	unsigned short cr;
	unsigned short cg;
	unsigned short cb;
	if(sizeof(r) == 1) // char and unsigned char
	{
		cr = (unsigned short)r;
		cg = (unsigned short)g;
		cb = (unsigned short)b;
	}
	else if(sizeof(r) == 2) // short and unsigned short
	{
		cr = (unsigned short)(r)>>8;
		cg = (unsigned short)(g)>>8;
		cb = (unsigned short)(b)>>8;
	}
	else // float or double
	{
		cr = (unsigned short)(r * 255.0f);
		cg = (unsigned short)(g * 255.0f);
		cb = (unsigned short)(b * 255.0f);
	}
	return ((cr >> 3) << 11) |	((cg >> 2) << 5) | (cb >> 3);
}

///////////////////////////////////////////////
template <class T>
inline unsigned int Color_tpl<T>::pack_bgr888() const
{
	unsigned char cr;
	unsigned char cg;
	unsigned char cb;
	if(sizeof(r) == 1) // char and unsigned char
	{
		cr = (unsigned char)r;
		cg = (unsigned char)g;
		cb = (unsigned char)b;
	}
	else if(sizeof(r) == 2) // short and unsigned short
	{
		cr = (unsigned short)(r)>>8;
		cg = (unsigned short)(g)>>8;
		cb = (unsigned short)(b)>>8;
	}
	else // float or double
	{
		cr = (unsigned char)(r * 255.0f);
		cg = (unsigned char)(g * 255.0f);
		cb = (unsigned char)(b * 255.0f);
	}
	return (cb << 16) | (cg << 8) | cr;
}

///////////////////////////////////////////////
template <class T>
inline unsigned int Color_tpl<T>::pack_rgb888() const
{
	unsigned char cr;
	unsigned char cg;
	unsigned char cb;
	if(sizeof(r) == 1) // char and unsigned char
	{
		cr = (unsigned char)r;
		cg = (unsigned char)g;
		cb = (unsigned char)b;
	}
	else if(sizeof(r) == 2) // short and unsigned short
	{
		cr = (unsigned short)(r)>>8;
		cg = (unsigned short)(g)>>8;
		cb = (unsigned short)(b)>>8;
	}
	else // float or double
	{
		cr = (unsigned char)(r * 255.0f);
		cg = (unsigned char)(g * 255.0f);
		cb = (unsigned char)(b * 255.0f);
	}
	return (cr << 16) | (cg << 8) | cb;
}

///////////////////////////////////////////////
template <class T>
inline unsigned int Color_tpl<T>::pack_abgr8888() const
{
	unsigned char cr;
	unsigned char cg;
	unsigned char cb;
	unsigned char ca;
	if(sizeof(r) == 1) // char and unsigned char
	{
		cr = (unsigned char)r;
		cg = (unsigned char)g;
		cb = (unsigned char)b;
		ca = (unsigned char)a;
	}
	else if(sizeof(r) == 2) // short and unsigned short
	{
		cr = (unsigned short)(r)>>8;
		cg = (unsigned short)(g)>>8;
		cb = (unsigned short)(b)>>8;
		ca = (unsigned short)(a)>>8;
	}
	else // float or double
	{
		cr = (unsigned char)(r * 255.0f);
		cg = (unsigned char)(g * 255.0f);
		cb = (unsigned char)(b * 255.0f);
		ca = (unsigned char)(a * 255.0f);
	}
	return (ca << 24) | (cb << 16) | (cg << 8) | cr;
}


///////////////////////////////////////////////
template <class T>
inline unsigned int Color_tpl<T>::pack_argb8888() const
{
	unsigned char cr;
	unsigned char cg;
	unsigned char cb;
	unsigned char ca;
	if(sizeof(r) == 1) // char and unsigned char
	{
		cr = (unsigned char)r;
		cg = (unsigned char)g;
		cb = (unsigned char)b;
		ca = (unsigned char)a;
	}
	else if(sizeof(r) == 2) // short and unsigned short
	{
		cr = (unsigned short)(r)>>8;
		cg = (unsigned short)(g)>>8;
		cb = (unsigned short)(b)>>8;
		ca = (unsigned short)(a)>>8;
	}
	else // float or double
	{
		cr = (unsigned char)(r * 255.0f);
		cg = (unsigned char)(g * 255.0f);
		cb = (unsigned char)(b * 255.0f);
		ca = (unsigned char)(a * 255.0f);
	}
	return (ca << 24) | (cr << 16) | (cg << 8) | cb;
}

///////////////////////////////////////////////
template <class T>
inline void Color_tpl<T>::toHSV(f32 &h, f32 &s, f32&v) const
{
	f32 r, g, b;
	if (sizeof(this->r) == 1) // 8bit integer
	{
		r = this->r * (1.0f / f32(0xff));
		g = this->g * (1.0f / f32(0xff));
		b = this->b * (1.0f / f32(0xff));
	}
	else if (sizeof(this->r) == 2) // 16bit integer
	{
		r = this->r * (1.0f / f32(0xffff));
		g = this->g * (1.0f / f32(0xffff));
		b = this->b * (1.0f / f32(0xffff));
	}
	else // floating point
	{
		r = this->r;
		g = this->g;
		b = this->b;
	}

	if ((b > g) && (b > r))
	{
		if (!::iszero(v = b))
		{
			const f32 min = r < g ? r : g;
			const f32 delta = v - min;
			if (!::iszero(delta))
			{
				s = delta / v;
				h = (240.0f/360.0f) + (r - g) / delta * (60.0f/360.0f);
			}
			else
			{
				s = 0.0f;
				h = (240.0f/360.0f) + (r - g) * (60.0f/360.0f);
			}
			if (h < 0.0f) h += 1.0f;
		}
		else
		{
			s = 0.0f;
			h = 0.0f;
		}
	}
	else if (g > r)
	{
		if (!::iszero(v = g))
		{
			const f32 min = r < b ? r : b;
			const f32 delta = v - min;
			if (!::iszero(delta))
			{
				s = delta / v;
				h = (120.0f/360.0f) + (b - r) / delta * (60.0f/360.0f);
			}
			else
			{
				s = 0.0f;
				h = (120.0f/360.0f) + (b - r) * (60.0f/360.0f);
			}
			if (h < 0.0f) h += 1.0f;
		}
		else
		{
			s = 0.0f;
			h = 0.0f;
		}
	}
	else
	{
		if (!::iszero(v = r))
		{
			const f32 min = g < b ? g : b;
			const f32 delta = v - min;
			if (!::iszero(delta))
			{
				s = delta / v;
				h = (g - b) / delta * (60.0f/360.0f);
			}
			else
			{
				s = 0.0f;
				h = (g - b) * (60.0f/360.0f);
			}
			if (h < 0.0f) h += 1.0f;
		}
		else
		{
			s = 0.0f;
			h = 0.0f;
		}
	}
}

///////////////////////////////////////////////
template <class T>
inline void Color_tpl<T>::fromHSV(f32 h, f32 s, f32 v)
{
	f32 r, g, b;
	if (::iszero(v))
	{
		r = 0.0f; g = 0.0f; b = 0.0f;
	}
	else if (::iszero(s))
	{
		r = v; g = v; b = v;
	}
	else
	{
		const f32 hi = h * 6.0f;
		const int32 i = (int32)::floor(hi);
		const f32 f = hi - i;

		const f32 v0 = v * (1.0f - s);
		const f32 v1 = v * (1.0f - s * f);
		const f32 v2 = v * (1.0f - s * (1.0f - f));

		switch (i)
		{
		case 0: r = v; g = v2; b = v0; break;
		case 1: r = v1; g = v; b = v0; break;
		case 2: r = v0; g = v; b = v2; break;
		case 3: r = v0; g = v1; b = v; break;
		case 4: r = v2; g = v0; b = v; break;
		case 5: r = v; g = v0; b = v1; break;

		case 6: r = v; g = v2; b = v0; break;
		case -1: r = v; g = v0; b = v1; break;
		default: r = 0.0f; g = 0.0f; b = 0.0f; break;
		}
	}

	if (sizeof(this->r) == 1) // 8bit integer
	{
		this->r = r * f32(0xff);
		this->g = g * f32(0xff);
		this->b = b * f32(0xff);
	}
	else if (sizeof(this->r) == 2) // 16bit integer
	{
		this->r = r * f32(0xffff);
		this->g = g * f32(0xffff);
		this->b = b * f32(0xffff);
	}
	else // floating point
	{
		this->r = r;
		this->g = g;
		this->b = b;
	}
}

///////////////////////////////////////////////
template <class T>
inline void Color_tpl<T>::clamp(T bottom, T top)
{
	if(r < bottom)	r = bottom;
	else if(r > top)	r = top;
	if(g < bottom)	g = bottom;
	else if(g > top)	g = top;
	if(b < bottom)	b = bottom;
	else if(b > top)	b = top;
	if(a < bottom)	a = bottom;
	else if(a > top)	a = top;
}

///////////////////////////////////////////////
template <class T>
void Color_tpl<T>::maximum(const Color_tpl<T> &ca, const Color_tpl<T> &cb)
{
	r = (ca.r > cb.r) ? ca.r : cb.r;
	g = (ca.g > cb.g) ? ca.g : cb.g;
	b = (ca.b > cb.b) ? ca.b : cb.b;
	a = (ca.a > cb.a) ? ca.a : cb.a;
}

///////////////////////////////////////////////
template <class T>
void Color_tpl<T>::minimum(const Color_tpl<T> &ca, const Color_tpl<T> &cb)
{
	r = (ca.r < cb.r) ? ca.r : cb.r;
	g = (ca.g < cb.g) ? ca.g : cb.g;
	b = (ca.b < cb.b) ? ca.b : cb.b;
	a = (ca.a < cb.a) ? ca.a : cb.a;
}

///////////////////////////////////////////////
template <class T>
void Color_tpl<T>::abs()
{
	r = (r < 0) ? -r : r;
	g = (g < 0) ? -g : g;
	b = (b < 0) ? -b : b;
	a = (a < 0) ? -a : a;
}

///////////////////////////////////////////////
template <class T>
void Color_tpl<T>::adjust_contrast(T c)
{
	r = 0.5f + c * (r - 0.5f);
	g = 0.5f + c * (g - 0.5f);
	b = 0.5f + c * (b - 0.5f);
	a = 0.5f + c * (a - 0.5f);
}

///////////////////////////////////////////////
template <class T>
void Color_tpl<T>::adjust_saturation(T s)
{
	// Approximate values for each component's contribution to luminance.
	// Based upon the NTSC standard described in ITU-R Recommendation BT.709.
	T grey = r * 0.2125f + g * 0.7154f + b * 0.0721f;    
	r = grey + s * (r - grey);
	g = grey + s * (g - grey);
	b = grey + s * (b - grey);
	a = grey + s * (a - grey);
}

///////////////////////////////////////////////
template <class T>
void Color_tpl<T>::lerpFloat(const Color_tpl<T> &ca, const Color_tpl<T> &cb, float s)
{
	r = (T)(ca.r + s * (cb.r - ca.r));
	g = (T)(ca.g + s * (cb.g - ca.g));
	b = (T)(ca.b + s * (cb.b - ca.b));
	a = (T)(ca.a + s * (cb.a - ca.a));
}

///////////////////////////////////////////////
template <class T>
void Color_tpl<T>::negative(const Color_tpl<T> &c)
{
	r = T(1.0f) - r;
	g = T(1.0f) - g;
	b = T(1.0f) - b;
	a = T(1.0f) - a;
}

///////////////////////////////////////////////
template <class T>
void Color_tpl<T>::grey(const Color_tpl<T> &c)
{
	T m = (r + g + b) / T(3);

	r = m;
	g = m;
	b = m;
	a = a;
}

//////////////////////////////////////////////////////////////////////////////////////////////
//#define RGBA8(r,g,b,a)   (ColorB((uint8)(r),(uint8)(g),(uint8)(b),(uint8)(a)))
#if defined(NEED_ENDIAN_SWAP)
	#define RGBA8(a,b,g,r)     ((uint32)(((uint8)(r)|((uint16)((uint8)(g))<<8))|(((uint32)(uint8)(b))<<16)) | (((uint32)(uint8)(a))<<24) )
#else
	#define RGBA8(r,g,b,a)     ((uint32)(((uint8)(r)|((uint16)((uint8)(g))<<8))|(((uint32)(uint8)(b))<<16)) | (((uint32)(uint8)(a))<<24) )
#endif
#define Col_Black		ColorF (0.0f, 0.0f, 0.0f)
#define Col_White		ColorF (1.0f, 1.0f, 1.0f)
#define Col_Aquamarine		ColorF (0.439216f, 0.858824f, 0.576471f)
#define Col_Blue		ColorF (0.0f, 0.0f, 1.0f)
#define Col_BlueViolet		ColorF (0.623529f, 0.372549f, 0.623529f)
#define Col_Brown		ColorF (0.647059f, 0.164706f, 0.164706f)
#define Col_CadetBlue		ColorF (0.372549f, 0.623529f, 0.623529f)
#define Col_Coral		ColorF (1.0f, 0.498039f, 0.0f)
#define Col_CornflowerBlue	ColorF (0.258824f, 0.258824f, 0.435294f)
#define Col_Cyan		ColorF (0.0f, 1.0f, 1.0f)
#define Col_DarkGray		ColorF (0.5f, 0.5f, 0.5f)
#define Col_DarkGreen		ColorF (0.184314f, 0.309804f, 0.184314f)
#define Col_DarkOliveGreen	ColorF (0.309804f, 0.309804f, 0.184314f)
#define Col_DarkOrchild		ColorF (0.6f, 0.196078f, 0.8f)
#define Col_DarkSlateBlue	ColorF (0.419608f, 0.137255f, 0.556863f)
#define Col_DarkSlateGray	ColorF (0.184314f, 0.309804f, 0.309804f)
#define Col_DarkSlateGrey	ColorF (0.184314f, 0.309804f, 0.309804f)
#define Col_DarkTurquoise	ColorF (0.439216f, 0.576471f, 0.858824f)
#define Col_DarkWood		ColorF (0.05f, 0.01f, 0.005f)
#define Col_DimGray		ColorF (0.329412f, 0.329412f, 0.329412f)
#define Col_DimGrey		ColorF (0.329412f, 0.329412f, 0.329412f)
#define Col_FireBrick		ColorF (0.9f, 0.4f, 0.3f)
#define Col_ForestGreen		ColorF (0.137255f, 0.556863f, 0.137255f)
#define Col_Gold		ColorF (0.8f, 0.498039f, 0.196078f)
#define Col_Goldenrod		ColorF (0.858824f, 0.858824f, 0.439216f)
#define Col_Gray		ColorF (0.752941f, 0.752941f, 0.752941f)
#define Col_Green		ColorF (0.0f, 1.0f, 0.0f)
#define Col_GreenYellow		ColorF (0.576471f, 0.858824f, 0.439216f)
#define Col_Grey		ColorF (0.752941f, 0.752941f, 0.752941f)
#define Col_IndianRed		ColorF (0.309804f, 0.184314f, 0.184314f)
#define Col_Khaki		ColorF (0.623529f, 0.623529f, 0.372549f)
#define Col_LightBlue		ColorF (0.74902f, 0.847059f, 0.847059f)
#define Col_LightGray		ColorF (0.658824f, 0.658824f, 0.658824f)
#define Col_LightGrey		ColorF (0.658824f, 0.658824f, 0.658824f)
#define Col_LightSteelBlue	ColorF (0.560784f, 0.560784f, 0.737255f)
#define Col_LightWood		ColorF (0.6f, 0.24f, 0.1f)
#define Col_LimeGreen		ColorF (0.196078f, 0.8f, 0.196078f)
#define Col_Magenta		ColorF (1.0f, 0.0f, 1.0f)
#define Col_Maroon		ColorF (0.556863f, 0.137255f, 0.419608f)
#define Col_MedianWood		ColorF (0.3f, 0.12f, 0.03f)
#define Col_MediumAquamarine	ColorF (0.196078f, 0.8f, 0.6f)
#define Col_MediumBlue		ColorF (0.196078f, 0.196078f, 0.8f)
#define Col_MediumForestGreen	ColorF (0.419608f, 0.556863f, 0.137255f)
#define Col_MediumGoldenrod	ColorF (0.917647f, 0.917647f, 0.678431f)
#define Col_MediumOrchid	ColorF (0.576471f, 0.439216f, 0.858824f)
#define Col_MediumSeaGreen	ColorF (0.258824f, 0.435294f, 0.258824f)
#define Col_MediumSlateBlue	ColorF (0.498039f, 0.0f, 1.0f)
#define Col_MediumSpringGreen	ColorF (0.498039f, 1.0f, 0.0f)
#define Col_MediumTurquoise	ColorF (0.439216f, 0.858824f, 0.858824f)
#define Col_MediumVioletRed	ColorF (0.858824f, 0.439216f, 0.576471f)
#define Col_MidnightBlue		ColorF (0.184314f, 0.184314f, 0.309804f)
#define Col_Navy						ColorF (0.137255f, 0.137255f, 0.556863f)
#define Col_NavyBlue				ColorF (0.137255f, 0.137255f, 0.556863f)
#define Col_Orange					ColorF (0.8f, 0.196078f, 0.196078f)
#define Col_OrangeRed				ColorF (0.0f, 0.0f, 0.498039f)
#define Col_Orchid					ColorF (0.858824f, 0.439216f, 0.858824f)
#define Col_PaleGreen				ColorF (0.560784f, 0.737255f, 0.560784f)
#define Col_Pink						ColorF (0.737255f, 0.560784f, 0.560784f)
#define Col_Plum						ColorF (0.917647f, 0.678431f, 0.917647f)
#define Col_Red							ColorF (1.0f, 0.0f, 0.0f)
#define Col_Salmon					ColorF (0.435294f, 0.258824f, 0.258824f)
#define Col_SeaGreen				ColorF (0.137255f, 0.556863f, 0.419608f)
#define Col_Sienna					ColorF (0.556863f, 0.419608f, 0.137255f)
#define Col_SkyBlue					ColorF (0.196078f, 0.6f, 0.8f)
#define Col_SlateBlue				ColorF (0.0f, 0.498039f, 1.0f)
#define Col_SpringGreen			ColorF (0.0f, 1.0f, 0.498039f)
#define Col_SteelBlue				ColorF (0.137255f, 0.419608f, 0.556863f)
#define Col_Tan							ColorF (0.858824f, 0.576471f, 0.439216f)
#define Col_Thistle					ColorF (0.847059f, 0.74902f, 0.847059f)
#define Col_Transparent				ColorF (0.0f, 0.0f, 0.0f, 0.0f)
#define Col_Turquoise				ColorF (0.678431f, 0.917647f, 0.917647f)
#define Col_Violet					ColorF (0.309804f, 0.184314f, 0.309804f)
#define Col_VioletRed				ColorF (0.8f, 0.196078f, 0.6f)
#define Col_Wheat						ColorF (0.847059f, 0.847059f, 0.74902f)
#define Col_Yellow					ColorF (1.0f, 1.0f, 0.0f)
#define Col_YellowGreen			ColorF (0.6f, 0.8f, 0.196078f)


#endif


