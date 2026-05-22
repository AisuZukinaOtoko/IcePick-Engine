#version 450 core
layout(location = 0) out vec4 OutColour;
layout(location = 1) out uvec4 OutEntityMat;

in vec3 v_Pos;
in vec2 v_TexCoord;
in vec3 v_Normal;

uniform vec3 u_CameraPosition;

#uniforms

#include "noiseFunctions.util.shader"
#include "picking.util.shader"

void main() {
#shader
#ifdef VIEW_PICKING
    OutEntityMat = GetEntityMatSlot();
#endif
};