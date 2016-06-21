#version 430

////////////////////////////////////////////////////////////////////////////////
// RESOURCES
////////////////////////////////////////////////////////////////////////////////

layout(binding = 1, offset = 0) uniform atomic_uint virus_aliveParticles;
layout(binding = 2, offset = 0) uniform atomic_uint randomCounter;
layout(binding = 3, offset = 0) uniform atomic_uint virus_emissionAttempts;

in vec4 virus_velocitiesV;
in float virus_floorHitV;
in vec4 virus_lastPositionsV;
in vec4 virus_positionsV;
in float virus_lifetimesV;
in float virus_sizeV;
in float virus_lineLifetimeV;
in vec4 virus_colorsV;
in vec2 virus_texCoordsV;
in vec4 virusPosV;

uniform mat4 model, view, projection;

uniform float virusAnimationRadius;
uniform float virus_maxParticles;
uniform float virus_toCreate;
uniform float virus_deltaTime;
uniform float virusAnimationAngle;
uniform vec2 mouseXY;


in vec3 normalV;
in vec3 lightDirV;

out vec4 color;

const vec4 diffuse = vec4(0,1,0,1);

void main()
{    
	/*if (virus_lifetimesV <= 0)
		discard;*/

	vec3 n = normalize(normalV);
	float intensity = max(0.0, dot(n, lightDirV));

	color = max(diffuse * 0.25, diffuse * intensity);
}