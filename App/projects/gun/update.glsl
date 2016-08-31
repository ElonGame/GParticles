/*
n - state
-------------
0 - standby
1 - charging super shot
2 - firing
3 - hit
4 - reloading
*/

void superShot()
{
	// if space bar is pressed
	if (spaceHold == 1)
	{
		 // count hold time
		if (atomicCounterIncrement(holdTimeAttempts) == 0)
		{
			holdTime[0] += @deltaTime;
		}

		// should we add another charged particle?
		if (@state[gid] == 0 && chargedParticles[0].x < superParticles && 
			chargedParticles[0].x < floor(holdTime[0] / delayPerCharge) &&
			atomicCounterIncrement(chargeAttempts) == 0)
		{
			@state[gid] = 1;
			chargedParticles[0].x++;
			int chargeInd = int(chargedParticles[0].x) - 1;
			chargedParticles[chargeInd].y = gid;
			chargedParticles[chargeInd].z = 0;
		}
		
		if (@state[gid] == 1)
		{
			// set final particle position at the center of superShot
			vec4 offesetPos = camUp * 2;

			for (int i = 0; i < chargedParticles[0].x - 1; i++)
			{
				if (gid == chargedParticles[i].y)
				{
					// if not final, offset particle around the center of superShot
					if (i < superParticles)
					{
						chargedParticles[i].z += chargedAngle * 360 * @deltaTime;
						mat4 rot = rotationMatrix(-camDir.xyz, radians(chargedParticles[i].z));
						float radius = 1;

						offesetPos += rot * camUp;
					}
				}
			}

			@positions[gid] = gunPoint + offesetPos;
			@velocities[gid] = vec4(0);
		}
	}
	else if (@state[gid] == 1)
	{
		@state[gid] = 0;
		// if superShot fully charged
		if (holdTime[0] > superParticles * delayPerCharge)
		{
			// set particle's state to "firing"
			@state[gid] = 2;
			// set particle's new velocity vector
			@velocities[gid].xyz = vec3(camDir) * 100;
		}

		if (atomicCounterIncrement(holdTimeAttempts) + 1 == chargedParticles[0].x)
		{
			chargedParticles[0].x = 0;
			holdTime[0] = 0;
		}
	}
}



void singleShot()
{
	// once per iteration, decrease fire cooldown so we can fire again later
	if (atomicCounterIncrement(cooldownAttempts) == 0)
	{
		cooldown[0] -= @deltaTime;
		if (cooldown[0] < 0)
		{
			cooldown[0] = 0;
		}	
	}

	// if we can and want to fire a single particle
	if (@state[gid] == 0 && rightHold == 1 && cooldown[0] == 0 && atomicCounterIncrement(fireAttempts) == 0)
	{
		// set firing cooldown (we get to fire again when cooldown == 0)
		cooldown[0] = fireDelay;
		// set particle's state to "firing"
		@state[gid] = 2;
		// set particle's new velocity vector
		@velocities[gid].xyz = vec3(camDir) * 100;
	}
	else if (@state[gid] == 0)
	{
		float n = 0.8;
		vec3 randJit = vec3(randInRange(-n,n), randInRange(-n,n), randInRange(-n,n));
		vec3 attractionVec = (gunPoint.xyz + randJit) - (@positions[gid].xyz);

		if (length(attractionVec) < 1)
			@velocities[gid].xyz = clamp(@velocities[gid].xyz + attractionVec, vec3(-3), vec3(3));
		else
			@velocities[gid].xyz = clamp(@velocities[gid].xyz + attractionVec, vec3(-9), vec3(9));
	}
}


void doReload()
{
	if (@state[gid] == 3 && rHold == 1)	
	{
		@state[gid] = 4;
	}
	
	if (@state[gid] == 4)
	{
		vec3 attractionVec = gunPoint.xyz - @positions[gid].xyz;

		if (length(attractionVec) < 3)
		{
			@state[gid] = 0;
		}
		else
			@velocities[gid].xyz = clamp(normalize(attractionVec) * 30, vec3(-30), vec3(30));
	}
}


void update()
{
	doReload();

	superShot();

	singleShot();

	@lastPositions[gid] = @positions[gid];
	@positions[gid].xyz += @velocities[gid].xyz * @deltaTime;
}