#include "input_handler.h"
#include "rendering/buffer.h"
#include "game_level.h"
#include "game.h"

glm::vec3 InputHandler::hover_pos;
glm::ivec3 InputHandler::add_pos;
glm::ivec3 InputHandler::remove_pos;

Placer InputHandler::placer;

glm::vec2 InputHandler::last_mouse_pos;
float InputHandler::camera_angle = 0;


void InputHandler::setAddRemovePos()
{
    glm::vec2 mouse_pos = Context::mouse_pos();
    // read world position
    using namespace rendering::buffer;
    glm::ivec2 res = Context::resolution();
    geom->bind();
    float mouse_wpos[3];
    float norm[3];
    glReadBuffer(GL_COLOR_ATTACHMENT1);
    glReadPixels(mouse_pos.x, res.y - mouse_pos.y, 1, 1, GL_RGB, GL_FLOAT, mouse_wpos);
    glReadBuffer(GL_COLOR_ATTACHMENT2);
    glReadPixels(mouse_pos.x, res.y - mouse_pos.y, 1, 1, GL_RGB, GL_FLOAT, norm);
    geom->unbind();
    //cout << "x: " << mouse_wpos[0] << " y: " << mouse_wpos[1] << " z: " << mouse_wpos[2] << endl;
    // cout << hover_pos << endl;
    hover_pos = glm::vec3(mouse_wpos[0], mouse_wpos[1], mouse_wpos[2]);
    if (hover_pos != glm::vec3(0)) {
        add_pos = glm::ivec3(mouse_wpos[0] + 0.501 + 0.5 * norm[0], mouse_wpos[1] + 0.001 + 0.5 * norm[1], mouse_wpos[2] + 0.501 + 0.5 * norm[2]);
        remove_pos = glm::ivec3(mouse_wpos[0] + 0.501 - 0.5 * norm[0], mouse_wpos[1] + 0.001 - 0.5 * norm[1], mouse_wpos[2] + 0.501 - 0.5 * norm[2]);
    }
}

void InputHandler::keyboard_callback(int key, int scancode, int action, int mods)
{
    if (the_game->state == GAME_BUILD) {
        if (key == GLFW_KEY_LEFT && action == GLFW_PRESS)
            placer.rotate(90.0f);
        if (key == GLFW_KEY_RIGHT && action == GLFW_PRESS)
            placer.rotate(-90.0f);
    }
}

void InputHandler::mouse_button_callback(int button, int action, int mods) {
    if (the_game->state == GAME_BUILD) {
        if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS && !Context::key_pressed(GLFW_KEY_LEFT_SHIFT)) {
            cout << "trying to place block at: " << add_pos << endl;
            placer.place();
        }
        if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS && Context::key_pressed(GLFW_KEY_LEFT_SHIFT))
            placer.remove(remove_pos);
    }
    if (the_game->state == GAME_ACTIVE) {
        if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS && Context::key_pressed(GLFW_KEY_LEFT_SHIFT))
            the_game->level->createProjectile();
        //if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS && hover_pos != glm::vec3(0))
        //    the_game->level->block_clicked(remove_pos);
    }
}

void InputHandler::mouse_scroll_callback(double xoffset, double yoffset){
    if(!get_debug()){
        CameraImpl::setRadius(yoffset);
    }
}
void InputHandler::build_input()
{

}

void InputHandler::active_input() {
    if (Context::mouse_button_pressed(GLFW_MOUSE_BUTTON_LEFT) && hover_pos != glm::vec3(0) && !Context::key_pressed(GLFW_KEY_LEFT_SHIFT))
        the_game->level->block_clicked(remove_pos);
}

void InputHandler::player_input() {
    glm::vec2 move_direction(0);
    auto cam = Camera::find("default");
    glm::vec2 dir = glm::vec2(cam->dir.x,cam->dir.z);
    if (Context::key_pressed(GLFW_KEY_W)) {
        move_direction += glm::vec2(dir.x, dir.y);
    }
    if (Context::key_pressed(GLFW_KEY_S)) {
        move_direction += glm::vec2(-dir.x, -dir.y);
    }
    if (Context::key_pressed(GLFW_KEY_A)) {
        move_direction += glm::vec2(dir.y, -dir.x);
    }
    if (Context::key_pressed(GLFW_KEY_D)) {
        move_direction += glm::vec2(-dir.y, dir.x);
    }
    // Scale the move_direction by the camera direction
    if (move_direction != glm::vec2(0))
        move_direction = glm::normalize(move_direction);
    the_game->level->player->move_direction = move_direction;
//    if (move_direction != glm::vec2(0))
//        the_game->level->player->set_rotation(glm::degrees(atan2(move_direction.y, move_direction.x)));
}

void InputHandler::addTmpBlock(BlockType type)
{
    placer.set(type);
}

void InputHandler::drawTmpBlock(Renderer &renderer)
{
    placer.routine(renderer);
}



//---------------------
// Placer

Placer::Placer() : state(BLOCKS), index(0) { }

void Placer::set(BlockType type) {
    clear();
    cout << "setting" << endl;
    place_objects.push_back(GameLevel::switchBlockType(type, InputHandler::add_pos, 0.0f));
    cout << "pushed back" << endl;
    block_linking = true;
}

void Placer::set(PowerUpType type) {
    clear();
    place_objects.push_back(GameLevel::switchPowerUpType(type, InputHandler::add_pos));
    block_linking = false;
}

void Placer::add(BlockType type) {
    cout << "adding" << endl;
    place_objects.push_back(GameLevel::switchBlockType(type, InputHandler::add_pos, 0.0f));
}

void Placer::set(EnemyType type) {
    clear();
    place_objects.push_back(GameLevel::switchEnemyType(type, InputHandler::add_pos));
    block_linking = false;
}

void Placer::add(EnemyType type) {
    cout << "adding" << endl;
    place_objects.push_back(GameLevel::switchEnemyType(type, InputHandler::add_pos));
    cout << "added" << endl;
}

void Placer::remove(glm::ivec3 pos) {
    if (place_objects.size() > 1) {
        bool remove = false;
        if (pos == place_objects[0]->tile_pos) {
            remove = true;
        }
        if (remove) {
            for (int i = 0; i < index; i++) {
                auto &block = place_objects[i];
                //cout << "removing: " << i << endl;
                the_game->level->remove(block->tile_pos);
                block = block->getNewObject();
            }
            index = 0;
        }
        else {
            the_game->level->remove(pos);
        }
    }
    else {
        the_game->level->remove(pos);
    }
}

void Placer::clear() {
    place_objects.clear();
    index = 0;
}

void Placer::rotate(float amount) {
    place_objects[index]->rotate(amount);
}

void Placer::place() {
    if (place_objects.empty())
        return;
    cout << "placing, index is: " << index << endl;
    // place
    if (place_objects[index]->place()) {
        if (block_linking) {
            for (int i = 1; i <= index; i++) {
                // maybe only temporary
                the_game->level->create_link(place_objects[i]->tile_pos, place_objects[0]->tile_pos);
                //the_game->level->create_link(place_objects[0]->tile_pos, place_objects[i]->tile_pos);
            }
        }
        index++;
    }
    // reset
    if (index == place_objects.size()) {
        for (auto &object : place_objects) {
            object = object->getNewObject();
        }
        index = 0;
    }
}

// update block pos
void Placer::routine(Renderer &renderer) {
    if (place_objects.empty())
        return;
    //cout << place_objects.size() << endl;
    place_objects[index]->placingRoutine(renderer, InputHandler::add_pos);
}