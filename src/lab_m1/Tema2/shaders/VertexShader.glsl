#version 330 core

// Input
layout(location = 0) in vec3 v_position;
layout(location = 1) in vec3 v_normal;
layout(location = 2) in vec2 v_texcoord;
layout(location = 3) in vec3 v_color;

// Uniform properties
uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;
uniform vec3 color;
uniform bool new_color;

// Output
out vec3 f_color;

void main()
{
    // Set the output color directly based on the vertex color
    if (new_color)
        f_color = color;
    else
        f_color = v_color;

    // Compute the final position without time dependency
    gl_Position = Projection * View * Model * vec4(v_position, 1);
}
