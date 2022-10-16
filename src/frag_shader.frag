#version 330 core
out vec4 frag_color;

in vec2 texture_coord;

uniform sampler2D input_texture1;
uniform sampler2D input_texture2;

uniform float mix_value;

void main()
{
    frag_color = mix(texture(input_texture1, texture_coord), texture(input_texture2, texture_coord), mix_value);
}