void emission()
{
	@lifetimes[gid] = 5;
	//@positions[gid] = spherePositionGenerator(0.5, true);
	@positions[gid] = conePositionGenerator(1.5, 3, false, true);
	@velocities[gid].xyz = velocityGenerator(@positions[gid].xyz, vec3(0), 2);
	@colors[gid] = vec4(0.1,0.9,0.9,1);

	@collided[gid] = 0;
}