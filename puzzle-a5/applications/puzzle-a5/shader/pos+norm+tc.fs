#version 430 core

#include "types.fs"


in VS_OUT {
    vec4 pos_wc;
    vec3 norm_wc;
    vec2 tc;
} fs_in;

layout(location = 0) out vec3 out_col;
layout(location = 1) out vec3 out_pos;
layout(location = 2) out vec3 out_norm;
layout(location = 3) out vec4 out_metal;

uniform sampler2D diffuse;
uniform Camera cam;

uniform int bb_toggle;
uniform int red;
uniform float radius;
uniform vec3 sphere_pos;
uniform vec3 boxMin;
uniform vec3 boxMax;

void main() {
    vec3 diff = texture(diffuse, fs_in.tc).rgb;
    diff = pow(diff, vec3(2.2f));

    out_col = diff;

    if(red == 2){
        out_col *= vec3(1,0,0);
    }
    out_pos = fs_in.pos_wc.xyz;
    out_norm = fs_in.norm_wc;
}
