#include "enemies.h"
#include "game.h"
#include "particles.h"
#include "input_handler.h"

Enemy::Enemy() : SquareObject(), type(NO_ENEMY), move_direction(0), hp(0) {}

Enemy::Enemy(shared_ptr<vector<Drawelement>> draws, glm::ivec3 pos, glm::vec3 size,
             int hp, EnemyType type) :
    SquareObject(draws, pos, size, 0.0f), type(type), 
    state(ENEMY_IDLE), attack_radius(4), movement_timer(), gravity_timer(), idle_duration(1500),
    stride_duration(500), move_direction(0), hp(hp), isSolid(true) { }

shared_ptr<GameObject> Enemy::getNewObject() {
    return GameLevel::switchEnemyType(type, tile_pos);
}

bool Enemy::place() {
    shared_ptr<Enemy> pointer = make_shared<Enemy>(*this);
    auto level = the_game->level;
    if (level->placeCheck(tile_pos)) {
        level->addEnemy(pointer, tile_pos);
        return true;
    }
    return false;
}

void Enemy::resolveCollision(Player& player) {
    glm::vec3 overlap = detectCollision(player);
    if (overlap == glm::vec3(0))
        return;

    // resolve collision along the axis with the smallest overlap
    AABB& aabb = player.bb;
    if (overlap.x < overlap.y && overlap.x < overlap.z) {
        if (bb.min.x < aabb.min.x) {
            player.move(glm::vec3(overlap.x, 0, 0));
        }
        else {
            player.move(glm::vec3(-overlap.x, 0, 0));
        }
        player.take_damage(1);
    }
    else if (overlap.y < overlap.z) {
        hp = 0;
    }
    else {
        if (bb.min.z < aabb.min.z) {
            player.move(glm::vec3(0, 0, overlap.z));
        }
        else {
            player.move(glm::vec3(0, 0, -overlap.z));
        }
        player.take_damage(1);
    }
}

void Enemy::resolveCollision(Projectile& projectile) {
    glm::vec3 direction = detectCollision(projectile);
    if (direction == glm::vec3(0))
        return;

    projectile.collision();
    take_damage(1);
}

void Enemy::resolveCollision(Enemy& enemy) {
    SquareObject::resolveCollision(enemy);
}

void Enemy::take_damage(int amount) {
    hp -= amount;
}

void Enemy::resetGravity() {
    gravity_timer.begin();
}

bool Enemy::update() {
    if (hp <= 0 || position.y < the_game->void_height) {
        particles_small->add(position + glm::vec3(0, 0.2, 0), glm::vec3(0), glm::vec4(0, 1, 0, 1), 1000, 200, true);
        return false;
    }

    // gravity
    static float gravity = -0.000001f;
    static float max_gravity = -0.4f;
    if (isSolid) {
        const float elapsed = float(gravity_timer.look());
        glm::vec3 gravity_move = glm::vec3(0.0f, max(max_gravity, elapsed * elapsed * gravity), 0.0f);
        move(gravity_move);
    }

    // move
    static float move_speed = 0.02f;
    move(move_speed * glm::vec3(move_direction.x, 0.0f, move_direction.y));

    // rotation
    if (move_direction != glm::vec2(0, 0)) {
        glm::vec2 v1 = glm::vec2(0, 1);
        glm::vec2 v2 = move_direction;
        float crossProduct = v1.x * v2.y - v1.y * v2.x;
        float dotProduct = glm::dot(v1, v2);
        float angleRad = std::atan2(crossProduct, dotProduct);
        setRotation(-glm::degrees(angleRad));
        particles->add(position + glm::vec3(-move_direction.x * 0.1, 0.1, -move_direction.y * 0.1), glm::vec3(0), glm::vec4(0.8, 0.8, 0.8, 1), 400, 1, true);
    }

    // calculate new move_direction
    auto player = the_game->level->player;
    glm::vec3 player_pos = player->get_position();

    float y_diff = abs(player_pos.y - position.y);
    glm::vec2 diff = glm::vec2(player_pos.x, player_pos.z) - glm::vec2(position.x, position.z);

    if (state == ENEMY_ATTACK) {
        // check if player is still nearby
        if (y_diff >= 1.0f || glm::length(diff) > attack_radius) {
            move_direction = glm::vec2(0);
            movement_timer.begin();
            state = ENEMY_IDLE;
            return true;
        }

        // update move_direction
        move_direction = glm::normalize(diff);
    }
    else {
        // check if player is nearby and attack if so
        if (y_diff < 1.0f && glm::length(diff) <= attack_radius) {
            move_direction = glm::normalize(diff);
            state = ENEMY_ATTACK;
            return true;;
        }

        // choose new random_direction or idle
        const float elapsed = float(movement_timer.look());
        if (elapsed > idle_duration) {
            if (elapsed > idle_duration + stride_duration) {
                move_direction = glm::vec2(0);
                movement_timer.begin();
                state = ENEMY_IDLE;
            }
            else if (state == ENEMY_IDLE) {
                move_direction = glm::normalize(random_vec2());
                state = ENEMY_STRIDE;
            }
        }
    }
    return true;
}



//------------------------
// OccupiedEnemy

OccupiedEnemy::OccupiedEnemy() : Enemy() {}

OccupiedEnemy::OccupiedEnemy(shared_ptr<vector<Drawelement>> draws, glm::ivec3 pos) :
    Enemy(draws, pos, glm::vec3(0), 1, OCCUPIED_ENEMY) {}

void OccupiedEnemy::placingRoutine(Renderer& renderer, glm::ivec3 pos) {
    setTilePos(pos);
    SquareObject::draw(renderer);
}

bool OccupiedEnemy::place() {
    shared_ptr<Enemy> pointer = make_shared<Enemy>(*this);
    auto level = the_game->level;
    if (level->placeCheck(tile_pos)) {
        level->addEnemy(pointer, tile_pos);
        InputHandler::placer.add(OCCUPIED_ENEMY);
        the_game->level->addPositionToPath(
                InputHandler::placer.place_objects[0]->tile_pos, tile_pos);
        return true;
    }
    return false;
}

void OccupiedEnemy::draw(Renderer& renderer) {
    if (the_game->state == GAME_BUILD)
        SquareObject::draw(renderer);
}

//-------------------------
// Ghost

Ghost::Ghost() : Enemy() {}

Ghost::Ghost(shared_ptr<vector<Drawelement>> draws, glm::ivec3 pos) :
    Enemy(draws, pos, enemy_sizes[GHOST], 1, GHOST)
{
    isSolid = false;
}

void Ghost::resolveCollision(Player& player) {
    glm::vec3 overlap = detectCollision(player);
    if (overlap == glm::vec3(0))
        return;

    // resolve collision along the axis with the smallest overlap
    AABB& aabb = player.bb;
    if (overlap.x < overlap.y && overlap.x < overlap.z) {
        player.take_damage(1);
    }
    else if (overlap.y < overlap.z) {
        hp = 0;
    }
    else {
        player.take_damage(1);
    }
}



//-------------------------
// Zombie

Zombie::Zombie() : Enemy() {}

Zombie::Zombie(shared_ptr<vector<Drawelement>> draws, glm::ivec3 pos) :
    Enemy(draws, pos, enemy_sizes[ZOMBIE], 2, ZOMBIE) {}



//-------------------------
// Spike

Spike::Spike() : Enemy() {}

Spike::Spike(shared_ptr<vector<Drawelement>> draws, glm::ivec3 pos, vector<glm::ivec3> rest_path) :
    Enemy(draws, pos, enemy_sizes[SPIKE], 3, SPIKE), index(0), next_index(1), dir(1)
{
    if (!rest_path.empty()) {
        vector<glm::ivec3> p;
        p.push_back(tile_pos);
        p.insert(p.end(), rest_path.begin(), rest_path.end());
        path = p;
        move_direction = normalize(glm::vec3(path[next_index]) - position);
    }
}

bool Spike::update() {
    if (hp <= 0 || position.y < the_game->void_height) {
        return false;
    }

    static float move_speed = 0.02f;
    glm::vec3 next_position = glm::vec3(path[next_index]);

    move(move_direction * move_speed);

    //cout << "position is: " << position << " next_position is: " << next_position << endl;
    if (glm::all(glm::epsilonEqual(position, next_position, 0.01f))) {
        setPosition(next_position);
        index = next_index;
        if (next_index == path.size() - 1 || next_index == 0)
            dir *= -1;
        next_index += dir;

        move_direction = normalize(glm::vec3(path[next_index]) - position);
    }
    return true;
}