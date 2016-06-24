#version 430

in float rocket_lifetimes;
in vec4 rocket_positions;
in vec4 rocket_lastPositions;
in vec4 rocket_colors;

uniform mat4 model, view, projection;

out float lifetimesV;
out vec4 rocket_lastPositionsV;
out vec4 colorsV;

void main()
{
	lifetimesV = rocket_lifetimes;
	rocket_lastPositionsV =	rocket_lastPositions;
	colorsV = rocket_colors;
	gl_Position = model * vec4(rocket_positions.xyz, 1);
}