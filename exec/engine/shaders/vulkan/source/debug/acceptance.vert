#version 450

layout(push_constant) uniform DrawParams
{
    float time;
    float aspect;
    vec2 padding;
} params;

layout(location = 0) out vec3 outColor;

const vec2 positions[3] = vec2[](
    vec2( 0.00, -0.62),
    vec2( 0.62,  0.48),
    vec2(-0.62,  0.48)
);

const vec3 colors[3] = vec3[](
    vec3(0.10, 0.78, 1.00),
    vec3(0.72, 0.28, 1.00),
    vec3(0.12, 1.00, 0.48)
);

void main()
{
    float angle = params.time * 0.65;
    mat2 rotation = mat2(cos(angle), -sin(angle), sin(angle), cos(angle));
    vec2 position = rotation * positions[gl_VertexIndex];
    position.x /= max(params.aspect, 0.001);
    gl_Position = vec4(position, 0.0, 1.0);
    outColor = colors[gl_VertexIndex];
}
