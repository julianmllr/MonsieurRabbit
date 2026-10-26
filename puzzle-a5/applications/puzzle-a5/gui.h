#pragma once

#include <cppgl.h>

using namespace std;

class Gui {
	
public:
    static void loadGui();
    static void loadImages();
	static void drawMenu();
	static void drawGUI();
	static void drawBuildMode();
    static void drawWin();
    static void drawGameOver();
    static void drawPause();
    static void loadModels();
    static void drawLvl();

private:
    static Texture2D pause_texture;
    static Texture2D health_heart_texture;
    static Texture2D health_heart_dark_texture;
    static Texture2D ground_texture;
    static Texture2D grass_half_texture;
    static Texture2D snow_half_texture;
    static Texture2D grass_block_texture;
    static Texture2D snow_block_texture;
    static Texture2D tree_green_texture;
    static Texture2D tree_snow_texture;
    static Texture2D ladder_texture;
    static Texture2D moving_platform_texture;
    static Texture2D rotating_platform_texture;
    static Texture2D click_platform_texture;
    static Texture2D pressure_plate_texture;
    static Texture2D door_texture;
    static Texture2D door_pressure_texture;
    static Texture2D spikes_texture;
    static Texture2D timed_spikes_texture;
    static Texture2D portal_texture;
    static Texture2D movable_block_texture;
    static Texture2D destroyable_block_texture;
    static Texture2D spike_texture;

    static Texture2D coin_texture;
    static Texture2D heart_texture;
    static Texture2D key_texture;
    static Texture2D shot_texture;
    static Texture2D jewel_texture;

    static Texture2D title;
    static Texture2D lvl_1;
    static Texture2D lvl_2;
    static Texture2D lvl_3;

    static Texture2D lvl_2_g;
    static Texture2D lvl_3_g;
     
    static Texture2D ghost_texture;
    static Texture2D zombie_texture;

    static string cur_filename;

	static void drawModel(vector<Drawelement>, glm::mat4, glm::mat4);
};

inline ImVec2 toImVec2(glm::vec2 v) { return ImVec2(v.x, v.y); }
