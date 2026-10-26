#include "particles.h"
#include "rendering/buffer.h"
#include "rendering/util.h"

#include <cppgl.h>
#include <iostream>

Shader Particles::draw_shader;
Shader Particles::splat_shader;
Texture2D Particles::acid_mask, Particles::fire_mask, Particles::magic_mask, Particles::frost_mask, Particles::cloud_mask;

Particles::Particles(unsigned int N, float particle_size) : N(N), position(N), direction(N), color(N), lifetime(N, 0), index_buffer(N), start(0), end(0), particle_size(particle_size) {
    static unsigned idx = 0;
    mesh = Mesh(std::string("particle-data") + std::to_string(idx++));
    vbo_id_pos = mesh->add_vertex_buffer(GL_FLOAT, 3, N, position.data(), GL_DYNAMIC_DRAW);
    vbo_id_life = mesh->add_vertex_buffer(GL_INT, 1, N, lifetime.data(), GL_DYNAMIC_DRAW);
    vbo_id_col = mesh->add_vertex_buffer(GL_FLOAT, 4, N, color.data(), GL_DYNAMIC_DRAW);
    for (unsigned i = 0; i < N; ++i) index_buffer[i] = i;
    mesh->add_index_buffer(N, index_buffer.data());
    if(!acid_mask) {
        acid_mask = Texture2D("acid_mask", "render-data/images/masks/bubble.png");
        fire_mask = Texture2D("fire_mask", "render-data/images/masks/fire.png");
        magic_mask = Texture2D("magic_mask", "render-data/images/masks/magic.png");
        frost_mask = Texture2D("frost_mask", "render-data/images/masks/snowflake.png");
        cloud_mask = Texture2D("cloud_mask", "render-data/images/masks/cloud.png");
    }

    // lazy init static stuff
    if (!draw_shader)
        draw_shader = Shader("particle-draw", "shader/particle-flare.vs", "shader/particle-flare.fs");
    if (!splat_shader)
        splat_shader = Shader("particle-splat", "shader/particle-splat.vs", "shader/particle-splat.gs", "shader/particle-splat.fs");
}

Particles::~Particles() {}

uint32_t Particles::advance(uint32_t& pointer) {
    pointer = (pointer + 1) % N;
    return pointer;
}

void Particles::add(const glm::vec3 &pos, const glm::vec3& dir, const glm::vec4 &col, int alive_for_ms, int amount, bool random)
{
    for (int i = 0; i < amount; i++)
    {
        const uint32_t at = end;
        if (advance(end) == start)
            advance(start);
        position[at] = pos;
        if(random)
            direction[at] = random_vec3() / glm::vec3(10);
        else
            direction[at] = dir;
        lifetime[at] = alive_for_ms;
        color[at] = col;
    }
}

void Particles::update() {
    const float dt = Context::frame_time();
    const int bound = (end >= start) ? end : end+N;
    for (int i = start; i < bound; ++i) {
        const uint32_t idx = i % N;
        lifetime[idx] -= uint32_t(dt);
        if (lifetime[idx] <= 0 && idx == start)
            advance(start);
        position[idx] = position[idx] + direction[idx] * dt * 0.0066f;
    }
    mesh->update_vertex_buffer(vbo_id_pos, position.data());
    mesh->update_vertex_buffer(vbo_id_life, lifetime.data());
    mesh->update_vertex_buffer(vbo_id_col, color.data());
}

void Particles::draw() {
    // draw pass as points
    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE);

    glDepthMask(GL_FALSE);
    glEnable(GL_PROGRAM_POINT_SIZE);
    glEnable(GL_POINT_SPRITE);

    draw_shader->bind();
    auto cam = current_camera();
    draw_shader->uniform("view", cam->view);
    draw_shader->uniform("proj", cam->proj);
    draw_shader->uniform("near_far", glm::vec2(cam->near, cam->far));
    draw_shader->uniform("screenres", glm::vec2(Context::resolution()));
    draw_shader->uniform("particle_size", particle_size);
    draw_shader->uniform("gbuf_depth", rendering::buffer::geom->depth_texture, 0);
    draw_shader->uniform("acid_mask", acid_mask, 1);
    draw_shader->uniform("fire_mask", fire_mask, 2);
    draw_shader->uniform("magic_mask", magic_mask, 3);
    draw_shader->uniform("frost_mask", frost_mask, 4);
    draw_shader->uniform("cloud_mask", cloud_mask, 5);

    mesh->bind(draw_shader);
    if (start < end) {
        glDrawElementsBaseVertex(GL_POINTS, end - start, GL_UNSIGNED_INT, 0, start);
    }
    else if (end < start) {
        glDrawElementsBaseVertex(GL_POINTS, N - end - 1, GL_UNSIGNED_INT, 0, start);
        glDrawElementsBaseVertex(GL_POINTS, end, GL_UNSIGNED_INT, 0, 0);
    }
    mesh->unbind();
    draw_shader->unbind();

    glDisable(GL_POINT_SPRITE);
    glDisable(GL_PROGRAM_POINT_SIZE);

    // splatting pass using a geometry shader
    splat_shader->bind();
    splat_shader->uniform("view", cam->view);
    splat_shader->uniform("proj", cam->proj);
    splat_shader->uniform("near_far", glm::vec2(cam->near, cam->far));
    splat_shader->uniform("screenres", glm::vec2(Context::resolution()));
    splat_shader->uniform("radius", particle_size * 5.f);
    splat_shader->uniform("gbuf_depth", rendering::buffer::geom->depth_texture, 0);
    splat_shader->uniform("gbuf_diff", rendering::buffer::geom->color_textures[0], 1);
    splat_shader->uniform("gbuf_pos", rendering::buffer::geom->color_textures[1], 2);
    splat_shader->uniform("gbuf_norm", rendering::buffer::geom->color_textures[2], 3);

    mesh->bind(splat_shader);
    if (start < end) {
        glDrawElementsBaseVertex(GL_POINTS, end - start, GL_UNSIGNED_INT, 0, start);
    }
    else if (end < start) {
        glDrawElementsBaseVertex(GL_POINTS, N - end - 1, GL_UNSIGNED_INT, 0, start);
        glDrawElementsBaseVertex(GL_POINTS, end, GL_UNSIGNED_INT, 0, 0);
    }
    mesh->unbind();

    splat_shader->unbind();


    //reset for other render calls
    glDepthMask(GL_TRUE);
    glDisable(GL_BLEND);
}
