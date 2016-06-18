in vec3 normalV;
in vec3 lightDirV;

out vec4 color;

const vec4 diffuse = vec4(0,1,0,1);

void main()
{    
	vec3 n = normalize(normalV);
	float intensity = max(0.0, dot(n, lightDirV));

	//color = max(vec4(normalV,1) * 0.25, vec4(normalV,1) * intensity);
	color = max(diffuse * 0.25, diffuse * intensity);
    //color = vec4(0.2, 0.7, 0.2, 0.9);
    //color = vec4(normalV, 1);
}