#version 430

#include "types.fs"

in vec2 tc;

out vec3 out_col;

uniform Camera cam;
uniform DirectionalLight light;
uniform PointLight p_light;
uniform Geom geom;


void main() {
    float frag_depth = texture(geom.depth, tc).r;
    gl_FragDepth = frag_depth;
    
    if (frag_depth == 1) {
        discard;
        return;
    }



    vec3 frag_diff = texture(geom.diff, tc).rgb;
    frag_diff = mix(frag_diff, vec3(0.7,0.9,1), 0.15); //to make colors less saturated
    vec3 frag_pos = texture(geom.pos, tc).xyz;
    vec3 frag_norm = normalize(texture(geom.norm, tc).xyz);

    vec3 light_dir = normalize(p_light.pos - frag_pos);

    float n_dot_l = max(0, dot(light_dir, frag_norm));
    float falloff = 1 / ((distance(frag_pos, p_light.pos) / p_light.radius) + 1e-10);

    vec3 f_amb = p_light.ambient * frag_diff;

    float df;
    float intensity = dot(light_dir, frag_norm);
    if (intensity > 0.8) n_dot_l = 0.8;
	else if (intensity > 0.5) n_dot_l = 0.75;
    else  n_dot_l = 0.3;

    float sf = step(0.5,n_dot_l);


    f_amb += 0.2 * frag_diff;
    vec3 f_diff = p_light.diffuse * frag_diff * n_dot_l * falloff * falloff;
    out_col = f_amb + f_diff;
}
