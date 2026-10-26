#pragma once

#include <cppgl.h>
#include "renderer.h"
#include "types.h"

using namespace std;

struct AABB {
    glm::vec3 min;
    glm::vec3 max;
};

struct SBB {
    glm::vec3 center;
    float radius;
};

class SphereObject;
class SquareObject;
class Player;
class Projectile;
class Enemy;
class Block;

class GameObject {
public:

    GameObject();
    GameObject(shared_ptr<vector<Drawelement>> draws, glm::ivec3 pos, float rotation);

    virtual void draw(Renderer &renderer) = 0;
    virtual glm::vec3 detectCollision(SquareObject &square) = 0;
    virtual glm::vec3 detectCollision(SphereObject &sphere) = 0;
    virtual void resolveCollision(Player &player) = 0;
    virtual void resolveCollision(Projectile & projectile) = 0;
    virtual void resolveCollision(Enemy& enemy) = 0;
    virtual void resolveCollision(Block& block) = 0;
    virtual void calcBoundingBox() = 0;

    virtual shared_ptr<GameObject> getNewObject() { return nullptr; };
    virtual bool place() { return false; };
    virtual void placingRoutine(Renderer& renderer, glm::ivec3 pos);
    inline glm::vec3 get_position(){return position;}

    void setPosition(glm::vec3 new_position);
    void setTilePos(glm::ivec3 new_pos);
    void move(glm::vec3 vector);
    void setRotation(float angle);
    void rotate(float degrees);

    inline float get_rotation() { return rotation; }

    glm::ivec3 tile_pos;
protected:
    glm::vec3 position;
    glm::mat4 model;
    float rotation;
    glm::mat4 model_rot;
    shared_ptr<vector<Drawelement>> draws;
    shared_ptr<vector<Drawelement>> draws_red;
};

// represents a round object
class SphereObject : public GameObject
{
public:
    SphereObject();
    SphereObject(shared_ptr<vector<Drawelement>> draws, glm::vec3 pos, float radius, float rotation);

    void draw(Renderer &renderer) override;
    glm::vec3 detectCollision(SquareObject &square) override;
    glm::vec3 detectCollision(SphereObject& sphere) override;
    void resolveCollision(Player &player) override;
    void resolveCollision(Projectile& projectile) override;
    void resolveCollision(Enemy& enemy) override;
    void resolveCollision(Block& block) override{};

    void calcBoundingBox() override;

    SBB bb;
};

class SquareObject : public GameObject
{

public:
    SquareObject();
    SquareObject(shared_ptr<vector<Drawelement>> draws, glm::vec3 pos, glm::vec3 size, float rotation);

    void draw(Renderer &renderer) override;
    void draw_anim(Renderer& renderer, Drawelement keyframes, int pose);
    glm::vec3 detectCollision(SquareObject& square) override;
    glm::vec3 detectCollision(SphereObject& sphere) override;
    void resolveCollision(Player &player) override;
    void resolveCollision(Projectile& projectile) override;
    void resolveCollision(Enemy& enemy) override;
    void resolveCollision(Block& block) override{};
    void calcBoundingBox() override;

    glm::vec3 size;
    AABB bb;
};