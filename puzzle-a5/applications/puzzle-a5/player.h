#pragma once

#include <cppgl.h>
#include "game_object.h"
#include "particles.h"

using namespace std;

enum player_state { DEFAULT, IDLE, WALKING, CLIMBING, CLICKING };
class Player : public SquareObject
{
    //vector<Drawelement>& default = *draws;
    Timer idle_timer;

public:
	Player();
	Player(shared_ptr<vector<Drawelement>> draws, glm::vec3 pos, glm::vec3 scale);

    void add_hp(int amount);
    void take_damage(int amount);
    void resetGravity();

	void update();
    void add_keyframes(Renderer& renderer_anim);
    void set_state();

	glm::vec2 move_direction;
    int max_hp;
    int hp;

    player_state state;
    player_state state_prev;

    Timer gravity_timer;
    Timer invincibility_timer;

    Timer climbing_timer;
    Timer move_timer;
};
extern Timer anim_timer; 
extern float anim_time;
extern Drawelement idle;
extern Drawelement climbing;
extern Drawelement clicking;
extern Drawelement walking;