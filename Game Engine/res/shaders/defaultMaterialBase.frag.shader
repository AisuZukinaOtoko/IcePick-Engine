#version 450 core
layout(location = 0) out vec4 OutColour;
layout(location = 1) out vec4 OutNormal;
layout(location = 2) out uvec4 OutEntityMat;

in vec2 v_TexCoord;
in vec3 v_Normal;

uniform sampler2D  u_AlbedoTexUnit;

#include "picking.shader"

void main() {
    vec4 diffuseColour = texture(u_AlbedoTexUnit, v_TexCoord);
    OutColour = vec4(0.3f, 0.5f, 0.2f, 1.0f);
    OutColour = diffuseColour;
    OutNormal = vec4(v_Normal, 1.0f);

#ifdef VIEW_PICKING
    OutEntityMat = GetEntityMatSlot();
#endif
};