////////////////////////////////////////////////////////////////////////////////
// values passed in as uniforms
float radius = 1.0;
float height = 1.0;
bool coneBaseIsOrigin = true;
bool positionsInVolume = false;

void conePositionGenerator()
{
	// compute random y in range [0, height]
	float y = randInRange(vec2(gid, gid * gid), 0, 1);

	// compute particle horizontal distance from cone axis
	float newRadius = (radius * y) / height;
	float dist = newRadius;
	if (positionsInVolume)
	{
		dist = randInRange(vec2(gid + 3 , gid + 4 * gid), 0, newRadius);
	}

	// compute rotation angle (in degrees) around cone axis
	float angle = randInRange(vec2(gid+7,gid+8*gid) * 0.01, 0, 360);

	// calculate corresponding x and z polar coordinates
	float x = dist * sin(angle);
	float z = dist * cos(angle);

	// compute cone orientation
	y = (coneBaseIsOrigin) ? height - y : y;

	explosion_positions[gid] = vec4(x, y, z, 1);
}

////////////////////////////////////////////////////////////////////////////////
bool computeFromOrigin = false;
bool randX = false;
bool randY = false;
bool randZ = false;
float intensity = 0.01;
bool randIntensity = false;
float maxInt = 0.01;
float minInt = 0;

void velocityGenerator()
{
	vec3 vel = vec3(0);

	// DIRECTION
	// check if velocity should be computed in regards of psystem origin
	if (computeFromOrigin) vel = explosion_positions[gid].xyz;

	// check if any direction should be randomized
	if (randX) vel.x = snoise(vec2(gid,gid*gid));
	if (randY) vel.y = snoise(vec2(gid+3,gid+4*gid));
	if (randZ) vel.z = snoise(vec2(gid+7,gid+8*gid) * 4);

	// avoid normalizing a zero vector
	if (vel != vec3(0)) vel = normalize(vel);

	// INTENSITY
	// check if intesity should be randomized
	if (randIntensity)
		intensity = randInRange(vec2(gid*gid,gid*gid), minInt, maxInt);

	explosion_velocities[gid].xyz = vel * intensity;
}