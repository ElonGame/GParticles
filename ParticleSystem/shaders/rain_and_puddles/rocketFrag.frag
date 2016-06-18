
in float rocket_lifetimesG;
in vec4 colorsG;
in vec2 rocket_texCoordsG;

uniform sampler2D ourTexture;

out vec4 LFragment;

void main()
{
	if (rocket_lifetimesG <= 0)
		discard;

	LFragment = colorsG;
	LFragment = texture(ourTexture, rocket_texCoordsG);
	LFragment.xyz = vec3(0.1, 0.8, 0.1);
}