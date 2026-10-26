#pragma once

#include "game_object.h"

using namespace std;

class PowerUp : public SphereObject
{
public:
    PowerUp();
    PowerUp(shared_ptr<vector<Drawelement>> draws, glm::ivec3 pos, float radius, 
        PowerUpType type);

    shared_ptr<GameObject> getNewObject() override;
    bool place() override;

    void resolveCollision(Player& player) override;

    virtual void collect() {};
    virtual bool update();

    PowerUpType type;
    bool to_remove;
};

class Coin : public PowerUp
{
public:
    Coin();
    Coin(shared_ptr<vector<Drawelement>> draws, glm::vec3 pos, float radius);

    void collect() override;
};

class Heart : public PowerUp
{
public:
    Heart();
    Heart(shared_ptr<vector<Drawelement>> draws, glm::vec3 pos, float radius);

    void collect() override;
};

class Key : public PowerUp
{
public:
    Key();
    Key(shared_ptr<vector<Drawelement>> draws, glm::vec3 pos, float radius);

    void collect() override;
};

class Shot : public PowerUp
{
public:
    Shot();
    Shot(shared_ptr<vector<Drawelement>> draws, glm::vec3 pos, float radius);

    void collect() override;
    bool update() override;

    bool respawn;
};

class Star : public PowerUp
{
public:
    Star();
    Star(shared_ptr<vector<Drawelement>> draws, glm::vec3 pos, float radius);

    void collect() override;
};



class Projectile : public SphereObject
{
public:
    Projectile();
    Projectile(shared_ptr<vector<Drawelement>> draws, glm::vec3 pos, float radius, glm::vec3 dir);

    void update();
    void draw(Renderer& renderer) override;

    void collision();

    bool shot;
    glm::vec3 dir;
    Timer duration;
};