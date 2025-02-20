#version 450

layout(binding = 0) uniform ProjectionMatrices {
    mat4 proj;
    mat4 view;
} mat;

layout(binding = 1) uniform ModelMatrix {
    mat4 model;
} obj;

layout(location = 0) in vec2 inPosition;
layout(location = 1) in vec3 inColor;

layout(location = 0) out vec3 fragColor;

void main() {
    gl_Position = mat.proj * mat.view * obj.model * vec4(inPosition, 0.0, 1.0);
    fragColor = inColor;
}
