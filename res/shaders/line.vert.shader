#version 450 core
layout(location = 0) in vec4 colour;
layout(location = 1) in vec3 position;

out vec3 v_Pos;
out vec4 v_Colour;

uniform mat4 u_ViewProjectionMatrix;

void main() {
    v_Pos = position;
    v_Colour = colour;
    gl_Position = u_ViewProjectionMatrix * vec4(position, 1.0f);
};