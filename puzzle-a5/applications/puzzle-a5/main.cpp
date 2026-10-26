#include <string>
#include <filesystem>
#include <algorithm>
#include <fstream>

#include <cppgl.h>
#include <nlohmann/json.hpp>

#include "scene.h"
#include "rendering/buffer.h"
#include "rendering/pass.h"
#include "rendering/util.h"
#include "game.h"
#include "input_handler.h"
#include "types.h"
#include "gui.h"
#include "particles.h"
#include "player.h"

shared_ptr<Particles> particles, particles_small, particles_big;
float anim_time;
Timer anim_timer;

Drawelement idle;
Drawelement climbing;
Drawelement clicking;
Drawelement walking;
namespace
{
    void on_resize(int w, int h)
    {
        rendering::buffer::geom->resize(w, h);
        rendering::buffer::lighting->resize(w, h);
    }

}

void GLAPIENTRY glDebugCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *message, const void *userParam)
{
    // Filter out specific messages you want to ignore or handle differently
    if (severity == GL_DEBUG_SEVERITY_NOTIFICATION && type == GL_DEBUG_TYPE_PERFORMANCE)
    {
        return;
    }

    // Handle or print the debug message as needed
    std::cerr << "GL_DEBUG: Severity: " << severity << ", Source: " << source << ", Type: " << type << ". Message: " << message << std::endl;
}

void load_keyframes() {
    vector<Mesh> keyframes_idle;
    vector<Mesh> keyframes_climbing;
    vector<Mesh> keyframes_clicking;
    vector<Mesh> keyframes_walking;

    auto shader = Shader("anim", "shader/pos+norm+tc.vs", "shader/pos+norm+tc.fs");

    //load idle animation
    Mesh mesh_idle = load_meshes_gpu("render-data/rabbit/anim/idle/idle-0.obj", false).front();
    idle = Drawelement("idle", shader, mesh_idle);
    keyframes_idle.push_back(load_meshes_gpu("render-data/rabbit/anim/idle/idle-1.obj", false).front());
    keyframes_idle.push_back(load_meshes_gpu("render-data/rabbit/anim/idle/idle-2.obj", false).front());
    keyframes_idle.push_back(load_meshes_gpu("render-data/rabbit/anim/idle/idle-3.obj", false).front());
    keyframes_idle.push_back(load_meshes_gpu("render-data/rabbit/anim/idle/idle-4.obj", false).front());
    keyframes_idle.push_back(load_meshes_gpu("render-data/rabbit/anim/idle/idle-5.obj", false).front());

    idle->raw_bind();
    for (auto& elem : keyframes_idle) {
        idle->mesh->add_vertex_buffer(GL_FLOAT, 3, uint32_t(elem->geometry->positions.size()), elem->geometry->positions.data());
        //idle->mesh->add_vertex_buffer(GL_FLOAT, 3, uint32_t(elem->geometry->normals.size()), elem->geometry->normals.data());
        //idle->mesh->add_vertex_buffer(GL_FLOAT, 2, uint32_t(elem->geometry->texcoords.size()), elem->geometry->texcoords.data());
    }
    idle->raw_unbind();

    //load climbing animation
    Mesh mesh_climb = load_meshes_gpu("render-data/rabbit/anim/climbing/climbing-1.obj", false).front();
    climbing = Drawelement("climbing", shader, mesh_climb);
    keyframes_climbing.push_back(load_meshes_gpu("render-data/rabbit/anim/climbing/climbing-2.obj", false).front());

    climbing->raw_bind();
    for (auto& elem : keyframes_climbing) {
        climbing->mesh->add_vertex_buffer(GL_FLOAT, 3, uint32_t(elem->geometry->positions.size()), elem->geometry->positions.data());
        //climbing->mesh->add_vertex_buffer(GL_FLOAT, 3, uint32_t(elem->geometry->normals.size()), elem->geometry->normals.data());
        //climbing->mesh->add_vertex_buffer(GL_FLOAT, 2, uint32_t(elem->geometry->texcoords.size()), elem->geometry->texcoords.data());
    }
    climbing->raw_unbind();

    //load climbing animation
    Mesh mesh_click = load_meshes_gpu("render-data/rabbit/anim/clicking/clicking-2.obj", false).front();
    clicking = Drawelement("clicking", shader, mesh_click);
    keyframes_clicking.push_back(load_meshes_gpu("render-data/rabbit/anim/clicking/clicking-3.obj", false).front());
    keyframes_clicking.push_back(load_meshes_gpu("render-data/rabbit/anim/clicking/clicking-4.obj", false).front());
    keyframes_clicking.push_back(load_meshes_gpu("render-data/rabbit/anim/clicking/clicking-5.obj", false).front());
    keyframes_clicking.push_back(load_meshes_gpu("render-data/rabbit/anim/clicking/clicking-1.obj", false).front());
    keyframes_clicking.push_back(load_meshes_gpu("render-data/rabbit/anim/clicking/clicking-0.obj", false).front());

    clicking->raw_bind();
    for (auto& elem : keyframes_clicking) {
        clicking->mesh->add_vertex_buffer(GL_FLOAT, 3, uint32_t(elem->geometry->positions.size()), elem->geometry->positions.data());
        // clicking->mesh->add_vertex_buffer(GL_FLOAT, 3, uint32_t(elem->geometry->normals.size()), elem->geometry->normals.data());
        // clicking->mesh->add_vertex_buffer(GL_FLOAT, 2, uint32_t(elem->geometry->texcoords.size()), elem->geometry->texcoords.data());
    }
    clicking->raw_unbind();

    //load walking animation
    Mesh mesh_walk = load_meshes_gpu("render-data/rabbit/anim/walking/walking-0.obj", false).front();
    walking = Drawelement("walking", shader, mesh_walk);
    keyframes_walking.push_back(load_meshes_gpu("render-data/rabbit/anim/walking/walking-1.obj", false).front());
    keyframes_walking.push_back(load_meshes_gpu("render-data/rabbit/anim/walking/walking-2.obj", false).front());
    keyframes_walking.push_back(load_meshes_gpu("render-data/rabbit/anim/walking/walking-3.obj", false).front());
    keyframes_walking.push_back(load_meshes_gpu("render-data/rabbit/anim/walking/walking-4.obj", false).front());
    keyframes_walking.push_back(load_meshes_gpu("render-data/rabbit/anim/walking/walking-5.obj", false).front());
    keyframes_walking.push_back(load_meshes_gpu("render-data/rabbit/anim/walking/walking-6.obj", false).front());

    walking->raw_bind();
    for (auto& elem : keyframes_walking) {
        walking->mesh->add_vertex_buffer(GL_FLOAT, 3, uint32_t(elem->geometry->positions.size()), elem->geometry->positions.data());
    }
    walking->raw_unbind();
}

int main(int argc, char **argv)
{
    // init context and set parameters
    ContextParameters params;
    params.title = "monsieur rabbit";
    params.font_ttf_filename = EXECUTABLE_DIR + string{"/render-data/fonts/utendo.ttf"};
    params.font_size_pixels = 15;
    Context::init(params);
    Context::set_resize_callback(&on_resize);
    Context::set_mouse_button_callback(&InputHandler::mouse_button_callback);
    Context::set_keyboard_callback(&InputHandler::keyboard_callback);
    Context::set_mouse_scroll_callback(&InputHandler::mouse_scroll_callback);
    // EXECUTABLE_DIR set via cmake, paths now relative to source/executable directory
    filesystem::current_path(EXECUTABLE_DIR);
    // init stuff
    load_keyframes();
    /* Set clear color to black to avoid confusion between light and unshaded
     * areas
     */
    glClearColor(0, 0, 0, 0);

    auto debug_cam = Camera("debug");
    auto lvl_cam = Camera("level");

    glDebugMessageCallback(glDebugCallback, nullptr);

    CameraImpl::setPosOnCircle();

    rendering::buffer::init();

    the_game = make_shared<Game>();
    the_scene = make_shared<Scene>();
    float particle_size = 0.05;
    particles = make_shared<Particles>(2000, particle_size * 2.0);
    particles_small = make_shared<Particles>(3000, particle_size * 0.1);
    Gui::loadImages();

    while (Context::running())
    {
        if (get_debug()) {
            make_camera_current(Camera::find("debug"));
            CameraImpl::default_input_handler(Context::frame_time());
        }
        else {
            make_camera_current(Camera::find("default"));
            CameraImpl::sphere_camera_input_handler(Context::frame_time());
        }
        current_camera()->update();
        the_game->update();
        particles->update();
        particles_small->update();
        
        static uint32_t reload_counter = 0;
        if (reload_counter++ % 100 == 0) {
            reload_counter = 1;
            reload_modified_shaders();
        }

        rendering::pass::geometry();

        rendering::pass::deferred_lighting();

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        rendering::pass::final();

        Context::swap_buffers();
    }

    return 0;
}
