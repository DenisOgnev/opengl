#version 330 core
layout (location = 0) in vec3 input_position;
layout (location = 1) in vec3 input_color;

out vec4 vertex_color;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;


void main()
{
    gl_Position = projection * view * model * vec4(input_position, 1.0);
    vertex_color = vec4(input_color, 1.0);
}