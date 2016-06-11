
in float rocket_lifetimes;
in vec4 rocket_positions;
in vec4 rocket_colors;

uniform mat4 model, view, projection;

out float lifetimesV;
out vec4 colorsV;

void main()
{
	lifetimesV = rocket_lifetimes;
	colorsV = rocket_colors;
	gl_Position = projection * view * model * vec4(rocket_positions.xyz, 1);
}