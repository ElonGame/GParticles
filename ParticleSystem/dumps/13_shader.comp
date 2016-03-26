#version 430

in float explosion_lifetimes;
in vec4 explosion_positions;
in vec4 explosion_colors;

uniform mat4 model, view, projection;

out float explosion_lifetimesV;
out vec4 colorsV;

void main()
{
	explosion_lifetimesV = explosion_lifetimes;
	colorsV = explosion_colors;
	gl_Position = projection * view * model * vec4(explosion_positions.xyz, 1);
}