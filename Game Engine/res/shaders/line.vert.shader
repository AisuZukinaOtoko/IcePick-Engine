#version 450 core
layout(location = 0) in vec4 colour;
layout(location = 1) in vec3 position;

#include "sceneDetails.util.shader"

out vec3 v_Pos;
out vec4 v_Colour;

void main() {
    v_Pos = position;
    v_Colour = colour;
    gl_Position = ViewProjectionMatrix * vec4(position, 1.0f);
};