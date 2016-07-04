void main()
{
	@colorsV = @colors;

//	gl_Position = (@state == 1) ? view * @positions : @positions;
	gl_Position =  view * @positions;
	//gl_Position = (@state == 1) ? view * @positions : @positions + camPos;
}