#version 430

in float exp_lifetimes;
in vec4 exp_positions;
in vec4 exp_colors;

uniform mat4 model, view, projection;

out float exp_lifetimesV;
out vec4 colorsV;

void main()
{
	exp_lifetimesV = exp_lifetimes;
	colorsV = exp_colors;
	gl_Position = projection * view * model * vec4(exp_positions.xyz, 1);
}