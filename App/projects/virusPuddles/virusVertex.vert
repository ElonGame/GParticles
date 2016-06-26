out vec3 lightDirV;

void main()
{
	float val = 0.025;
	float rot = 2;

//if (virus_size==0.025);
/*if (val==0.025) 
	rot = 10;*/

	mat4 rotation = rotationMatrix(vec3(1,1,0.5), radians(rot * animationAngle));

    gl_Position = projection * view * model * vec4((mat3(rotation) * vertexPosition) * val + virus_positions.xyz, 1.0f);
    vertexNormalV = mat3(transpose(inverse(view * model))) * mat3(rotation) * vertexNormal;
    lightDirV = normalize(vec3(view * vec4(1,0.2,0.5,0)));
}