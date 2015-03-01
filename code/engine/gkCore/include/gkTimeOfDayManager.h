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
// Name:   	gkTimeOfDayManager.h
// Desc:		
// 
// Author:  Kaiming
// Date:	2011/10/27 
// Modify:	2011/10/27
// 
//////////////////////////////////////////////////////////////////////////

#ifndef _GKTIMEOFDAYMANAGER_H_
#define _GKTIMEOFDAYMANAGER_H_

#include "ITimeOfDay.h"

struct IRapidXmlAuthor;
struct CRapidXmlAuthorNode;
struct IGameObject;

class gkTimeOfDayManager : 
	public ITimeOfDay,
	public IFileChangeMonitorListener
{
public:
	gkTimeOfDayManager(void);
	virtual ~gkTimeOfDayManager(void);//saveTODSequence(_T("..\\engine\\config\\default.tod"));}
	virtual float getTime() {return m_fCurTime;}
	virtual float getSpeed() {return m_fSpeed;}

	virtual void setTime(float time) {m_fCurTime = time; update(0, true);}
	virtual void setSpeed(float speed) {m_fSpeed = speed; }

	virtual const STimeOfDayKey& getCurrentTODKeyPure() const;
	virtual const STimeOfDayKey& getCurrentTODKey() const;
	virtual const STimeOfDayKey& getPrevTODKey() const;
	virtual const STimeOfDayKey& getNextTODKey() const;


	virtual void setTODKey(STimeOfDayKey& key);
	virtual void removeTODKey(float time);
	virtual TODKeySequence& getTODKeys();
	virtual void setPreviewTODKey(STimeOfDayKey& key);

	virtual bool saveTODSequence(const TCHAR* relativePath);
	virtual bool loadTODSequence(CRapidXmlParseNode* node, bool update = false);
	virtual bool loadTODSequence(const TCHAR* relativePath, bool update = false);

	void update(float fElapsedTime, bool forced = false);
	void processCurTodKey() const;

	void init();

	virtual bool bindSunFocus(IGameObject* obj);

private:
	void addFloatValue(uint16 PARAMID, IRapidXmlAuthor& todAuthor, CRapidXmlAuthorNode* father, const TCHAR* name);
	void addColorValue(uint16 PARAMID, IRapidXmlAuthor& todAuthor, CRapidXmlAuthorNode* father, const TCHAR* name);

	void readFloatValue(uint16 PARAMID, int count, const TCHAR* value, const TCHAR* name, const TCHAR* paraname);
	void readColorValue(uint16 PARAMID, int count, const TCHAR* value, const TCHAR* name, const TCHAR* paraname);

	virtual void OnFileChange( const TCHAR* filename );


protected:
	float m_fCurTime;
	float m_fSpeed;

	mutable STimeOfDayKey m_sCurTODKeyProcessed;
	STimeOfDayKey m_sCurTODKey;
	TODKeySequence m_listTODKeySeq;
	mutable float m_fHDRMultiplier;
	IGameObject* m_sunFocus;

	gkStdString m_cachedTodFile;
};

#endif