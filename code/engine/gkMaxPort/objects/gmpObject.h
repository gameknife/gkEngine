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



/**********************************************************************
*<
FILE: gkObjectInMax2.cpp

DESCRIPTION:  Geodesic Sphere Object
Edition 2 rewritten using ParamBlock2's

CREATED BY: Steve Anderson
Ed 2. John Wainwright

HISTORY: created 12/18/95

*>	Copyright (c) 1995, All Rights Reserved.
**********************************************************************/
#include "../system/gkMaxPort.h"

#include "iparamm2.h"
#include "Simpobj.h"
#include "surf_api.h"
#include "macroRec.h"
#include "RealWorldMapUtils.h"
#include <shlobj.h>
#include <string>

#define GEOSPHERE_CLASS_ID 12124
#define PBLOCK_REF_NO	 0
ClassDesc* GetgkObjectInMaxDesc();

// block IDs
enum { geo_creation_type, geo_type_in, geo_params, };
// geo_creation_type param IDs
enum { geo_create_meth, };
// geo_type_in param IDs
enum { geo_ti_pos, geo_ti_radius, };
// geo_param param IDs
enum { geo_meshpath, geo_smooth,};

enum { phy_nophysx = 3, phy_type, phy_shape,};

// JBW: IParamArray has gone since the class variable UI paramters are stored in static ParamBlocks
//      all corresponding class vars have gone, including the ParamMaps since they are replaced 
//      by the new descriptors
class gkObjectInMax : public SimpleObject2, public RealWorldMapSizeInterface 
{
public:	
	// Class vars
	static IObjParam*	ip;
	static BOOL			typeinCreate;
	bool				m_bCreated;
	bool				m_bNeedUpdate;
	std::string			m_strMeshFile;
	char				m_szMeshFilename[512];

	// JBW: minimal constructor, call MakeAutoParamBlocks() on my ClassDesc to
	//      have all the declared per-instance P_AUTO_CONSTRUCT blocks made, initialized and
	//      wired in.
	gkObjectInMax(BOOL loading)
	{
		m_strMeshFile = "Default.gkm";
		GetgkObjectInMaxDesc()->MakeAutoParamBlocks(this);
#ifdef PHYSICAL_SCALE_UVS
		if (!loading && !GetPhysicalScaleUVsDisabled())
			SetUsePhysicalScaleUVs(true);
#endif
	}

	CreateMouseCallBack* GetCreateMouseCallBack();
	void BeginEditParams( IObjParam  *ip, ULONG flags, Animatable *prev);
	void EndEditParams( IObjParam *ip, ULONG flags, Animatable *next);
	RefTargetHandle Clone(RemapDir& remap);
	TCHAR *GetObjectName() { return GetString(IDS_GEOSPHERE); }
	BOOL HasUVW();
	void SetGenUVW(BOOL sw);

	// From Object
	int CanConvertToType(Class_ID obtype);
	Object* ConvertToType(TimeValue t, Class_ID obtype);
	//void GetCollapseTypes(Tab<Class_ID> &clist, Tab<TSTR*> &nlist);

	// From GeomObject
	int IntersectRay(TimeValue t, Ray& ray, float& at, Point3& norm);

	// Animatable methods		
	void DeleteThis() {delete this;}
	Class_ID ClassID() { return Class_ID(GEOSPHERE_CLASS_ID, 32670); } 
	// JBW: direct ParamBlock access is added
	int	NumParamBlocks() { return 1; }					// return number of ParamBlocks in this instance
	IParamBlock2* GetParamBlock(int i) { return pblock2; } // return i'th ParamBlock
	IParamBlock2* GetParamBlockByID(BlockID id) { return (pblock2->ID() == id) ? pblock2 : NULL; } // return id'd ParamBlock

	// JBW: the Load() post-load callback insertion has gone since versioning is 
	//		handled automatically by virtue of permanent parameter IDs.  These IDs
	//		are defined in enums and are never retired so that old versions can be
	//		automatically re-mapped to new ones
	//
	//      Note that this is only true in new plug-ins; old plug-ins need to 
	//		continue to support version re-mapping as before for version up until
	//		converting to the new descriptors
	IOResult Load(ILoad *iload);

	// JBW: all the IParamArray methods are gone since we don't need them for the class variables


	// override From SimpleObject
	void BuildMesh(TimeValue t);
	BOOL OKtoDisplay(TimeValue t);
	void InvalidateUI();
	// JBW: the GetParamName() and GetParamDim() function have gone	as this all 
	//      is available in the descriptors. REFMSG_GET_PARAM_NAME, etc. become unnecessary as well

	// override
	// Get/Set the UsePhyicalScaleUVs flag.
	BOOL GetUsePhysicalScaleUVs();
	void SetUsePhysicalScaleUVs(BOOL flag);
	void UpdateUI();

	//From FPMixinInterface
	BaseInterface* GetInterface(Interface_ID id) 
	{ 
		if (id == RWS_INTERFACE) 
			return this; 

		BaseInterface* pInterface = FPMixinInterface::GetInterface(id);
		if (NULL != pInterface)
		{
			return pInterface;
		}

		return SimpleObject2::GetInterface(id);
	} 
};