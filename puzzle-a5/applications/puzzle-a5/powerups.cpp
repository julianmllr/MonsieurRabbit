#include "powerups.h"
#include "game.h"

//----------------------------
// PowerUp
PowerUp::PowerUp() : SphereObject(), type(NO_POWERUP) { }

PowerUp::PowerUp(shared_ptr<vector<Drawelement>> draws, glm::ivec3 pos, float radius, 
    PowerUpType type) :
    SphereObject(draws, pos, radius, 0.0f), type(type), to_remove(false) {}

shared_ptr<GameObject> PowerUp::getNewObject() {
    return GameLevel::switchPowerUpType(type, tile_pos);
}

bool PowerUp::place() {
    shared_ptr<PowerUp> pointer = make_shared<PowerUp>(*this);
    auto level = the_game->level;
    if (level->placeCheck(tile_pos)) {
        level->addPowerUp(pointer, tile_pos);
        return true;
    }
    return false;
}

void PowerUp::resolveCollision(Player & player) {
    glm::vec3 direction = detectCollision(player);
    if (direction == glm::vec3(0))
        return;

    float distance = glm::length(direction);
    // resolve collision
    if (distance < bb.radius) {
        cout << "powerup collision detected" << endl;
        collect();
    }
}

bool PowerUp::update() {
    if (to_remove)
        return false;
    static float rot_speed = 5.f;
    rotate(rot_speed);
    return true;
}



//----------------------------
// Coin

Coin::Coin() : PowerUp() {}

Coin::Coin(shared_ptr<vector<Drawelement>> draws, glm::vec3 pos, float radius) :
    PowerUp(draws, pos, radius, COIN) {}

void Coin::collect() {
    cout << "collect coin" << endl;
    the_game->level->coin_amount++;
    to_remove = true;
}



//----------------------
// Heart

Heart::Heart() : PowerUp() {}

Heart::Heart(shared_ptr<vector<Drawelement>> draws, glm::vec3 pos, float radius) :
        PowerUp(draws, pos, radius, HEART) {}

void Heart::collect() {
    cout << "collect heart" << endl;
    the_game->level->player->add_hp(1);
    to_remove = true;
}



//----------------------
// Key

Key::Key() : PowerUp() {}

Key::Key(shared_ptr<vector<Drawelement>> draws, glm::vec3 pos, float radius) :
        PowerUp(draws, pos, radius, KEY) {}

void Key::collect() {
    cout << "collect key" << endl;
    the_game->level->key_amount++;
    to_remove = true;
}


//-------------------
// Shot

Shot::Shot() : PowerUp() {}

Shot::Shot(shared_ptr<vector<Drawelement>> draws, glm::vec3 pos, float radius) :
        PowerUp(draws, pos, radius, SHOT), respawn(false) {}

void Shot::collect() {
    cout << "collect shot" << endl;
    the_game->level->shot_amount = 5;
}

bool Shot::update(){
    return true;
}



//-------------------
// Star

Star::Star() : PowerUp() {}

Star::Star(shared_ptr<vector<Drawelement>> draws, glm::vec3 pos, float radius) :
    PowerUp(draws, pos, radius, STAR) {}

void Star::collect() {
    cout << "collect star" << endl;
    the_game->level->star_amount++;
    to_remove = true;
}



//-------------------
// Projectile

Projectile::Projectile() : SphereObject(), shot(false) {}

Projectile::Projectile(shared_ptr<vector<Drawelement>> draws, glm::vec3 pos,
                       float radius, glm::vec3 dir) : SphereObject(draws, pos, radius, 0.0f), shot(true), dir(dir) {}

void Projectile::update()
{
    if (duration.look() > 2500.0f)
        shot = false;

    if (shot)
        setPosition(position + 0.2f * dir);
}

void Projectile::draw(Renderer& renderer) {
    if (shot)
    {
        renderer.draw(*draws, model, model_rot);
        particles_small->add(position + glm::vec3(0, 0.45, 0), glm::vec3(0), glm::vec4(1, 0, 0, 1), 250, 25, true);
    }
}

void Projectile::collision() {
    particles->add(position + glm::vec3(0, 0.2, 0), glm::vec3(0), glm::vec4(1, 0, 0, 1), 500, 50, true);
    shot = false;
}