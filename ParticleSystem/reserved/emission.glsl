////////////////////////////////////////////////////////////////////////////////
// EMISSION
////////////////////////////////////////////////////////////////////////////////

// -- Emission Primitives --
////////////////////////////////////////////////////////////////////////////////
vec4 conePositionGenerator(float radius, float height, bool coneBaseIsOrigin, bool positionsInVolume)
{
	// compute random y in range [0, height]
	float y = randInRange(0, 1);

	// compute particle horizontal distance from cone axis
	float newRadius = (radius * y) / height;
	float dist = newRadius;
	if (positionsInVolume)
	{
		dist = randInRange(0, newRadius);
	}

	// compute rotation angle (in degrees) around cone axis
	float angle = randInRange(0, 360);

	// calculate corresponding x and z polar coordinates
	float x = dist * sin(angle);
	float z = dist * cos(angle);

	// compute cone orientation
	y = (coneBaseIsOrigin) ? height - y : y;

	return vec4(x, y, z, 1);
}


////////////////////////////////////////////////////////////////////////////////
vec4 spherePositionGenerator(float maxRadius, bool positionsInVolume)
{
	float radius = maxRadius;
	if (positionsInVolume)
		radius = randInRange(0, maxRadius);

	// compute rotation angle (in degrees) around cone axis
	float angle = randInRange(0, 360);
	float angle2 = randInRange(0, 180);

	// calculate corresponding x and z polar coordinates
	float x = radius * sin(angle) * cos(angle2);
	float z = radius * cos(angle);
	float y = radius * sin(angle) * sin(angle2);

	return vec4(x, y, z, 1);
}


// -- Velocity --
////////////////////////////////////////////////////////////////////////////////
vec3 velocityGenerator(vec3 dir, vec3 randomize, float intensity)
{
	vec3 vel = dir;

	// check if any direction should be randomized
	if (randomize.x > 0) vel.x = randInRange(-1,1);
	if (randomize.y > 0) vel.y = randInRange(-1,1);
	if (randomize.z > 0) vel.z = randInRange(-1,1);

	// avoid normalizing a zero vector
	if (vel != vec3(0)) vel = normalize(vel);

	return vel * intensity;
}


////////////////////////////////////////////////////////////////////////////////
vec3 velocityGenerator(vec3 dir, vec3 randomize, float minInt, float maxInt)
{
	float intensity = randInRange(minInt, maxInt);

	return velocityGenerator(dir, randomize, intensity);
}

