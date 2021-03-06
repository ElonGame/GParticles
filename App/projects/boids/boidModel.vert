
out vec3 lightDirV;


void main()
{
	vec3 dir = normalize((@positions - @lastPositions).xyz);


    //gl_Position = projection * view * (newModel * modelRot * vec4(vertexPosition.xyz, 0.0f) + model * @positions);
    //gl_Position = projection * view * model * (vec4(mat3(rotation) * vertexPosition, 1) + @positions);

    // just newModel
    //gl_Position = vec4(0,0,0,1);//projection * view * (newModel * vec4(vertexPosition, 1) + model * @positions);*/

    //vertexNormalV = normalize(  mat3(transpose(inverse(view * model))) * mat3(newModel) * mat3(modelRot)* vertexNormal);
    //vertexNormalV = mat3(transpose(inverse(view * model))) * mat3(rotation) * vertexNormal;


    mat4 newModel = construct3DSpace(@positions.xyz, true, false);
    mat4 rotation = rotationMatrix(vec3(1,0,0) , radians(90));

    gl_Position = projection * view * (newModel * vec4(mat3(rotation) * vertexPosition, 1) + model * @positions);

    vertexNormalV = mat3(transpose(inverse(view * model))) * vec3(0,1,0);

	lightDirV = normalize(vec3(view *vec4(1,0.2,0.5,0)));
    @colorsV = @colors;


    //vertexNormalV = mat3(transpose(inverse(view * model))) * vertexNormal;
}