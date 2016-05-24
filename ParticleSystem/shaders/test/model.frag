#version 430

in vec4 colorsV;
in vec3 normalV;
in vec2 TexCoords;
in vec3 lightDirV;

out vec4 color;

uniform sampler2D texture_diffuse1;

void main()
{    
	vec3 n = normalize(normalV);
	float intensity = max(0.0, dot(lightDirV, n));

    color = vec4(texture(texture_diffuse1, TexCoords));
    color = max(colorsV * 0.10, colorsV * intensity);
    color.a = 1.0;
    //color.xyz = n;
}