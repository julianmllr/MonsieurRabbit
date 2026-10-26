#pragma once

#include <cppgl.h>
#include "renderer.h"
#include "blocks.h"

class Placer;

using namespace std;

class InputHandler {
public:
	static void keyboard_callback(int key, int scancode, int action, int mods);
	static void mouse_button_callback(int button, int action, int mods);
    static void mouse_scroll_callback(double xoffset, double yoffset);
	static void setAddRemovePos();
	static void build_input();
	static void active_input();
	static void player_input();

	static void addTmpBlock(BlockType type);
	static void drawTmpBlock(Renderer &renderer);

    static glm::vec3 hover_pos;
    static glm::ivec3 add_pos;
    static glm::ivec3 remove_pos;
	static Placer placer;
protected:

	static bool show_tmp_block;
	static glm::vec2 last_mouse_pos;
	static float camera_angle;
};

enum PlacerState {
    BLOCKS,
    POWERUPS,
    ENEMIES
};

class Placer
{
public:
    Placer();

    void set(BlockType types);
    void add(BlockType type);
    void set(PowerUpType type);
    void set(EnemyType type);
    void add(EnemyType type);
    void remove(glm::ivec3 pos);
    void clear();
    void rotate(float degrees);
    void place();
    void routine(Renderer &renderer);

    vector<shared_ptr<GameObject>> place_objects;
    bool block_linking;
private:
    PlacerState state;
    int index;
    vector<glm::ivec3> info;
};