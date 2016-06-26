void main()
{
	@colorsV = @colors;
	gl_Position = view * model * @positions;
}