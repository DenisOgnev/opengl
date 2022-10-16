#version 330 core
out vec4 frag_color;

in vec4 vertex_color;
in vec2 texture_coord;

uniform sampler2D input_texture;

void main()
{
    frag_color = texture(input_texture, texture_coord) * vec4(vertex_color);
}