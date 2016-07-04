uniform sampler2D particleTexture;

out vec4 fragColor;

void main()
{
	fragColor = texture(particleTexture, @texCoordsG);
	fragColor.xyz = @colorsG.xyz;
}