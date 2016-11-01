void main()
{
	if (@lifetimes[gid] <= 0)
		return;
	
	if (@collided[gid] == 1)
	{
		@lifetimes[gid] -= @deltaTime;

		if (@lifetimes[gid] <= 0)
		{
			@lifetimes[gid] = -1;
			atomicCounterDecrement(@aliveParticles);

			return;
		}
	}
	
	update();
}