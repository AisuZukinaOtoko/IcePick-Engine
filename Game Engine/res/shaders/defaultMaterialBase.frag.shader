#version 450 core
layout(location = 0) out vec4 OutColour;
layout(location = 1) out uvec4 OutEntityMat;

in vec2 v_TexCoord;
in vec3 v_Normal;

uniform sampler2D  u_AlbedoTexUnit;

#include "picking.util.shader"

void main() {
    vec4 diffuseColour = texture(u_AlbedoTexUnit, v_TexCoord);
    OutColour = diffuseColour;

#ifdef VIEW_PICKING
    OutEntityMat = GetEntityMatSlot();
#endif
};