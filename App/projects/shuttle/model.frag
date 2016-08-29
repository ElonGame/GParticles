
in vec3 lightDirV;

out vec4 color;

void main()
{    
	float intensity = max(0.0, dot(normalize(vertexNormalV), lightDirV));

	color = max(@colorsV * 0.15, @colorsV * intensity);
}