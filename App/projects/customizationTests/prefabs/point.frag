out vec4 fragColor;

void main()
{
	if (@lifetimesV <= 0)
		discard;

	fragColor = vec4(@colorsV.xyz, @lifetimesV);
}