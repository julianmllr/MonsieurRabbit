#version 430
layout(location = 0) in vec3 in_pos;
layout(location = 1) in int in_lifetime;
layout(location = 2) in vec4 in_color;

flat out int time;
flat out vec4 col;
flat out vec4 particle_pos;
flat out float point_radius;

uniform mat4 view;
uniform mat4 proj;
uniform vec2 near_far;
uniform vec2 screenres;
uniform float particle_size;

float compute_pointsize(float radius, vec4 pos_ec) {
    float d = pos_ec.z;
    vec4 v = vec4(radius, radius, d, 1.0);
    vec4 tmp = vec4(v.x*proj[0].x, v.y*proj[1].y, v.z*proj[2].z+proj[3].z, -v.z);
    vec3 tmp2 = tmp.xyz / tmp.w;
    tmp2.x = tmp2.x * screenres.x;
    tmp2.y = tmp2.y * screenres.y;
    float size = tmp2.x * 1.15;
    return size;
}

void main() {
    vec4 pos_ec = view * vec4(in_pos, 1);
    float point_size = compute_pointsize(particle_size, pos_ec);
    gl_PointSize = in_lifetime > 0 ? point_size : 0;
    time = in_lifetime;
    col = in_color;
    gl_Position = proj * pos_ec;
    particle_pos = proj * pos_ec;
    point_radius = point_size / 2.0;
}
