#version 330 core

// Input
layout(location = 0) in vec3 v_position; 

// Uniform properties
uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;

out float f_height;   

// 2D Random
float random(in vec2 st) {
    return fract(sin(dot(st.xy, vec2(16.9898, 78.233))) * 56758.5453123);
}

// 2D Noise
float noise(in vec2 st) {
    vec2 i = floor(st);
    vec2 f = fract(st);

    float a = random(i);
    float b = random(i + vec2(1.0, 0.0));
    float c = random(i + vec2(0.0, 1.0));
    float d = random(i + vec2(1.0, 1.0));

    vec2 u = f * f * (3.0 - 2.0 * f);

    return mix(a, b, u.x) +
           (c - a) * u.y * (1.0 - u.x) +
           (d - b) * u.x * u.y;
}

void main() {
    vec2 pos = v_position.xz * 0.2;
    float n = noise(pos);

    vec3 modified_position = v_position;
    modified_position.y += n * -2.0;

    f_height = n;

    gl_Position = Projection * View * Model * vec4(modified_position, 1.0);
}
