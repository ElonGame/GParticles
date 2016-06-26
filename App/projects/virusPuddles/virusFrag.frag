
in vec3 lightDirV;

out vec4 color;

const vec4 diffuse = vec4(0,1,0,1);

void main()
{    
	float intensity = max(0.0, dot(normalize(vertexNormalV), lightDirV));

	color = max(diffuse * 0.25, diffuse * intensity);
}