#pragma once

#include <cppgl.h>

using namespace std;

class Renderer
{
public:
    Renderer(Shader shader);

    void bind();
    void unbind();
    void draw(vector<Drawelement> &draws, glm::mat4 model, 
        glm::mat4 model_rot = glm::mat4(1));

    void draw_anim(Drawelement draws, glm::mat4 model, glm::mat4 model_rot, int state);
    Shader shader;
};