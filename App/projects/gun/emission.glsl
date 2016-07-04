void emission()
{
	@lifetimes[gid] = 3600;
	@positions[gid] = gunPoint + spherePositionGenerator(0.5, false);
	@positions[gid].w = 1;
	@velocities[gid].xyz = velocityGenerator(vec3(0), vec3(1), 3);
	@colors[gid] = vec4(0.1,0.9,0.1,1);
	@state[gid] = 0;
}