
vec2 noiseVec2(vec2 UV) {
    float x = dot(UV, vec2(123.4f, 234.5f));
    float y = dot(UV, vec2(345.6f, 456.7f));
    vec2 noise = vec2(sin(x), sin(y)) * 43758.5453;
    return fract(noise);
}

float SimpleNoise(vec2 UV, float t) {
    
    return 0.68f;
}

float VoronoiNoise(vec2 UV, float t) {
    // Grid setup
    float gridSize = t;
    UV = UV * gridSize;
    vec2 gridId = floor(UV);
    vec2 gridPosition = fract(UV) - vec2(0.5f);

    // Calculate distance to closest grid center
    float minDistanceToClosestGrid = 2000.0f;
    for (float i = -1.0f; i <= 1.0f; i++) {
        for (float j = -1.0f; j <= 1.0f; j++) {
            vec2 checkGridCenter = vec2(i, j);
            vec2 noise = noiseVec2(gridId + checkGridCenter);
            vec2 gridCenter = checkGridCenter + sin(8.0f * noise) * 0.5;
            float currentDistance = distance(gridPosition, gridCenter);
            minDistanceToClosestGrid = min(currentDistance, minDistanceToClosestGrid);
        }
    }

    return minDistanceToClosestGrid;
}
