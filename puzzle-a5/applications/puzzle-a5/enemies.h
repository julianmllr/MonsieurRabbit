#pragma once

#include <cppgl.h>
#include "game_object.h"

using namespace std;

enum EnemyState {
	ENEMY_IDLE,
	ENEMY_STRIDE,
	ENEMY_ATTACK
};

class Enemy : public SquareObject
{
public:
	Enemy();
	Enemy(shared_ptr<vector<Drawelement>> draws, glm::ivec3 pos, glm::vec3 size,
          int hp, EnemyType type);

	shared_ptr<GameObject> getNewObject() override;
	bool place() override;
	void resolveCollision(Player& player) override;
	void resolveCollision(Projectile & projectile) override;
    void resolveCollision(Enemy& enemy) override;

	virtual bool update();

    void take_damage(int amount);
	void resetGravity();

	EnemyType type;
	EnemyState state;

	float attack_radius;
	Timer movement_timer;
	Timer gravity_timer;
	float idle_duration;
	float stride_duration;
	glm::vec2 move_direction;

	int hp;

	bool isSolid;
};

class OccupiedEnemy : public Enemy
{
public:
    OccupiedEnemy();
    OccupiedEnemy(shared_ptr<vector<Drawelement>> draws, glm::ivec3 pos);

    void resolveCollision(Player& player) override {};
    void resolveCollision(Projectile & projectile) override {};
    void resolveCollision(Enemy& enemy) override {};
	bool update() override { return true; };

    void draw(Renderer& renderer) override;
    void placingRoutine(Renderer& renderer, glm::ivec3 pos) override;
    bool place() override;
};

class Ghost : public Enemy
{
public:
	Ghost();
	Ghost(shared_ptr<vector<Drawelement>> draws, glm::ivec3 pos);

	void resolveCollision(Player& player) override;
};

class Zombie : public Enemy
{
public:
	Zombie();
	Zombie(shared_ptr<vector<Drawelement>> draws, glm::ivec3 pos);
};

class Spike : public Enemy
{
public:
	Spike();
	Spike(shared_ptr<vector<Drawelement>> draws, glm::ivec3 pos, vector<glm::ivec3> path);

	bool update() override;

    vector<glm::ivec3> path;
    int index, next_index, dir;
    glm::vec3 move_direction;
};