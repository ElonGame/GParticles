#version 430

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

in vec4 position;
in float lifetime;

out float lifetimeV;

void main()
{
	lifetimeV = lifetime;
	vec4 projectedCoords = projection * view * model * vec4(position.xyz, 1);
	gl_Position = projectedCoords;
	/*gl_PointSize = (position.z + 1.0) * 0.5 * 10;*/
}