void emission()
{
	@lifetimes[gid] = 3600;
	@positions[gid].w = 1;
	@positions[gid].xyz = vec3(0,0,stg3ZOffset);
	@colors[gid] = vec4(0.9,0.9,0.1,1);
}