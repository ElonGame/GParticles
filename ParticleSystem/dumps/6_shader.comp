#version 430

in float rocket_lifetimes;
in vec4 rocketpositions;
in vec4 rocket_colors;

uniform mat4 model, view, projection;

out float rocket_lifetimesV;
out vec4 colorsV;

void main()
{
	rocket_lifetimesV = rocket_lifetimes;
	colorsV = rocket_colors;
	gl_Position = projection * view * model * vec4(rocketpositions.xyz, 1);
}