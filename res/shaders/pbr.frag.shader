#version 330 core
layout(location = 0) out vec4 OutColour;
layout(location = 1) out vec4 OutNormal;
layout(location = 2) out uvec4 OutEntityMat;

const int SAMPLE_ALBEDO = 1 << 0;
const int SAMPLE_NORMAL = 1 << 1;
const int SAMPLE_ROUGHNESS = 1 << 2;
const int SAMPLE_METALLIC = 1 << 3;
const int SAMPLE_EMMISIVE = 1 << 4;


in vec3 v_Pos;
in vec2 v_TexCoord;
in vec3 v_Normal;

uniform vec3 u_CameraPosition;

uniform int MaterialSampleFlags;
uniform sampler2D  u_AlbedoTexUnit;
uniform sampler2D  u_NormalTexUnit;
uniform sampler2D  u_RoughnessTexUnit;
uniform sampler2D  u_MetallicUnit;
uniform sampler2D  u_EmissiveUnit;

uniform vec3 u_AmbientColour;
uniform vec4 u_AlbedoColour;
uniform vec3 u_SpecularColour;
uniform float u_RoughnessValue;
uniform float u_MetalValue;
uniform float u_EmissiveValue;

const vec3 tempLightPosition = vec3(2.0f, 3.0f, 4.0f);
const vec3 tempLightColour = vec3(1.0f, 1.0f, 1.0f);
const vec3 tempAmbientColour = vec3(0.15f, 0.165f, 0.165f);

#include "picking.shader"

void main() {
    vec4 OutputColour = vec4(0.0f);

    vec3 L = normalize(tempLightPosition - v_Pos);
    vec3 R = normalize(reflect(-L, normalize(v_Normal)));
    vec3 V = normalize(u_CameraPosition - v_Pos);

    vec4 diffuseColour = texture(u_AlbedoTexUnit, v_TexCoord);
    //vec4 diffuseColour = ((MaterialSampleFlags & SAMPLE_ALBEDO) != 0) ? texture(u_AlbedoTexUnit, v_TexCoord) : u_AlbedoColour;

    if (dot(v_Normal, L)  < 0.0f){
        OutputColour = vec4(tempAmbientColour * diffuseColour.xyz, diffuseColour.a);
    }
    else {
        OutputColour += vec4(tempAmbientColour * diffuseColour.xyz, diffuseColour.a); // ambient
        OutputColour += vec4(diffuseColour.rgb * dot(normalize(v_Normal), L), diffuseColour.a); // diffuse
        OutputColour += vec4(tempLightColour * pow(max(dot(V, R), 0.0), 32.0f), 0.0f); // specular
    }

    OutColour = vec4(1.0f);
    OutColour = diffuseColour;
    OutColour = OutputColour;
    OutNormal = vec4(v_Normal, 1.0f);
#ifdef VIEW_PICKING
    OutEntityMat = GetEntityMatSlot();
#endif
};