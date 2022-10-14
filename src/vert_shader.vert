#version 330 core
layout (location = 0) in vec3 input_pos;

void main()
{
    gl_Position = vec4(input_pos.x, input_pos.y, input_pos.z, 1.0);
}