void sphereCollision(vec4 sphere, vec3 collisionPoint, vec3 normal)
{
	
}

void planeCollision(vec4 plane, vec3 collisionPoint, vec3 normal, float distance)
{
	if (@state[gid] == 2)
	{
		@state[gid] = 3;
		@positions[gid].xyz = collisionPoint;
		@velocities[gid].xyz = vec3(0);
	}
}