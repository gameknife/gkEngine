#ifdef TC_HIGHP
varying highp vec4		outTC;
#else
varying mediump vec4		outTC;
#endif
varying highp vec4			outWorldPos;
#ifdef ZPASS
varying mediump vec4		outTangent;
varying mediump vec3		outBinormal;
#else
varying highp vec4			outScreenTC;
#endif


