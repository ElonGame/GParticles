
in vec4 colorsV;
in vec3 normalV;
in vec3 lightDirV;

out vec4 color;

void main()
{    
	vec3 n = normalize(normalV);
	float intensity = max(0.0, dot(lightDirV, n));

    color = max(colorsV * 0.10, colorsV * intensity);
    color.a = 1.0;
}