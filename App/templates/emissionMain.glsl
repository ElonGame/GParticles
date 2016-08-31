void main()
{
	// if maxParticles limit has been reached
	if (atomicCounter(@aliveParticles) >= @maxParticles)
		return;

	// if particle dead at this index, another might be allowed to be emitted
	if (@lifetimes[gid] <= 0)
	{
		// if max number of particles to be emitted on current iteration has
		// not been reached, call emission and emit a new particle
		if (atomicCounterIncrement(@emissionAttempts) < @toCreate * @deltaTime)
		{
			atomicCounterIncrement(@aliveParticles);

			emission();
		}
	}
}