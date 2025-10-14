#version 330 core
layout(location = 0) out vec4 OutColour;
layout(location = 1) out vec4 OutNormal;
layout(location = 2) out uvec4 OutEntityMat;

in vec3 v_Pos;
in vec2 v_TexCoord;
in vec3 v_Normal;

#uniforms
#include "picking.shader"

void main() {
#shader
#ifdef VIEW_PICKING
    OutEntityMat = GetEntityMatSlot();
#endif
};