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
// Name:   	ITimeOfDay.h
// Desc:		
// 
// Author:  Kaiming
// Date:	2011/10/27 
// Modify:	2011/10/27
// 
//////////////////////////////////////////////////////////////////////////

#ifndef _ITIMEOFDAY_H_
#define _ITIMEOFDAY_H_
#include "gk_Color.h"

struct CRapidXmlParseNode;
struct IGameObject;

enum ETODVARS
{
	// FIRST keytime
	KEYTIME = 0,

	// GLOBAL VARS
	HDRBRIGHTNESS = 1,
	HDRCONSTACT,
	SSAOINTENSITY,
	CLSKYLIGHT,
	CLSKYLIGHT1,
	CLSKYLIGHT2,
	CLSKYLIGHT3,
	CLGROUNDAMB,
	CLGROUNDAMB1,
	CLGROUNDAMB2,
	CLGROUNDAMB3,
	GROUNDAMBMIN,
	GROUNDAMBMAX,

	// SUN VARS
	CLSUNLIGHT,
	CLSUNLIGHT1,
	CLSUNLIGHT2,
	CLSUNLIGHT3,
	SUNINTENSITY,
	SUNSPECINTENSITY,
	SUNSHAFTVISIBILITY,
	SUNSHAFTATTEN,
	SUNSHAFTLENGTH,

	// SKYBOX VARS
	CLZENITHBOTTOM,
	CLZENITHBOTTOM1,
	CLZENITHBOTTOM2,
	CLZENITHBOTTOM3,
	CLZENITHTOP,
	CLZENITHTOP1,
	CLZENITHTOP2,
	CLZENITHTOP3,
	ZENITHSHIFT,

	// SKYBOX VER2
	TURBIDITY,
	RAYLEIGH,
	MIECOEFFICENT,
	MIEDIRECTIONALG,

	// FOG VARS
	CLFOGCOLOR,
	CLFOGCOLOR1,
	CLFOGCOLOR2,
	CLFOGCOLOR3,
	FOGDENSITY,
	FOGSTART,

	// HDR VARS
	BLOOMINTENSITY,
	CLBLOOMCOLOR,
	CLBLOOMCOLOR1,
	CLBLOOMCOLOR2,
	CLBLOOMCOLOR3,

	STREAKINTENSITY,
	CLSTREAKCOLOR,
	CLSTREAKCOLOR1,
	CLSTREAKCOLOR2,
	CLSTREAKCOLOR3,

	SATURATE,
	CONSTRACT,
	BRIGHTNESS,

	LEVELINPUTMIN,
	LEVEL,
	LEVELINPUTMAX,

	DOFBLURINTENSITY,
	DOFDISTANCE,
	DOFDISTANCENEAR,
	DOFFOCUSPLANE,
	DOFFOCUSDISTANCE,

	LDRFAKEBRIGHTNESS,

	VAR_END,
};


struct STimeOfDayKey
{
	// we are all float, 
	// so we interpole, 
	// we can use memoffset, not the name of us

	// change it to first one [12/28/2011 Kaiming]
	// first one, the time of this key
	float fKeyTime;

	// global adjust
	float fHDRBrightness;
	float fHDRConstract;
	float fSSAOIntensity;
	ColorF clSkyLight;
	ColorF clGroundAmb;
	float fGroundAmbMin;
	float fGroundAmbMax;

	// sun adjust
	ColorF clSunLight;
	float fSunIntensity;
	float fSunSpecIntensity;
	float fSunShaftVisibility;
	float fSunShaftAtten;
	float fSunShaftLength;

	// skybox adjust
	ColorF clZenithBottom;
	ColorF clZenithTop;
	float fZenithShift;

	// skybox ver2
	float fTurbidity;
	float fRayleigh;
	float fMieCoefficent;
	float fMieDirectionalG;

	// FOG
	ColorF clFogColor;
	float clFogDensity;
	float clFogStart;

	// HDR
	float fBloomIntensity;
	ColorF clBloomColor;

	float fStreakIntensity;
	ColorF clStreakColor;

	// ColorCorrection
	float fSaturate;
	float fConstract;
	float fBrightness;

	float fLevelInputMin;
	float fLevel;
	float fLeveInputMax;

	// DOF
	float fDofBlurIntensity;
	float fDofDistance;
	float fDofDistanceNear;
	float fDofFocusPlane;
	float fDofFocusDistance;

	float fLdrFakeBrightness;

	STimeOfDayKey()
	{
		fHDRBrightness = 1.0f;
		fHDRConstract = 1.0f;
		fSSAOIntensity = 1.0f;
		clSkyLight = ColorF(153.0f/255.0f, 204.0f/255.0f, 255.0f/255.0f, 1.0f);
		clGroundAmb = ColorF(1.0f/255.0f, 8.0f/255.0f, 25.0f/255.0f, 1.0f);
		fGroundAmbMin = 0.0f;
		fGroundAmbMax = 360.0f;

		// sun adjust
		clSunLight = ColorF(255.0f/255.0f, 235.0f/255.0f, 182.0f/255.0f, 1.0f);
		fSunIntensity = 5.0f;
		fSunSpecIntensity = 1.0f;
		fSunShaftVisibility = 1.0f;
		fSunShaftAtten = 10.0f;
		fSunShaftLength = 1.0f;

		// skybox adjust
		clZenithBottom = ColorF(23.f/255.0f, 51.f/255.0f, 102.f/255.0f, 1.0f);
		clZenithTop = ColorF(12.f/255.0f, 29.f/255.0f, 85.f/255.0f, 1.0f);
		fZenithShift = 0.5f;

		// skybox ver2 init
		fTurbidity = 1.0f;
		fRayleigh = 3.0f;
		fMieCoefficent = 0.1f;
		fMieDirectionalG = 0.6f;

		// fog
		clFogColor = ColorF(0.1f, 0.15f, 0.3f, 0.001f);
		clFogDensity = 0.004f;
		clFogStart = 0.015f;

		// HDR
		fBloomIntensity = 0.3f;
		clBloomColor = ColorF(0.8f, 0.8f, 1.0f, 1.0f);

		fStreakIntensity = 0.01f;
		clStreakColor = ColorF(0.8f, 0.8f, 1.0f, 1.0f);


		// ColorCorrection
		fSaturate = 0.82f;
		fConstract = 1.0f;
		fBrightness = 1.0f;

		fLevelInputMin = 0.0f;
		fLevel = 0.92f;
		fLeveInputMax = 239.0f;

		// DOF
		fDofBlurIntensity = 1.0f;
		fDofDistance = 600.0f;
		fDofDistanceNear = 0.5f;
		fDofFocusPlane = 1.0f;
		fDofFocusDistance = 50.0f;

		fLdrFakeBrightness = 4.0f;

		fKeyTime = 9.0f;
	}

	STimeOfDayKey interpole(const STimeOfDayKey& rhs, float amount)
	{
		amount = clamp(amount, 0.0f, 1.0f);

		STimeOfDayKey ret;

		int lennn = sizeof(STimeOfDayKey) / sizeof(float);
		float* pSrc = (float*)(this);
		float* pDst = (float*)(&rhs);
		float* pOut = (float*)(&ret);

		for(int i= 1; i < lennn; ++i)
		{
			// Linear interpolation is based on the following formula: x*(1-s) + y*s which can equivalently be written as x + s(y-x).
			pOut[i] = pSrc[i] + amount * (pDst[i] - pSrc[i]);
		}

		return ret;
	}

	/// Comparison operator for sorting
	bool operator < (const STimeOfDayKey& rhs) const
	{
		return this->fKeyTime < rhs.fKeyTime;
	}
};

typedef std::vector<STimeOfDayKey> TODKeySequence;

struct ITimeOfDay
{
	virtual float getTime()=0;
	virtual float getSpeed()=0;

	virtual void setTime(float time)=0;
	virtual void setSpeed(float speed)=0;

	virtual const STimeOfDayKey& getCurrentTODKeyPure() const =0;
	virtual const STimeOfDayKey& getCurrentTODKey() const =0;
	virtual const STimeOfDayKey& getPrevTODKey() const =0;
	virtual const STimeOfDayKey& getNextTODKey() const =0;
	virtual void setTODKey(STimeOfDayKey& key) =0;
	virtual void setPreviewTODKey(STimeOfDayKey& key) =0;
	virtual void removeTODKey(float time) =0;
	virtual TODKeySequence& getTODKeys()=0;

	virtual bool saveTODSequence(const TCHAR* relativePath) =0;
	virtual bool loadTODSequence(CRapidXmlParseNode* node, bool bUpdate = false) =0;
	virtual bool loadTODSequence(const TCHAR* relativePath, bool update = false) =0;

	virtual bool bindSunFocus(IGameObject* obj) =0;

	virtual ~ITimeOfDay(void) {}
};


#endif