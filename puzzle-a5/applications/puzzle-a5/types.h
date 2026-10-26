#pragma once

using namespace std;

class Player;

enum BlockType {
    NO_BLOCK = -1,
    GROUND,
    GRASS_HALF,
    SNOW_HALF,
    GRASS_BLOCK,
    SNOW_BLOCK,
    TREE_GREEN,
    TREE_SNOW,
    LADDER,
    BARREL,
    ROTATING_PLATFORM,
    PLATFORM,
    CLICK_PLATFORM,
    PRESSURE_PLATE,
    DOOR,
    SPIKES,
    TIMED_SPIKES,
    OCCUPIED,
    PORTAL,
    MOVABLE_BLOCK,
    DESTROYABLE_BLOCK,
    SIZE_TILE
};

enum PowerUpType {
    NO_POWERUP = -1,
    COIN,
    HEART,
    KEY,
    SHOT,
    STAR,
    SIZE_POWERUP
};

enum EnemyType {
    NO_ENEMY = -1,
    GHOST,
    ZOMBIE,
    SPIKE,
    OCCUPIED_ENEMY,
    SIZE_ENEMY
};

enum ObjectType {
    PROJECTILE,
    PLAYER,
    SIZE_OBJECT
};

inline vector<glm::vec3> init_block_sizes() {
    vector<glm::vec3> vec = vector<glm::vec3>(SIZE_TILE);
    vec[GROUND] = { 1, 1, 1 };
    vec[GRASS_HALF] = { 1, 0.5, 1 };
    vec[SNOW_HALF] = { 1, 0.5, 1 };
    vec[GRASS_BLOCK] = { 1, 1, 1 };
    vec[SNOW_BLOCK] = { 1, 1, 1 };
    vec[TREE_GREEN] = { 1, 2, 1 };
    vec[TREE_SNOW] = { 1, 2, 1 };
    vec[LADDER] = { 0.5, 1, 0.5 };
    vec[BARREL] = { 0.6, 0.8, 0.6 };
    vec[ROTATING_PLATFORM] = { 1, 0.25, 1 };
    vec[PLATFORM] = { 1, 0.25, 1 };
    vec[CLICK_PLATFORM] = { 1, 0.5, 1 };
    vec[PRESSURE_PLATE] = {1, 0.1, 1};
    vec[DOOR] = {1, 1, 1};
    vec[SPIKES] = {0.55,0.080,0.55};
    vec[TIMED_SPIKES] = {0.55,0.080,0.55};
    vec[PORTAL] = {1, 1, 1};
    vec[MOVABLE_BLOCK] = { 1,1,1};
    vec[DESTROYABLE_BLOCK] = { 1, 1, 1 };
    return vec;
}

// corresponding sizes for tiles
static vector<glm::vec3> block_sizes = init_block_sizes();

inline vector<float> init_powerUp_radi() {
    vector<float> vec = vector<float>(SIZE_POWERUP);
    vec[COIN] = 0.2f;
    vec[HEART] = 0.2f;
    vec[KEY] = 0.2f;
    vec[SHOT] = 0.2f;
    vec[STAR] = 0.2f;
    return vec;
}

static vector<float> powerUp_radi = init_powerUp_radi();

inline vector<glm::vec3> init_enemy_sizes() {
    vector<glm::vec3> vec = vector<glm::vec3>(SIZE_ENEMY);
    vec[GHOST] = { 0.4, 0.9, 0.4 };
    vec[ZOMBIE] = { 0.4, 0.9, 0.4 };
    vec[SPIKE] = { 0.4, 0.4, 0.4 };
    return vec;
}

static vector<glm::vec3> enemy_sizes = init_enemy_sizes();