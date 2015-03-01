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
// Name:   	IGameExporter.h
// Desc:		
// 
// Author:  Kaiming-Desktop
// Date:	2011 /9/4
// Modify:	2011 /9/4
// 
//////////////////////////////////////////////////////////////////////////

#ifndef _IGAMEEXPORTER_H_
#define _IGAMEEXPORTER_H_

#include "system/gkMaxPort.H"
#include "impexp.h"
#include "IGame.h"
#include "IGameObject.h"
#include "IGameProperty.h"
#include "IGameControl.h"
#include "IGameModifier.h"
#include "IConversionManager.h"
#include "IGameError.h"
#include "IGameFX.h"

#include "Ixmlutil.h"

#define BEZIER	0
#define TCB		1
#define LINEAR	2
#define SAMPLE	3

	class IGameExporter : public SceneExport, public IRapidXmlAuthor {
	public:

		static IGameScene * pIgame;

		static HWND hParams;

		// author nodes [12/4/2011 Kaiming]
		CRapidXmlAuthorNode* authorSceneNode;
		CRapidXmlAuthorNode* authorsceneInfo;
		CRapidXmlAuthorNode* authormatlistNode;

		int curNode;

		int staticFrame;
		int framePerSample;
		BOOL exportGeom;
		BOOL exportNormals;
		BOOL exportVertexColor;
		BOOL exportControllers;
		BOOL exportFaceSmgp;
		BOOL exportTexCoords;
		BOOL exportMappingChannel;
		BOOL exportConstraints;
		BOOL exportMaterials;
		BOOL exportSplines;
		BOOL exportModifiers;
		BOOL exportSkin;
		BOOL exportGenMod;
		BOOL forceSample;
		BOOL splitFile;
		BOOL exportQuaternions;
		BOOL exportObjectSpace;
		BOOL exportRelative;
		BOOL exportNormalsPerFace;
		int cS;
		int exportCoord;
		bool showPrompts;
		bool exportSelected;

		TSTR fileName;
		TSTR splitPath;

		// kaiming add, for sync scene [9/4/2011 Kaiming-Desktop]
		TSTR syncfilename;

		float igameVersion, exporterVersion;



		int				ExtCount();					// Number of extensions supported
		const TCHAR *	Ext(int n);					// Extension #n (i.e. "3DS")
		const TCHAR *	LongDesc();					// Long ASCII description (i.e. "Autodesk 3D Studio File")
		const TCHAR *	ShortDesc();				// Short ASCII description (i.e. "3D Studio")
		const TCHAR *	AuthorName();				// ASCII Author name
		const TCHAR *	CopyrightMessage();			// ASCII Copyright message
		const TCHAR *	OtherMessage1();			// Other message #1
		const TCHAR *	OtherMessage2();			// Other message #2
		unsigned int	Version();					// Version number * 100 (i.e. v3.01 = 301)
		void			ShowAbout(HWND hWnd);		// Show DLL's "About..." box

		BOOL SupportsOptions(int ext, DWORD options);
		int	DoExport(const TCHAR *name,ExpInterface *ei,Interface *i, BOOL suppressPrompts=FALSE, DWORD options=0);
		int	DoSync() {return DoExport(syncfilename, 0, 0, 0, 0);}

		void ExportSceneInfo();
		void ExportNodeInfo(IGameNode * node);
		void ExportChildNodeInfo(IGameNode * child, CObjectNode* parentAuthor);
		void ExportMaterials();

		static void DumpMaterial(IGameMaterial * mat, CRapidXmlAuthorNode* root);
		static void AuthorMaterial(IGameMaterial * mat, int index, LPCTSTR filename = 0);
		static void AuthorMaterial(Mtl * mat, LPCTSTR filename);

		void DumpProperties(IGameProperty * prop);
		static void DumpFXProperties(IParamBlock2* prop, CRapidXmlAuthorNode* parentNode);
		void DumpNamedProperty(TCHAR*, IGameProperty* );
		void DumpProperty(IGameProperty*);

		void DumpLight(IGameLight *lt);
		void DumpCamera(IGameCamera *ca);
		void DumpMatrix(Matrix3 tm, CObjectNode* node);


		void MakeSplitFilename(IGameNode * node, TSTR & buf);
		void makeValidURIFilename(TSTR&, bool = false);
		BOOL ReadConfig();
		void WriteConfig();
		TSTR GetCfgFilename();
		IGameExporter();
		~IGameExporter();
	};


#endif
