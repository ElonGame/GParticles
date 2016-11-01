void emission()
{
	@lifetimes[gid] = 5;
	@positions[gid] = spherePositionGenerator(1.5, true);
	@velocities[gid].xyz = velocityGenerator(@positions[gid].xyz, vec3(0), 0.25);
	@colors[gid] = vec4(0.1,0.9,0.9,1);

	@collided[gid] = 0;
}