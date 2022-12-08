#version 330 core
out vec4 frag_color;

uniform vec3 input_color;
uniform vec3 light_color;
uniform vec3 light_position;
uniform vec3 view_position;

in vec3 normal;
in vec3 frag_pos;

void main()
{
    float ambient_strength = 0.1f;
    vec3 ambient = ambient_strength * light_color;

    vec3 norm = normalize(normal);
    vec3 light_direction = normalize(light_position - frag_pos);
    float diffuse = max(dot(norm, light_direction), 0.0f);

    float specular_strength = 0.5f;
    vec3 view_direction = normalize(view_position - frag_pos);
    vec3 reflected_direction = reflect(-light_direction, norm);
    float specular_coef = pow(max(dot(view_direction, reflected_direction), 0.0f), 32);
    vec3 specular = specular_strength * specular_coef * light_color;

    vec3 result = (ambient + diffuse + specular) * input_color;

    frag_color = vec4(result, 1.0f);
}