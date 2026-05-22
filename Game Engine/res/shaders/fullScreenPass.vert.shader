#version 450 core
layout(location = 0) in vec2 screenCoord;
layout(location = 1) in vec2 UV;

out vec2 v_TexCoord;

void main() {
   v_TexCoord = UV;
   gl_Position = vec4(screenCoord, 0.0f, 1.0f);
};