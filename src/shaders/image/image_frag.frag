#version 450

layout(location = 0) in vec2  fragUV;
layout(location = 1) in vec2  fragSize;
layout(location = 2) in float fragRadius;
layout(location = 3) in vec4  fragColor;

layout(set = 0, binding = 0) uniform sampler2D tex;

layout(location = 0) out vec4 outColor;

float roundedBoxSDF(vec2 p, vec2 halfSize, float r) {
    vec2 q = abs(p) - halfSize + r;
    return length(max(q, 0.0)) + min(max(q.x, q.y), 0.0) - r;
}

void main() {
    vec2 halfSize = fragSize * 0.5;
    vec2 p = (fragUV - 0.5) * fragSize;
    float r = min(fragRadius, min(halfSize.x, halfSize.y));

    float d = roundedBoxSDF(p, halfSize, r);
    float aa = fwidth(d) * 0.5;
    float mask = 1.0 - smoothstep(-aa, aa, d);
    if (mask <= 0.0) discard;

    vec4 texel = texture(tex, fragUV) * fragColor;
    outColor = vec4(texel.rgb, texel.a * mask);
}