
in float puddle_lifetimes;
in vec4 puddle_velocities;
in vec4 puddle_positions;
in vec4 puddle_colors;
in float puddle_floorHit;

uniform mat4 model, view, projection;

out vec4 puddle_velocitiesV;
out float puddle_lifetimesV;
out vec4 colorsV;
out float puddle_floorHitV;

void main()
{
	puddle_velocitiesV = puddle_velocities;
	puddle_lifetimesV = puddle_lifetimes;
	colorsV = puddle_colors;
	puddle_floorHitV = puddle_floorHit;
	gl_Position = view * model * vec4(puddle_positions.xyz, 1);
}