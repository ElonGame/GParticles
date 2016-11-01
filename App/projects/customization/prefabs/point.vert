void main()
{
	@lifetimesV = @lifetimes;
	@colorsV = @colors;

	gl_Position = projection * view * model * vec4(@positions.xyz, 1);
}