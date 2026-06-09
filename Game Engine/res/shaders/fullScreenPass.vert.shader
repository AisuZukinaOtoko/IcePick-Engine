#version 450 core

out vec2 v_TexCoord;

const vec2 vertices[3] = vec2[](
    vec2(-1.0,-1.0),
    vec2( 3.0,-1.0),
    vec2(-1.0, 3.0)
);

void main() {
    v_TexCoord = vertices[gl_VertexID] * 0.5 + 0.5;
    gl_Position = vec4(vertices[gl_VertexID], 0.0, 1.0);
};