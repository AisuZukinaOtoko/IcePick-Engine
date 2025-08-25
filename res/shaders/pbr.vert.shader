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
};