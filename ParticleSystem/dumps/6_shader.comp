#version 430
in vec3 position;
in vec3 normal;
in vec2 texCoords;
in float rocket_lifetimes;
in vec4 rocket_positions;
in vec4 rocket_colors;

out vec2 TexCoords;
out vec4 colors;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = projection * view * model * (vec4(position.xyz, 0.0f) + rocket_positions);
    TexCoords = texCoords;
    colors = rocket_colors;
}