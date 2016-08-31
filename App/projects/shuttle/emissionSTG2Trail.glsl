void emission()
{
	@lifetimes[gid] = trail2Lifetime;

	@positions[gid] = discPositionGenerator(vec3(0,0,-1), 2, true);
	@velocities[gid].xyz = velocityGenerator(vec3(0,0,-1), vec3(0), 15);

	@colors[gid] = vec4(0.6,0.1,0.1,1);
}