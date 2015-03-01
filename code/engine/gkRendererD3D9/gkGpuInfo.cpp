#include "RendererD3D9StableHeader.h"
#include "gkGpuInfo.h"
#include "DXUT.h"


void InitGpuInfo()
{
	DXUTDeviceSettings setting =  DXUTGetDeviceSettings();
	CD3D9Enumeration* pd3dEnum = DXUTGetD3D9Enumeration();
	CD3D9EnumAdapterInfo* info = pd3dEnum->GetAdapterInfo( setting.d3d9.AdapterOrdinal );
	
#ifdef UNICODE
	MultiByteToWideChar(CP_ACP, 0, info->AdapterIdentifier.Description, -1, gEnv->pSystemInfo->gpuDesc, 255);
#else
	_tcscpy( gEnv->pSystemInfo->gpuDesc, info->AdapterIdentifier.Description );
#endif
	
	gEnv->pSystemInfo->gpuVendorID = info->AdapterIdentifier.VendorId;
}
