void main()
{
	@colorsV = @colors;
	gl_Position = projection * view * @positions;
}