#version 430
layout(points) in;
layout(line_strip, max_vertices=2) out;

in float lifetimesV[];
in vec4 rocket_lastPositionsV[];
in vec4 colorsV[];
in vec2 rocket_texCoordsV[];

out float lifetimesG;
out vec4 colorsG;

uniform mat4 model, view, projection;


void main()
{	
	lifetimesG = lifetimesV[0];
	colorsG = colorsV[0];

    gl_Position = projection * view * model * (rocket_lastPositionsV[0]);
    EmitVertex();

    gl_Position = projection * view * (gl_in[0].gl_Position);
    EmitVertex();

    EndPrimitive();
}  

