#version 450 core
layout(location = 0) in vec2 position;
layout(location = 1) in vec2 texCoord;

#include "sceneDetails.util.shader"

out vec3 v_Pos;
out vec2 v_TexCoord;
out vec3 v_Normal;

uniform float u_HeightScale;
uniform float u_TexelSize;
uniform sampler2D  u_HeightFieldTextureUnit;

float GetHeightMapValue(vec2 uv) {
    return texture(u_HeightFieldTextureUnit, uv).r * u_HeightScale;
}

vec3 CalculateNormal() {
    float leftHeight = GetHeightMapValue(texCoord + vec2(-u_TexelSize, 0.0f));
    float rightHeight = GetHeightMapValue(texCoord + vec2(u_TexelSize, 0.0f));
    float upHeight = GetHeightMapValue(texCoord + vec2(0.0f, u_TexelSize));
    float downHeight = GetHeightMapValue(texCoord + vec2(0.0f, -u_TexelSize));

    vec3 tangent   = vec3(2.0f * u_TexelSize, rightHeight - leftHeight, 0.0f);
    vec3 bitangent = vec3(0.0f, upHeight - downHeight, 2.0f * u_TexelSize);

    return normalize(cross(bitangent, tangent));
}

void main() {
    float terrainHeight = GetHeightMapValue(texCoord);
    v_Pos = vec3(position.x, terrainHeight, position.y);
    v_TexCoord = texCoord;
    v_Normal = CalculateNormal();
    gl_Position = ViewProjectionMatrix * vec4(v_Pos, 1.0f);
}