#version 450 core
layout(location = 0) out vec4 OutColour;

uniform sampler2D  u_ColourTexture;

vec3 PBRNeutralToneMapper(vec3 color) {
    float startCompression = 0.8 - 0.04;
    float desaturation = 0.15;

    float x = min(color.r, min(color.g, color.b));
    float offset = x < 0.08 ? x - 6.25 * x * x : 0.04;
    color -= offset;

    float peak = max(color.r, max(color.g, color.b));
    if (peak < startCompression) return color;

    float d = 1. - startCompression;
    float newPeak = 1. - d * d / (peak + d - startCompression);
    color *= newPeak / peak;

    float g = 1. - 1. / (desaturation * (peak - newPeak) + 1.);
    return mix(color, newPeak * vec3(1, 1, 1), g);
}

void main() {
    vec4 colour = texture(u_ColourTexture, gl_FragCoord.xy);
    OutColour = vec4(PBRNeutralToneMapper(colour.xyz), colour.a);
};