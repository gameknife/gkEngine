//////////////////////////////////////////////////////////////////////////
//
// yikaiming (C) 2015
// gkENGINE Source File 
//
// Name:   	ddsCompiler.h
// Desc:	
// 	
// 
// Author:  gameKnife
// Date:	2015/8/16
// 
//////////////////////////////////////////////////////////////////////////

#ifndef _ddsCompiler_h_
#define _ddsCompiler_h_

#include "icompiler.h"

class ddsCompiler : public ICompiler
{
public:
	ddsCompiler(void);
	~ddsCompiler(void);

	virtual bool encode(const char* filename, const char* cfg);

	virtual bool writeFile(const char* filename, const char* cfg);

	virtual bool supportInput(const char* ext);

	virtual bool supportOutput(const char* ext);

	virtual const char* getName();

};

#endif

