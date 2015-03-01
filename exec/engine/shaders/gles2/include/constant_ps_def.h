////////////////////////////////////////////////////////////////////////////////////////////////////
// Common Param
////////////////////////////////////////////////////////////////////////////////////////////////////
uniform highp vec3 g_LightDir;
uniform lowp vec4 g_MatDiffuse;
uniform lowp vec4 g_MatSpecular;
uniform mediump float g_Gloss;

uniform lowp vec4 g_LightDiffuse;
uniform lowp vec4 g_LightSpecular;
uniform lowp vec4 g_SkyLight;

uniform highp vec3		g_camPos;
uniform highp vec3		g_camDir;

uniform mediump float g_Fresnel;
uniform mediump float g_FresnelBia;
uniform mediump float g_FresnelScale;

uniform highp float g_Glow;
uniform highp float g_ldrbright;
