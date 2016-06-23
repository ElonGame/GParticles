#version 430

////////////////////////////////////////////////////////////////////////////////
// RESOURCES
////////////////////////////////////////////////////////////////////////////////

layout(binding = 1, offset = 0) uniform atomic_uint simple_aliveParticles;
layout(binding = 2, offset = 0) uniform atomic_uint simple_emissionAttempts;
layout(binding = 3, offset = 0) uniform atomic_uint randomCounter;

in float simple_lifetimes;
in vec4 simple_lastPositions;
in float simple_lineLifetime;
in float simple_size;
in vec4 simple_positions;
in vec4 simple_velocities;
in vec4 simple_colors;
in vec2 simple_texCoords;

out float simple_lifetimesV;
out vec4 simple_lastPositionsV;
out float simple_lineLifetimeV;
out float simple_sizeV;
out vec4 simple_positionsV;
out vec4 simple_velocitiesV;
out vec4 simple_colorsV;
out vec2 simple_texCoordsV;
uniform mat4 model, view, projection;

uniform float simple_maxParticles;
uniform float simple_toCreate;
uniform float simple_deltaTime;
uniform vec2 mouseXY;





void main()
{
	simple_lifetimesV = simple_lifetimes;
	simple_colorsV = simple_colors;
	gl_Position = projection * view * model * vec4(simple_positions.xyz, 1);
}