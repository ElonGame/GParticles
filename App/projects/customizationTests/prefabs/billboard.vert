void main()
{
	@colorsV = @colors;
	@lifetimesV = @lifetimes;

	gl_Position =  view * model * @positions;
}