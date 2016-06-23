out vec4 fragColor;

void main()
{
	if (simple_lifetimesV <= 0)
		discard;

	fragColor = simple_colorsV;
}