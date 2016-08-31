void update()
{
	@positions[gid].xyz += 3 * @velocities[gid].xyz * @deltaTime;
}