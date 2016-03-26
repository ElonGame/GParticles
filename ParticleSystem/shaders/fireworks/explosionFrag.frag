#version 430

in float explosion_lifetimesV;
in vec4 colorsV;

out vec4 LFragment;

void main()
{
	if (explosion_lifetimesV <= 0)
		discard;

	LFragment = colorsV;
}