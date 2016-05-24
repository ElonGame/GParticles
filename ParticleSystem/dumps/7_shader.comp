#version 430

in float lifetimesV;
in vec4 colorsV;

out vec4 fragColor;

void main()
{
	if (lifetimesV <= 0)
		discard;

	fragColor = colorsV;
}