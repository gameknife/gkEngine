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
// Name:   	gkVtxMovieListener.h
// Desc:	这个类比较尴尬，看看以后有没有可能直接不要让外界接触vtx
//
// 描述:	
// 
// Author:  Kaiming-Desktop
// Date:	2011/04/17
// Modify:	2011/4/17
// 
//////////////////////////////////////////////////////////////////////////
#pragma once
#include "gkPrerequisites.h"
//#include "vtxScriptParameters.h"
//#include "vtxMovie.h"
//#include "vtxScriptCallbackListener.h"

#if 0//ndef WIN64

class gkVtxMovieListener : 
	public vtx::Movie::Listener, 
	public vtx::ScriptCallbackListener
{
public:
	gkVtxMovieListener();
	~gkVtxMovieListener();

	vtx::ScriptParam scriptCallback(const vtx::String& callback_name, const vtx::ScriptParamList& args);
	bool loadingCompleted(vtx::Movie* movie);
	bool loadingFailed(vtx::Movie* movie);

	// you must implement that
	virtual void functionCallFromAS3( const std::string& callback_name ) = 0;
	virtual void callFunctionInAS3( const std::string& callback_name,  int argc = 0, float argv[] = 0 );

private:
	vtx::Movie* m_pAttachedMovie;
};

#endif