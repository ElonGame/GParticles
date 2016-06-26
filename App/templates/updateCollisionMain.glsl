void collision()
{
	for (int i = 0; i < MAX_SPHERES; i++)
	{
		vec4 sphere = spheres[i];
		if (distance(@positions[gid].xyz, sphere.xyz) < sphere.w )
		{
			vec3 normal = normalize(@positions[gid].xyz - sphere.xyz);
			vec3 collisionPoint = sphere.xyz + normal * sphere.w;

			sphereCollision(sphere, collisionPoint, normal);
		}
	}

	for (int i = 0; i < MAX_PLANES; i++)
	{
		vec4 plane = planes[i];

		vec3 vCurrent = plane.xyz * @positions[gid].xyz;
		vec3 vLast = plane.xyz * @lastPositions[gid].xyz;
		float currentDist = vCurrent.x + vCurrent.y + vCurrent.z + plane.w;
		float previousDist = vLast.x + vLast.y + vLast.z + plane.w;

		// there is a collision if point "passed through" plane
		if (sign(currentDist) + sign(previousDist) == 0)
		{
			vec3 normal = normalize(-plane.xyz);
			vec3 collisionPoint = @positions[gid].xyz + @velocities[gid].xyz * currentDist;

			planeCollision(plane, collisionPoint, normal, currentDist);
		}
	}
}

void main()
{
	// if particle is not alive
	if (@lifetimes[gid] <= 0)
		return;

	// age particle
	@lifetimes[gid] -= @deltaTime;

	// if particle just died
	if (@lifetimes[gid] <= 0)
	{
		@lifetimes[gid] = -1;
		atomicCounterDecrement(@aliveParticles);

		return;
	}
	
	update();

	collision();
}