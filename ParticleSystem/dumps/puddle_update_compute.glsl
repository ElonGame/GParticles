#version 430
#extension GL_ARB_compute_shader : enable
#extension GL_ARB_shader_storage_buffer_object : enable
#extension GL_ARB_compute_variable_group_size : enable

////////////////////////////////////////////////////////////////////////////////
// RESOURCES
////////////////////////////////////////////////////////////////////////////////

layout(binding = 0, offset = 0) uniform atomic_uint puddle_aliveParticles;
layout(binding = 1, offset = 0) uniform atomic_uint puddle_emissionAttempts;
layout(binding = 2, offset = 0) uniform atomic_uint randomCounter;

layout(std430, binding = 3) buffer Puddle_velocities
{
	vec4 puddle_velocities[];
};

layout(std430, binding = 4) buffer Puddle_floorHit
{
	float puddle_floorHit[];
};

layout(std430, binding = 5) buffer Puddle_lastPositions
{
	vec4 puddle_lastPositions[];
};

layout(std430, binding = 6) buffer Puddle_lifetimes
{
	float puddle_lifetimes[];
};

layout(std430, binding = 7) buffer Puddle_size
{
	float puddle_size[];
};

layout(std430, binding = 8) buffer Puddle_lineLifetime
{
	float puddle_lineLifetime[];
};

layout(std430, binding = 9) buffer Puddle_positions
{
	vec4 puddle_positions[];
};

layout(std430, binding = 10) buffer Puddle_colors
{
	vec4 puddle_colors[];
};

layout(std430, binding = 11) buffer Puddle_texCoords
{
	vec2 puddle_texCoords[];
};

layout(std430, binding = 12) buffer VirusPos
{
	vec4 virusPos[];
};

layout(local_size_variable) in;

uniform float puddle_maxParticles;
uniform float puddle_toCreate;
uniform float puddle_deltaTime;
uniform float virusAnimationRadius;
uniform float virusAnimationAngle;
uniform vec2 mouseXY;


const uint MAX_SPHERES = 2;
const vec4 spheres[MAX_SPHERES] =
{
	vec4(0.000000, -2.000000, -3.000000, 1.000000),
	vec4(-3.000000, -2.000000, 0.000000, 1.000000)
};

const uint MAX_PLANES = 1;
const vec4 planes[MAX_PLANES] =
{
	vec4(0.000000, 1.000000, 0.000000, 3.000000)
};


const uint gid = gl_GlobalInvocationID.x;



////////////////////////////////////////////////////////////////////////////////
// UTILITIES
////////////////////////////////////////////////////////////////////////////////

// Random Number Generation in range [-1,1]
////////////////////////////////////////////////////////////////////////////////
// Description : Array and textureless GLSL 2D simplex noise function.
//      Author : Ian McEwan, Ashima Arts.
//  Maintainer : ijm
//     Lastmod : 20110822 (ijm)
//     License : Copyright (C) 2011 Ashima Arts. All rights reserved.
//               Distributed under the MIT License. See LICENSE file.
//               https://github.com/ashima/webgl-noise

vec3 mod289(vec3 x) {
 	return x - floor(x * (1.0 / 289.0)) * 289.0;
}

vec2 mod289(vec2 x) {
 	return x - floor(x * (1.0 / 289.0)) * 289.0;
}

vec3 permute(vec3 x) {
 	return mod289(((x*34.0)+1.0)*x);
}

float snoise(vec2 v) {
 	const vec4 C = vec4(0.211324865405187,  // (3.0-sqrt(3.0))/6.0
					  	0.366025403784439,  // 0.5*(sqrt(3.0)-1.0)
						-0.577350269189626,  // -1.0 + 2.0 * C.x
						0.024390243902439); // 1.0 / 41.0
// First corner
 	vec2 i  = floor(v + dot(v, C.yy) );
	vec2 x0 = v -   i + dot(i, C.xx);

// Other corners
	vec2 i1;
  	//i1.x = step( x0.y, x0.x ); // x0.x > x0.y ? 1.0 : 0.0
  	//i1.y = 1.0 - i1.x;
  	i1 = (x0.x > x0.y) ? vec2(1.0, 0.0) : vec2(0.0, 1.0);
  	// x0 = x0 - 0.0 + 0.0 * C.xx ;
  	// x1 = x0 - i1 + 1.0 * C.xx ;
  	// x2 = x0 - 1.0 + 2.0 * C.xx ;
  	vec4 x12 = x0.xyxy + C.xxzz;
  	x12.xy -= i1;

// Permutations
  	i = mod289(i); // Avoid truncation effects in permutation
  	vec3 p = permute(permute( i.y + vec3(0.0, i1.y, 1.0 )) +
					 i.x + vec3(0.0, i1.x, 1.0 ));

 	vec3 m = max(0.5 - vec3(dot(x0,x0), dot(x12.xy,x12.xy), dot(x12.zw,x12.zw)), 0.0);
  	m = m*m ;
  	m = m*m ;

// Gradients: 41 points uniformly over a line, mapped onto a diamond.
// The ring size 17*17 = 289 is close to a multiple of 41 (41*7 = 287)

  	vec3 x = 2.0 * fract(p * C.www) - 1.0;
  	vec3 h = abs(x) - 0.5;
  	vec3 ox = floor(x + 0.5);
  	vec3 a0 = x - ox;

// Normalise gradients implicitly by scaling m
// Approximation of: m *= inversesqrt( a0*a0 + h*h );
  	m *= 1.79284291400159 - 0.85373472095314 * ( a0*a0 + h*h );

// Compute final noise value at P
  	vec3 g;
  	g.x  = a0.x  * x0.x  + h.x  * x0.y;
  	g.yz = a0.yz * x12.xz + h.yz * x12.yw;
  	return 130.0 * dot(m, g);
}



// Returns a random float in the given range
////////////////////////////////////////////////////////////////////////////////
float randInRange(float minVal, float maxVal)
{
	float val = snoise(vec2(atomicCounterIncrement(randomCounter)));
	float percentage = val * 0.5 + 0.5;
	return mix(minVal, maxVal, percentage);
}



// Returns a vec2 with random components in the given range
////////////////////////////////////////////////////////////////////////////////
vec2 randInRangeV2(float minVal, float maxVal)
{
	return vec2(randInRange(minVal, maxVal),
				randInRange(minVal, maxVal));
}



// Returns a vec3 with random components in the given range
////////////////////////////////////////////////////////////////////////////////
vec3 randInRangeV3(float minVal, float maxVal)
{
	return vec3(randInRange(minVal, maxVal),
				randInRange(minVal, maxVal),
				randInRange(minVal, maxVal));
}



// Returns a matrix that describes the rotation around an arbitrary axis by the
// given angle in radians
////////////////////////////////////////////////////////////////////////////////
mat4 rotationMatrix(vec3 axis, float angle)
{
	axis = normalize(axis);
	float s = sin(angle);
	float c = cos(angle);
	float oc = 1.0 - c;
	
	return mat4(oc * axis.x * axis.x + c,           oc * axis.x * axis.y - axis.z * s,  oc * axis.z * axis.x + axis.y * s,  0.0,
				oc * axis.x * axis.y + axis.z * s,  oc * axis.y * axis.y + c,           oc * axis.y * axis.z - axis.x * s,  0.0,
				oc * axis.z * axis.x - axis.y * s,  oc * axis.y * axis.z + axis.x * s,  oc * axis.z * axis.z + c,           0.0,
				0.0,                                0.0,                                0.0,                                1.0);
}



// Returns a matrix constructed from the given direction axis that holds the
// axis of a 3D space
////////////////////////////////////////////////////////////////////////////////
mat4 construct3DSpace(vec3 dir, bool flipRight, bool flipUp)
{
	dir = normalize(dir);
	vec3 right, up;

	if (flipRight)
	  right = normalize(cross(vec3(0, 1, 0), dir));
	else
	  right = normalize(cross(dir, vec3(0, 1, 0)));

	if (flipUp)
	  up = normalize(cross(right, dir));
	else
	  up = normalize(cross(dir, right));
	
	return mat4(vec4(right, 0),
				vec4(up, 0),
				vec4(dir, 0),
				vec4(0, 0, 0, 1));
}

void update()
{
	// if particle hasn't hit the floor update its position and velocity
	if (puddle_floorHit[gid] == 0)
	{
		puddle_lastPositions[gid].xyz = puddle_positions[gid].xyz;

		// add a little "gravity"
		puddle_velocities[gid].y -= 0.5 * puddle_deltaTime;
		puddle_positions[gid].xyz += puddle_velocities[gid].xyz * puddle_deltaTime;
	}
}

void sphereCollision(vec4 sphere, vec3 collisionPoint, vec3 normal)
{
	// offset particle position to collision point
	puddle_positions[gid].xyz = collisionPoint;
	puddle_velocities[gid].xyz = reflect(puddle_velocities[gid].xyz, normal) * 0.85;
}

void planeCollision(vec4 plane, vec3 collisionPoint, float distance)
{
	if (distance < 0)
	{
		puddle_floorHit[gid] = 1;
		puddle_positions[gid].xyz = collisionPoint;
		puddle_lifetimes[gid] = 1;
	}
}

void collision()
{
	for (int i = 0; i < MAX_SPHERES; i++)
	{
		vec4 sphere = spheres[i];
		if (distance(puddle_positions[gid].xyz, sphere.xyz) < sphere.w )
		{
			vec3 normal = normalize(puddle_positions[gid].xyz - sphere.xyz);
			vec3 collisionPoint = sphere.xyz + normal * sphere.w;

			sphereCollision(sphere, collisionPoint, normal);
		}
	}

	for (int i = 0; i < MAX_PLANES; i++)
	{
		vec4 plane = planes[i];

		vec3 vCurrent = plane.xyz * puddle_positions[gid].xyz;
		vec3 vLast = plane.xyz * puddle_lastPositions[gid].xyz;
		float currentDist = vCurrent.x + vCurrent.y + vCurrent.z + plane.w;
		float previousDist = vLast.x + vLast.y + vLast.z + plane.w;

		// there is a collision if point "passed through" plane
		if (sign(currentDist) + sign(previousDist) == 0)
		{
			vec3 collisionPoint = puddle_positions[gid].xyz + puddle_velocities[gid].xyz * currentDist;

			planeCollision(plane, collisionPoint, currentDist);
		}
	}
}

void main()
{
	// if particle is not alive
	if (puddle_lifetimes[gid] <= 0)
		return;

	// age particle
	puddle_lifetimes[gid] -= puddle_deltaTime;

	// if particle just died
	if (puddle_lifetimes[gid] <= 0)
	{
		puddle_lifetimes[gid] = -1;
		atomicCounterDecrement(puddle_aliveParticles);

		return;
	}
	
	update();

	collision();
}