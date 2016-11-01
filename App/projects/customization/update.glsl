void update()
{
	@positions[gid].xyz += @velocities[gid].xyz * @deltaTime;

	/*@velocities[gid].y -= 1 * @deltaTime;
	if (@positions[gid].y < -4)
	{
		@velocities[gid].y *= -0.1;
		@collided[gid] = 1;
	}*/
}