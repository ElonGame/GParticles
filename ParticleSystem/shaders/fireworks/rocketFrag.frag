#version 430

in float rocket_lifetimesV;
in vec4 colorsV;

out vec4 LFragment;

void main()
{
	if (rocket_lifetimesV <= 0)
		discard;

	LFragment = colorsV;
}