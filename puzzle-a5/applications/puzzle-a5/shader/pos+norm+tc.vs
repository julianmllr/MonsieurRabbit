#version 430
layout(location = 0) in vec3 in_pos;
layout(location = 1) in vec3 in_norm;
layout(location = 2) in vec2 in_tc;

layout(location = 3) in vec3 in_pos1;
layout(location = 4) in vec3 in_pos2;
layout(location = 5) in vec3 in_pos3;
layout(location = 6) in vec3 in_pos4;
layout(location = 7) in vec3 in_pos5;
layout(location = 8) in vec3 in_pos6;
layout(location = 9) in vec3 in_pos7;


out VS_OUT {
    vec4 pos_wc;
    vec3 norm_wc;
    vec2 tc;
} vs_out;

uniform mat4 model;
uniform mat4 model_normal;
uniform mat4 view;
uniform mat4 proj;
uniform int pose;
uniform float timer;

void main() {
    
    float t = timer;
    vec3 anim_pos = in_pos;
    if (pose == 1){
        if (t >= 8) anim_pos = in_pos;
        else if (t >= 7) anim_pos = mix(in_pos5, in_pos, t-7);
        else if (t >= 6) anim_pos = mix(in_pos4, in_pos5, t-6);
        else if (t >= 5) anim_pos = mix(in_pos3, in_pos4, t-5);
        else if (t >= 4) anim_pos = in_pos3;
        else if (t >= 3) anim_pos = mix(in_pos2, in_pos3, t-3);
        else if (t >= 2) anim_pos = mix(in_pos1, in_pos2, t-2);
        else if (t >= 1) anim_pos = in_pos1;
        else if (t >= 0) anim_pos = mix(in_pos, in_pos1, t);
        vs_out.pos_wc = model * vec4(anim_pos, 1.0);}
    
    else if (pose == 2){
        if (t >= 6) anim_pos = mix(in_pos6, in_pos, t-6);
        else if (t >= 5) anim_pos = mix(in_pos5, in_pos6, t-5);
        else if (t >= 4) anim_pos = mix(in_pos4, in_pos5, t-4);
        else if (t >= 3) anim_pos = mix(in_pos3, in_pos4, t-3);
        else if (t >= 2) anim_pos = mix(in_pos2, in_pos3, t-2);
        else if (t >= 1) anim_pos = mix(in_pos1, in_pos2, t-1);
        else if (t >= 0) anim_pos = mix(in_pos, in_pos1, t);
        vs_out.pos_wc = model * vec4(anim_pos, 1.0);
    }

    else if (pose == 3){
    if (t >= 3) anim_pos = mix(in_pos1, in_pos, t-3);
    else if (t >= 2) anim_pos = mix(in_pos, in_pos1, t-2);
    else if (t >= 1) anim_pos = mix(in_pos1, in_pos, t-1);
    else if (t >= 0) anim_pos = mix(in_pos, in_pos1, t);

    vs_out.pos_wc = model * vec4(anim_pos, 1.0);}

    else if (pose == 4){
        if (t >= 4) anim_pos = in_pos5;
        else if (t >= 3) anim_pos = mix(in_pos3, in_pos5, t-3);
        else if (t >= 2) anim_pos = mix(in_pos1, in_pos2, t-2);
        else if (t >= 1) anim_pos = mix(in_pos, in_pos1, t-1);

        else if (t >= 0) anim_pos = mix(in_pos5, in_pos, t);
        vs_out.pos_wc = model * vec4(anim_pos, 1.0);
        }
        
    else if (pose == 0) vs_out.pos_wc = model * vec4(in_pos, 1.0);
    else if (pose == -1) vs_out.pos_wc = model * vec4(in_pos, 1.0);

    vs_out.norm_wc = normalize(mat3(model_normal) * in_norm);
    vs_out.tc = in_tc;
    gl_Position = proj * view * vs_out.pos_wc;
}
