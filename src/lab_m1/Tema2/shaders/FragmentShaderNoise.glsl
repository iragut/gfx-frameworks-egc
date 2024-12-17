#version 330 core

// Input from Vertex Shader
in float f_height;   

// Output color
layout(location = 0) out vec4 out_color;

void main() {
    vec3 highColor = vec3(0.49, 0.81, 0.54); 
    vec3 lowColor = vec3(0.41, 0.54, 0.25); 

    vec3 finalColor = mix(lowColor, highColor, f_height);

    out_color = vec4(finalColor, 1.0);
}
