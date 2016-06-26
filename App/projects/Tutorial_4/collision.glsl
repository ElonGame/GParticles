void sphereCollision(vec4 sphere, vec3 collisionPoint, vec3 normal)
{
	@colors[gid].xyz = vec3(0.9, 0.1, 0.1);
}

void planeCollision(vec4 plane, vec3 collisionPoint, vec3 normal, float distance)
{
	@positions[gid].xyz = collisionPoint;
	@velocities[gid].xyz = reflect(@velocities[gid].xyz, normal) * 0.85;
}