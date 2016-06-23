out vec4 fragColor;

void main()
{
	if (@lifetimesV <= 0)
		discard;

	fragColor = @colorsV;
}