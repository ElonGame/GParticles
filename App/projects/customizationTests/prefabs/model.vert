void main()
{
	@colorsV = @colors;
	vec4 surfacePointPos = vec4(vertexPosition,1) + @positions;
	gl_Position = projection * view * model * surfacePointPos;
}