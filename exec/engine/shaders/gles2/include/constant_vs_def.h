attribute highp vec3	inPosition;
#ifdef TC_HIGHP
attribute highp vec4	inTexcoord;
#else
attribute mediump vec4	inTexcoord;
#endif
attribute mediump vec4	inTangent;
attribute mediump vec3	inBinormal;
attribute mediump vec4    inBlendWight;
attribute mediump vec4    inBlendIndex;

uniform highp mat4		g_matWVP;
uniform highp mat4		g_matWorld;
uniform highp mat4		g_matViewI;
uniform highp mat4		g_matWorldI;
uniform highp mat4		g_mViewToLightProj;

//uniform highp vec4      g_screenSize;

uniform highp vec3		g_camPos;
uniform highp vec3		g_camDir;

uniform highp vec4		g_UVParam;
