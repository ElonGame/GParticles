#version 430

////////////////////////////////////////////////////////////////////////////////
// RESOURCES
////////////////////////////////////////////////////////////////////////////////

layout(binding = 1, offset = 0) uniform atomic_uint simple_aliveParticles;
layout(binding = 2, offset = 0) uniform atomic_uint simple_emissionAttempts;
layout(binding = 3, offset = 0) uniform atomic_uint randomCounter;

in float simple_lifetimesV;
in vec4 simple_lastPositionsV;
in float simple_lineLifetimeV;
in float simple_sizeV;
in vec4 simple_positionsV;
in vec4 simple_velocitiesV;
in vec4 simple_colorsV;
in vec2 simple_texCoordsV;

uniform mat4 model, view, projection;

uniform float simple_maxParticles;
uniform float simple_toCreate;
uniform float simple_deltaTime;
uniform vec2 mouseXY;





out vec4 fragColor;

void main()
{
	if (simple_lifetimesV <= 0)
		discard;

	fragColor = simple_colorsV;
}