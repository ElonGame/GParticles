#version 430
in vec3 position;
in vec3 normal;
in vec2 texCoords;
in float rocket_lifetimes;
in vec4 rocket_positions;
in vec4 rocket_lastPositions;
in vec4 rocket_colors;

out vec2 TexCoords;
out vec4 colorsV;
out vec3 normalV;
out vec3 lightDirV;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 normalMatrix;

void main()
{
	vec3 dir = normalize((rocket_positions - rocket_lastPositions).xyz);
	/*vec3 fakeUp = (dir != vec3(0,1,0))? vec3(0,1,0) : vec3(1,1,0);*/
	vec3 right = normalize(cross(vec3(0,1,0), dir));
	vec3 newUp = normalize(cross(dir, right));
	mat4 newModel = mat4(	vec4(right, 0),
							vec4(newUp, 0),
							vec4(dir, 0),
							model[3]
						);

	float angle = -1.6;
	mat4 modelRot = mat4(	vec4(1,0,0,0),
							vec4(0,cos(angle), sin(angle), 0),
							vec4(0, -sin(angle), cos(angle), 0),
							vec4(0,0,0,1)
						);


    gl_Position = projection * view * (newModel * modelRot * vec4(position.xyz, 0.0f) + model * rocket_positions);
    TexCoords = texCoords;
    colorsV = rocket_colors;

    normalV = normalize( mat3(normalMatrix) * normal);
	lightDirV = normalize(mat3(view) * mat3(model) * normalize(vec3(0,1,0)));
}