
uniform sampler2D ourTexture;

out vec4 LFragment;

void main()
{
	if (puddle_lifetimesG <= 0)
		discard;

	LFragment = texture(ourTexture, puddle_texCoordsG);
	LFragment.xyz = puddle_colorsG.xyz;
}