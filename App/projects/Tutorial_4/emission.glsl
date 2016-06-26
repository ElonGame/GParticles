void emission()
{
	@lifetimes[gid] = 10;
	@positions[gid] = spherePositionGenerator(1.5, false);
	@velocities[gid].xyz = velocityGenerator(@positions[gid].xyz, vec3(0), 0.1);
	@colors[gid] = vec4(0.1,0.9,0.1,1);
}