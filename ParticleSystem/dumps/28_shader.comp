#version 430

in float lifetimesV;
in vec4 colorsV;

out vec4 LFragment;

void main()
{
	if (lifetimesV <= 0)
		discard;

	LFragment = colorsV;
}