#version 430

uniform float testuni;

in float lifetimesV;
in vec4 colorsV;

out vec4 LFragment;

void main()
{
	if (lifetimesV <= 0)
		discard;

	LFragment = colorsV;
	/*if (testuni == 10)
		LFragment = vec4( 1, 1, 1, 1.0 );	*/

}