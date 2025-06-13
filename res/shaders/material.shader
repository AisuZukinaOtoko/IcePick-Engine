#shader vertex
#version 330 core
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texCoord;

out vec3 v_Pos;
out vec2 v_TexCoord;
out vec3 v_Normal;

uniform mat4 u_MVP;
uniform mat4 u_Modelmatrix;
uniform mat3 u_NormalMatrix;

void main() {
    gl_Position = u_MVP * vec4(position, 1.0f);
    v_Pos = (u_Modelmatrix * vec4(position, 1.0)).xyz;
    v_Normal = normalize(u_NormalMatrix * normal);
    v_TexCoord = texCoord;
    //v_Normal = normal;
    //v_TexCoord = texCoord;
    //v_Normal = vec3(1.0f);
};

//--------------------------------------------------------
#shader fragment
#version 330 core
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
uniform vec3 u_AlbedoColour;
uniform vec3 u_SpecularColour;
uniform float u_RoughnessValue;
uniform float u_MetalValue;
uniform float u_EmissiveValue;

const vec3 tempLightPosition = vec3(1.0f, 2.0f, 1.0f);
const vec3 tempLightColour = vec3(0.5f, 0.5f, 0.5f);
const vec3 tempAmbientColour = vec3(0.1f, 0.1f, 0.1f);

void main() {
    vec4 OutputColour = vec4(0.0f);
    //OutputColour *= vec4(v_Normal, 1.0);

    vec3 L = normalize(tempLightPosition - v_Pos);
    vec3 R = reflect(-L, v_Normal);
    vec3 V = normalize(u_CameraPosition - v_Pos);

    if (dot(v_Normal, L)  < 0.0f){
        //OutputColour = vec4(0.05f, 0.05f, 0.05f, 1.0f);
        OutputColour = vec4(tempAmbientColour, 1.0f);
    }
    else {
        OutputColour += vec4(tempAmbientColour, 1.0f); // ambient
        OutputColour += vec4((tempLightColour * dot(v_Normal, L)), 1.0f); // diffuse
        OutputColour += vec4(tempLightColour * pow(max(dot(V, R), 0.0), 32.0f), 1.0f); // specular
    }
    //if ((MaterialSampleFlags & SAMPLE_ALBEDO) != 0) {
    //    OutputColour *= texture(u_AlbedoTexUnit, v_TexCoord);
    //}
    //else {
    //    OutputColour *= vec4(u_AlbedoColour, 1.0);
    //}
    gl_FragColor = OutputColour;
};