#version 430
#extension GL_ARB_compute_shader : enable
#extension GL_ARB_shader_storage_buffer_object : enable
#extension GL_ARB_compute_variable_group_size : enable

////////////////////////////////////////////////////////////////////////////////
// RESOURCES
////////////////////////////////////////////////////////////////////////////////

layout(binding = 0, offset = 0) uniform atomic_uint virus_aliveParticles;
layout(binding = 1, offset = 0) uniform atomic_uint virus_emissionAttempts;
layout(binding = 2, offset = 0) uniform atomic_uint randomCounter;

layout(std430, binding = 3) buffer Virus_velocities
{
	vec4 virus_velocities[];
};

layout(std430, binding = 4) buffer Virus_floorHit
{
	float virus_floorHit[];
};

layout(std430, binding = 5) buffer Virus_lastPositions
{
	vec4 virus_lastPositions[];
};

layout(std430, binding = 6) buffer Virus_lifetimes
{
	float virus_lifetimes[];
};

layout(std430, binding = 7) buffer Virus_size
{
	float virus_size[];
};

layout(std430, binding = 8) buffer Virus_lineLifetime
{
	float virus_lineLifetime[];
};

layout(std430, binding = 9) buffer Virus_positions
{
	vec4 virus_positions[];
};

layout(std430, binding = 10) buffer Virus_colors
{
	vec4 virus_colors[];
};

layout(std430, binding = 11) buffer Virus_texCoords
{
	vec2 virus_texCoords[];
};

layout(std430, binding = 12) buffer VirusPos
{
	vec4 virusPos[];
};

layout(local_size_variable) in;

uniform float virus_maxParticles;
uniform float virus_toCreate;
uniform float virus_deltaTime;
uniform float virusAnimationRadius;
uniform float virusAnimationAngle;
uniform vec2 mouseXY;



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
	vec2 pos = vec2(virusAnimationRadius * sin(radians(virusAnimationAngle)),
					virusAnimationRadius * cos(radians(virusAnimationAngle)));

	virus_positions[gid].xz = pos;

	// store virus position in global buffer so puddle particles can access it
	virusPos[0] = virus_positions[gid];
}

void collision()
{
	;
}

void main()
{
	// if particle is not alive
	if (virus_lifetimes[gid] <= 0)
		return;

	// age particle
	virus_lifetimes[gid] -= virus_deltaTime;

	// if particle just died
	if (virus_lifetimes[gid] <= 0)
	{
		virus_lifetimes[gid] = -1;
		atomicCounterDecrement(virus_aliveParticles);

		return;
	}
	
	update();

	collision();
}