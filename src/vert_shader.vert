#version 330 core
layout (location = 0) in vec3 input_position;
layout (location = 1) in vec3 input_color;
layout (location = 2) in vec2 input_texture_coord;

out vec4 vertex_color;
out vec2 texture_coord;

void main()
{
    gl_Position = vec4(input_position, 1.0);
    vertex_color = vec4(input_color, 1.0);
    texture_coord = input_texture_coord;
}