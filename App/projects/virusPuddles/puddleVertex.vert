void main()
{
	puddle_velocitiesV = puddle_velocities;
	puddle_lifetimesV = puddle_lifetimes;
	puddle_colorsV = puddle_colors;
	puddle_floorHitV = puddle_floorHit;
	gl_Position = view * model * vec4(puddle_positions.xyz, 1);
}