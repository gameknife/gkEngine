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



//
//  TestCase_StaticHeader.h
//  TestCases
//
//  Created by gameKnife on 13-5-5.
//  Copyright (c) 2013骞gameKnife. All rights reserved.
//

#ifndef TestCases_TestCase_StaticLoader_h
#define TestCases_TestCase_StaticLoader_h

#include "TestCaseFramework.h"

#include "Testcase_LoadCharacter.cpp"
#include "TestCase_LoadInDoorScene.cpp"
#include "TestCase_LoadOutDoorScene.cpp"
#include "TestCase_ThreadTest.cpp"
#include "TestCase_LoadStaticGeo.cpp"
#include "TestCase_ArtAnatomy.cpp"
#include "TestCase_CharacterAnimation.cpp"
#include "TestCase_TrackBusTest.cpp"
#include "TestCase_LoadScene.cpp"

void load_static_testcases()
{
    //g_TestCase_LoadCharacter.OnInit();
    //gkLogMessage( g_TestCase_LoadCharacter.OnInit() );
    
    g_cateTestCases[eTcc_Rendering].push_back( &g_TestCase_InDoorRendering);
    g_cateTestCases[eTcc_Rendering].push_back( &g_TestCase_OutDoorRendering );
    g_cateTestCases[eTcc_Loading].push_back( &g_TestCase_LoadScene );
    g_cateTestCases[eTcc_Loading].push_back( &g_TestCase_LoadCharacter );
    g_cateTestCases[eTcc_Loading].push_back( &g_TestCase_LoadStaticGeo );
	g_cateTestCases[eTcc_Animation].push_back( &g_TestCase_CharacterAnimation );
	g_cateTestCases[eTcc_Animation].push_back( &g_TestCase_TrackBusTest );
}

#endif
