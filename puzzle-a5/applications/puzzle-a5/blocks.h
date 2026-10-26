#pragma once

#include <cppgl.h>
#include "game_object.h"
#include "particles.h"

using namespace std;

class Platform;

// all objects that are to be inserted into the level data structure
class Block : public SquareObject
{
public:
    Block();
    Block(shared_ptr<vector<Drawelement>> draws, glm::ivec3 tile_pos,
          glm::vec3 size, BlockType type, float rotation);

    shared_ptr<GameObject> getNewObject() override;
    bool place() override;

    virtual bool update() { return true;};
    virtual void link(shared_ptr<Block> new_link) {};
    virtual void onClick() {};
    virtual void activate() {};
    virtual void deactivate() {};
    virtual void resetGravity() {};

    BlockType type;
    glm::vec2 direction;
};

class MovableBlock : public Block {
public:
    MovableBlock();
    MovableBlock(shared_ptr<vector<Drawelement>> draws, glm::ivec3 tile_pos,
            glm::vec3 size, float rotation);
    void resolveCollision(Player& player) override;
    void resolveCollision(Block& block) override;
    void resetGravity() override;
    bool update() override;
    Timer gravity_timer;
    bool stuck;
};

class DestroyableBlock : public Block
{
public:
    DestroyableBlock();
    DestroyableBlock(shared_ptr<vector<Drawelement>> draws, glm::ivec3 tile_pos,
          glm::vec3 size, BlockType type, float rotation);
    void resolveCollision(Projectile& projectile) override;
    bool update() override;

    bool to_remove;
};

class Occupied : public Block
{
public:
    Occupied();
    Occupied(shared_ptr<vector<Drawelement>> draws, glm::ivec3 tile_pos);

    void draw(Renderer& renderer) override;
    void resolveCollision(Player & player) override {};
    void resolveCollision(Projectile & projectile) override {};
    void resolveCollision(Enemy& enemy) override {};

    void link(shared_ptr<Block> new_link) override;
    void placingRoutine(Renderer& renderer, glm::ivec3 pos) override;
    bool place() override;
    void onClick() override;

    shared_ptr<Block> linked_block;
};

class Platform : public Block
{
public:
    Platform();
    Platform(shared_ptr<vector<Drawelement>> draws, glm::ivec3 tile_pos, glm::vec3 size,
             vector<glm::ivec3> path, float rotation);

    glm::vec3 calcElevatedPos(glm::ivec3 tile_pos);
    void placingRoutine(Renderer& renderer, glm::ivec3 pos) override;
    void resolveCollision(Player &player) override;
    void onClick() override;
    bool update() override;

    bool stopped;
    vector<glm::ivec3> path;
    int index, next_index, dir;
    glm::vec3 move_direction;
};

class RotatingBlock : public Block
{
public:
    RotatingBlock();
    RotatingBlock(shared_ptr<vector<Drawelement>> draws, glm::ivec3 tile_pos, glm::vec3 size,
                        int range, float rotation);

    void placingRoutine(Renderer& renderer, glm::ivec3 pos) override;
    bool place() override;
    void onClick() override;
//    void placingRoutine(Renderer &renderer, glm::ivec3) override;
//    bool place(shared_ptr<Block> pointer) override;
    bool update() override;
    void resolveCollision(Player &player) override;

    bool stopped;
    int range;
    float duration;
    glm::vec2 dir;
};

class ClickPlatform : public Block
{
public:
    ClickPlatform();
    ClickPlatform(shared_ptr<vector<Drawelement>> draws, glm::ivec3 tile_pos, glm::vec3 size,
                  vector<glm::ivec3> path, float rotation);

    glm::vec3 calcElevatedPos(glm::ivec3 tile_pos);
    void placingRoutine(Renderer& renderer, glm::ivec3 pos) override;
    void resolveCollision(Player& player) override;
    void onClick() override;
    bool update() override;

    bool moving;
    vector<glm::ivec3> path;
    int index, next_index, dir;
    glm::vec3 move_direction;
};

class Door : public Block
{
public:
    Door();
    Door(shared_ptr<vector<Drawelement>> draws, glm::ivec3 tile_pos, glm::vec3 size, float rotation);

    void resolveCollision(Player& player) override;
    void activate() override;
    void deactivate() override;

    shared_ptr<vector<Drawelement>> closed_draws;
    static shared_ptr<vector<Drawelement>> open_draws;
};

class PressurePlate : public Block
{
public:
    PressurePlate();
    PressurePlate(shared_ptr<vector<Drawelement>> draws, glm::ivec3 tile_pos, glm::vec3 size, float rotation);

    void link(shared_ptr<Block> new_link) override;
    void resolveCollision(Player& player) override;
    void resolveCollision(Enemy& enemy) override;
    void resolveCollision(Block & block) override;
    bool update() override;

    vector<shared_ptr<Block>> linked_blocks;
};

class Spikes : public Block
{
public:
    Spikes();
    Spikes(shared_ptr<vector<Drawelement>> draws, glm::ivec3 tile_pos, glm::vec3 size, float rotation);

    void activate() override;
    void deactivate() override;
    void resolveCollision(Player& player) override;

    static shared_ptr<vector<Drawelement>> in_draws;
    shared_ptr<vector<Drawelement>> out_draws;
    bool active;
};


class TimedSpikes : public Block
{
public:
    TimedSpikes();
    TimedSpikes(shared_ptr<vector<Drawelement>> draws, glm::ivec3 tile_pos, glm::vec3 size, float rotation);

    bool update() override;
    void resolveCollision(Player& player) override;

    shared_ptr<vector<Drawelement>> in_draws;
    static shared_ptr<vector<Drawelement>> out_draws;
    Timer duration;
    bool active;

private:
    void activate() override;
    void deactivate() override;
};

class Ladder : public Block
{
public:
    Ladder();
    Ladder(shared_ptr<vector<Drawelement>> &draws, glm::ivec3 tile_pos,
        glm::vec3 size, float rotation);

    void placingRoutine(Renderer& renderer, glm::ivec3 pos) override;
    void resolveCollision(Player& player) override;
};

class Portal : public Block
{
public:
    Portal();
    Portal(shared_ptr<vector<Drawelement>> draws, glm::ivec3 tile_pos, glm::vec3 size, float rotation);

    void draw(Renderer& renderer) override;
    void link(shared_ptr<Block> new_link) override;
    void resolveCollision(Player& player) override;
    void placingRoutine(Renderer& renderer, glm::ivec3 pos) override;

    shared_ptr<Block> linked_block;
};