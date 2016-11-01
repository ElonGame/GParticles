void emission()
{
	@lifetimes[gid] = 5;
	@positions[gid] = conePositionGenerator(1.5, 3, false, true);
	@velocities[gid].xyz = velocityGenerator(@positions[gid].xyz, vec3(0), 3);
	@colors[gid] = vec4(0.1,0.9,0.9,1);

	@collided[gid] = 0;
}