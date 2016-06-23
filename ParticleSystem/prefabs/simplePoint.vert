void main()
{
	simple_lifetimesV = simple_lifetimes;
	simple_colorsV = simple_colors;
	gl_Position = projection * view * model * vec4(simple_positions.xyz, 1);
}