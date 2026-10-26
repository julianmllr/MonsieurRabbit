#include "renderer.h"

#include "player.h"
Renderer::Renderer(Shader shader) : shader(shader) { }

void Renderer::bind() {

    shader->bind();
}

void Renderer::unbind() {
    shader->unbind();
}

void Renderer::draw(vector<Drawelement>& draws, glm::mat4 model, glm::mat4 model_rot) {
    //model[3][1] -= 0.5f;
    for (auto& elem : draws) {
        //elem->shader = shader;
        elem->shader->uniform("pose", -1);
        elem->model = model * model_rot;
        elem->raw_bind();
        elem->draw();
        elem->raw_unbind();
    }
}

void Renderer::draw_anim(Drawelement elem, glm::mat4 model, glm::mat4 model_rot, int state) {
    elem->model = model * model_rot;
    elem->raw_bind();
    float t = anim_time;
    float ta = anim_timer.look();
    if (state == 2 ) ta = ta * 8 / 1000;
    else if (state == 3) ta = ta * 4 / 1000;
    else ta = ta * 2 / 1000; 
    elem->shader->uniform("time", t);
    elem->shader->uniform("timer", ta);
    elem->shader->uniform("pose", state);
    elem->draw();
    elem->raw_unbind();
}
