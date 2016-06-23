#version 430
layout(points) in;
layout(triangle_strip, max_vertices=10) out;
//layout(points, max_vertices=15) out;
//layout(line_strip, max_vertices=2) out;

in float rocket_lifetimesV[];
in vec4 colorsV[];
in vec2 rocket_texCoordsV[];

out float rocket_lifetimesG;
out vec4 colorsG;
out vec2 rocket_texCoordsG;

uniform mat4 model, view, projection;

void getDirs(vec4 prevPos, vec4 pos, vec4 nextPos, out vec4 d1, out vec4 d2)
{
    // prev position
    vec4 dp1, dp2;
    dp1 = normalize(pos - prevPos).yxzw;
    dp2 = dp1;
    dp1.x *= -1;
    dp2.y *= -1;

	// next position
    vec4 dn1, dn2;
    dn1 = normalize(nextPos - pos).yxzw;
    dn2 = dn1;
    dn1.x *= -1;
    dn2.y *= -1;

    d1 = (dp1 + dn1) / 2.0;
    d2 = (dp2 + dn2) / 2.0;
}

void main()
{	
	rocket_lifetimesG = rocket_lifetimesV[0];
	colorsG = colorsV[0];

    gl_Position = projection * (gl_in[0].gl_Position + vec4(-0.1, -0.1, 0.0, 0.0));
    rocket_texCoordsG = vec2(0,0);
    EmitVertex();

    gl_Position = projection * (gl_in[0].gl_Position + vec4(-0.1, 0.1, 0.0, 0.0));
    rocket_texCoordsG = vec2(0,1);
    EmitVertex();

    gl_Position = projection * (gl_in[0].gl_Position + vec4(0.1, -0.1, 0.0, 0.0));
    rocket_texCoordsG = vec2(1,0);
    EmitVertex();

    gl_Position = projection * (gl_in[0].gl_Position + vec4(0.1, 0.1, 0.0, 0.0));
    rocket_texCoordsG = vec2(1,1);
    EmitVertex();

    /*gl_Position = projection * (gl_in[0].gl_Position);
    EmitVertex();*/

	/*gl_Position = projection * view * (vec4(0,0,-1,1));
    EmitVertex();

    gl_Position = projection * view * (gl_in[0].gl_Position);
    EmitVertex();*/

    /*float dist = 0.1;
    vec4 pArray[5];
    pArray[0] = vec4(-0.5,0,-1,1);
    pArray[1] = vec4(-0.25,0.15,-1,1);
    pArray[2] = vec4(0.0,0.45,-1,1);
    pArray[3] = vec4(0.25,1,-1,1);
    pArray[4] = vec4(0.5,2,-1,1);*/

	/*gl_Position = projection * (pArray[0]);
    EmitVertex();

	gl_Position = projection * (pArray[1]);
    EmitVertex();

	gl_Position = projection * (pArray[2]);
    EmitVertex();

	gl_Position = projection * (pArray[3]);
    EmitVertex();

	gl_Position = projection * (pArray[4]);
    EmitVertex();*/

    /*vec4 d1, d2;

    d1 = normalize(pArray[1] - pArray[0]).yxzw;
    d2 = d1;
    d1.x *= -1;
    d2.y *= -1;

    gl_Position = projection * view * (pArray[0] + d1*dist);
    EmitVertex();

    gl_Position = projection * view * (pArray[0] + d2*dist);
    EmitVertex();

    for (int i = 1; i<4; i++)
    {
    	getDirs(pArray[i-1], pArray[i], pArray[i+1], d1, d2);

	    gl_Position = projection * view * (pArray[i] + d1*dist);
	    EmitVertex();

	    gl_Position = projection * view * (pArray[i] + d2*dist);
	    EmitVertex();
    }

    d1 = normalize(pArray[4] - pArray[3]).yxzw;
    d2 = d1;
    d1.x *= -1;
    d2.y *= -1;

    gl_Position = projection * view * (pArray[4] + d1*dist);
    EmitVertex();

    gl_Position = projection * view * (pArray[4] + d2*dist);
    EmitVertex();

    EndPrimitive();	*/
}  

