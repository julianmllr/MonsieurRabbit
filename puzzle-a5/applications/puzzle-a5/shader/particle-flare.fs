#version 430
flat in int time;
flat in vec4 col;
flat in vec4 particle_pos;
flat in float point_radius;

uniform vec2 near_far;
uniform vec2 screenres;
uniform sampler2D gbuf_depth;
uniform sampler2D acid_mask;
uniform sampler2D fire_mask;
uniform sampler2D magic_mask;
uniform sampler2D frost_mask;
uniform sampler2D cloud_mask;

out vec4 out_col;

float linear_depth(in float depth, in float near, in float far) { return (2.0 * near) / (far + near - depth * (far - near)); }

void main() {
    //vec4 col = vec4(1, 115 / 255.f, 65 / 255.f, 1);
    float fade_time = clamp(time / 500.f, 0, 1);
    out_col = pow(fade_time, 2) * col;
    vec2 tc = gl_FragCoord.xy / screenres;
    if (gl_FragCoord.z > texture(gbuf_depth,tc).r)
        out_col = vec4(0,1,0,1);
    float frag_depth = linear_depth(gl_FragCoord.z, near_far.x, near_far.y);
    float geom_depth = linear_depth(texture(gbuf_depth, gl_FragCoord.xy / screenres).r, near_far.x, near_far.y);
    float fade_depth = clamp(abs(frag_depth - geom_depth) / 0.001f, 0, 1);
    out_col *= vec4(fade_depth * fade_depth);

    vec3 center_ndc = particle_pos.xyz / particle_pos.w;
    vec2 center_tc = (center_ndc.xy + 1.0) / 2.0;
    vec2 center_sc = center_tc * screenres;
    vec2 cur_sc = gl_FragCoord.xy;

    vec2 diff_sc = cur_sc - center_sc;
    vec2 diff_ndc = diff_sc / point_radius;
    vec2 diff_tc = (diff_ndc + 1.0) / 2.0;

    if (col.xyz == vec3(1, 0, 0))
        out_col *= texture(fire_mask, diff_tc);
    else if (col.xyz == vec3(1, 0, 1))
        out_col *= texture(magic_mask, diff_tc);
    else if (col.xyz == vec3(0, 1, 0))
        out_col *= texture(acid_mask, diff_tc);
    else if (col.xyz == vec3(0, 1, 1)) {
        out_col *= texture(frost_mask, diff_tc);
    } else if(col.xyz == vec3(0.96, 0.87, 0.69)) {
        out_col *= texture(cloud_mask, diff_tc);
    }
}
