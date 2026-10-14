layout(std140, binding = 0) uniform SceneDetails {
    mat4 ViewProjectionMatrix;
    vec3 CameraPosition;
    float AccumulatedTime;
};