#version 430

in float lifetimes;
in vec4 positions;
in vec4 colors;

uniform mat4 model, view, projection;

out float lifetimesV;
out vec4 colorsV;

void main()
{
	lifetimesV = lifetimes;
	colorsV = colors;
	vec4 projectedCoords = projection * view * model * vec4(positions.xyz, 1);
	gl_Position = projectedCoords;
}