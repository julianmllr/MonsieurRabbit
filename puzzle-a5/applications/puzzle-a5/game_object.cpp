#include "game_object.h"
#include "player.h"
#include "powerups.h"
#include "enemies.h"
#include "game.h"
#include "blocks.h"

GameObject::GameObject() : tile_pos(0), position(0.0f), model(1), rotation(0), model_rot(1) { }

GameObject::GameObject(shared_ptr<vector<Drawelement>> draws, glm::ivec3 pos, float rotation) : draws(draws),
    tile_pos(pos), position(pos), model(1), rotation(rotation), model_rot(1)
{
    model[3] = glm::vec4(pos, 1);
    model_rot = glm::rotate(glm::mat4(1), glm::radians(rotation), glm::vec3(0,1,0));
}

void GameObject::placingRoutine(Renderer& renderer, glm::ivec3 pos) {
    // tile_pos and actual position are identical
    setTilePos(pos);
    if(!the_game->level->placeCheck(pos))
        renderer.shader->uniform("red", 1);
    draw(renderer);
    if(!the_game->level->placeCheck(pos))
        renderer.shader->uniform("red", 0);
}

void GameObject::setPosition(glm::vec3 new_position) {
    position = new_position;
    model[3] = glm::vec4(new_position, 1);
    calcBoundingBox();
}

void GameObject::setTilePos(glm::ivec3 new_pos) {
    setPosition(new_pos);
    tile_pos = new_pos;
}

void GameObject::move(glm::vec3 vector) {
    position += vector;
    model[3] += glm::vec4(vector, 0);

    calcBoundingBox();
}

void GameObject::setRotation(float angle) {
    rotation = angle;
    model_rot = glm::rotate(glm::mat4(1), glm::radians(angle), glm::vec3(0,1,0));
}

void GameObject::rotate(float degrees) {
    rotation += degrees;
    model_rot = glm::rotate(model_rot, glm::radians(degrees), glm::vec3(0, 1, 0));
}



//-------------------
// SphereObject
SphereObject::SphereObject() : GameObject(), bb()  { }

SphereObject::SphereObject(shared_ptr<vector<Drawelement>> draws, glm::vec3 pos, float radius, float rotation) :
    GameObject(draws, pos, rotation), bb()
{
    bb.radius = radius;
    SphereObject::calcBoundingBox();
}

void SphereObject::calcBoundingBox() {
    bb.center = glm::vec3(position.x, position.y + bb.radius, position.z);
}

void SphereObject::draw(Renderer &renderer) {
    renderer.shader->uniform("bb_toggle", 1);
    renderer.shader->uniform("radius", bb.radius);
    renderer.shader->uniform("sphere_pos", bb.center);
    renderer.draw(*draws, model, model_rot);
}

glm::vec3 SphereObject::detectCollision(SquareObject& square) {
    AABB& aabb = square.bb;
    // check collision: get box closest point to sphere center by clamping
    glm::vec3 clamped_pos;
    clamped_pos.x = max(aabb.min.x, min(bb.center.x, aabb.max.x));
    clamped_pos.y = max(aabb.min.y, min(bb.center.y, aabb.max.y));
    clamped_pos.z = max(aabb.min.z, min(bb.center.z, aabb.max.z));

    glm::vec3 direction = clamped_pos - bb.center;
    float distance = glm::length(direction);

    if (distance < bb.radius) {
        return direction;
    }
    return glm::vec3(0);
}

glm::vec3 SphereObject::detectCollision(SphereObject& sphere) {
    SBB& sbb = sphere.bb;

    glm::vec3 direction = sbb.center - bb.center;
    float distance = glm::length(direction);

    if (distance < bb.radius + sbb.radius) {
        return direction;
    }
    return glm::vec3(0);
}

void SphereObject::resolveCollision(Player &player) {
    glm::vec3 direction = detectCollision(player);
    if (direction == glm::vec3(0))
        return;

    // resolve collision
    float distance = glm::length(direction);
    float penetration = bb.radius - distance;

    glm::vec3 normalized_direction = glm::normalize(direction);

    move(glm::vec3(penetration) * normalized_direction);
}

void SphereObject::resolveCollision(Projectile& projectile) {
    glm::vec3 direction = detectCollision(projectile);
    if (direction == glm::vec3(0))
        return;

    projectile.collision();
}

void SphereObject::resolveCollision(Enemy & enemy) {
    if (!enemy.isSolid)
        return;

    glm::vec3 direction = detectCollision(enemy);
    if (direction == glm::vec3(0))
        return;

    // resolve collision
    float distance = glm::length(direction);
    float penetration = bb.radius - distance;

    glm::vec3 normalized_direction = glm::normalize(direction);

    move(glm::vec3(penetration) * normalized_direction);
}



//-------------------------------
// SquareObject

SquareObject::SquareObject() : GameObject(), size(1), bb() {}

SquareObject::SquareObject(shared_ptr<vector<Drawelement>> draws, glm::vec3 pos, glm::vec3 size, float rotation) :
    GameObject(draws, pos, rotation), size(size), bb()
{
    SquareObject::calcBoundingBox();
}

void SquareObject::calcBoundingBox() {
    bb.min.x = position.x - (size.x / 2.0f);
    bb.max.x = position.x + (size.x / 2.0f);
    bb.min.y = position.y;
    bb.max.y = position.y + size.y;
    bb.min.z = position.z - (size.z / 2.0f);
    bb.max.z = position.z + (size.z / 2.0f);
}

void SquareObject::draw(Renderer& renderer) {
    renderer.shader->uniform("bb_toggle", 2);
    renderer.shader->uniform("boxMin", bb.min);
    renderer.shader->uniform("boxMax", bb.max);
    renderer.draw(*draws, model, model_rot);
}

void SquareObject::draw_anim(Renderer& renderer, Drawelement keyframes, int pose) {
    renderer.shader->uniform("bb_toggle", 2);
    renderer.shader->uniform("boxMin", bb.min);
    renderer.shader->uniform("boxMax", bb.max);
    renderer.draw_anim(keyframes, model, model_rot, pose);
}


glm::vec3 SquareObject::detectCollision(SquareObject& square) {
    AABB& aabb = square.bb;
    // check collision
    if (bb.min.x < aabb.max.x && aabb.min.x < bb.max.x &&
        bb.min.y < aabb.max.y && aabb.min.y < bb.max.y &&
        bb.min.z < aabb.max.z && aabb.min.z < bb.max.z) {

        glm::vec3 overlap;
        overlap.x = min(bb.max.x - aabb.min.x, aabb.max.x - bb.min.x);
        overlap.y = min(bb.max.y - aabb.min.y, aabb.max.y - bb.min.y);
        overlap.z = min(bb.max.z - aabb.min.z, aabb.max.z - bb.min.z);
        return overlap;
    }
    return glm::vec3(0);
}

glm::vec3 SquareObject::detectCollision(SphereObject& sphere) {
    SBB& sbb = sphere.bb;
    // check collision: get box closest point to sphere center by clamping
    glm::vec3 clamped_pos;
    clamped_pos.x = max(bb.min.x, min(sbb.center.x, bb.max.x));
    clamped_pos.y = max(bb.min.y, min(sbb.center.y, bb.max.y));
    clamped_pos.z = max(bb.min.z, min(sbb.center.z, bb.max.z));

    glm::vec3 direction = clamped_pos - sbb.center;
    float distance = glm::length(direction);

    if (distance < sbb.radius) {
        return direction;
    }
    return glm::vec3(0);
}

void SquareObject::resolveCollision(Player & player) {
    glm::vec3 overlap = detectCollision(player);
    if (overlap == glm::vec3(0))
        return;

    // resolve collision along the axis with the smallest overlap
    AABB& aabb = player.bb;
    if (overlap.x < overlap.y && overlap.x < overlap.z) {
        //cout << "x collision: overlap is: " << overlap.x << " bbmin is: " << bb.min.x
               // << " aabbmin is: " << aabb.min.x << " bbmax is: " << bb.max.x
          //  << " aabbmax is: " << aabb.max.x << endl;
        if (bb.min.x < aabb.min.x) {
            player.move(glm::vec3(overlap.x, 0, 0));
        }
        else {
            player.move(glm::vec3(-overlap.x, 0, 0));
        }
    }
    else if (overlap.y < overlap.z) {
        //cout << "y collision" << endl;
        if (bb.min.y < aabb.min.y) {
            player.move(glm::vec3(0, overlap.y, 0));
        }
        else {
            player.move(glm::vec3(0, -overlap.y, 0));
        }
        player.resetGravity();
    }
    else {
        //cout << "z collision" << endl;
        if (bb.min.z < aabb.min.z) {
            player.move(glm::vec3(0, 0, overlap.z));
        }
        else {
            player.move(glm::vec3(0, 0, -overlap.z));
        }
    }
}

void SquareObject::resolveCollision(Projectile& projectile) {
    glm::vec3 direction = detectCollision(projectile);
    if (direction == glm::vec3(0))
        return;

    projectile.collision();
}

void SquareObject::resolveCollision(Enemy& enemy) {
    if (enemy.type == GHOST || enemy.type == SPIKE)
        return;

    glm::vec3 overlap = detectCollision(enemy);
    if (overlap == glm::vec3(0))
        return;

    // resolve collision along the axis with the smallest overlap
    AABB& aabb = enemy.bb;
    if (overlap.x < overlap.y && overlap.x < overlap.z) {
        //cout << "x collision: overlap is: " << overlap.x << " bbmin is: " << bb.min.x
          //  << " aabbmin is: " << aabb.min.x << " bbmax is: " << bb.max.x
           // << " aabbmax is: " << aabb.max.x << endl;
        if (bb.min.x < aabb.min.x) {
            enemy.move(glm::vec3(overlap.x, 0, 0));
        }
        else {
            enemy.move(glm::vec3(-overlap.x, 0, 0));
        }
    }
    else if (overlap.y < overlap.z) {
        //cout << "y collision" << endl;
        if (bb.min.y < aabb.min.y) {
            enemy.move(glm::vec3(0, overlap.y, 0));
        }
        else {
            enemy.move(glm::vec3(0, -overlap.y, 0));
        }
        enemy.resetGravity();
    }
    else {
        //cout << "z collision" << endl;
        if (bb.min.z < aabb.min.z) {
            enemy.move(glm::vec3(0, 0, overlap.z));
        }
        else {
            enemy.move(glm::vec3(0, 0, -overlap.z));
        }
    }
}