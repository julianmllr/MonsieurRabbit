#version 430
out vec4 out_col;

in vec3 pos;

uniform samplerCube skybox;

void main()
{   
    // mind the flip
    out_col = texture(skybox, vec3(pos.x, pos.y, pos.z));
}