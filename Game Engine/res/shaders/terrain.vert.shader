#version 450 core
layout(location = 0) in vec2 position;
layout(location = 1) in vec2 texCoord;

out vec3 v_Pos;
out vec2 v_TexCoord;
out vec3 v_Normal;

uniform mat4 u_ViewProjectionMatrix;

void main() {
    v_Pos = vec3(position.x, 0.0f, position.y);
    v_TexCoord = texCoord;
    v_Normal = vec3(0.0f, 1.0f, 0.0f);
    gl_Position = u_ViewProjectionMatrix * vec4(v_Pos, 1.0f);
};