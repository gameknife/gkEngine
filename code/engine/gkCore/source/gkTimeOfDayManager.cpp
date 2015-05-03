#include "gkStableHeader.h"
#include "gkTimeOfDayManager.h"
#include "gkSceneManager.h"
#include "gk3DEngine.h"
#include "gkSun.h"
#include "IXmlUtil.h"
#include <iomanip>
#include "ICamera.h"


gkTimeOfDayManager::gkTimeOfDayManager( void )
{
	m_fCurTime = 9.0f;
	m_fSpeed = 0.0f;
	m_fHDRMultiplier = 1.f;
	m_listTODKeySeq.clear();

	m_sunFocus = NULL;
}


gkTimeOfDayManager::~gkTimeOfDayManager( void )
{
#ifdef OS_WIN32
	gEnv->pFileChangeMonitor->RemoveListener(this);
#endif
}


void gkTimeOfDayManager::setTODKey( STimeOfDayKey& key )
{
	// find a pos to put in
	if (m_listTODKeySeq.empty())
	{
		m_listTODKeySeq.push_back(key);
		return;
	}

	if (m_listTODKeySeq.size() == 1)
	{
		if (key < m_listTODKeySeq[0])
		{
			m_listTODKeySeq.push_back(m_listTODKeySeq[0]);
			m_listTODKeySeq[0] = key;
		}
		else
		{
			m_listTODKeySeq.push_back(key);
		}
		return;
	}

	// now more than two, we can insert
	bool flag = 1;
	TODKeySequence::iterator it = m_listTODKeySeq.begin();
	for(; it != m_listTODKeySeq.end(); ++it)
	{
		if (key < *it)
		{
			m_listTODKeySeq.insert(it, key);
			flag = 0;
			break;
		}
	}

	if (flag)
	{
		// u r the biggest
		m_listTODKeySeq.push_back(key);
	}
}

void gkTimeOfDayManager::update( float fElapsedTime, bool forced )
{
	static float prevTime = 9.0f;
	m_fCurTime += fElapsedTime * m_fSpeed;
	if (m_fCurTime > 24.0f)
	{
		m_fCurTime = 0.0f;
	}
	else if (m_fCurTime < 0.0f)
	{
		m_fCurTime = 24.0f;
	}

	if (fabsf(m_fCurTime - prevTime) > 0.00001f || forced)
	{
		// here the time change, we should make a new TODkey now
		
		STimeOfDayKey srcKey;// = getPrevTODKey();
		STimeOfDayKey dstKey;// = getNextTODKey();

		if (m_listTODKeySeq.size() > 1)
		{
			// 1. find the place i am
			if (m_fCurTime < m_listTODKeySeq.front().fKeyTime || m_fCurTime > m_listTODKeySeq.back().fKeyTime)
			{
				// we are the smallest one
				srcKey = m_listTODKeySeq.front();
				dstKey = m_listTODKeySeq.back();
			}
			else
			{
				for(uint32 i=1; i < m_listTODKeySeq.size(); ++i)
				{
					if (m_fCurTime <= m_listTODKeySeq[i].fKeyTime)
					{
						// we got this one
						dstKey = m_listTODKeySeq[i];
						srcKey = m_listTODKeySeq[i-1];
						break;
					}
				}

			}
			//GK_ASSERT(0);
		}
		
		// interpole
		m_sCurTODKey = srcKey.interpole(dstKey, (m_fCurTime- srcKey.fKeyTime) / (dstKey.fKeyTime - srcKey.fKeyTime) );

		m_sCurTODKey.fKeyTime = m_fCurTime;
		

	}

	prevTime = m_fCurTime;

	// set sun
	getSceneMngPtr()->getSun()->setTime(m_fCurTime);



	if (m_sunFocus)
	{
		Vec3 campos = get3DEngine()->getMainCamera()->getPosition();
		Vec3 pos = m_sunFocus->getWorldPosition();

		getSceneMngPtr()->getSun()->setOffset( pos - campos );

	}
}

void gkTimeOfDayManager::init()
{
	//if(!loadTODSequence(_T("engine\\config\\default.tod")))
 	{
		//GK_ASSERT(0);
 		STimeOfDayKey key9am;
 		key9am.fKeyTime = 9.0f;
 
 		STimeOfDayKey key15am = key9am;
 		key15am.fKeyTime = 15.0f;
 		setTODKey(key9am);
 		setTODKey(key15am);
 	}
#ifdef OS_WIN32
	gEnv->pFileChangeMonitor->AddListener(this);
#endif
}

void gkTimeOfDayManager::setPreviewTODKey( STimeOfDayKey& key )
{
	m_sCurTODKey = key;
}

void gkTimeOfDayManager::removeTODKey( float time )
{
	TODKeySequence::iterator it = m_listTODKeySeq.begin();
	for (;it != m_listTODKeySeq.end(); ++it)
	{
		if (fabsf(it->fKeyTime - time) < 0.01f)
		{
			m_listTODKeySeq.erase(it);
			return;
		}
	}
}

TODKeySequence& gkTimeOfDayManager::getTODKeys()
{
	return m_listTODKeySeq;
}

const STimeOfDayKey& gkTimeOfDayManager::getPrevTODKey() const
{
	if (m_listTODKeySeq.size() > 1)
	{
		// 0. move a little, so that, we could ensure, we can get the prev
		float meansureTime = m_fCurTime - 0.001f;
		// 1. find the place i am
		if (meansureTime < m_listTODKeySeq.front().fKeyTime || meansureTime > m_listTODKeySeq.back().fKeyTime)
		{
			// we are the smallest one
			// m_listTODKeySeq.front();
			return m_listTODKeySeq.back();
		}
		else
		{
			for(uint32 i=1; i < m_listTODKeySeq.size(); ++i)
			{
				if (meansureTime <= m_listTODKeySeq[i].fKeyTime)
				{
					// we got this one
					//dstKey = m_listTODKeySeq[i];
					return m_listTODKeySeq[i-1];
					break;
				}
			}

		}
		GK_ASSERT(0);

	}

	static STimeOfDayKey nullkey;
	return nullkey;
}

const STimeOfDayKey& gkTimeOfDayManager::getNextTODKey() const
{
	if (m_listTODKeySeq.size() > 1)
	{
		// 0. move a little, so that, we could ensure, we can get the prev
		float meansureTime = m_fCurTime + 0.1f;
		// 1. find the place i am
		if (meansureTime < m_listTODKeySeq.front().fKeyTime || meansureTime > m_listTODKeySeq.back().fKeyTime)
		{
			// we are the smallest one
			return m_listTODKeySeq.front();
			//srcKey = m_listTODKeySeq.back();
		}
		else
		{
			for(uint32 i=1; i < m_listTODKeySeq.size(); ++i)
			{
				if (meansureTime <= m_listTODKeySeq[i].fKeyTime/* + 0.00001f*/)
				{
					// we got this one
					return m_listTODKeySeq[i];
					//srcKey = m_listTODKeySeq[i-1];
					break;
				}

			}

		}
		GK_ASSERT(0);
	}

	static STimeOfDayKey nullkey;
	return nullkey;
}


void gkTimeOfDayManager::processCurTodKey() const
{
	m_sCurTODKeyProcessed = m_sCurTODKey;
	m_fHDRMultiplier = powf( 2.f, m_sCurTODKey.fHDRBrightness);

	m_sCurTODKeyProcessed.clSunLight*= m_fHDRMultiplier;
	m_sCurTODKeyProcessed.clFogColor*= m_fHDRMultiplier;
	m_sCurTODKeyProcessed.clSkyLight*= m_fHDRMultiplier;
	m_sCurTODKeyProcessed.clGroundAmb*= m_fHDRMultiplier;
	m_sCurTODKeyProcessed.clZenithBottom*= m_fHDRMultiplier;
	m_sCurTODKeyProcessed.clZenithTop*= m_fHDRMultiplier;
}

const STimeOfDayKey& gkTimeOfDayManager::getCurrentTODKey() const
{
	processCurTodKey();
	return m_sCurTODKeyProcessed;
}

const STimeOfDayKey& gkTimeOfDayManager::getCurrentTODKeyPure() const
{
	return m_sCurTODKey;
}

void gkTimeOfDayManager::addFloatValue(uint16 PARAMID, IRapidXmlAuthor& todAuthor, CRapidXmlAuthorNode* father, const TCHAR* name)
{
	gkStdStringstream wss;
	//wss << _T("float");
	TODKeySequence::iterator it = m_listTODKeySeq.begin();
	for (it; it != m_listTODKeySeq.end(); ++it)
	{
		// push to float*
		float* key = (float*)(&(*it));
		wss << std::setprecision(3) << key[PARAMID] << _T(" ");
	}

 	CRapidXmlAuthorNode* node = todAuthor.createRapidXmlNode(_T("float"), wss.str().c_str(), father);
	node->AddAttribute( _T("name"), name);

}

void gkTimeOfDayManager::addColorValue( uint16 PARAMID, IRapidXmlAuthor& todAuthor, CRapidXmlAuthorNode* father, const TCHAR* name )
{
	gkStdStringstream wss;
	TODKeySequence::iterator it = m_listTODKeySeq.begin();
	for (it; it != m_listTODKeySeq.end(); ++it)
	{
		// push to float*
		float* key = (float*)(&(*it));
		
		wss << std::setprecision(3) << key[PARAMID] << _T(" ") << key[PARAMID + 1] << _T(" ") << key[PARAMID + 2] << _T(" ") << key[PARAMID + 3] << _T(" ");
	}

	CRapidXmlAuthorNode* node = todAuthor.createRapidXmlNode(_T("color"), wss.str().c_str(), father);
	node->AddAttribute( _T("name"), name);
}

#define ADD_TODFLOAT(_name)	(addFloatValue(_name, todAuthor, rootNode, _T(#_name)))
#define ADD_TODCOLOR(_name)	(addColorValue(_name, todAuthor, rootNode, _T(#_name)))


bool gkTimeOfDayManager::saveTODSequence( const TCHAR* relativePath )
{
	IRapidXmlAuthor todAuthor;
	todAuthor.initializeSceneWrite();

	CRapidXmlAuthorNode* rootNode = todAuthor.createRapidXmlNode(_T("TimeOfDay"));

	// add time [12/28/2011 Kaiming]
	rootNode->AddAttribute(_T("time"), m_fCurTime);
	// add tod key count
	rootNode->AddAttribute(_T("keycount"), (int)(m_listTODKeySeq.size()));
	ADD_TODFLOAT( KEYTIME );

	// add all key's, group by value
	ADD_TODFLOAT( HDRBRIGHTNESS );

	ADD_TODFLOAT( HDRCONSTACT );
	ADD_TODFLOAT( 	SSAOINTENSITY );
	ADD_TODCOLOR( CLSKYLIGHT );
	ADD_TODCOLOR(	CLGROUNDAMB );
	ADD_TODFLOAT( 	GROUNDAMBMIN );
	ADD_TODFLOAT( 	GROUNDAMBMAX );

	// SUN VARS
	ADD_TODCOLOR(	CLSUNLIGHT );
	ADD_TODFLOAT( 	SUNINTENSITY );
	ADD_TODFLOAT( 	SUNSPECINTENSITY );
	ADD_TODFLOAT( 	SUNSHAFTVISIBILITY );
	ADD_TODFLOAT( 	SUNSHAFTATTEN );
	ADD_TODFLOAT(   SUNSHAFTLENGTH );

	// SKYBOX VARS
	ADD_TODCOLOR(	CLZENITHBOTTOM );
	ADD_TODCOLOR(	CLZENITHTOP );
	ADD_TODFLOAT( 	ZENITHSHIFT );

	// SKYBOX 
	ADD_TODFLOAT(TURBIDITY);
	ADD_TODFLOAT(RAYLEIGH);
	ADD_TODFLOAT(MIECOEFFICENT);
	ADD_TODFLOAT(MIEDIRECTIONALG);

	// FOG VARS
	ADD_TODCOLOR(	CLFOGCOLOR );
	ADD_TODFLOAT( 	FOGDENSITY );
	ADD_TODFLOAT( 	FOGSTART );

	// HDR VARS
	ADD_TODFLOAT( 	BLOOMINTENSITY );
	ADD_TODCOLOR(	CLBLOOMCOLOR );

	ADD_TODFLOAT( 	STREAKINTENSITY );
	ADD_TODCOLOR(	CLSTREAKCOLOR );

	ADD_TODFLOAT( 	SATURATE );
	ADD_TODFLOAT( 	CONSTRACT );
	ADD_TODFLOAT( 	BRIGHTNESS );

	ADD_TODFLOAT( 	LEVELINPUTMIN );
	ADD_TODFLOAT( 	LEVEL );
	ADD_TODFLOAT( 	LEVELINPUTMAX );

	ADD_TODFLOAT( 	DOFBLURINTENSITY );
	ADD_TODFLOAT( 	DOFDISTANCE );
	ADD_TODFLOAT( 	DOFDISTANCENEAR );
	ADD_TODFLOAT( 	DOFFOCUSPLANE );
	ADD_TODFLOAT( 	DOFFOCUSDISTANCE );

	ADD_TODFLOAT(	LDRFAKEBRIGHTNESS );


	
	
	gkStdString absPath = gkGetGameRootDir();
	absPath = absPath + _T("\\") + relativePath;

	todAuthor.writeToFile(absPath.c_str());

	return true;
}

void gkTimeOfDayManager::readFloatValue( uint16 PARAMID, int count, const TCHAR* value, const TCHAR* name, const TCHAR* paraname )
{
 	GK_ASSERT( count <= m_listTODKeySeq.size());
 	gkStdStringstream iss(value);
 	if (!_tcsicmp(name, paraname))
 	{
 		for(int i=0; i < count; i++)
 		{
 			iss >> ((float*)(&(m_listTODKeySeq[i])))[PARAMID];
 		}		
 	}
}

void gkTimeOfDayManager::readColorValue( uint16 PARAMID, int count, const TCHAR* value, const TCHAR* name, const TCHAR* paraname )
{
	GK_ASSERT( count <= m_listTODKeySeq.size());
	gkStdStringstream iss(value);
	if (!_tcsicmp(name, paraname))
	{
		for(int i=0; i < count; i++)
		{
			iss >> ((float*)(&(m_listTODKeySeq[i])))[PARAMID] >> ((float*)(&(m_listTODKeySeq[i])))[PARAMID+1] >> ((float*)(&(m_listTODKeySeq[i])))[PARAMID+2] >> ((float*)(&(m_listTODKeySeq[i])))[PARAMID+3];
		}		
	}
}

#define READ_TODFLOAT(_name, _count, _value, _pname)	(readFloatValue(_name, _count, _value, _T(#_name), _pname))
#define READ_TODCOLOR(_name, _count, _value, _pname)	(readColorValue(_name, _count, _value, _T(#_name), _pname))

#define READ_TODFLOAT_FAST(_name) READ_TODFLOAT(_name, nCount, node->GetValue(), node->GetAttribute(_T("name")));
#define READ_TODCOLOR_FAST(_name) READ_TODCOLOR(_name, nCount, node->GetValue(), node->GetAttribute(_T("name")));


bool gkTimeOfDayManager::loadTODSequence( const TCHAR* relativePath, bool update /*= false*/ )
{
	bool ret = false;

	IRapidXmlParser todParser;
	todParser.initializeReading(relativePath);
	ret = gEnv->p3DEngine->getTimeOfDay()->loadTODSequence(todParser.getRootXmlNode(), update);
	todParser.finishReading();

	if (ret)
	{
		m_cachedTodFile = relativePath;
		gkNormalizePath(m_cachedTodFile);
	}

	return ret;
}

bool gkTimeOfDayManager::loadTODSequence( CRapidXmlParseNode* rootnode, bool update )
{
	float fTime = 0;
	int nCount = 0;

	if (!rootnode)
	{
		return false;
	}

	rootnode->GetAttribute(_T("time"), fTime);
	rootnode->GetAttribute(_T("keycount"), nCount);
  
    	
  	STimeOfDayKey key;
  	m_listTODKeySeq.assign(nCount, key);

	CRapidXmlParseNode* keynode = rootnode->getChildNode();
  
  	for (CRapidXmlParseNode* node = keynode; node; node = node->getNextSiblingNode())
  	{
 		
  		if (!_tcsicmp(node->GetName(), _T("float")))
  		{
  			READ_TODFLOAT_FAST(KEYTIME);//, nCount, node->GetValue(), node->GetAttribute(_T("name")));

  			READ_TODFLOAT_FAST(HDRBRIGHTNESS);
  			READ_TODFLOAT_FAST(HDRCONSTACT);
  			READ_TODFLOAT_FAST(SSAOINTENSITY);
  			READ_TODFLOAT_FAST(GROUNDAMBMIN);
  			READ_TODFLOAT_FAST(GROUNDAMBMAX);
  			READ_TODFLOAT_FAST(SUNINTENSITY);
  			READ_TODFLOAT_FAST(SUNSPECINTENSITY);
  			READ_TODFLOAT_FAST(SUNSHAFTVISIBILITY);
  			READ_TODFLOAT_FAST(SUNSHAFTATTEN);
  			READ_TODFLOAT_FAST(SUNSHAFTLENGTH);	

  			READ_TODFLOAT_FAST(ZENITHSHIFT);

			READ_TODFLOAT_FAST(TURBIDITY);
			READ_TODFLOAT_FAST(RAYLEIGH);
			READ_TODFLOAT_FAST(MIECOEFFICENT);
			READ_TODFLOAT_FAST(MIEDIRECTIONALG);

  			READ_TODFLOAT_FAST(FOGDENSITY);
  			READ_TODFLOAT_FAST(FOGSTART);
  			READ_TODFLOAT_FAST(BLOOMINTENSITY);
  			READ_TODFLOAT_FAST(STREAKINTENSITY);
  			READ_TODFLOAT_FAST(SATURATE);
  			READ_TODFLOAT_FAST(CONSTRACT);
  			READ_TODFLOAT_FAST(BRIGHTNESS);
  
  			READ_TODFLOAT_FAST(LEVELINPUTMIN);
  			READ_TODFLOAT_FAST(LEVEL);
  			READ_TODFLOAT_FAST(LEVELINPUTMAX);
  			READ_TODFLOAT_FAST(DOFBLURINTENSITY);
  			READ_TODFLOAT_FAST(DOFDISTANCE);
			READ_TODFLOAT_FAST(DOFDISTANCENEAR);
			READ_TODFLOAT_FAST(DOFFOCUSPLANE);
			READ_TODFLOAT_FAST(DOFFOCUSDISTANCE);  
			READ_TODFLOAT_FAST(LDRFAKEBRIGHTNESS);
  		}
  		if (!_tcsicmp(node->GetName(), _T("color")))
  		{
  			READ_TODCOLOR_FAST(CLSKYLIGHT);
  			READ_TODCOLOR_FAST(CLGROUNDAMB);
  			READ_TODCOLOR_FAST(CLSUNLIGHT);
  			READ_TODCOLOR_FAST(CLZENITHBOTTOM);
  			READ_TODCOLOR_FAST(CLZENITHTOP);
  			READ_TODCOLOR_FAST(CLFOGCOLOR);
  			READ_TODCOLOR_FAST(CLBLOOMCOLOR);
  			READ_TODCOLOR_FAST(CLSTREAKCOLOR);
  		}
  	}
  
  	m_fCurTime = fTime;
  	if (update)
  	{
  		setTime(fTime);
  	}
	

	return true;
}

void gkTimeOfDayManager::OnFileChange( const TCHAR* filename )
{
	if ( !_tcsicmp(filename, m_cachedTodFile.c_str()))
	{
		loadTODSequence( m_cachedTodFile.c_str(), true );
	}
}

bool gkTimeOfDayManager::bindSunFocus( IGameObject* obj )
{
	m_sunFocus = obj;
	return true;
}





