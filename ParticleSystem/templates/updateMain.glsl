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
}