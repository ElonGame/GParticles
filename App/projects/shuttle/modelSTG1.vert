out vec3 lightDirV;

void main()
{
	vec3 pos = @positions.xyz;
	pos.z -= 14;

	mat4 rotation = rotationMatrix(vec3(0,1,0), radians(47/*mouseXY.x * 0.25*/));

  float a = distance(@initialPositions.xyz, @positions.xyz);
  vec3 right = normalize(cross(normalize(pos), vec3(0, 0, 1)));
	mat4 detachRot = rotationMatrix(right, radians(@detachRotSpeed * a));


  gl_Position = projection * view * model * vec4((mat3(rotation) * mat3(detachRot) * vertexPosition) * elemScale + mat3(rotation) * pos, 1.0f);
  vertexNormalV = mat3(transpose(inverse(view * model))) * mat3(rotation) * mat3(detachRot) * vertexNormal;
  lightDirV = normalize(vec3(view * vec4(0.3,0.3,1,0)));
  @colorsV = @colors;
}