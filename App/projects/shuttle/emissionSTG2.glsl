void emission()
{
	@lifetimes[gid] = 3600;
	@positions[gid].w = 1;
	@colors[gid] = vec4(0.9,0.9,0.1,1);
}