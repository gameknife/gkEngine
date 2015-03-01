//--------------------------------------------------------------------------------------
// Global variables
//--------------------------------------------------------------------------------------
shared float4x4 g_mWorld      			: 		WORLD;
shared float4x4 g_mWorldI				:		WorldInverse;
shared float4x4 g_mView       			: 		VIEW;
shared float4x4 g_mViewI				:		ViewInverse;
shared float4x4 g_mProjection 			: 		PROJECTION;
shared float4x4 g_mWorldViewProj 		: 		WORLDVIEWPROJ;
shared float4x4 g_mWorldView 			: 		WORLDVIEW;

float3 g_LightPos<
	string UIName = "PointLight Position"; 
	string Object = "PointLight";
	string Space = "World";
	int refID = 0;
> = float3(-500,-500,-500);

float g_SpecularPow<
	string UIName = "Spec Power";
	string UIType = "FloatSpinner";
	float UIMin = 0.0f;
	float UIMax = 100.0f;	
> = 8.0f;
