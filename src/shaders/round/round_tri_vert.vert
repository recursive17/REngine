#version 450

layout(location = 0) in vec2  inPosition;
layout(location = 1) in vec4  inColor;
layout(location = 2) in vec2  inP0;
layout(location = 3) in vec2  inP1;
layout(location = 4) in vec2  inP2;
layout(location = 5) in float inRadius;

layout(location = 0) out vec4  fragColor;
layout(location = 1) out vec2  fragP0;
layout(location = 2) out vec2  fragP1;
layout(location = 3) out vec2  fragP2;
layout(location = 4) out float fragRadius;

void main() {
    gl_Position = vec4(inPosition, 0.0, 1.0);
    fragColor  = inColor;
    fragP0     = inP0;
    fragP1     = inP1;
    fragP2     = inP2;
    fragRadius = inRadius;
}