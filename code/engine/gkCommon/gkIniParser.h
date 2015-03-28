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
// Name:   	gkIniParser.h
// Desc:	精简的跨平台ini解析器实现，使用stl
// 	
// Author:  Kaiming
// Date:	2012/7/3
// Modify:	
// 
//////////////////////////////////////////////////////////////////////////

#ifndef _gkIniParser_h_
#define _gkIniParser_h_

#include "gkPlatform.h"
#include "IResFile.h"

class gkIniParser
{
public:
	typedef std::map<gkStdString, gkStdString>             INIMAP;
	typedef std::map<gkStdString, gkStdString>::iterator   INIMAP_IT;

	gkIniParser(const TCHAR* filename) { m_filename = filename; gkNormalizeResName(m_filename); }
	~gkIniParser() {}
	void Parse();
	// fetch value according to section and key .
	gkStdString FetchValue(const TCHAR* section,const TCHAR* key);

private:
	INIMAP  dict;    //dictionary!
	gkStdString m_filename;
	
};

//-------------------------------------------------------------------------------
inline void::gkIniParser::Parse()
{
	bool bSearch = false;
	if (gEnv->pSystem->IsEditor())
	{
		bSearch = true;
	}

	IResFile* file = gEnv->pFileSystem->loadResFile( m_filename.c_str() , bSearch );

	if ( file )
	{
		dict.clear();

		//////////////////////////////////////////////////////////////////////////
		// change CRLF -> LF

		std::string copyout((char*)file->DataPtr());

		size_t pos = 0;
		pos = copyout.find_first_of("\r", pos);
		while (std::string::npos != pos)
		{
			copyout.replace(pos, 2, "\n");
			pos = copyout.find_first_of("\r", pos + 1);
		}
		

#ifdef _UNICODE
		int size = file->Size() / sizeof(char);
		TCHAR* textbuf = new TCHAR[size + 1];
		MultiByteToWideChar(CP_ACP, 0, copyout.c_str(), copyout.length(), textbuf, size + 1);
		textbuf[size] = 0;

		gkStdStringstream ss( textbuf );
		delete[] textbuf;
#else
		gkStdStringstream ss(copyout.c_str());
#endif

		gkStdString  strline;  //saves per line from INI file 
		gkStdString  strSection ;  //saves section+key
		const gkStdString   jointor = _T("__@@__");  // a map key may like this:section__@@__key;
		while (std::getline(ss,strline))
		{

			if (!strline.empty() && !(strline.at(0) == _T(';')))
			{
				gkStdString::size_type beginPos = strline.find(_T('['));
				gkStdString::size_type endPos = strline.rfind(_T(']'));
				gkStdString::size_type keyPos = strline.find(_T('=')); 

				if ( beginPos != gkStdString::npos && endPos != gkStdString::npos)
				{
					strSection = strline.substr(beginPos+1 ,endPos-beginPos-1);
					strSection += jointor;
				}
				if ( keyPos != gkStdString::npos)
				{
					gkStdString strKey = strline.substr(0,keyPos);;

					// trim the space
					gkStdString::size_type strimpos = strKey.find_last_not_of(_T(' '));
					if ( strimpos != gkStdString::npos )
					{
						strKey = strKey.substr(0,strimpos + 1);
					}

					gkStdString strValue = strline.substr(keyPos+1,gkStdString::npos);
					// trim the \n

// 					strimpos = strValue.find_last_not_of(_T('\r'));
// 					if (strimpos != gkStdString::npos)
// 					{
// 						strValue = strValue.substr(0, strimpos);
// 					}

					strimpos = strValue.find_last_not_of(_T('\n'));
					if ( strimpos != gkStdString::npos )
					{
						strValue = strValue.substr(0, gkStdString::npos);
						//strValue = strValue.substr(0, )
					}

					// trim the space
					strimpos = strValue.find_first_not_of(_T(' '));
					if ( strimpos != gkStdString::npos )
					{
						strValue = strValue.substr(strimpos, gkStdString::npos);
					}

					strimpos = strValue.find_last_not_of(_T(' '));
					if ( strimpos != gkStdString::npos )
					{
						strValue = strValue.substr(0, strimpos + 1);
					}



					strKey = strSection + strKey;
					dict.insert(std::make_pair(strKey,strValue));
				}
			}
		}
	}

	gEnv->pFileSystem->closeResFile(file);


}

//-------------------------------------------------------------------------------
inline gkStdString gkIniParser::FetchValue(const TCHAR* section,const TCHAR* key)
{
	const gkStdString   jointor = _T("__@@__");
	gkStdString searchKey ;
	searchKey = section + jointor + key;
	INIMAP_IT it = dict.find(searchKey);
	if ( it != dict.end())
	{
		return it->second;
	}
	else
		return _T("");

}

#endif


