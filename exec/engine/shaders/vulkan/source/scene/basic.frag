#version 450

layout(location = 0) in vec4 vertexColor;
layout(location = 1) in vec2 texCoord;
layout(location = 0) out vec4 outColor;

layout(set = 0, binding = 0) uniform sampler2D diffuseTexture;
layout(constant_id = 0) const bool alphaTest = false;
layout(constant_id = 1) const bool alphaOnlyTexture = false;

void main()
{
    vec4 sampled = texture(diffuseTexture, texCoord);
    if (alphaOnlyTexture)
        sampled = vec4(1.0, 1.0, 1.0, sampled.r);
    vec4 albedo = sampled * vertexColor;
    if (alphaTest && albedo.a < 0.5)
        discard;
    outColor = albedo;
}
