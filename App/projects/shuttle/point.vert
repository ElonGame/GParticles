void main()
{
	@lifetimesV = @lifetimes;

	mat4 rotation = rotationMatrix(vec3(0,1,0), radians(47/*mouseXY.x * 0.25*/));
	vec3 pos = @positions.xyz;
	pos.z -= 14;
	gl_Position = projection * view * model * vec4(mat3(rotation) * pos, 1);

	@colorsV = @colors;
}