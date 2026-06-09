#version 450 core
layout(location = 0) out vec4 OutColour;

in vec2 v_TexCoord;

uniform sampler2D  u_SceneTextureSlot;

void main(){
    OutColour = texture(u_SceneTextureSlot, v_TexCoord);
};