#version 430

in float lifetimes;
in vec4 positions;
in vec4 colors;

out float lifetimesV;
out vec4 colorsV;

void main()
{
	lifetimesV = lifetimes;
	colorsV = colors;
	gl_Position = positions;
}