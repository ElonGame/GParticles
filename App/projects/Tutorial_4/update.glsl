void update()
{
	@velocities[gid].y -= 0.4 * @deltaTime;
	@positions[gid].xyz += @velocities[gid].xyz * @deltaTime;
	@colors[gid].xyz = vec3(0.1, 0.9, 0.1);
}