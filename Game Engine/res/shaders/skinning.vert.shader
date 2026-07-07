#version 450 core
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texCoord;
layout(location = 3) in uvec4 boneIds;
layout(location = 4) in vec4  boneWeights;

out vec3 v_Pos;
out vec2 v_TexCoord;
out vec3 v_Normal;

uniform mat4 u_MVP;
uniform mat4 u_Modelmatrix;
uniform mat3 u_NormalMatrix;

layout(std430, binding = 0) readonly buffer BoneMatrices {
    mat4 bones[];
};


void main() {
     mat4 skin =
          bones[boneIds.x] * boneWeights.x
        + bones[boneIds.y] * boneWeights.y
        + bones[boneIds.z] * boneWeights.z
        + bones[boneIds.w] * boneWeights.w;

    vec4 skinPosition = skin * vec4(position, 1.0);

    gl_Position = u_MVP * skinPosition;
    v_Pos = (u_Modelmatrix * skinPosition).xyz;
    v_Normal = normalize(u_NormalMatrix * normal);
    v_TexCoord = texCoord;
};