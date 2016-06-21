void main()
{
	// if maxParticles limit has been reached
	if (atomicCounter(@aliveParticles) >= @maxParticles)
		return;

	// if particle is not currently alive at this gid another can be emitted
	if (@lifetimes[gid] <= 0)
	{
		// if max number of particles toCreate on current iteration has not
		// been reached emitt a new particle
		if (atomicCounterIncrement(@emissionAttempts) < @toCreate)
		{
			atomicCounterIncrement(@aliveParticles);

			emission();
		}
	}
}