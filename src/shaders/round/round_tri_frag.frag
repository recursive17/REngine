#version 450

layout(location = 0) in vec4  fragColor;
layout(location = 1) in vec2  fragP0;
layout(location = 2) in vec2  fragP1;
layout(location = 3) in vec2  fragP2;
layout(location = 4) in float fragRadius;

layout(location = 0) out vec4 outColor;

float sdTriangle(vec2 p, vec2 p0, vec2 p1, vec2 p2) {
    vec2 e0 = p1 - p0, v0 = p - p0;
    vec2 e1 = p2 - p1, v1 = p - p1;
    vec2 e2 = p0 - p2, v2 = p - p2;

    vec2 pq0 = v0 - e0 * clamp(dot(v0, e0) / dot(e0, e0), 0.0, 1.0);
    vec2 pq1 = v1 - e1 * clamp(dot(v1, e1) / dot(e1, e1), 0.0, 1.0);
    vec2 pq2 = v2 - e2 * clamp(dot(v2, e2) / dot(e2, e2), 0.0, 1.0);

    float s = sign(e0.x * e2.y - e0.y * e2.x);
    vec2 d = min(min(vec2(dot(pq0, pq0), s * (v0.x * e0.y - v0.y * e0.x)),
                     vec2(dot(pq1, pq1), s * (v1.x * e1.y - v1.y * e1.x))),
                     vec2(dot(pq2, pq2), s * (v2.x * e2.y - v2.y * e2.x)));

    return -sqrt(d.x) * sign(d.y);
}

vec2 insetCorner(vec2 p, vec2 a, vec2 b, float r) {
    vec2 e0 = normalize(a - p);
    vec2 e1 = normalize(b - p);
    vec2 bis = normalize(e0 + e1);
    float cosHalf = dot(e0, bis);
    float sinHalf = sqrt(max(1.0 - cosHalf * cosHalf, 0.0001));
    return p + bis * (r / sinHalf);
}

void main() {
    float r = fragRadius;

    vec2 a0 = insetCorner(fragP0, fragP1, fragP2, r);
    vec2 a1 = insetCorner(fragP1, fragP2, fragP0, r);
    vec2 a2 = insetCorner(fragP2, fragP0, fragP1, r);

    float d = sdTriangle(gl_FragCoord.xy, a0, a1, a2) - r;

    float aa = fwidth(d) * 0.5;
    float alpha = 1.0 - smoothstep(-aa, aa, d);

    if (alpha <= 0.0) discard;
    outColor = vec4(fragColor.rgb, fragColor.a * alpha);
}