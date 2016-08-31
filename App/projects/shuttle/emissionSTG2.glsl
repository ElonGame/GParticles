void emission()
{
	@lifetimes[gid] = 3600;
	@colors[gid] = vec4(0.6,0.1,0.1,1);

	@positions[gid] = vec4(0,0,0,1);
	@initialPositions[gid] = @positions[gid];

	@velocities[gid].xyz = vec3(0,0,-2);
}