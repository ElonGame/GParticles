out vec3 lightDirV;

void main()
{
		mat4 rotation =rotationMatrix(vec3(0,1,0), radians(180));
		vec3 pos = @positions.xyz;
		pos.z -= 14;
		
    gl_Position = projection * view * model * vec4((mat3(rotation) * vertexPosition) * elemScale + mat3(rotation) * pos, 1.0f);
    vertexNormalV = mat3(transpose(inverse(view * model))) * mat3(rotation) * vertexNormal;
    lightDirV = normalize(vec3(view * vec4(0.3,0.3,1,0)));
    @colorsV = @colors;
}