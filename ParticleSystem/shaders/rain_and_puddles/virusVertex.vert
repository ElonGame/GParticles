in vec4 virus_positions;
in float virus_size;

in vec3 position;
in vec3 normal;

uniform mat4 model, view, projection, normalMatrix;

out vec3 normalV;
out vec3 lightDirV;

void main()
{
	float val = 0.025;
	float b = virus_size;

if (virus_size == 0.025) ;
//if (virus_size == 0.025) val = 0.025;
//else val = 1;

	mat4 rotation = rotationMatrix(vec3(1,1,0.5), radians(2 * virus_positions.w));

    gl_Position = projection * view * model * vec4((mat3(rotation) * position) * val + virus_positions.xyz, 1.0f);
    normalV = mat3(transpose(inverse(view * model))) * mat3(rotation) * normal;
    lightDirV = normalize(vec3(view * vec4(1,0.2,0.5,0)));
}