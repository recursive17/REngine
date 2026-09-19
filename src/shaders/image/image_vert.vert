#version 450

layout(location = 0) in vec2  inPosition;
layout(location = 1) in vec2  inUV;
layout(location = 2) in vec2  inSize;
layout(location = 3) in float inRadius;
layout(location = 4) in vec4  inColor;

layout(location = 0) out vec2  fragUV;
layout(location = 1) out vec2  fragSize;
layout(location = 2) out float fragRadius;
layout(location = 3) out vec4  fragColor;

void main() {
    gl_Position = vec4(inPosition, 0.0, 1.0);
    fragUV     = inUV;
    fragSize   = inSize;
    fragRadius = inRadius;
    fragColor  = inColor;
}