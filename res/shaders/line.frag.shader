#version 450 core
layout(location = 0) out vec4 OutColour;

in vec3 v_Pos;
in vec4 v_Colour;

void main() {
    OutColour = v_Colour;
};