#include "camera.h"
#include "context.h"
#include "imgui/imgui.h"
#include <iostream>
#include <GL/glew.h>
#include <GL/gl.h>
#include <GLFW/glfw3.h>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>


CPPGL_NAMESPACE_BEGIN

static Camera current_cam;

Camera current_camera() {
    static Camera default_cam("default");
    return current_cam ? current_cam : default_cam;
}

void make_camera_current(const Camera& cam) {
    current_cam = cam;
}

static glm::mat4 get_projection_matrix(float left, float right, float top, float bottom, float n, float f) {
    glm::mat4 proj = glm::mat4(0);
    proj[0][0] = (2.f*n) / (right - left);
    proj[1][1] = (2.f*n) / (top- bottom);
    proj[2][0] = (right + left) / (right - (left));
    proj[2][1] = (bottom+top) / (top-bottom);
    proj[2][2] = -(f + n) / (f - n);
    proj[2][3] = -1.f;
    proj[3][2] = (-2 * f * n) / (f - n);
    return proj;
}

// ----------------------------------------------------
// CameraImpl

float CameraImpl::default_camera_movement_speed = 0.005f;
// standard values are defined in resetCamera()
float CameraImpl::min_radius = 5.f;
float CameraImpl::max_radius = 20.f;
float CameraImpl::radius = max_radius;
glm::vec3 CameraImpl::center = glm::vec3(1.0, 1.0, 1.0);
float CameraImpl::rot_angle_polar = 3 * M_PI / 8;
float CameraImpl::rot_angle_az = - 3 * M_PI / 4;

CameraImpl::CameraImpl(const std::string& name) : name(name), pos(0, 0, 0), dir(1, 0, 0), up(0, 1, 0), fov_degree(70),
    near(0.01f), far(1000), left(-100), right(100), bottom(-100), top(100),
    perspective(true), skewed(false), fix_up_vector(true) {
    update();
}

CameraImpl::~CameraImpl() {}


void CameraImpl::update() {
    update_view();
    update_proj();
}

void CameraImpl::update_view() {
    dir = glm::normalize(dir);
    up = glm::normalize(up);
    view = glm::lookAt(pos, pos + dir, up);
    view_normal = glm::transpose(glm::inverse(view));
}

void CameraImpl::update_proj() {
    proj = perspective ? (skewed ? get_projection_matrix(left, right, top, bottom, near, far)
                                 : glm::perspective(glm::radians(fov_degree), aspect_ratio(), near, far))
                        : glm::ortho(left, right, bottom, top, near, far);
}

void CameraImpl::forward(float by) { pos += by * dir; }
void CameraImpl::backward(float by) { pos -= by * dir; }
void CameraImpl::leftward(float by) { pos -= by * glm::cross(dir, up); }
void CameraImpl::rightward(float by) { pos += by * glm::cross(dir, up); }
void CameraImpl::upward(float by) { pos += by * glm::normalize(glm::cross(glm::cross(dir, up), dir)); }
void CameraImpl::downward(float by) { pos -= by * glm::normalize(glm::cross(glm::cross(dir, up), dir)); }

void CameraImpl::yaw(float angle) { dir = glm::normalize(glm::rotate(dir, angle * float(M_PI) / 180.f, up)); }
void CameraImpl::pitch(float angle) {
    dir = glm::normalize(glm::rotate(dir, angle * float(M_PI) / 180.f, glm::normalize(glm::cross(dir, up))));
    if (!fix_up_vector) up = glm::normalize(glm::cross(glm::cross(dir, up), dir));
}
void CameraImpl::roll(float angle) { up = glm::normalize(glm::rotate(up, angle * float(M_PI) / 180.f, dir)); }

void CameraImpl::from_lookat(const glm::vec3& pos, const glm::vec3& lookat, const glm::vec3& up) {
    this->pos = pos;
    this->dir = glm::normalize(lookat - pos);
    this->up = up;
    update();
}

void CameraImpl::load(const glm::vec3& pos, const glm::quat& rot) {
    this->pos = pos;
    this->view = glm::mat4_cast(rot);
    this->dir = -glm::vec3(view[0][2], view[1][2], view[2][2]);
    this->up = glm::vec3(view[0][1], view[1][1], view[2][1]);
    this->update();
}


float CameraImpl::aspect_ratio() {
    const glm::ivec2 res = Context::resolution();
    return float(res.x) / float(res.y);
}

bool CameraImpl::default_input_handler(double dt_ms) {
    bool moved = false;
    if (!ImGui::GetIO().WantCaptureKeyboard) {
        // keyboard
        if (Context::key_pressed(GLFW_KEY_U)) {
            current_camera()->forward(float(dt_ms * default_camera_movement_speed));
            moved = true;
        }
        if (Context::key_pressed(GLFW_KEY_J)) {
            current_camera()->backward(float(dt_ms * default_camera_movement_speed));
            moved = true;
        }
        if (Context::key_pressed(GLFW_KEY_H)) {
            current_camera()->leftward(float(dt_ms * default_camera_movement_speed));
            moved = true;
        }
        if (Context::key_pressed(GLFW_KEY_K)) {
            current_camera()->rightward(float(dt_ms * default_camera_movement_speed));
            moved = true;
        }
        if (Context::key_pressed(GLFW_KEY_R)) {
            current_camera()->upward(float(dt_ms * default_camera_movement_speed));
            moved = true;
        }
        if (Context::key_pressed(GLFW_KEY_F)) {
            current_camera()->downward(float(dt_ms * default_camera_movement_speed));
            moved = true;
        }
        if (Context::key_pressed(GLFW_KEY_Q)) {
            current_camera()->roll(float(dt_ms * -0.1));
            moved = true;
        }
        if (Context::key_pressed(GLFW_KEY_E)) {
            current_camera()->roll(float(dt_ms * 0.1));
            moved = true;
        }
    }
    // mouse
    static float rot_speed = 0.1f;
    static glm::vec2 last_mouse_pos(-1);

    const glm::vec2 curr_mouse_pos = Context::mouse_pos();
    if (last_mouse_pos == glm::vec2(-1)) last_mouse_pos = curr_mouse_pos;

    const glm::vec2 diff = last_mouse_pos - curr_mouse_pos;
    if (!ImGui::GetIO().WantCaptureMouse && Context::mouse_button_pressed(GLFW_MOUSE_BUTTON_RIGHT)) {
      if (glm::length(diff) > 0.01) {
            current_camera()->pitch(diff.y * rot_speed);
            current_camera()->yaw(diff.x * rot_speed);
            moved = true;
        }
        moved = true;
    }
    last_mouse_pos = curr_mouse_pos;
    return moved;
}

bool CameraImpl::sphere_camera_input_handler(double dt_ms) {
    bool moved = false;

    // mouse
    static float rot_speed = 0.001f;
    static glm::vec2 last_mouse_pos(-1);

    const glm::vec2 curr_mouse_pos = Context::mouse_pos();
    if (last_mouse_pos == glm::vec2(-1)) last_mouse_pos = curr_mouse_pos;

    const glm::vec2 diff = last_mouse_pos - curr_mouse_pos;
    if (!ImGui::GetIO().WantCaptureMouse && Context::mouse_button_pressed(GLFW_MOUSE_BUTTON_RIGHT)) {
        rot_angle_az -= diff.x * rot_speed;
        rot_angle_polar += diff.y * rot_speed;
        setPosOnCircle();
        moved = true;
    }
    last_mouse_pos = curr_mouse_pos;
    return moved;
}

glm::vec3 CameraImpl::calcPosOnCircle() {
    glm::vec3 position_on_circle;
    position_on_circle.x = center.x + radius * sin(rot_angle_polar) * cos(rot_angle_az);
    position_on_circle.y = center.y + radius * cos(rot_angle_polar);
    position_on_circle.z = center.z + radius * sin(rot_angle_polar) * sin(rot_angle_az);
    return position_on_circle;
}

void CameraImpl::setPosOnCircle() {
    glm::vec3 position_on_circle = calcPosOnCircle();
    glm::vec3 dir = center - position_on_circle;
    current_camera()->pos = position_on_circle;
    current_camera()->dir = dir;
}

void CameraImpl::setCenter(glm::vec3 player_center, glm::vec3 level_center) {
    glm::vec3 diff = level_center - player_center;
    float range = max_radius - min_radius;
    float delta = (radius - min_radius) / range;
    center = player_center + delta * diff;
    setPosOnCircle();
}

void CameraImpl::setRadius(float change) {
    using namespace std;
    radius = min(max(current_camera()->radius - float(change), min_radius), max_radius);
    setPosOnCircle();
}

void CameraImpl::resetCamera(glm::vec3 level_center) {
    radius = max_radius;
    center = level_center;
    rot_angle_polar = 3 * M_PI / 8;
    rot_angle_az = - 3 * M_PI / 4;
    setPosOnCircle();
}

CPPGL_NAMESPACE_END
