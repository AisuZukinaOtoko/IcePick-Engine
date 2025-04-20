#shader vertex
#version 330 core
layout(location = 0) in vec3 position;
//layout(location = 1) in vec2 texCoord;

out vec2 v_TexCoord;
uniform mat4 u_MVP;

void main() {
    gl_Position = u_MVP * vec4(position, 1.0f);
    //v_TexCoord = texCoord;
    v_TexCoord = vec2(0.0f);
};

//--------------------------------------------------------
#shader fragment
#version 330 core
const int SAMPLE_ALBEDO = 1 << 0;
const int SAMPLE_NORMAL = 1 << 1;
const int SAMPLE_ROUGHNESS = 1 << 2;
const int SAMPLE_METALLIC = 1 << 3;
const int SAMPLE_EMMISIVE = 1 << 4;


in vec2 v_TexCoord;
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

void main() {
    vec4 OutputColour = vec4(1.0);
    if ((MaterialSampleFlags & SAMPLE_ALBEDO) != 0) {
        OutputColour *= texture(u_AlbedoTexUnit, v_TexCoord);
    }
    else {
        OutputColour *= vec4(u_AlbedoColour, 1.0);
    }
    gl_FragColor = OutputColour;
};