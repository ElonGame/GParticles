
layout(points) in;
layout(triangle_strip, max_vertices=4) out;

in vec4 puddle_velocitiesV[];
in float puddle_lifetimesV[];
in vec4 colorsV[];
in float puddle_floorHitV[];

out float puddle_lifetimesG;
out vec4 colorsG;
out vec2 puddle_texCoordsG;

uniform mat4 model, view, projection;


void main()
{	
    puddle_lifetimesG = puddle_lifetimesV[0];
    colorsG = colorsV[0];

    vec4 quad[4];

    if (puddle_floorHitV[0] == 1)
    {
        // puddle_lifetimesG starts at 1 so we'll a puddle with size 0.2 for a
        // brief moment and then we shrink it until it is gone
        float edgeSize = min(puddle_lifetimesG/4, 0.2);

        quad = getQuad(edgeSize, edgeSize);
        quad = billboardFaceXZ(quad, view * model);
    }
    else
    {
        quad = getStretchedQuad(0.1, 0.1, puddle_velocitiesV[0].xyz, false);
        quad = billboardDirectionCamera(quad,
                                        (view*model*puddle_velocitiesV[0]).xyz,
                                        -vec3(gl_in[0].gl_Position.xyz));
    }

    // generate final vertex positions
    gl_Position = projection * (gl_in[0].gl_Position + quad[0]);
    puddle_texCoordsG = vec2(0,0);
    EmitVertex();

    gl_Position = projection * (gl_in[0].gl_Position + quad[1]);
    puddle_texCoordsG = vec2(0,1);
    EmitVertex();

    gl_Position = projection * (gl_in[0].gl_Position + quad[2]);
    puddle_texCoordsG = vec2(1,0);
    EmitVertex();

    gl_Position = projection * (gl_in[0].gl_Position + quad[3]);
    puddle_texCoordsG = vec2(1,1);
    EmitVertex();

    EndPrimitive();
}  

