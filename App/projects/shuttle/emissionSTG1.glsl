void emission()
{
	@lifetimes[gid] = 3600;
	@colors[gid] = vec4(0.1,0.9,0.9,1);

// positions around stage 2 propeller
	float angle = radians(stg1AngleInc * gid);
	@positions[gid] = vec4(	cos(angle) * stg1Radius,
													sin(angle) * stg1Radius,
													stg1ZOffset,
													1);
	@initialPositions[gid] = @positions[gid];

// detach action velocity vector
	vec3 velDir = normalize(vec3(@positions[gid].xy, -2 * @positions[gid].z)) * 2;
	@velocities[gid].xyz = velDir;

	if (randInRange(-1, 1) > 0)
		@detachRotSpeed[gid] = randInRange(1, 2);
	else
		@detachRotSpeed[gid] = randInRange(4, 5);
}