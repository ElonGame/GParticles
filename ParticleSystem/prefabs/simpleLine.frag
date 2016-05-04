#version 430

in float lifetimesG;
in vec4 colorsG;

out vec4 fragColor;

void main()
{
	if (lifetimesG <= 0)
		discard;

	fragColor = colorsG;
}