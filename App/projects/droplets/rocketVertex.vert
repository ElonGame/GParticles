#version 430

in float rocket_lifetimes;
in vec4 rocket_positions;
in vec4 rocket_colors;
in vec2 rocket_texCoords;

uniform mat4 model, view, projection;

out float rocket_lifetimesV;
out vec4 colorsV;
out vec2 rocket_texCoordsV;

void main()
{
	rocket_lifetimesV = rocket_lifetimes;
	colorsV = rocket_colors;
	rocket_texCoordsV = rocket_texCoords;
	gl_Position = view * model * vec4(rocket_positions.xyz, 1);
}