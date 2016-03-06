#version 430

uniform float alive;

in float lifetimeV;

out vec4 LFragment;

void main()
{
	if (lifetimeV > 0)
		LFragment = vec4( 1.0, 1.0, 1.0, 1.0 );
	else 
		discard;
}