#include "pass.h"

#include <cppgl.h>
#include <stdexcept>
#include <utility>
#include <glm/gtc/type_ptr.hpp>

#include "buffer.h"
#include "util.h"
#include "../scene.h"
#include "../renderer.h"
#include "../game.h"
#include "../particles.h"
#include "../input_handler.h"

extern std::shared_ptr<Particles> particles, particles_small;

using namespace rendering;

void pass::geometry() {
    static Renderer geom_renderer{ Shader("pos_norm_tc", "shader/pos+norm+tc.vs", "shader/pos+norm+tc.fs") };
    using namespace rendering::buffer;
    geom->bind();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    geom_renderer.bind();
    util::setup_camera(geom_renderer.shader, current_camera());
    geom_renderer.shader->uniform("view", current_camera()->view);
    geom_renderer.shader->uniform("view_normal", current_camera()->view_normal);
    geom_renderer.shader->uniform("proj", current_camera()->proj);
    //the_scene->draw_geometry(pos_norm_tc);
    the_game->draw(geom_renderer);
    InputHandler::setAddRemovePos(); // watch out for buffer (un-)binding!
    geom->bind();
    //cout << "draw temporaries" << endl;
    the_game->drawTemporaries(geom_renderer);
    //cout << "end draw temporaries" << endl;
    geom_renderer.unbind();
    geom->unbind();
}


void pass::deferred_lighting() {
    static Shader deferred_lighting_shader { "deferred_lighting", "shader/copytex.vs", "shader/deferred_lighting.fs" };

    using namespace rendering::buffer;

    lighting->bind();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    the_scene->draw();

    deferred_lighting_shader->bind();

    util::setup_camera(deferred_lighting_shader, current_camera());
    util::setup_light(deferred_lighting_shader, point_light);

    deferred_lighting_shader->uniform("geom.depth", 	geom->depth_texture,     0);
    deferred_lighting_shader->uniform("geom.diff",  	geom->color_textures[0], 1);
    deferred_lighting_shader->uniform("geom.pos",   	geom->color_textures[1], 2);
    deferred_lighting_shader->uniform("geom.norm",  	geom->color_textures[2], 3);

    // glDisable(GL_DEPTH_TEST);
    Quad::draw();
    // glEnable(GL_DEPTH_TEST);

    deferred_lighting_shader->unbind();

    particles->draw();
    particles_small->draw();
    
    lighting->unbind();
}

void pass::final() {
    static Shader final_shader { "final", "shader/copytex.vs", "shader/final.fs" };

    using namespace rendering::buffer;

    final_shader->bind();

    final_shader->uniform("lighting.depth", lighting->depth_texture, 0);
    final_shader->uniform("lighting.color", lighting->color_textures[0], 1);

    glDisable(GL_DEPTH_TEST);
    Quad::draw();
    glEnable(GL_DEPTH_TEST);

    final_shader->unbind();
}

