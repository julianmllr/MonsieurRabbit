#pragma once

#include <cppgl.h>
#include <nlohmann_json/include/nlohmann/json.hpp>
#include "blocks.h"
#include "types.h"
#include "player.h"
#include "powerups.h"
#include "enemies.h"

#define B_TYPES 1

#define S_TYPES 0

using namespace std;

struct CompareVec3 {
    bool operator()(const glm::ivec3& a, const glm::ivec3& b) const {
        if (a.x < b.x) return true;
        if (a.x > b.x) return false;
        if (a.y < b.y) return true;
        if (a.y > b.y) return false;
        return (a.z < b.z);
    }
};

class GameLevel
{
public:
    static vector<shared_ptr<vector<Drawelement>>> object_draws;
    static vector<shared_ptr<vector<Drawelement>>> powerUp_draws;
    static vector<shared_ptr<vector<Drawelement>>> block_draws;
    static vector<shared_ptr<vector<Drawelement>>> enemy_draws;

    static shared_ptr<Block> switchBlockType(BlockType type, glm::ivec3 pos, float rotation);
    static shared_ptr<PowerUp> switchPowerUpType(PowerUpType type, glm::ivec3 pos);
    static shared_ptr<Enemy> switchEnemyType(EnemyType type, glm::ivec3 pos);
    GameLevel();

    bool placeCheck(glm::ivec3 pos);
    //bool check_powerUp(glm::ivec3 pos);
    void resize(glm::ivec3 pos);
    void addBlock(shared_ptr<Block> &block, glm::ivec3 pos);
    void addPowerUp(shared_ptr<PowerUp> &powerUp, glm::ivec3 pos);
    void addEnemy(shared_ptr<Enemy> &enemy, glm::ivec3 pos);
    void addPositionToPath(glm::ivec3 index, glm::ivec3 pos);
    void remove(glm::ivec3 pos);
    void block_clicked(glm::ivec3 pos);
    void create_link(glm::ivec3 link_pos, glm::ivec3 to_link_pos);
    // loads level
    void load(const string& fullFilePath);
    void exportToFile(const string& fullFilePath);

    void setCameraPos();
    void update();
    // render level
    void draw(Renderer &renderer);
    void raw_draw(Renderer& renderer);
    // check if the level is completed
    bool isCompleted();

    void createProjectile();
    // level state
    vector<vector<vector<BlockType>>> blocks_vec;
    vector<vector<vector<PowerUpType>>> powerUps_vec;
    vector<vector<vector<EnemyType>>> enemies_vec;
    vector<pair<glm::ivec3, glm::ivec3>> links_vec;
    vector<vector<vector<float>>> rotations_vec;
    map<glm::ivec3, vector<glm::ivec3>, CompareVec3> paths;

    unsigned int width, height, depth;
    glm::vec3 level_center;
    glm::vec3 spawn;

    map<glm::ivec3, shared_ptr<Block>, CompareVec3> blocks;
    map<glm::ivec3, shared_ptr<PowerUp>, CompareVec3> powerUps;
    map<glm::ivec3, shared_ptr<Enemy>, CompareVec3> enemies;
    Projectile projectile;

    //player
    shared_ptr<Player> player;

    unsigned int coin_amount;
    unsigned int max_coins;
    unsigned int key_amount;
    unsigned int shot_amount;
    unsigned int star_amount;
protected:
    // initialize level from tile data
    void loadFromFile(const string& fullFilePath);
    void initBlocks();
};