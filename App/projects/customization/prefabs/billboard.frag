uniform sampler2D particleTexture;

out vec4 fragColor;

void main()
{
	if (@lifetimesG <= 0)
		discard;

	fragColor = texture(particleTexture, @texCoordsG);
	fragColor.xyz = @colorsG.xyz;
}