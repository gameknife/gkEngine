#version 450

layout(location = 0) in vec4 vertexColor;
layout(location = 1) in vec2 texCoord;
layout(location = 2) in vec3 normal;
layout(location = 3) flat in vec3 sunDirection;
layout(location = 0) out vec4 outColor;

layout(set = 0, binding = 0) uniform sampler2D diffuseTexture;
layout(constant_id = 0) const bool alphaTest = false;

void main()
{
    float diffuse = max(dot(normalize(normal), normalize(sunDirection)), 0.0);
    vec4 albedo = texture(diffuseTexture, texCoord) * vertexColor;
    if (alphaTest && albedo.a < 0.5)
        discard;
    outColor = vec4(albedo.rgb * (0.22 + 0.78 * diffuse), albedo.a);
}
