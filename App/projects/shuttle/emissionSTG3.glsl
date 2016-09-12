void emission()
{
	@lifetimes[gid] = 3600;
	@colors[gid] = vec4(0.9,0.9,0.1,1);
	
	@positions[gid] = vec4(0,0,stg3ZOffset, 1);
}