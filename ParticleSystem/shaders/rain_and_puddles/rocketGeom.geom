
layout(points) in;
layout(triangle_strip, max_vertices=4) out;

in vec4 rocket_velocitiesV[];
in float rocket_lifetimesV[];
in vec4 colorsV[];
in vec2 rocket_texCoordsV[];
in float rocket_floorHitV[];

out float rocket_lifetimesG;
out vec4 colorsG;
out vec2 rocket_texCoordsG;

uniform mat4 model, view, projection;


void main()
{	
    rocket_lifetimesG = rocket_lifetimesV[0];
    colorsG = colorsV[0];

    float original = 0.1;

    float ratio = clamp(length(rocket_velocitiesV[0].xyz) * 40, 1.0, 2.0);

    if (rocket_floorHitV[0] == 1)
    {
        ratio = 1;
        original *= 2;
        original = min(rocket_lifetimesG/5, original);
    }

    float newX = original * ratio;
    float newY = original / ratio;

    vec4 quad[4] = {vec4(-newX,-newY,0,0),
                    vec4(-newX, newY, 0, 0),
                    vec4(newX, -newY, 0, 0),
                    vec4(newX, newY, 0, 0)};

    if (rocket_floorHitV[0] == 1)
        quad = billboardFaceXZ(quad, view * model);
    else
    quad = billboardDirectionCamera(quad,
                                    (view*model*rocket_velocitiesV[0]).xyz,
                                    -vec3(gl_in[0].gl_Position.xyz));

    // generate final vertex positions
    gl_Position = projection * (gl_in[0].gl_Position + quad[0]);
    rocket_texCoordsG = vec2(0,0);
    EmitVertex();

    gl_Position = projection * (gl_in[0].gl_Position + quad[1]);
    rocket_texCoordsG = vec2(0,1);
    EmitVertex();

    gl_Position = projection * (gl_in[0].gl_Position + quad[2]);
    rocket_texCoordsG = vec2(1,0);
    EmitVertex();

    gl_Position = projection * (gl_in[0].gl_Position + quad[3]);
    rocket_texCoordsG = vec2(1,1);
    EmitVertex();
}  

