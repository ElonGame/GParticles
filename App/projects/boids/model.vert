in vec3 position;
in vec3 normal;
in vec4 rocket_positions;
in vec4 rocket_lastPositions;
in vec4 rocket_colors;

out vec4 colorsV;
out vec3 normalV;
out vec3 lightDirV;

uniform mat4 model, view, projection;

void main()
{
	vec3 dir = normalize((rocket_positions - rocket_lastPositions).xyz);
	mat4 newModel = construct3DSpace(dir, true, false);
	mat4 modelRot = rotationMatrix(vec3(1,0,0) , radians(90));

    gl_Position = projection * view * (newModel * modelRot * vec4(position.xyz, 0.0f) + model * rocket_positions);
    colorsV = rocket_colors;

    normalV = normalize(  mat3(transpose(inverse(view * model))) * mat3(newModel) * mat3(modelRot)* normal);
	lightDirV = normalize(vec3(view *vec4(1,0.2,0.5,0)));
}