in vec3 lightDirV;

out vec4 color;
const vec4 diffuse = vec4(0.1,0.9,0.9,1);

void main()
{    
	/*vec3 n = normalize(vertexNormalV);
	float intensity = max(0.0, dot(lightDirV, n));

    color = max(@colorsV * 0.10, @colorsV * intensity);
    color.a = 1.0;*/

	float intensity = max(0.0, dot(normalize(vertexNormalV), lightDirV));

	color = max(@colorsV * 0.25, @colorsV * intensity);
}