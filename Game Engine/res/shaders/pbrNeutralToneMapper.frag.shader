#version 450 core
layout(location = 0) out vec4 OutColour;

in vec2 v_TexCoord;

uniform sampler2D  u_SceneTextureSlot;

#include "toneMapper.util.shader"

void main(){
    vec4 hdrColour = texture(u_SceneTextureSlot, v_TexCoord);
    vec4 toneMapped = vec4(PBRNeutralToneMapper(hdrColour.rgb), hdrColour.a);
    OutColour = vec4(pow(toneMapped.xyz, vec3(1.0f / 2.2f)), hdrColour.a);
};