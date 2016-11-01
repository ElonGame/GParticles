out vec3 lightDirV;

void main()
{

  vec3 pos = @positions.xyz;
  pos.z -= 14;

  mat4 rotation = rotationMatrix(vec3(0,1,0), radians(47));

  float a = distance(@initialPositions.xyz, @positions.xyz);
  mat4 detachRot1 = rotationMatrix(normalize(vec3(0,0,1)), radians(10 * a));
  mat4 detachRot2 = rotationMatrix(normalize(vec3(1,0,0)), radians(1 * a));
  
  gl_Position = projection * view * model * vec4((mat3(rotation) * mat3(detachRot2) * mat3(detachRot1) * vertexPosition) * elemScale + mat3(rotation) * pos, 1.0f);
  vertexNormalV = mat3(transpose(inverse(view * model))) * mat3(rotation) * mat3(detachRot2) * mat3(detachRot1) * vertexNormal;
  lightDirV = normalize(vec3(view * vec4(0.3,0.3,1,0)));
  @colorsV = @colors;
}