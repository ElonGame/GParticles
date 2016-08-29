////////////////////////////////////////////////////////////////////////////////
// EMISSION - requires: utilities.glsl
////////////////////////////////////////////////////////////////////////////////

// Returns particle position inside a cone primitive
////////////////////////////////////////////////////////////////////////////////
vec4 conePositionGenerator(float radius, float height, bool coneBaseIsOrigin,
						   bool positionsInVolume)
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



// Returns particle position inside a sphere primitive
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



// Returns particle position from a disc primitive
////////////////////////////////////////////////////////////////////////////////
vec4 discPositionGenerator(vec3 direction, float radius, bool positionsInDisc)
{
  vec3 perpendicular = normalize(cross(direction, vec3(0, 1, 0)));

  float angle = randInRange(0, 450);

  float dist = radius;
  if (positionsInDisc)
  	dist = randInRange(0, radius);

	vec4 final = vec4(perpendicular * dist, 1);

  return rotationMatrix(direction, radians(angle)) * final;
}



// Returns particle position from a plane primitive
////////////////////////////////////////////////////////////////////////////////
vec4 planePositionGenerator(float width, float height, bool centeredAtOrigin,
							bool horizontal)
{
	float minMultiplier = 0;
	float maxMultiplier = 1;
	if (centeredAtOrigin)
	{
		minMultiplier = 0.5;
		maxMultiplier = 0.5;
	}

	vec4 pos = vec4(0,0,0,1);

	pos.x = randInRange(-width * minMultiplier, width * maxMultiplier);
	pos.y = randInRange(-height * minMultiplier, height * maxMultiplier);

	if (horizontal)
		pos.zy = pos.yz;

	return pos;
}



// Returns velocity vector with magnitude = intensity
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



// Returns velocity vector with random magnitude (minInt < magnitude < maxInt)
////////////////////////////////////////////////////////////////////////////////
vec3 velocityGenerator(vec3 dir, vec3 randomize, float minInt, float maxInt)
{
	float intensity = randInRange(minInt, maxInt);

	return velocityGenerator(dir, randomize, intensity);
}