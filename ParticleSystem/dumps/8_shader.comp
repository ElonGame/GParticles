#version 430

////////////////////////////////////////////////////////////////////////////////
// RESOURCES
////////////////////////////////////////////////////////////////////////////////

layout(binding = 1, offset = 0) uniform atomic_uint randomCounter;
layout(binding = 2, offset = 0) uniform atomic_uint puddle_aliveParticles;
layout(binding = 3, offset = 0) uniform atomic_uint puddle_emissionAttempts;

in float puddle_floorHitG;
in vec4 puddle_lastPositionsG;
in float puddle_lifetimesG;
in float puddle_sizeG;
in float puddle_lineLifetimeG;
in vec4 puddle_colorsG;
in vec4 puddle_positionsG;
in vec2 puddle_texCoordsG;
in vec4 puddle_velocitiesG;
in vec4 virusPosG;

uniform mat4 model, view, projection;


uniform sampler2D ourTexture;

out vec4 LFragment;

void main()
{
	if (puddle_lifetimesG <= 0)
		discard;

	LFragment = texture(ourTexture, puddle_texCoordsG);
	LFragment.xyz = puddle_colorsG.xyz;
}