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



/**
  @file TangentSpaceViewer.h
  
  @author Kaiming

  更改日志 history
  ver:1.0
   
 */

#ifndef TangentSpaceViewer_h__
#define TangentSpaceViewer_h__

#include "IGame.h"
#include "IInputManager.h"

class TangentSpaceViewer : 
	public IGame, 
	public IInputEventListener
{
public:
	TangentSpaceViewer(void);
	~TangentSpaceViewer(void);

	virtual bool OnInit();

	virtual bool OnDestroy();

	virtual bool OnUpdate();

	void UpdateGUI();

	void CreateScene();

	virtual bool OnLevelLoaded();

	virtual bool OnInputEvent( const SInputEvent &event );

	bool HandleFreeCam( const SInputEvent &input_event );

private:
	struct IGameObject* mainModel;
	float m_camDist;
	float m_camHeight;
	struct ITerrianSystem* terrian;
};

#endif



