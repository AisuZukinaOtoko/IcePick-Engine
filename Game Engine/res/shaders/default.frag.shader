#version 450 core
layout(location = 0) out vec4 OutColour;
layout(location = 1) out uvec4 OutEntityMat;


in vec3 v_Pos;
in vec2 v_TexCoord;
in vec3 v_Normal;

uniform vec3 u_CameraPosition;

uniform sampler2D  u_AlbedoTexUnit;

const vec3 tempLightPosition = vec3(2.0f, 3.0f, 4.0f);
const vec3 tempLightColour = vec3(1.0f, 1.0f, 1.0f);
const float tempLightIntensity = 0.68f;
const vec3 tempAmbientColour = vec3(0.525f, 0.565f, 0.565f);

#include "picking.util.shader"
#include "toneMapper.util.shader"

void main() {
    vec4 OutputColour = vec4(0.0f);

    vec3 L = normalize(tempLightPosition - vec3(0.0f));
    vec3 R = normalize(reflect(-L, normalize(v_Normal)));
    vec3 V = normalize(u_CameraPosition - v_Pos);

    vec4 diffuseColour = texture(u_AlbedoTexUnit, v_TexCoord);

    if (dot(v_Normal, L)  < 0.0f){
        OutputColour = vec4(tempAmbientColour * diffuseColour.xyz, diffuseColour.a);
    }
    else {
        OutputColour += vec4(tempAmbientColour * diffuseColour.xyz, diffuseColour.a); // ambient
        OutputColour += vec4((diffuseColour.rgb * dot(normalize(v_Normal), L)) * (tempLightColour * tempLightIntensity), diffuseColour.a); // diffuse
        OutputColour += vec4(diffuseColour.rgb * tempLightColour * pow(max(dot(V, R), 0.0), 32.0f), 1.0f);
    }

    OutColour = vec4(PBRNeutralToneMapper(OutputColour.rgb), OutputColour.a);
#ifdef VIEW_PICKING
    OutEntityMat = GetEntityMatSlot();
#endif
};