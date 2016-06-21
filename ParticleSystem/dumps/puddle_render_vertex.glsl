#version 430

////////////////////////////////////////////////////////////////////////////////
// RESOURCES
////////////////////////////////////////////////////////////////////////////////

layout(binding = 1, offset = 0) uniform atomic_uint puddle_aliveParticles;
layout(binding = 2, offset = 0) uniform atomic_uint puddle_emissionAttempts;
layout(binding = 3, offset = 0) uniform atomic_uint randomCounter;

in vec4 puddle_velocities;
in float puddle_floorHit;
in vec4 puddle_lastPositions;
in float puddle_lifetimes;
in float puddle_size;
in float puddle_lineLifetime;
in vec4 puddle_positions;
in vec4 puddle_colors;
in vec2 puddle_texCoords;
in vec4 virusPos;

out vec4 puddle_velocitiesV;
out float puddle_floorHitV;
out vec4 puddle_lastPositionsV;
out float puddle_lifetimesV;
out float puddle_sizeV;
out float puddle_lineLifetimeV;
out vec4 puddle_positionsV;
out vec4 puddle_colorsV;
out vec2 puddle_texCoordsV;
out vec4 virusPosV;
uniform mat4 model, view, projection;

uniform float puddle_maxParticles;
uniform float puddle_toCreate;
uniform float puddle_deltaTime;
uniform float virusAnimationRadius;
uniform float virusAnimationAngle;
uniform vec2 mouseXY;


void main()
{
	puddle_velocitiesV = puddle_velocities;
	puddle_lifetimesV = puddle_lifetimes;
	puddle_colorsV = puddle_colors;
	puddle_floorHitV = puddle_floorHit;
	gl_Position = view * model * vec4(puddle_positions.xyz, 1);
}