#include "player.h"
#include "game.h"
#include "game_object.h"

glm::vec3 CameraImpl::center;

Player::Player() : SquareObject(), move_direction(0), max_hp(0), hp(0), gravity_timer() {}

Player::Player(shared_ptr<vector<Drawelement>> draws, glm::vec3 pos, glm::vec3 scale) :
    SquareObject(draws, pos, glm::vec3(0.4,0.9,0.40), 0.0f), move_direction(0),
        max_hp(3), hp(max_hp), gravity_timer(), invincibility_timer()
{ 
    model = glm::scale(model, scale);
    invincibility_timer.begin();
    state = IDLE;
    anim_timer.begin();
}

void Player::add_hp(int amount) {
    hp += amount;
    hp = min(hp, max_hp);
}

void Player::take_damage(int amount){
    static float invincibility = 1000.0f;
    const float elapsed = float(invincibility_timer.look());
    if (elapsed > invincibility) {
        hp -= amount;
        cout << "schaden genommen, hp ist: " << hp << endl;
        invincibility_timer.begin();
    }
}

void Player::resetGravity() {
    gravity_timer.begin();
}

void Player::update()
{
    if (!get_debug()) {
        glm::vec3 level_center = 0.5f * glm::vec3(the_game->level->width, the_game->level->height,
                  the_game->level->depth) - 0.5f;
        CameraImpl::setCenter(position, level_center);
    }

    // gravity needs to be smaller than move_speed! otherwise collision will trigger incorrectly.
    // if that's annoying, collision needs to be checked twice!
    static float gravity = -0.000001f;
    static float max_gravity = -0.4f;
    const float elapsed = float(gravity_timer.look());
    glm::vec3 gravity_move = glm::vec3(0.0f, max(max_gravity, elapsed * elapsed * gravity), 0.0f);
    move(gravity_move);

    static float move_speed = 0.05f;
    move(glm::vec3(move_speed * move_direction.x, 0.0f, move_speed * move_direction.y));
   
    if (move_direction != glm::vec2(0, 0)){

        glm::vec2 v1 = glm::vec2(0,1);
        glm::vec2 v2 = move_direction;
        float crossProduct = v1.x * v2.y - v1.y * v2.x;
        float dotProduct = glm::dot(v1, v2);
        float angleRad = std::atan2(crossProduct, dotProduct);
        setRotation(-glm::degrees(angleRad));
        particles->add(position + glm::vec3(-move_direction.x * 0.1, 0.1, -move_direction.y * 0.1), glm::vec3(0), glm::vec4(0.96, 0.87, 0.69, 1), 400, 1, true);
    }
}

void Player::set_state() {
    state_prev = state;
    if (move_direction != glm::vec2(0, 0)) { 
        state = WALKING; 
        if (anim_timer.look() * 8 > 7000) anim_timer.begin();
    }
    else {
        switch (state) {
        case CLICKING:
            if (anim_timer.look() * 2 > 6000) { state = DEFAULT; idle_timer.begin(); } // end animation
            break;

        case CLIMBING:
            if (anim_timer.look() >= 2000) anim_timer.begin();
            break;

        case IDLE:
            idle_timer.begin();
            if (anim_timer.look() * 2 > 9000) anim_timer.begin();
            break;

        case WALKING:
            state = DEFAULT;
            idle_timer.begin();
            break;

        case DEFAULT:
            if (idle_timer.look() * 2 > 2000) state = IDLE; // set idle after not moving for a few seconds
            break;
        }
    }
}

void Player::add_keyframes(Renderer& renderer) {
    if (state_prev != state) { anim_timer.begin(); }

    if (state == IDLE) draw_anim(renderer, idle, (int) state);
    else if (state == CLIMBING) draw_anim(renderer, climbing, (int)state);
    else if (state == CLICKING) draw_anim(renderer, clicking, (int)state);
    else if (state == WALKING) draw_anim(renderer, walking, (int)state);
    else draw_anim(renderer, idle, 0);

    //static Shader deferred_lighting_shader{ "deferred_lighting", "shader/copytex.vs", "shader/deferred_lighting.fs" };
    //int num = 2;
    //deferred_lighting_shader->uniform("player_num", num);
}

