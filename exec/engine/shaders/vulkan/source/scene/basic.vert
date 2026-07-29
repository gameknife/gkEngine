#version 450

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec2 inTexCoord;
layout(location = 2) in vec4 inColor;
layout(constant_id = 2) const bool useVertexColor = false;

layout(push_constant) uniform DrawConstants
{
    mat4 worldViewProjection;
    vec4 color;
    vec4 uvTransform;
} draw;

layout(location = 0) out vec4 vertexColor;
layout(location = 1) out vec2 texCoord;

void main()
{
    vec4 clip = draw.worldViewProjection * vec4(inPosition, 1.0);
    clip.y = -clip.y;
    gl_Position = clip;
    vertexColor = draw.color * (useVertexColor ? inColor : vec4(1.0));
    texCoord = inTexCoord * draw.uvTransform.xy + draw.uvTransform.zw;
}
