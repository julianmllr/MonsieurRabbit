#include "blocks.h"
#include "game_level.h"
#include "game.h"
#include "input_handler.h"

shared_ptr<vector<Drawelement>> Door::open_draws;
shared_ptr<vector<Drawelement>> Spikes::in_draws;
shared_ptr<vector<Drawelement>> TimedSpikes::out_draws;


glm::vec2 calcDir(float degrees){
    int rot = (int)round(degrees) % 360;
    if(rot == 90 || rot == -270)
        return glm::vec2(1,0);
    else if(rot == -90 || rot == 270)
        return glm::vec2(-1,0);
    else if(abs(rot) == 180)
        return glm::vec2(0,-1);
    else
        return glm::vec2(0,1);
}
//todo ich weiß gerade unschön aber auf die schnelle musste ichs so machen

//-------------------------------
// Block

Block::Block() : SquareObject(), type(NO_BLOCK), direction(0) {}

Block::Block(shared_ptr<vector<Drawelement>> draws, glm::ivec3 tile_pos,
             glm::vec3 size, BlockType type, float rotation)
    : SquareObject(draws, tile_pos, size, rotation), type(type), direction(0)
{ }

shared_ptr<GameObject> Block::getNewObject() {
    return GameLevel::switchBlockType(type, tile_pos, rotation);
}

bool Block::place() {
    shared_ptr<Block> pointer = make_shared<Block>(*this);
    auto level = the_game->level;
    if (level->placeCheck(tile_pos)) {
        level->addBlock(pointer, tile_pos);
        return true;
    }
    return false;
}


//-------------------------------
// MovableBlock

MovableBlock::MovableBlock() : Block() {}

MovableBlock::MovableBlock(shared_ptr<vector<Drawelement>> draws, glm::ivec3 tile_pos,
             glm::vec3 size, float rotation)
        : Block(draws, tile_pos, size, MOVABLE_BLOCK, rotation), gravity_timer(), stuck(false)
{
    model = glm::scale(model, glm::vec3(1.8f));
}

void MovableBlock::resolveCollision(Player &player) {
    glm::vec3 overlap = detectCollision(player);
    if (overlap == glm::vec3(0))
        return;

    cout << stuck << endl;
    // resolve collision along the axis with the smallest overlap
    AABB& aabb = player.bb;
    if (overlap.x < overlap.y && overlap.x < overlap.z) {
        if (bb.min.x < aabb.min.x) {
            if(!stuck)
                move(glm::vec3(-overlap.x, 0, 0));
            else {
                //cout << "player push" << endl;
                player.move(glm::vec3(overlap.x, 0, 0));
            }
        }
        else {
            if(!stuck)
                move(glm::vec3(overlap.x, 0, 0));
            else {
                player.move(glm::vec3(-overlap.x, 0, 0));
                //cout << "player push" << endl;
            }
        }
    }
    else if (overlap.y < overlap.z) {
        if (bb.min.y < aabb.min.y) {
            player.move(glm::vec3(0, overlap.y, 0));
        }
        else {
            player.move(glm::vec3(0, -overlap.y, 0));
        }
        player.resetGravity();
    }
    else {
        if (bb.min.z < aabb.min.z) {
            if(!stuck)
                move(glm::vec3(0, 0, -overlap.z));
            else {
                //cout << "player push" << endl;
                player.move(glm::vec3(0, 0, overlap.z));
            }
        }
        else {
            if(!stuck)
                move(glm::vec3(0, 0, overlap.z));
            else {
                //cout << "player push" << endl;
                player.move(glm::vec3(0, 0, -overlap.z));
            }
        }
    }
}

void MovableBlock::resolveCollision(Block& block){
    glm::vec3 overlap = detectCollision(block);
    if (overlap == glm::vec3(0) || block.type == PRESSURE_PLATE)
        return;

    stuck = false;
    // resolve collision along the axis with the smallest overlap
    AABB& aabb = block.bb;
    if (overlap.x < overlap.y && overlap.x < overlap.z) {
        cout << "x collision: overlap is: " << overlap.x << " bbmin is: " << bb.min.x
             << " aabbmin is: " << aabb.min.x << " bbmax is: " << bb.max.x
             << " aabbmax is: " << aabb.max.x << endl;
        if (bb.min.x < aabb.min.x) {
            move(glm::vec3(-overlap.x, 0, 0));
        }
        else {
            move(glm::vec3(overlap.x, 0, 0));
        }
        stuck = true;
    }
    else if (overlap.y < overlap.z) {
        //cout << "y collision" << endl;
        if (bb.min.y < aabb.min.y) {
            move(glm::vec3(0, -overlap.y, 0));
        }
        else {
            move(glm::vec3(0, overlap.y, 0));
        }
        resetGravity();
    }
    else {
        cout << "z collision" << endl;
        if (bb.min.z < aabb.min.z) {
            move(glm::vec3(0, 0, -overlap.z));
        }
        else {
            move(glm::vec3(0, 0, overlap.z));
        }
        stuck = true;
    }
}

void MovableBlock::resetGravity() {
    gravity_timer.begin();
}

bool MovableBlock::update(){
    if (position.y <= the_game->void_height) {
        return false;
    }
    static float gravity = -0.00003f;
    static float max_gravity = -0.4f;
    const float elapsed = float(gravity_timer.look());
    glm::vec3 gravity_move = glm::vec3(0.0f, max(max_gravity, elapsed * elapsed * gravity), 0.0f);
    move(gravity_move);
    return true;
}
//-------------------------------
// DestroyableBlock

DestroyableBlock::DestroyableBlock() : Block() {}

DestroyableBlock::DestroyableBlock(shared_ptr<vector<Drawelement>> draws, glm::ivec3 tile_pos,
             glm::vec3 size, BlockType type, float rotation)
        : Block(draws, tile_pos, size, DESTROYABLE_BLOCK, rotation), to_remove(false)
{
    model = glm::scale(model, glm::vec3(2));
}

void DestroyableBlock::resolveCollision(Projectile& projectile){
    glm::vec3 direction = detectCollision(projectile);
    if (direction == glm::vec3(0))
        return;
    to_remove = true;
    projectile.collision();
}

bool DestroyableBlock::update() {
    return !to_remove;
}



//-------------------------------
// Occupied
Occupied::Occupied() : Block() {}

Occupied::Occupied(shared_ptr<vector<Drawelement>> draws, glm::ivec3 tile_pos) :
    Block(draws, tile_pos, glm::vec3(0), OCCUPIED, 0.0f) {
}

void Occupied::link(shared_ptr<Block> new_link) {
    linked_block = new_link;
}

void Occupied::placingRoutine(Renderer& renderer, glm::ivec3 pos) {
    setTilePos(pos);
    SquareObject::draw(renderer);
}

bool Occupied::place() {
    shared_ptr<Block> pointer = make_shared<Block>(*this);
    auto level = the_game->level;
    if (level->placeCheck(tile_pos)) {
        level->addBlock(pointer, tile_pos);
        InputHandler::placer.add(OCCUPIED);
        the_game->level->addPositionToPath(
            InputHandler::placer.place_objects[0]->tile_pos, tile_pos);
        return true;
    }
    return false;
}

void Occupied::onClick() {
    if (linked_block)
        linked_block->onClick();
}

void Occupied::draw(Renderer& renderer) {
    if (the_game->state == GAME_BUILD)
        SquareObject::draw(renderer);
}

//-------------------------------
// Platform

Platform::Platform() : Block(), stopped(false) {}

Platform::Platform(shared_ptr<vector<Drawelement>> draws, glm::ivec3 tile_pos,
                   glm::vec3 size, vector<glm::ivec3> rest_path, float rotation)
    : Block(draws, tile_pos, size, PLATFORM, rotation), stopped(false),
    index(0), next_index(1), dir(1)
{
    if (!rest_path.empty()) {
        vector<glm::ivec3> p;
        p.push_back(tile_pos);
        p.insert(p.end(), rest_path.begin(), rest_path.end());
        path = p;
        setPosition(calcElevatedPos(tile_pos));
        move_direction = normalize(calcElevatedPos(path[next_index]) - position);
    }
}

glm::vec3 Platform::calcElevatedPos(glm::ivec3 tile_pos) {
    return {tile_pos.x, tile_pos.y + (1-size.y), tile_pos.z};
}

void Platform::placingRoutine(Renderer &renderer, glm::ivec3 pos) {
    tile_pos = pos;
    // elevate the actual position, but not the tile_pos
    setPosition(calcElevatedPos(tile_pos));
    draw(renderer);
    if(!the_game->level->placeCheck(pos))
        renderer.shader->uniform("red", 1);
    draw(renderer);
    if(!the_game->level->placeCheck(pos))
        renderer.shader->uniform("red", 0);
}

void Platform::resolveCollision(Player &player){
    bool stopped_save = stopped;
    stopped = false;

    glm::vec3 overlap = detectCollision(player);
    if (overlap == glm::vec3(0))
        return;

    // resolve collision along the axis with the smallest overlap
    AABB& aabb = player.bb;
    if (overlap.x < overlap.y && overlap.x < overlap.z) {
        cout << "x collision: overlap is: " << overlap.x << " bbmin is: " << bb.min.x
             << " aabbmin is: " << aabb.min.x << " bbmax is: " << bb.max.x
             << " aabbmax is: " << aabb.max.x << endl;
        if (bb.min.x < aabb.min.x) {
            player.move(glm::vec3(overlap.x, 0, 0));
        }
        else {
            player.move(glm::vec3(-overlap.x, 0, 0));
        }
    }
    else if (overlap.y < overlap.z) {
        if (bb.min.y < aabb.min.y) {
            if (!stopped_save)
                player.move(glm::vec3(move_direction.x * 0.02f, move_direction.y * 0.02f, move_direction.z * 0.02f));
            else 
                player.move(glm::vec3(0, overlap.y, 0));
        }
        else {
            player.move(glm::vec3(0, -overlap.y, 0));
        }
        player.resetGravity();
    }
    else {
        cout << "z collision" << endl;
        if (bb.min.z < aabb.min.z) {
            player.move(glm::vec3(0, 0, overlap.z));
        }
        else {
            player.move(glm::vec3(0, 0, -overlap.z));
        }
    }
}

void Platform::onClick() {
    cout << "block clicked" << endl;
    the_game->level->player->state = CLICKING;
    
    stopped = true;
}

bool Platform::update()
{
    glm::vec3 next_position = calcElevatedPos(path[next_index]);
    static float move_speed = 0.02f;

    if (!stopped)
        move(move_direction * move_speed);

    //cout << "position is: " << position << " next_position is: " << next_position << endl;
    if (glm::all(glm::epsilonEqual(position, next_position, 0.01f))) {
        setPosition(next_position);
        index = next_index;
        if (next_index == path.size() - 1 || next_index == 0)
            dir *= -1;
        next_index += dir;

        move_direction = normalize(calcElevatedPos(path[next_index]) - position);
    }

    return true;
}



//-------------------------------
// RotatingBlock

RotatingBlock::RotatingBlock() : Block(), stopped(false), range(0), duration(0), dir(glm::vec2(0)) {}

RotatingBlock::RotatingBlock(shared_ptr<vector<Drawelement>> draws, glm::ivec3 tile_pos,
                                         glm::vec3 size, int range, float rotation)
        : Block(draws, tile_pos, size, ROTATING_PLATFORM, rotation), stopped(false), range(range), duration(0), dir(glm::vec2(0))
{
    setPosition(glm::vec3(tile_pos) + glm::vec3(range,(1-size.y), 0));
}

void RotatingBlock::placingRoutine(Renderer &renderer, glm::ivec3 pos) {
    tile_pos = pos + glm::ivec3(range,0,0);
    setPosition(glm::vec3(tile_pos) + glm::vec3(0,(1-size.y), 0));
    if(!the_game->level->placeCheck(pos))
        renderer.shader->uniform("red", 1);
    draw(renderer);
    if(!the_game->level->placeCheck(pos))
        renderer.shader->uniform("red", 0);
    // draw occupying blocks

    for (int i = -range; i <= range; i++) {
        for (int j = -range; j <= range; ++j) {
            if (!(i == range && j == 0)) {
                glm::mat4 up_model = model;
                up_model[3] = glm::vec4(pos.x + i, pos.y, pos.z + j, 1.0f);
                renderer.draw(*GameLevel::block_draws[OCCUPIED], up_model);
            }
        }
    }
}

bool RotatingBlock::place() {
    shared_ptr<Block> pointer = make_shared<Block>(*this);
    auto level = the_game->level;
    // check all tiles
    for (int i = -range; i <= range; i++) {
        for (int j = -range; j <= range; ++j) {
            glm::ivec3 new_pos(tile_pos.x - range + i, tile_pos.y, tile_pos.z + j);
            if (!level->placeCheck(new_pos))
                return false;
        }
    }

    // add at all positions
    level->addBlock(pointer, tile_pos);
    for (int i = -range; i <= range; i++) {
        for (int j = -range; j <= range; ++j) {
            if (!(i == range && j == 0)) {
                glm::ivec3 new_pos(tile_pos.x - range + i, tile_pos.y, tile_pos.z + j);
                shared_ptr<Block> occ = GameLevel::switchBlockType(OCCUPIED, new_pos, 0.0f);
                level->addBlock(occ, new_pos);
                level->create_link(occ->tile_pos, tile_pos);
            }
        }
    }
    return true;
}

void RotatingBlock::onClick() {
    cout << "block clicked" << endl;
    the_game->level->player->state = CLICKING;
    stopped = true;
}

bool RotatingBlock::update()
{
    if(stopped) {
        dir = glm::vec2(0);
        stopped = false;
        return true;
    }

    static float rot_speed = 0.01f;
    duration += rot_speed;

    if(duration / (10.f * (float)range) > 1.0f)
        duration = 0.f;

    static float PI = 3.14159265358979323846;
    float theta = (2 * PI) * (duration / (2.f * (float)range));
    float x = (float)range * std::cos(theta);
    float z = (float)range * std::sin(theta);
    glm::vec3 new_pos = glm::vec3(tile_pos.x - range + x, position.y, tile_pos.z + z);
    dir = glm::vec2(new_pos.x - position.x, new_pos.z - position.z);
    setPosition(new_pos);
    stopped = false;
    return true;
}

void RotatingBlock::resolveCollision(Player &player) {
    glm::vec3 overlap = detectCollision(player);
    if (overlap == glm::vec3(0))
        return;

    // resolve collision along the axis with the smallest overlap
    AABB& aabb = player.bb;
    if (overlap.x < overlap.y && overlap.x < overlap.z) {
        cout << "x collision: overlap is: " << overlap.x << " bbmin is: " << bb.min.x
             << " aabbmin is: " << aabb.min.x << " bbmax is: " << bb.max.x
             << " aabbmax is: " << aabb.max.x << endl;
        if (bb.min.x < aabb.min.x) {
            player.move(glm::vec3(overlap.x, 0, 0));
        }
        else {
            player.move(glm::vec3(-overlap.x, 0, 0));
        }
    }
    else if (overlap.y < overlap.z) {
        if (bb.min.y < aabb.min.y) {
            player.move(glm::vec3(dir.x, +overlap.y, dir.y));
        }
        else {
            player.move(glm::vec3(0, -overlap.y, 0));
        }
        player.resetGravity();
    }
    else {
        cout << "z collision" << endl;
        if (bb.min.z < aabb.min.z) {
            player.move(glm::vec3(0, 0, overlap.z));
        }
        else {
            player.move(glm::vec3(0, 0, -overlap.z));
        }
    }
}
//-------------------------------
// ClickPlatform

ClickPlatform::ClickPlatform() : Block(), moving(false) {}

ClickPlatform::ClickPlatform(shared_ptr<vector<cppgl::Drawelement>> draws, glm::ivec3 tile_pos, glm::vec3 size,
                             vector<glm::ivec3> rest_path, float rotation) :
        Block(draws, tile_pos, size, CLICK_PLATFORM,  rotation), moving(false),
        index(0), next_index(1), dir(1)
{
    if (!rest_path.empty()) {
    vector<glm::ivec3> p;
    p.push_back(tile_pos);
    p.insert(p.end(), rest_path.begin(), rest_path.end());
    path = p;
    setPosition(calcElevatedPos(tile_pos));
    move_direction = normalize(calcElevatedPos(path[next_index]) - position);
    }
}

glm::vec3 ClickPlatform::calcElevatedPos(glm::ivec3 tile_pos) {
    return {tile_pos.x, tile_pos.y + (1-size.y), tile_pos.z};
}

void ClickPlatform::placingRoutine(Renderer &renderer, glm::ivec3 pos) {
    tile_pos = pos;
    // elevate the actual position, but not the tile_pos
    setPosition(calcElevatedPos(tile_pos));
    draw(renderer);
}

void ClickPlatform::resolveCollision(Player& player) {
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
            if (moving)
                player.move(glm::vec3(move_direction.x * 0.02f, move_direction.y * 0.02f, move_direction.z * 0.02f));
            else
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

void ClickPlatform::onClick() {
    the_game->level->player->state = CLICKING;
    moving = true;
}

bool ClickPlatform::update() {
    if (!moving)
        return true;

    static float move_speed = 0.02f;
    glm::vec3 next_position = calcElevatedPos(path[next_index]);

    move(move_direction * move_speed);

    cout << "position is: " << position << " next_position is: " << next_position << endl;
    if (glm::all(glm::epsilonEqual(position, next_position, 0.01f))) {
        setPosition(next_position);
        index = next_index;
        if (next_index == path.size() - 1 || next_index == 0)
            dir *= -1;
        next_index += dir;

        move_direction = normalize(calcElevatedPos(path[next_index]) - position);
        moving = false;
    }
    return true;
}



//--------------------------------
// Door

Door::Door() : Block() { }

Door::Door(shared_ptr<vector<Drawelement>> draws, glm::ivec3 tile_pos, glm::vec3 size, float rotation) :
    Block(draws, tile_pos, size, DOOR, rotation), closed_draws(draws)
{ 
    // init draws for open door
    if (!open_draws) {
        static auto dummy_openDoor_shader = Shader("dummy_openDoor", "shader/pos+norm+tc.vs", "shader/pos+norm+tc.fs");
        open_draws = make_shared<vector<Drawelement>>();
        auto meshes = load_meshes_gpu("render-data/models/stone/doorLargeOpen.obj", false);
        for (auto& m : meshes)
        {
            Drawelement d = Drawelement(m->name, dummy_openDoor_shader, m);
            open_draws->push_back(d);
        }
    }
}

void Door::resolveCollision(Player &player) {
    glm::vec3 overlap = detectCollision(player);
    if (overlap == glm::vec3(0))
        return;

    if(draws == closed_draws && the_game->level->key_amount > 0){
        the_game->level->key_amount--;
        the_game->level->player->state = CLICKING;
        activate();
        return;
    }

    // resolve collision along the axis with the smallest overlap
    AABB& aabb = player.bb;
    if (overlap.x < overlap.y && overlap.x < overlap.z) {
        //cout << "x collision: overlap is: " << overlap.x << " bbmin is: " << bb.min.x
                //<< " aabbmin is: " << aabb.min.x << " bbmax is: " << bb.max.x
               // << " aabbmax is: " << aabb.max.x << endl;
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
        cout << "z collision" << endl;
        if (bb.min.z < aabb.min.z) {
            player.move(glm::vec3(0, 0, overlap.z));
        }
        else {
            player.move(glm::vec3(0, 0, -overlap.z));
        }
    }
}

void Door::activate() {
    size = glm::vec3(0);
    calcBoundingBox();
    draws = open_draws;
}

void Door::deactivate() {
    size = block_sizes[DOOR];
    calcBoundingBox();
    draws = closed_draws;
}



//--------------------------------
// PressurePlate

PressurePlate::PressurePlate() : Block() { }

PressurePlate::PressurePlate(shared_ptr<vector<cppgl::Drawelement>> draws, glm::ivec3 tile_pos, glm::vec3 size, float rotation) :
    Block(draws, tile_pos, size, PRESSURE_PLATE, rotation) { }

void PressurePlate::link(shared_ptr<Block> new_link) {
    linked_blocks.push_back(new_link);
}

// dont forget player.collision()!
void PressurePlate::resolveCollision(Player& player) {
    glm::vec3 overlap = detectCollision(player);
    if (overlap == glm::vec3(0)) {
        std::cout << 0;
        return;
    }
    for (auto& linked_block : linked_blocks)
        linked_block->activate();

    AABB& aabb = player.bb;
    float climb = size.y;
    if (overlap.x < overlap.y && overlap.x < overlap.z) {
        //cout << "x collision" << endl
        if (bb.min.x < aabb.min.x) {
            player.move(glm::vec3(overlap.x, climb, 0));
        }
        else {
            player.move(glm::vec3(-overlap.x, climb, 0));
        }
    }
    else if (overlap.y < overlap.z) {
        //cout << "y collision" << endl;
        if (bb.min.y < aabb.min.y) {
            the_game->level->player->state = CLICKING;
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
            player.move(glm::vec3(0, climb, overlap.z));
        }
        else {
            player.move(glm::vec3(0, climb, -overlap.z));
        }
    }
}

void PressurePlate::resolveCollision(Enemy & enemy) {
    glm::vec3 overlap = detectCollision(enemy);
    if (overlap == glm::vec3(0)) {
        return;
    }
    for (auto& linked_block : linked_blocks)
        linked_block->activate();

    AABB& aabb = enemy.bb;
    float climb = size.y;
    if (overlap.x < overlap.y && overlap.x < overlap.z) {
        //cout << "x collision" << endl
        if (bb.min.x < aabb.min.x) {
            enemy.move(glm::vec3(overlap.x, climb, 0));
        }
        else {
            enemy.move(glm::vec3(-overlap.x, climb, 0));
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
            enemy.move(glm::vec3(0, climb, overlap.z));
        }
        else {
            enemy.move(glm::vec3(0, climb, -overlap.z));
        }
    }
}

void PressurePlate::resolveCollision(Block & block) {
    glm::vec3 overlap = detectCollision(block);
    if (overlap == glm::vec3(0)) {
        return;
    }
    cout << "activating" << endl;
    for (auto& linked_block : linked_blocks)
        linked_block->activate();

    AABB& aabb = block.bb;
    float climb = size.y;
    if (overlap.x < overlap.y && overlap.x < overlap.z) {
        //cout << "x collision" << endl
        if (bb.min.x < aabb.min.x) {
            block.move(glm::vec3(overlap.x, climb, 0));
        }
        else {
            block.move(glm::vec3(-overlap.x, climb, 0));
        }
    }
    else if (overlap.y < overlap.z) {
        //cout << "y collision" << endl;
        if (bb.min.y < aabb.min.y) {
            block.move(glm::vec3(0, overlap.y, 0));
        }
        else {
            block.move(glm::vec3(0, -overlap.y, 0));
        }
        block.resetGravity();
    }
    else {
        //cout << "z collision" << endl;
        if (bb.min.z < aabb.min.z) {
            block.move(glm::vec3(0, climb, overlap.z));
        }
        else {
            block.move(glm::vec3(0, climb, -overlap.z));
        }
    }
}

bool PressurePlate::update() {
    for (auto& linked_block : linked_blocks)
        linked_block->deactivate();
    return true;
}



//-------------------------------
// Spikes

Spikes::Spikes() : Block(), active(true) { }

Spikes::Spikes(shared_ptr<vector<Drawelement>> draws, glm::ivec3 tile_pos, glm::vec3 size, float rotation) :
        Block(draws, tile_pos, size, SPIKES, rotation), out_draws(draws), active(true)
{
    // init draws for out spikes
    if (!in_draws) {
        static auto dummy_outSpikes_shader = Shader("dummy_outSpikes", "shader/pos+norm+tc.vs", "shader/pos+norm+tc.fs");
        in_draws = make_shared<vector<Drawelement>>();
        auto meshes = load_meshes_gpu("render-data/models/traps/spikesHidden.obj", false);
        for (auto& m : meshes)
        {
            Drawelement d = Drawelement(m->name, dummy_outSpikes_shader, m);
            in_draws->push_back(d);
        }
    }
}

void Spikes::resolveCollision(Player &player) {
    glm::vec3 overlap = detectCollision(player);
    if (overlap == glm::vec3(0))
        return;

    if(active)
        player.take_damage(1);

    AABB& aabb = player.bb;
    float climb = size.y;
    if (overlap.x < overlap.y && overlap.x < overlap.z) {
        //cout << "x collision" << endl
        if (bb.min.x < aabb.min.x) {
            player.move(glm::vec3(overlap.x, climb, 0));
        }
        else {
            player.move(glm::vec3(-overlap.x, climb, 0));
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
            player.move(glm::vec3(0, climb, overlap.z));
        }
        else {
            player.move(glm::vec3(0, climb, -overlap.z));
        }
    }
}

void Spikes::activate() {
    draws = in_draws;
    active = false;
}

void Spikes::deactivate() {
    draws = out_draws;
    active = true;
}



//-------------------------------
// TimedSpike

TimedSpikes::TimedSpikes() : Block(), active(false) { }

TimedSpikes::TimedSpikes(shared_ptr<vector<Drawelement>> draws, glm::ivec3 tile_pos, glm::vec3 size, float rotation) :
        Block(draws, tile_pos, size, TIMED_SPIKES, rotation), in_draws(draws), active(false)
{
    // init draws for out spikes
    if (!out_draws) {
        static auto dummy_outSpikes_shader = Shader("dummy_outSpikes", "shader/pos+norm+tc.vs", "shader/pos+norm+tc.fs");
        out_draws = make_shared<vector<Drawelement>>();
        auto meshes = load_meshes_gpu("render-data/models/traps/spikes.obj", false);
        for (auto& m : meshes)
        {
            Drawelement d = Drawelement(m->name, dummy_outSpikes_shader, m);
            out_draws->push_back(d);
        }
    }
    duration.begin();
}

bool TimedSpikes::update() {
    if (duration.look() >= 1500.0){
        if(active)
            deactivate();
        else
            activate();
    }
    return true;
}

void TimedSpikes::resolveCollision(Player &player) {
    glm::vec3 overlap = detectCollision(player);
    if (overlap == glm::vec3(0))
        return;

    if(active)
        player.take_damage(1);

    AABB& aabb = player.bb;
    float climb = size.y;
    if (overlap.x < overlap.y && overlap.x < overlap.z) {
        //cout << "x collision" << endl
        if (bb.min.x < aabb.min.x) {
            player.move(glm::vec3(overlap.x, climb, 0));
        }
        else {
            player.move(glm::vec3(-overlap.x, climb, 0));
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
            player.move(glm::vec3(0, climb, overlap.z));
        }
        else {
            player.move(glm::vec3(0, climb, -overlap.z));
        }
    }
}

void TimedSpikes::activate() {
    draws = out_draws;
    active = true;
    duration.begin();
}

void TimedSpikes::deactivate() {
    draws = in_draws;
    active = false;
    duration.begin();
}



//-------------------------------
// Ladder

Ladder::Ladder() : Block() {}

Ladder::Ladder(shared_ptr<vector<Drawelement>> &draws, glm::ivec3 tile_pos,glm::vec3 size, float rotation) :
    Block(draws, tile_pos, size, LADDER, rotation) {
    direction = calcDir(rotation);
    setPosition(glm::vec3(tile_pos) + glm::vec3(0.4 * direction.x,0,0.4 * direction.y));
}

void Ladder::placingRoutine(Renderer& renderer, glm::ivec3 pos) {
    setTilePos(pos);
    direction = calcDir(rotation);
    setPosition(glm::vec3(pos) + glm::vec3(0.4 * direction.x,0,0.4 * direction.y));
    if(!the_game->level->placeCheck(pos))
        renderer.shader->uniform("red", 1);
    draw(renderer);
    if(!the_game->level->placeCheck(pos))
        renderer.shader->uniform("red", 0);
}

void Ladder::resolveCollision(Player & player) {
    glm::vec3 overlap = detectCollision(player);
    if (overlap == glm::vec3(0))
        return;

    // always do collision
    player.resetGravity();

    AABB& aabb = player.bb;
    static float climb_velocity = 0.04f;
    float climb = 0.0f;

    if (overlap.x < overlap.y && overlap.x < overlap.z) {
        if ((int)round(rotation) % 180 == 90 || (int)round(rotation) % 180 == -90)
            climb = climb_velocity;

        if (bb.min.x < aabb.min.x) {
            player.move(glm::vec3(overlap.x, climb, 0));
            the_game->level->player->state = CLIMBING;

        }
        else {
            the_game->level->player->state = CLIMBING;
            player.move(glm::vec3(-overlap.x, climb, 0));
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
    }
    else {
        //cout << "z collision" << endl; 

        if ((int)round(rotation) % 180 == 0) {
            climb = climb_velocity;
            
        }
        if (bb.min.z < aabb.min.z) {
            player.move(glm::vec3(0, climb, overlap.z));
            the_game->level->player->state = CLIMBING;
        }
        else {
            player.move(glm::vec3(0, climb, -overlap.z));
            the_game->level->player->state = CLIMBING;
        }
    }
}

//-------------------
// Portal

Portal::Portal() : Block() { }

Portal::Portal(shared_ptr<vector<Drawelement>> draws, glm::ivec3 tile_pos, glm::vec3 size, float rotation) :
        Block(draws, tile_pos, size, PORTAL, rotation) {
    direction = calcDir(rotation);
    setPosition(glm::vec3(tile_pos) + glm::vec3(0.1 * direction.x,0,0.1 * direction.y));
}

void Portal::draw(Renderer& renderer) {
    renderer.draw(*draws, model, model_rot);
    for (int i = 0; i < 50; ++i) {
        glm::vec3 part = random_vec3() / 10.f;
        if(direction == glm::vec2(-1,0) || direction == glm::vec2(1,0))
            part.x = 0;
        else
            part.z = 0;
        particles_small->add(position + glm::vec3(0,0.4,0), part, glm::vec4(0.9,0,1,1), 800, 1, false);
    }
}

void Portal::link(shared_ptr<Block> new_link) {
    linked_block = new_link;
    cout << linked_block->direction << endl;
}

void Portal::resolveCollision(Player & player) {
    glm::vec3 overlap = detectCollision(player);
    if (overlap == glm::vec3(0))
        return;

    AABB& aabb = player.bb;
    if (overlap.x < overlap.y && overlap.x < overlap.z) {
        if (linked_block)
            player.setPosition(glm::vec3(linked_block->tile_pos) + glm::vec3(linked_block->direction.x, 0, linked_block->direction.y));
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
        cout << linked_block->tile_pos << endl;
        if (linked_block)
            player.setPosition(glm::vec3(linked_block->tile_pos) + glm::vec3(linked_block->direction.x, 0, linked_block->direction.y));
    }
}

void Portal::placingRoutine(Renderer &renderer, glm::ivec3 pos) {
    direction = calcDir(rotation);
    setTilePos(pos);
    setPosition(glm::vec3(position) + glm::vec3(0.1 * direction.x,0,0.1 * direction.y));
    if(!the_game->level->placeCheck(pos))
        renderer.shader->uniform("red", 1);
    draw(renderer);
    if(!the_game->level->placeCheck(pos))
        renderer.shader->uniform("red", 0);
}

