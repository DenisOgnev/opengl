#version 330 core
layout (location = 0) in vec3 input_position;
layout (location = 1) in vec3 input_normal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 normal;
out vec3 frag_pos;

void main()
{
    gl_Position = projection * view * model * vec4(input_position, 1.0f);
    frag_pos = vec3(model * vec4(input_position, 1.0f));
    normal = mat3(transpose(inverse(model))) * input_normal;
}