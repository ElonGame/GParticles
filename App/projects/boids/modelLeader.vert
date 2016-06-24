in vec3 position;
in vec3 normal;
in vec4 leader_positions;
in vec4 leader_lastPositions;
in vec4 leader_colors;

out vec4 colorsV;
out vec3 normalV;
out vec3 lightDirV;

uniform mat4 model, view, projection;

void main()
{
	vec3 dir = normalize((leader_positions - leader_lastPositions).xyz);
	mat4 newModel = construct3DSpace(dir, true, false);
	mat4 modelRot = rotationMatrix(vec3(1,0,0) , radians(90));

    gl_Position = projection * view * (newModel * modelRot * vec4(position.xyz, 0.0f) + model * leader_positions);
    colorsV = leader_colors;

    normalV = normalize(  mat3(transpose(inverse(view * model))) * mat3(newModel) * mat3(modelRot)* normal);
	lightDirV = normalize(vec3(view *vec4(1,0.2,0.5,0)));
}