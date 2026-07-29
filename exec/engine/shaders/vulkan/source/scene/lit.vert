#version 450

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec2 inTexCoord;
layout(location = 2) in vec4 inTangent;
layout(location = 3) in vec3 inBinormal;

layout(push_constant) uniform DrawConstants
{
    mat4 worldViewProjection;
    vec4 color;
    vec4 uvTransform;
    vec4 lightDirection;
} draw;

layout(location = 0) out vec4 vertexColor;
layout(location = 1) out vec2 texCoord;
layout(location = 2) out vec3 normal;
layout(location = 3) flat out vec3 sunDirection;

void main()
{
    vec4 clip = draw.worldViewProjection * vec4(inPosition, 1.0);
    clip.y = -clip.y;
    gl_Position = clip;
    vertexColor = draw.color;
    texCoord = inTexCoord * draw.uvTransform.xy + draw.uvTransform.zw;
    normal = normalize(cross(inTangent.xyz, inBinormal) * inTangent.w);
    sunDirection = draw.lightDirection.xyz;
}
