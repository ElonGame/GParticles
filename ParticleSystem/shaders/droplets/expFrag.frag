#version 430

in float exp_lifetimesV;
in vec4 colorsV;

out vec4 LFragment;

void main()
{
	if (exp_lifetimesV <= 0)
		discard;

	LFragment = vec4(colorsV.xyz, exp_lifetimesV);
}