float4 Light_Point(float3 NormDir,  // WorldSpace
					float3 LightDir, // WorldSpace No Normal
					float4 LightColor, float4 LightAttenuation, 
					float3 EyeDir  // WorldSpace
					)
{
// 计算光线方向和距离
// Determine the distance from the light to the vertex and the direction
float  Dist = length(LightDir);
LightDir = LightDir / Dist;

// 计算夹角向量
// Compute half vector
float3 HalfVect = normalize(LightDir-EyeDir);

// 计算光线衰减
// Compute distance based attenuation. This is defined as:
// Attenuation = 1 / ( LA.x + LA.y*Dist + LA.z*Dist*Dist )
float DistAttn = saturate( 1 / ( LightAttenuation.x +
LightAttenuation.y * Dist + 
LightAttenuation.z * Dist * Dist ));

// 计算高光部分
// Specular
float fLightSpecular =  pow( saturate(dot(NormDir, HalfVect)),g_SpecularPow);

// 计算漫反射部分
// Diffuse
float fLightDiffuse = saturate(dot(NormDir, LightDir) );

// 得到子阴影系数
float4 fSelfShadow = float4(1,1,1,1);
fSelfShadow.rgb *= saturate(4 * fLightDiffuse);

// 合成最终颜色
// Compute final lighting
return LightColor * DistAttn * fSelfShadow * (fLightSpecular * g_Specular + fLightDiffuse * g_Diffuse);
}

float4 Light_Point_Phong(float3 NormDir,  // WorldSpace
					float3 LightDir, // WorldSpace
					float4 LightColor, float4 LightAttenuation, 
					float3 EyeDir  // WorldSpace
					)
{
// 计算光线方向和距离
// Determine the distance from the light to the vertex and the direction
float  Dist = length(LightDir);
LightDir = LightDir / Dist;

// 计算夹角向量
// Compute half vector
NormDir = normalize(NormDir);
float3 Reflect = 2 * dot(LightDir, -NormDir) * NormDir + LightDir;

// 计算光线衰减
// Compute distance based attenuation. This is defined as:
// Attenuation = 1 / ( LA.x + LA.y*Dist + LA.z*Dist*Dist )
float DistAttn = saturate( 1 / ( LightAttenuation.x +
LightAttenuation.y * Dist + 
LightAttenuation.z * Dist * Dist ));

// 计算高光部分
// Specular
float fLightSpecular =  pow( saturate(dot(EyeDir, Reflect)),g_SpecularPow);

// 计算漫反射部分
// Diffuse
float fLightDiffuse = saturate(dot(NormDir, LightDir) );

// 得到子阴影系数
float4 fSelfShadow = float4(1,1,1,1);
fSelfShadow.rgb *= saturate(4 * fLightDiffuse);

// 合成最终颜色
// Compute final lighting
return LightColor * DistAttn * fSelfShadow * (fLightSpecular * g_Specular + fLightDiffuse * g_Diffuse);
}

float3 NormalCalc(float3 mapNorm, float BumpScale)
{
	float3 v = {0.5f,0.5f,1.0f};
	mapNorm = lerp(v, mapNorm, BumpScale );
	mapNorm = ( mapNorm * 2.0f ) - 1.0f;
	return mapNorm;
}