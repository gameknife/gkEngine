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
// Name:   	IXmlUtil.h
// Desc:	对rapidxml的封装，xml解析和书写接口
// 	
// 
// Author:  yikaiming
// Date:	2013/6/1	
// 
//////////////////////////////////////////////////////////////////////////

#ifndef IXmlUtil_h__
#define IXmlUtil_h__

#include "rapidxml.hpp"
#include "rapidxml_print.hpp"

#include "gkPlatform.h"
#include "IResFile.h"

#ifdef _3DMAX
#include "max.h"
#define ColorF Color
#endif


struct IRapidXmlNode;
struct CObjectNode;
struct CRapidXmlAuthorNode;
struct CRapidXmlParseNode;

	/**
	 @brief xml文件书写器
	 @remark 
	*/
struct IRapidXmlAuthor
{
	IRapidXmlAuthor(void) {}
	virtual ~IRapidXmlAuthor(void);

	// whole scene processing
	void initializeSceneWrite();

	int writeToFile(const TCHAR* filename);
	// node info 
	CObjectNode* createObjectNode(IRapidXmlNode* parent = NULL);
	// node info 
	CRapidXmlAuthorNode* createRapidXmlNode(const TCHAR* name, const TCHAR* info = 0, IRapidXmlNode* parent = NULL);

	std::vector<IRapidXmlNode*> m_creatednodes;
	rapidxml::xml_document<TCHAR> *m_xmlDoc;
};
	/**
	 @brief xml文件解析器
	 @remark 
	*/
struct IRapidXmlParser
{
	IRapidXmlParser(void):m_xmlDoc(NULL),m_file(NULL) {}
	virtual ~IRapidXmlParser(void);

	// whole scene processing
	void initializeReading(const TCHAR* filename);
	int finishReading();

	// node info 
	CRapidXmlParseNode* getRootXmlNode(const TCHAR* name = NULL);
	CRapidXmlParseNode* createRapidXmlNode(rapidxml::xml_node<TCHAR>* node);

	std::vector<IRapidXmlNode*> m_creatednodes;
	rapidxml::xml_document<TCHAR> *m_xmlDoc;
	IResFile* m_file;
};

	/**
	 @brief xmlnode的抽象
	 @remark 
	*/
struct IRapidXmlNode
{
public:

	IRapidXmlNode(rapidxml::xml_node<TCHAR>* node, IRapidXmlAuthor* author, IRapidXmlParser* parser) : m_xmlNode(node), m_author(author), m_parser(parser)
	{
	}

	virtual ~IRapidXmlNode() {}

#ifdef OS_WIN32
	// author set
	virtual void AddAttribute(const TCHAR* name, const wchar_t* value)
	{
#ifdef _UNICODE
		TCHAR* rapidLifeString = m_xmlNode->document()->allocate_string(value);

#else
		CHAR buffer[MAX_PATH];
		WideCharToMultiByte( CP_ACP, 0, value, -1, buffer, MAX_PATH, NULL, NULL );
		TCHAR* rapidLifeString = m_xmlNode->document()->allocate_string(buffer);
#endif
		rapidxml::xml_attribute<TCHAR>* att = m_xmlNode->document()->allocate_attribute(name, rapidLifeString);
		m_xmlNode->append_attribute(att);
	}

#endif

	virtual void AddAttribute(const TCHAR* name, const char* value)
	{
#ifdef _UNICODE
		TCHAR buffer[MAX_PATH];
		MultiByteToWideChar( CP_ACP, 0, value, -1, buffer, MAX_PATH );
		TCHAR* rapidLifeString = m_xmlNode->document()->allocate_string(buffer);
#else
		TCHAR* rapidLifeString = m_xmlNode->document()->allocate_string(value);
#endif
		
		rapidxml::xml_attribute<TCHAR>* att = m_xmlNode->document()->allocate_attribute(name, rapidLifeString);
		m_xmlNode->append_attribute(att);
	}

	virtual void AddAttribute(const TCHAR* name, bool trueorfalse)
	{
		TCHAR* rapidLifeString = NULL;
		if (trueorfalse)
			rapidLifeString = m_xmlNode->document()->allocate_string(_T("true"));
		else
			rapidLifeString = m_xmlNode->document()->allocate_string(_T("false"));
		rapidxml::xml_attribute<TCHAR>* att = m_xmlNode->document()->allocate_attribute(name, rapidLifeString);
		m_xmlNode->append_attribute(att);
	}

	virtual void AddAttribute(const TCHAR* name, float value)
	{
		TCHAR buffer[256];
		_stprintf_s(buffer, 256, _T("%.6f"), value);
		TCHAR* rapidLifeString = m_xmlNode->document()->allocate_string(buffer);
		rapidxml::xml_attribute<TCHAR>* att = m_xmlNode->document()->allocate_attribute(name, rapidLifeString);
		m_xmlNode->append_attribute(att);
	}

	virtual void AddAttribute(const TCHAR* name, int value)
	{
		TCHAR buffer[256];
		_stprintf_s(buffer, 256, _T("%d"), value);
		TCHAR* rapidLifeString = m_xmlNode->document()->allocate_string(buffer);
		rapidxml::xml_attribute<TCHAR>* att = m_xmlNode->document()->allocate_attribute(name, rapidLifeString);
		m_xmlNode->append_attribute(att);
	}


	// parser get

	int GetAttribute(const TCHAR* name, int& value, bool sensible = false)
	{
		rapidxml::xml_attribute<TCHAR>* att = m_xmlNode->first_attribute(name, 0, sensible);
		if ( att )
		{
			value = _ttoi( att->value() );
		}
		return value;
	}

	float GetAttribute(const TCHAR* name, float& value, bool sensible = false)
	{
		rapidxml::xml_attribute<TCHAR>* att = m_xmlNode->first_attribute(name, 0, sensible);
		if ( att )
		{
			double dvalue = _tstof( att->value() );
			value = static_cast<float>(dvalue);
		}
		return value;
	}

	bool GetAttribute(const TCHAR* name, bool& value, bool sensible = false)
	{
		rapidxml::xml_attribute<TCHAR>* att = m_xmlNode->first_attribute(name, 0, sensible);
		if ( att )
		{
			if (_tcsicmp(att->value(), _T("false")))
			{
				value = true;
			}
			else
			{
				value = false;
			}
		}

		return value;
	}

	TCHAR* GetAttribute(const TCHAR* name, TCHAR* value, bool sensible = false)
	{
		rapidxml::xml_attribute<TCHAR>* att = m_xmlNode->first_attribute(name, 0, sensible);
		if ( att )
		{
			value = att->value();
		}
		return value;
	}

	TCHAR* GetAttribute(const TCHAR* name)
	{
		rapidxml::xml_attribute<TCHAR>* att = m_xmlNode->first_attribute(name, 0, false);
		if ( att )
		{
			return att->value();
		}
		return NULL;
	}
#ifndef _3DSMAX

	virtual void AddAttribute(const TCHAR* name, const ColorF& color)
	{
		TCHAR buffer[256];
		_stprintf_s(buffer, 256, _T("%.5f %.5f %.5f"), color.r, color.g, color.b, color.a);
		TCHAR* rapidLifeString = m_xmlNode->document()->allocate_string(buffer);
		rapidxml::xml_attribute<TCHAR>* att = m_xmlNode->document()->allocate_attribute(name, rapidLifeString);
		m_xmlNode->append_attribute(att);
	}

	virtual void AddAttribute(const TCHAR* name, const Vec3& vec)
	{
		TCHAR buffer[256];
		_stprintf_s(buffer, 256, _T("%.5f %.5f %.5f"), vec.x, vec.y, vec.z);
		TCHAR* rapidLifeString = m_xmlNode->document()->allocate_string(buffer);
		rapidxml::xml_attribute<TCHAR>* att = m_xmlNode->document()->allocate_attribute(name, rapidLifeString);
		m_xmlNode->append_attribute(att);
	}

	Vec3& GetAttribute(const TCHAR* name, Vec3& vec, bool sensible = false)
	{
		rapidxml::xml_attribute<TCHAR>* att = m_xmlNode->first_attribute(name, 0, sensible);
		if ( att )
		{
#ifdef _UNICODE
			gkStdStringstream wss( att->value() );
#else
			std::stringstream wss( att->value() );
#endif
			wss >> vec.x >> vec.y >> vec.z;
		}
		return vec;
	}

	virtual void AddAttribute(const TCHAR* name, const Quat& quat)
	{
		TCHAR buffer[256];
#ifdef MAX_API_NUM
		_stprintf_s(buffer, 256, _T("%.5f %.5f %.5f %.5f"), quat.x, quat.y, quat.z, quat.w);
#else
		_stprintf_s(buffer, 256, _T("%.5f %.5f %.5f %.5f"), quat.v.x, quat.v.y, quat.v.z, quat.w);
#endif
		TCHAR* rapidLifeString = m_xmlNode->document()->allocate_string(buffer);
		rapidxml::xml_attribute<TCHAR>* att = m_xmlNode->document()->allocate_attribute(name, rapidLifeString);
		m_xmlNode->append_attribute(att);
	}

	Quat& GetAttribute(const TCHAR* name, Quat& quat, bool sensible = false)
	{
		rapidxml::xml_attribute<TCHAR>* att = m_xmlNode->first_attribute(name, 0, sensible);
		if ( att )
		{
#ifdef _UNICODE
			gkStdStringstream wss( att->value() );
#else
			std::stringstream wss( att->value() );
#endif

#ifdef MAX_API_NUM
			wss >> quat.x >> quat.y >> quat.z >> quat.w;
#else
			wss >> quat.v.x >> quat.v.y >> quat.v.z >> quat.w;
#endif
		}
		return quat;
	}

	ColorF& GetAttribute(const TCHAR* name, ColorF& color, bool sensible = false)
	{
		rapidxml::xml_attribute<TCHAR>* att = m_xmlNode->first_attribute(name, 0, sensible);
		if ( att )
		{
#ifdef _UNICODE
			gkStdStringstream wss( att->value() );
#else
			std::stringstream wss( att->value() );
#endif
			wss >> color.r >> color.g >> color.b >> color.a;
		}
		return color;
	}
#endif
	Vec4& GetAttribute(const TCHAR* name, Vec4& vec4, bool sensible = false)
	{
		rapidxml::xml_attribute<TCHAR>* att = m_xmlNode->first_attribute(name, 0, sensible);
		if ( att )
		{
#ifdef _UNICODE
			gkStdStringstream wss( att->value() );
#else
			std::stringstream wss( att->value() );
#endif
			wss >> vec4.x >> vec4.y >> vec4.z >> vec4.w;
		}
		return vec4;
	}
 
	TCHAR* GetName()
	{
		return m_xmlNode->name();
	}

	TCHAR* GetValue()
	{
		return m_xmlNode->value();
	}

	Vec3& GetTranslation(Vec3& translation)
	{
		rapidxml::xml_node<TCHAR>* nodeTM = m_xmlNode->first_node(_T("NodeTM"));
		if (nodeTM)
		{
			rapidxml::xml_node<TCHAR>* transNode = nodeTM->first_node(_T("Translation"));
			if (transNode)
			{
#ifdef _UNICODE
				gkStdStringstream wss( transNode->value() );
#else
				std::stringstream wss( transNode->value() );
#endif
				wss >> translation.x >> translation.y >> translation.z;
			}
		}

		return translation;
	}

	Quat& GetOrientation(Quat& orientation)
	{
		rapidxml::xml_node<TCHAR>* nodeTM = m_xmlNode->first_node(_T("NodeTM"));
		if (nodeTM)
		{
			rapidxml::xml_node<TCHAR>* transNode = nodeTM->first_node(_T("Rotation"));
			if (transNode)
			{
#ifdef _UNICODE
				gkStdStringstream wss( transNode->value() );
#else
				std::stringstream wss( transNode->value() );
#endif

#ifndef MAX_API_NUM
				wss >> orientation.v.x >> orientation.v.y >> orientation.v.z >> orientation.w;
#else
				wss >> orientation.x >> orientation.y >> orientation.z >> orientation.w;
#endif
			}
		}

		return orientation;
	}

	Vec3& GetScale(Vec3& scale)
	{
		rapidxml::xml_node<TCHAR>* nodeTM = m_xmlNode->first_node(_T("NodeTM"));
		if (nodeTM)
		{
			rapidxml::xml_node<TCHAR>* transNode = nodeTM->first_node(_T("Scale"));
			if (transNode)
			{
#ifdef _UNICODE
				gkStdStringstream wss( transNode->value() );
#else
				std::stringstream wss( transNode->value() );
#endif
				wss >> scale.x >> scale.y >> scale.z;
			}
		}

		return scale;
	}


	rapidxml::xml_node<TCHAR>* m_xmlNode;
	IRapidXmlAuthor* m_author;
	IRapidXmlParser* m_parser;
};

struct CRapidXmlAuthorNode : public IRapidXmlNode
{
	CRapidXmlAuthorNode(rapidxml::xml_node<TCHAR>* node, IRapidXmlAuthor* author) : IRapidXmlNode(node, author, NULL)
	{

	}

	~CRapidXmlAuthorNode() {}

	CRapidXmlAuthorNode* createChildNode(const TCHAR* name, const TCHAR* info = NULL)
	{
		return (m_author->createRapidXmlNode(name, info, this));
	}
};

struct CRapidXmlParseNode : public IRapidXmlNode
{
	CRapidXmlParseNode(rapidxml::xml_node<TCHAR>* node, IRapidXmlParser* parser) : IRapidXmlNode(node, NULL, parser)
	{

	}

	~CRapidXmlParseNode() {}

	CRapidXmlParseNode* getChildNode(const TCHAR* name = NULL, bool sensitive = false)
	{
		rapidxml::xml_node<TCHAR>* childnode = m_xmlNode->first_node(name, 0, sensitive);
		if (childnode)
		{
			return (m_parser->createRapidXmlNode(childnode));
		}
		return NULL;
	}

	CRapidXmlParseNode* getNextSiblingNode(const TCHAR* name = NULL, bool sensitive = false)
	{
		rapidxml::xml_node<TCHAR>* childnode = m_xmlNode->next_sibling(name, 0, sensitive);
		if (childnode)
		{
			return (m_parser->createRapidXmlNode(childnode));
		}
		return NULL;
	}
};


struct CObjectNode : public IRapidXmlNode
{
	CObjectNode(rapidxml::xml_node<TCHAR>* node, IRapidXmlAuthor* author) : IRapidXmlNode(node, author, NULL)
	{
		if ( m_xmlNode)
		{
			// allocate subnode "NodeTM" for this [12/3/2011 Kaiming]
			m_xmlSubNode_TM = m_xmlNode->document()->allocate_node(rapidxml::node_element, _T("NodeTM"));
			m_xmlNode->append_node(m_xmlSubNode_TM);
		}
	}

	virtual ~CObjectNode() {}

	virtual void SetTranslation( float x, float y, float z)
	{
		TCHAR buf[MAX_PATH];
		_stprintf_s( buf, MAX_PATH, _T("%0.6f %0.6f %0.6f"), x, y, z );

		TCHAR* rapidLifeString = m_xmlNode->document()->allocate_string(buf);

		rapidxml::xml_node<TCHAR>* trans;
		trans = m_xmlSubNode_TM->document()->allocate_node(rapidxml::node_element, _T("Translation"), rapidLifeString);
		m_xmlSubNode_TM->append_node(trans);
	}
	virtual void SetRotation( float x, float y, float z, float w)
	{
		TCHAR buf[MAX_PATH];
		_stprintf_s( buf, MAX_PATH, _T("%0.6f %0.6f %0.6f %0.6f"), x, y, z, w );

		TCHAR* rapidLifeString = m_xmlNode->document()->allocate_string(buf);

		rapidxml::xml_node<TCHAR>* trans;
		trans = m_xmlSubNode_TM->document()->allocate_node(rapidxml::node_element, _T("Rotation"), rapidLifeString);
		m_xmlSubNode_TM->append_node(trans);
	}
	virtual void SetScale( float kx, float ky, float kz )
	{
		TCHAR buf[MAX_PATH];
		_stprintf_s( buf, MAX_PATH, _T("%f %f %f %f %f %f %f %f"),  kx, ky, kz, .0f, .0f, .0f, .0f );

		TCHAR* rapidLifeString = m_xmlNode->document()->allocate_string(buf);

		rapidxml::xml_node<TCHAR>* trans;
		trans = m_xmlSubNode_TM->document()->allocate_node(rapidxml::node_element, _T("Scale"), rapidLifeString);
		m_xmlSubNode_TM->append_node(trans);
	}

	rapidxml::xml_node<TCHAR>* m_xmlSubNode_TM;
};

// whole scene processing
inline void IRapidXmlAuthor::initializeSceneWrite()
{
	m_xmlDoc = new rapidxml::xml_document<TCHAR>;
}

inline int IRapidXmlAuthor::writeToFile(const TCHAR* filename)
{
#ifdef OS_WIN32
	FILE* fp = 0;
	errno_t err = _tfopen_s( &fp, filename, _T("w") );
	if ( !err && fp )
	{
		//TCHAR buffer[163840];                      // You are responsible for making the buffer large enough!
		TCHAR* buffer;
		buffer = new TCHAR[1024 * 5000];			 // allocate 5MB for big scene.

		TCHAR *end = print(buffer, *m_xmlDoc, 0);      // end contains pointer to character after last printed character
		*end = 0;                               // Add string terminator after XML

		_ftprintf_s( fp, _T("%s"), buffer );

		delete[] buffer;

		fclose(fp);
	}

	delete m_xmlDoc;
#else


#endif
	return 1;
}
//virtual void processSceneRead() =0;

// node info 
inline CObjectNode* IRapidXmlAuthor::createObjectNode(IRapidXmlNode* parent) {
	if (m_xmlDoc)
	{
		rapidxml::xml_node<TCHAR>* node = m_xmlDoc->allocate_node(rapidxml::node_element, _T("gkObject"));
		// add as child
		if (parent)
		{
			parent->m_xmlNode->append_node(node);
		}
		else
		{
			m_xmlDoc->append_node(node);
		}

		// add this node to list
		CObjectNode* authornodes = new CObjectNode(node, this);

		m_creatednodes.push_back(authornodes);

		return authornodes;
	}
	
	return NULL;
}

// node info 
inline CRapidXmlAuthorNode* IRapidXmlAuthor::createRapidXmlNode(const TCHAR* name, const TCHAR* info, IRapidXmlNode* parent) {
	if (m_xmlDoc)
	{
		rapidxml::xml_node<TCHAR>* node;
		if (info != 0)
		{
			TCHAR* rapidLifeString = m_xmlDoc->allocate_string(info);
			node = m_xmlDoc->allocate_node(rapidxml::node_element, name, rapidLifeString);
		}
		else
		{
			node = m_xmlDoc->allocate_node(rapidxml::node_element, name);
		}		

		// add as child
		if (parent)
		{
			parent->m_xmlNode->append_node(node);
		}
		else
		{
			m_xmlDoc->append_node(node);
		}

		// add this node to list
		CRapidXmlAuthorNode* authornodes = new CRapidXmlAuthorNode(node, this);

		m_creatednodes.push_back(authornodes);

		return authornodes;
	}
	
	return NULL;
}

// whole scene processing
inline void IRapidXmlParser::initializeReading(const TCHAR* filename)
{
	m_xmlDoc = new rapidxml::xml_document<TCHAR>;

	bool bSearch = false;
	if (gEnv->pSystem->IsEditor())
	{
		bSearch = true;
	}
	m_file = gEnv->pFileSystem->loadResFile( filename, bSearch );

	if (m_file)
	{

		
#ifdef _UNICODE
		int size = m_file->Size() / sizeof(char);
		TCHAR* textbuf = new TCHAR[size + 1];
		MultiByteToWideChar( CP_ACP, 0, (char*)m_file->DataPtr(), m_file->Size(), textbuf, size + 1 );
		textbuf[size] = 0;

		TCHAR* rapidlifebuf = m_xmlDoc->allocate_string( textbuf );
		m_xmlDoc->parse<0>(rapidlifebuf);
		SAFE_DELETE_ARRAY( textbuf );
#else
        int size = m_file->Size() / sizeof(char);
		char* textbuf = new char[ size + 1];
        
        memcpy(textbuf, (char*)m_file->DataPtr(), size );
        textbuf[size] = 0;
        
		char* rapidlifebuf = m_xmlDoc->allocate_string( textbuf, m_file->Size() + 1 );
		m_xmlDoc->parse<0>(rapidlifebuf);
        
        SAFE_DELETE_ARRAY( textbuf );
#endif
	}
	else
	{
		gkLogError(_T("parsing xml [%s] failed..."), filename);
	}
	//getRapidWideBufferFromFile(filename, buf);

}
inline int IRapidXmlParser::finishReading()
{
	delete m_xmlDoc;
	gEnv->pFileSystem->closeResFile(m_file);
	//delete[] buf;

	return true;
}

// node info 
inline CRapidXmlParseNode* IRapidXmlParser::createRapidXmlNode(rapidxml::xml_node<TCHAR>* node)
{
	CRapidXmlParseNode* parsernode = new CRapidXmlParseNode(node, this);
	m_creatednodes.push_back(parsernode);
	return parsernode;
}

inline CRapidXmlParseNode* IRapidXmlParser::getRootXmlNode(const TCHAR* name)
{
	rapidxml::xml_node<TCHAR>* node = m_xmlDoc->first_node(name, 0, false);
	if (node)
	{
		CRapidXmlParseNode* parsernode = new CRapidXmlParseNode(node, this);
		m_creatednodes.push_back(parsernode);

		return parsernode;
	}

	return NULL;
}

inline IRapidXmlParser::~IRapidXmlParser(void)
{
	for(uint32 i=0; i<m_creatednodes.size(); ++i)
		delete m_creatednodes[i];
}

inline IRapidXmlAuthor::~IRapidXmlAuthor(void) 
{
	for(uint32 i=0; i<m_creatednodes.size(); ++i)
		delete m_creatednodes[i];
}

inline int getRapidWideBufferFromFile(const TCHAR* filename, TCHAR* &buf)
{
#ifdef OS_WIN32
	// OPEN FILE
	char szFilename[MAX_PATH];
#ifdef _UNICODE
	WideCharToMultiByte(CP_ACP, 0,  filename, -1, szFilename, MAX_PATH, NULL, NULL);
#else
	strcpy_s(szFilename, filename);
#endif

	FILE* file = 0;
	errno_t err = fopen_s( &file, szFilename, "rb" );
	if ( err || !file )
		return 0;

	// Get the file size, so we can pre-allocate the string. HUGE speed impact.
	long length = 0;
	fseek( file, 0, SEEK_END );
	length = ftell( file );
	fseek( file, 0, SEEK_SET );

	// Strange case, but good to handle up front.
	if ( length <= 0 )
	{
		//gkLogMessage(_T("rapidxml::load file [ %s ] failed."), filename);
		return false;
	}

	char* szbuf = new char[ length+1 ];
	szbuf[0] = 0;

#ifdef  _UNICODE
	//wcscpy_s(buf, length+1, (WCHAR*)(file->_base));
#else
	//fscanf( file, "%s\0", szbuf);
#endif
	if ( fread( szbuf, length, 1, file ) != 1 ) {
		delete [] szbuf;
		return false;
	}

	//int end = strlen(buf);
	szbuf[length] = 0;

	fclose(file);

	buf = new TCHAR[ length+1 ];

#ifdef _UNICODE
	MultiByteToWideChar( CP_ACP, 0, szbuf, -1, buf, length + 1 );
#else
	strcpy(buf, szbuf);
#endif

	delete[] szbuf;
#endif
    
    return true;
}


inline int getRapidBufferFromFile(const CHAR* filename, CHAR* &buf)
{
#ifdef OS_WIN32
	// OPEN FILE
	FILE* file = 0;
	errno_t err = fopen_s( &file, filename, "rb" );
	if ( err || !file )
		return 0;

	// Get the file size, so we can pre-allocate the string. HUGE speed impact.
	long length = 0;
	fseek( file, 0, SEEK_END );
	length = ftell( file );
	fseek( file, 0, SEEK_SET );

	// Strange case, but good to handle up front.
	if ( length <= 0 )
	{
		//gkLogMessage(_T("rapidxml::load file [ %s ] failed."), filename);
		return false;
	}

	buf = new CHAR[ length+1 ];
	buf[0] = 0;

	#ifdef  _UNICODE
	//wcscpy_s(buf, length+1, (WCHAR*)(file->_base));
	#else
	//fscanf( file, "%s\0", buf);
	#endif
  	if ( fread( buf, length, 1, file ) != 1 ) {
  		delete [] buf;
  		return false;
	}

	//int end = strlen(buf);
	buf[length] = 0;

	fclose(file);

#endif
    return true;
}

#endif