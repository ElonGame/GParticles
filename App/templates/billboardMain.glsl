layout(points) in;
layout(triangle_strip, max_vertices=4) out;


void main()
{	
    vec4 quad[4] = transform();

    // generate final vertex positions
    gl_Position = projection * (gl_in[0].gl_Position + quad[0]);
    @texCoordsG = vec2(0,0);
    EmitVertex();

    gl_Position = projection * (gl_in[0].gl_Position + quad[1]);
    @texCoordsG = vec2(0,1);
    EmitVertex();

    gl_Position = projection * (gl_in[0].gl_Position + quad[2]);
    @texCoordsG = vec2(1,0);
    EmitVertex();

    gl_Position = projection * (gl_in[0].gl_Position + quad[3]);
    @texCoordsG = vec2(1,1);
    EmitVertex();

    EndPrimitive();
}  

