void emission()
{
	@lifetimes[gid] = trail1Lifetime;

	@positions[gid] = discPositionGenerator(vec3(0,0,-1), 4, true);
	@velocities[gid].xyz = velocityGenerator(vec3(0,0,-1), vec3(0), 10);

	float angle = radians(stg1AngleInc * gid);
	@positions[gid].xyz += vec3(cos(angle) * stg1Radius, sin(angle) * stg1Radius, stg1ZOffset);

	@colors[gid] = vec4(0.1,0.9,0.9,1);
}