#include "game_level.h"
#include "rendering/buffer.h"
#include "game.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include "input_handler.h"

using json = nlohmann::json;

vector<shared_ptr<vector<Drawelement>>> GameLevel::object_draws;
vector<shared_ptr<vector<Drawelement>>> GameLevel::powerUp_draws;
vector<shared_ptr<vector<Drawelement>>> GameLevel::block_draws;
vector<shared_ptr<vector<Drawelement>>> GameLevel::enemy_draws;

vector<glm::ivec3> from_json(const vector<int>& json_data) {
    vector<glm::ivec3> ret;
    for (size_t i = 0; i < json_data.size(); i += 3) {
        ret.emplace_back(json_data[i], json_data[i + 1], json_data[i + 2]);
    }
    return ret;
}

void from_json(const json& j, map<glm::ivec3, vector<glm::ivec3>, CompareVec3>& data) {
    for (const auto& pair : j.items()) {
        glm::ivec3 key;
        sscanf(pair.key().c_str(), "%d_%d_%d", &key.x, &key.y, &key.z);
        data[key] = from_json(pair.value());
    }
}

vector<int> to_json(const vector<glm::ivec3>& vecs) {
    vector<int> ret;
    for (auto ivec : vecs) {
        ret.push_back(ivec.x);
        ret.push_back(ivec.y);
        ret.push_back(ivec.z);
    }
    return ret;
}

void to_json(json& j, map<glm::ivec3, vector<glm::ivec3>, CompareVec3>& data) {
    for (const auto& pair : data) {
        j[to_string(pair.first.x) + "_" + to_string(pair.first.y) + "_" + to_string(pair.first.z)] = to_json(pair.second);
    }
}

void load_type(int index, string path, vector<shared_ptr<vector<Drawelement>>>& draws) {
    static auto dummy_shader = Shader("dummy", "shader/pos+norm+tc.vs", "shader/pos+norm+tc.fs");
    if (!draws[index])
        draws[index] = make_shared<vector<Drawelement>>();
    auto meshes = load_meshes_gpu(path, false);
    for (auto& m : meshes)
    {
        Drawelement d = Drawelement(m->name, dummy_shader, m);
        draws[index]->push_back(d);
    }
}

shared_ptr<Block> GameLevel::switchBlockType(BlockType type, glm::ivec3 pos, float rotation)
{
    glm::vec3 size = block_sizes[type];
    switch (type)
    {
        case PLATFORM:
        {
            vector<glm::ivec3> path;
            auto iterator = the_game->level->paths.find(pos);
            if (iterator != the_game->level->paths.end())
                path = iterator->second;
            return make_shared<Platform>(block_draws[type], pos, size, path, rotation);
        }
        case CLICK_PLATFORM:
        {
            vector<glm::ivec3> path;
            auto iterator = the_game->level->paths.find(pos);
            if (iterator != the_game->level->paths.end())
                path = iterator->second;
            return make_shared<ClickPlatform>(block_draws[type], pos, size, path, rotation);
        }
        case PRESSURE_PLATE:
            return make_shared<PressurePlate>(block_draws[type], pos, size, rotation);
        case DOOR:
            return make_shared<Door>(block_draws[type], pos, size, rotation);
        case OCCUPIED:
            return make_shared<Occupied>(block_draws[type], pos);
        case LADDER:
            return make_shared<Ladder>(block_draws[type], pos, size, rotation);
        case SPIKES:
            return make_shared<Spikes>(block_draws[type], pos, size, rotation);
        case TIMED_SPIKES:
            return make_shared<TimedSpikes>(block_draws[type], pos, size, rotation);
        case PORTAL:
            return make_shared<Portal>(block_draws[type], pos, size, rotation);
        case MOVABLE_BLOCK:
            return make_shared<MovableBlock>(block_draws[type], pos, size, rotation);
        case DESTROYABLE_BLOCK:
            return make_shared<DestroyableBlock>(block_draws[type], pos, size, type, rotation);
        case ROTATING_PLATFORM:
            return make_shared<RotatingBlock>(block_draws[type], pos, size, 2, rotation);
        default:
            return make_shared<Block>(block_draws[type], pos, size, type, rotation);
    }
}

shared_ptr<PowerUp> GameLevel::switchPowerUpType(PowerUpType type, glm::ivec3 pos)
{
    float radius = powerUp_radi[type];
    switch (type)
    {
        case COIN:
            return make_shared<Coin>(powerUp_draws[COIN], pos, radius);
        case HEART:
            return make_shared<Heart>(powerUp_draws[HEART], pos, radius);
        case KEY:
            return make_shared<Key>(powerUp_draws[KEY], pos, radius);
        case SHOT:
            return make_shared<Shot>(powerUp_draws[SHOT], pos, radius);
        case STAR:
            return make_shared<Star>(powerUp_draws[STAR], pos, radius);
    }
}

shared_ptr<Enemy> GameLevel::switchEnemyType(EnemyType type, glm::ivec3 pos) {
    switch (type) {
        case GHOST:
            return make_shared<Ghost>(enemy_draws[GHOST], pos);
        case ZOMBIE:
            return make_shared<Zombie>(enemy_draws[ZOMBIE], pos);
        case SPIKE: {
            vector<glm::ivec3> path;
            auto iterator = the_game->level->paths.find(pos);
            if (iterator != the_game->level->paths.end())
                path = iterator->second;
            return make_shared<Spike>(enemy_draws[SPIKE], pos, path);
        }
        case OCCUPIED_ENEMY:
            return make_shared<OccupiedEnemy>(enemy_draws[OCCUPIED_ENEMY], pos);
    }
}


GameLevel::GameLevel() : width(0), height(0), depth(0), level_center(0), spawn(0), projectile(),
    coin_amount(0), max_coins(0), key_amount(0), shot_amount(5), star_amount(0)
{

    if (block_draws.empty())
    {
        block_draws = vector<shared_ptr<vector<Drawelement>>>(SIZE_TILE);
        powerUp_draws = vector<shared_ptr<vector<Drawelement>>>(SIZE_POWERUP);
        object_draws = vector<shared_ptr<vector<Drawelement>>>(SIZE_OBJECT);
        enemy_draws = vector<shared_ptr<vector<Drawelement>>>(SIZE_ENEMY);

        // blocks
        load_type(GROUND, "render-data/models/dirt/blockDirt.obj", block_draws);
        load_type(GRASS_HALF, "render-data/models/grass/blockHalf.obj", block_draws);
        load_type(SNOW_HALF, "render-data/models/snow/blockSnowHalf.obj", block_draws);
        load_type(GRASS_BLOCK, "render-data/models/grass/block.obj", block_draws);
        load_type(SNOW_BLOCK, "render-data/models/snow/blockSnow.obj", block_draws);
        load_type(TREE_GREEN, "render-data/models/nature/treePine.obj", block_draws);
        load_type(LADDER, "render-data/models/wood/ladder.obj", block_draws);
        load_type(BARREL, "render-data/models/wood/barrel.obj", block_draws);
        load_type(ROTATING_PLATFORM, "render-data/models/utilities/blockMoving.obj", block_draws);
        load_type(PLATFORM, "render-data/models/utilities/blockMoving.obj", block_draws);
        load_type(CLICK_PLATFORM, "render-data/models/utilities/blockMovingBlue.obj", block_draws);
        load_type(PRESSURE_PLATE, "render-data/models/utilities/buttonSquare.obj", block_draws);
        load_type(DOOR, "render-data/models/stone/doorLargeClosed.obj", block_draws);
        load_type(SPIKES, "render-data/models/traps/spikes.obj", block_draws);
        load_type(TIMED_SPIKES, "render-data/models/traps/spikesHidden.obj", block_draws);
        load_type(OCCUPIED, "render-data/models/occupied.obj", block_draws);
        load_type(PORTAL, "render-data/models/stone/doorLargeOpen.obj", block_draws);
        load_type(MOVABLE_BLOCK, "render-data/models/stone/crateStrong.obj", block_draws);
        load_type(DESTROYABLE_BLOCK, "render-data/models/wood/crate.obj", block_draws);
        load_type(TREE_SNOW, "render-data/models/nature/treePineSnow.obj", block_draws);

        load_type(COIN, "render-data/models/utilities/coinGold.obj", powerUp_draws);
        load_type(HEART, "render-data/models/utilities/heart.obj", powerUp_draws);
        load_type(KEY, "render-data/models/utilities/key.obj", powerUp_draws);
        load_type(SHOT, "render-data/models/traps/bomb.obj", powerUp_draws);
        load_type(STAR, "render-data/models/utilities/jewel.obj", powerUp_draws);

        load_type(PROJECTILE, "render-data/models/traps/bomb.obj", object_draws);
        load_type(PLAYER, "render-data/rabbit/default/default.obj", object_draws);

        load_type(GHOST, "render-data/graveyard_models/character-ghost.obj", enemy_draws);
        load_type(ZOMBIE, "render-data/graveyard_models/character-zombie.obj", enemy_draws);
        load_type(SPIKE, "render-data/models/traps/spikeBlock.obj", enemy_draws);
        load_type(OCCUPIED_ENEMY, "render-data/models/occupied.obj", enemy_draws);

        cout << "level draws geladen" << endl;
    }
}

void GameLevel::loadFromFile(const string &fullFilePath)
{
    cout << "loading json" << endl;
    std::ifstream file(fullFilePath);
    if (!file.is_open())
    {
        std::cerr << "Error: File " << fullFilePath << " not found." << std::endl;
        return;
    }

    json j;
    file >> j;

    spawn = glm::vec3(j["spawn"][0], j["spawn"][1], j["spawn"][2]);
    blocks_vec = j["blocks"].get<vector<vector<vector<BlockType>>>>();
    powerUps_vec = j["powerups"].get<vector<vector<vector<PowerUpType>>>>();
    enemies_vec = j["enemies"].get<vector<vector<vector<EnemyType>>>>();
    json links_json_array = j["links"];
    max_coins = j["max_coins"];
    for (const auto& link : links_json_array) {
        glm::vec3 first(link[0], link[1], link[2]);
        glm::vec3 second(link[3], link[4], link[5]);
        links_vec.push_back(make_pair(first, second));
    }
    rotations_vec = j["rotation"].get<vector<vector<vector<float>>>>();
    from_json(j["paths"], paths);

    file.close();

    width = blocks_vec.size();
    height = blocks_vec[0].size();
    depth = blocks_vec[0][0].size();
    level_center = 0.5f * glm::vec3(width, height,depth) - 0.5f;

    cout << "done loading json" << endl;
    initBlocks();
}

void GameLevel::initBlocks()
{
    cout << "start blocks init" << endl;
    // erstelle Bl�cke
    for (unsigned int x = 0; x < width; ++x)
    {
        for (unsigned int y = 0; y < height; ++y)
        {
            for (unsigned int z = 0; z < depth; ++z)
            {
                // check block type from level data (3D level array)
                BlockType block_type = blocks_vec[x][y][z];
                PowerUpType powerUp_type = powerUps_vec[x][y][z];
                EnemyType enemy_type = enemies_vec[x][y][z];
                float rotation = rotations_vec[x][y][z];
                glm::ivec3 pos(x, y, z);

                if (block_type != NO_BLOCK)
                    blocks.emplace(pos, switchBlockType(block_type, pos, rotation));

                if (powerUp_type != NO_POWERUP)
                    powerUps.emplace(pos, switchPowerUpType(powerUp_type, pos));

                if (enemy_type != NO_ENEMY)
                    enemies.emplace(pos, switchEnemyType(enemy_type, pos));
            }
        }
    }
    for (auto &pair : links_vec) {
        auto link_block = blocks.find(pair.first);
        auto to_link = blocks.find(pair.second);
        if (link_block != blocks.end() && to_link != blocks.end()) {
            cout << "linking" << endl;
            link_block->second->link(to_link->second);
            to_link->second->link(link_block->second);
        }
    }
    cout << "init blocks done" << endl;
}

void GameLevel::load(const string &fullFilePath)
{
    loadFromFile(fullFilePath);

    glm::vec3 level_center = 0.5f * glm::vec3(width, height, depth) - 0.5f;
    make_camera_current(Camera::find("default"));
    CameraImpl::resetCamera(level_center);
    player = make_shared<Player>(object_draws[PLAYER], spawn, glm::vec3(0.3));
}



// in-game functions
bool GameLevel::placeCheck(glm::ivec3 pos) {
    if (blocks.count(pos) == 1 || powerUps.count(pos) == 1 || enemies.count(pos) == 1) {
        //cout << "check block was false, pos already exists: " << pos << endl;
        return false;
    }
    return true;
    //if (pos.x >= width || pos.y >= height || pos.z >= depth) {
    //    cout << "check block was true" << endl;
    //    return true;
    //}
    //cout << "check block if occupied" << endl;
    //return (blocks_vec[pos.x][pos.y][pos.z] == NO_BLOCK);
}

void GameLevel::resize(glm::ivec3 pos) {
    // Resize the vector if necessary
    if (pos.x >= width) {
        cout << "more width" << endl;
        width = pos.x + 1;
        blocks_vec.resize(width, vector<vector<BlockType>>(height, vector<BlockType>(depth, NO_BLOCK)));
        powerUps_vec.resize(width, vector<vector<PowerUpType>>(height, vector<PowerUpType>(depth, NO_POWERUP)));
        enemies_vec.resize(width, vector<vector<EnemyType>>(height, vector<EnemyType>(depth, NO_ENEMY)));
        rotations_vec.resize(width, vector<vector<float>>(height, vector<float>(depth, 0.0f)));
    }
    if (pos.y >= height) {
        cout << "more height" << endl;
        height = pos.y + 1;
        for (int i = 0; i < blocks_vec.size(); i++) {
            blocks_vec[i].resize(height, vector<BlockType>(depth, NO_BLOCK));
            powerUps_vec[i].resize(height, vector<PowerUpType>(depth, NO_POWERUP));
            enemies_vec[i].resize(height, vector<EnemyType>(depth, NO_ENEMY));
            rotations_vec[i].resize(height, vector<float>(depth, 0.0f));
        }
    }
    if (pos.z >= depth) {
        cout << "more depth" << endl;
        depth = pos.z + 1;
        for (int i = 0; i < blocks_vec.size(); i++)
            for (int j = 0; j < blocks_vec[0].size(); j++) {
                blocks_vec[i][j].resize(depth, NO_BLOCK);
                powerUps_vec[i][j].resize(depth, NO_POWERUP);
                enemies_vec[i][j].resize(depth, NO_ENEMY);
                rotations_vec[i][j].resize(depth, 0.0f);
            }
    }

    // calculate new level center
    level_center = 0.5f * glm::vec3(width, height, depth) - 0.5f;
}

void GameLevel::addBlock(shared_ptr<Block> &block, glm::ivec3 pos)
{
    if (!placeCheck(pos)) // just for safety
        return;

    blocks.emplace(pos, block);

    resize(pos);

    blocks_vec[pos.x][pos.y][pos.z] = block->type;
    rotations_vec[pos.x][pos.y][pos.z] = block->get_rotation();
}

void GameLevel::addPowerUp(shared_ptr<PowerUp>& powerUp, glm::ivec3 pos)
{
    if (!placeCheck(pos)) // just for safety
        return;

    powerUps.emplace(pos, powerUp);

    resize(pos);

    powerUps_vec[pos.x][pos.y][pos.z] = powerUp->type;
}

void GameLevel::addEnemy(shared_ptr<Enemy>& enemy, glm::ivec3 pos)
{
    if (!placeCheck(pos)) // just for safety
        return;

    enemies.emplace(pos, enemy);

    resize(pos);

    enemies_vec[pos.x][pos.y][pos.z] = enemy->type;
    //rotations_vec[pos.x][pos.y][pos.z] = block->get_rotation();
}

void GameLevel::addPositionToPath(glm::ivec3 index, glm::ivec3 pos) {
    paths[index].push_back(pos);
}

void GameLevel::remove(glm::ivec3 pos)
{
    if (blocks.erase(pos) == 1) {
        if (pos.x < width && pos.y < height && pos.z < depth) // just for safety
            blocks_vec[pos.x][pos.y][pos.z] = NO_BLOCK;
        // remove links
        for (auto it = links_vec.begin(); it != links_vec.end();) {
            if (it->first == pos || it->second == pos)
                it = links_vec.erase(it);
            else
                ++it;
        }
        paths.erase(pos);
    }
    if (powerUps.erase(pos) == 1)
        if (pos.x < width && pos.y < height && pos.z < depth) // just for safety
            powerUps_vec[pos.x][pos.y][pos.z] = NO_POWERUP;
    if (enemies.erase(pos) == 1) {
        if (pos.x < width && pos.y < height && pos.z < depth) // just for safety
            enemies_vec[pos.x][pos.y][pos.z] = NO_ENEMY;
        paths.erase(pos);
    }
}

void GameLevel::block_clicked(glm::ivec3 pos) {
    auto block = blocks.find(pos);
    if (block != blocks.end())
        block->second->onClick();
}

void GameLevel::create_link(glm::ivec3 link_pos, glm::ivec3 to_link_pos) {
    links_vec.emplace_back(link_pos, to_link_pos);
}

//void GameLevel::setCameraPos() {
//
//}

void GameLevel::update()
{
    player->set_state();    
    CameraImpl::setCenter(player->get_position(), level_center);
    cout << "start update" << endl;
    projectile.update();
    cout << "block update" << endl;
    for (auto block = blocks.begin(); block != blocks.end();) {
        if (!block->second->update()) {
            block = blocks.erase(block);
            continue;
        }
        // collision
        block->second->resolveCollision(*player);
        for (auto& block_sec : blocks)
            if(block->first != block_sec.first)
                block->second->resolveCollision(*block_sec.second);
        if (projectile.shot)
            block->second->resolveCollision(projectile);
        for (auto& enemy : enemies)
            block->second->resolveCollision(*enemy.second);
        ++block;
    }
    //cout << "end block update" << endl;
    player->update();

    // weird loop in order to be able to remove enemies
    for (auto enemy = enemies.begin(); enemy != enemies.end();) {
        if (!enemy->second->update()) {
            enemy = enemies.erase(enemy);
            continue;
        }
        for (auto& enemy_sec : enemies)
            if(enemy->first != enemy_sec.first)
                enemy->second->resolveCollision(*enemy_sec.second);

        enemy->second->resolveCollision(*player);
        if (projectile.shot)
            enemy->second->resolveCollision(projectile);
        ++enemy;
    }
    cout << "powerUp update" << endl;
    for (auto powerUp = powerUps.begin(); powerUp != powerUps.end();) {
        if (!powerUp->second->update()) {
            powerUp = powerUps.erase(powerUp);
            continue;
        }
        powerUp->second->resolveCollision(*player);
        ++powerUp;
    }
    cout << "end update" << endl;
}

void GameLevel::draw(Renderer &renderer)
{
    raw_draw(renderer);
    if (projectile.shot)
        projectile.draw(renderer);
}

void GameLevel::raw_draw(Renderer& renderer) {
    for (auto& block : blocks)
        block.second->draw(renderer);
    for (auto& enemy : enemies)
        enemy.second->draw(renderer);
    for (auto& powerUp : powerUps)
        powerUp.second->draw(renderer);
    player->add_keyframes(renderer);
}

void GameLevel::exportToFile(const string &fullFilePath)
{

    json j;
    j["spawn"] = {spawn.x, spawn.y, spawn.z};
    j["blocks"] = blocks_vec;
    j["powerups"] = powerUps_vec;
    j["enemies"] = enemies_vec;
    json links_json_array;
    for (const auto& link : links_vec) {
        links_json_array.push_back({ link.first.x, link.first.y, link.first.z,
                                     link.second.x, link.second.y, link.second.z });
    }
    j["links"] = links_json_array;
    j["rotation"] = rotations_vec;
    unsigned int coin_counter = 0;
    for (int x = 0; x < powerUps_vec.size(); x++) {
        for (int y = 0; y < powerUps_vec[0].size(); y++) {
            for (int z = 0; z < powerUps_vec[0][0].size(); z++) {
                if (powerUps_vec[x][y][z] == COIN)
                    coin_counter++;
            }
        }
    }
    j["max_coins"] = coin_counter;

    json path_json_map;
    to_json(path_json_map, paths);
    j["paths"] = path_json_map;

    ofstream file(fullFilePath);
    if (!file)
    {
        cerr << "Error: Cannot open file " << fullFilePath << " for writing." << endl;
        return;
    }

    file << j.dump(4);

    file.close();
}

float linear_depth(float depth, float near, float far) {
    return (2.0f * near) / (far + near - depth * (far - near));
}


void GameLevel::createProjectile()
{
    if (!projectile.shot && shot_amount > 0)
    {
        /*
        //transform to clip space
        glm::vec2 mouse_pos = Context::mouse_pos();
        glm::vec2 res = Context::resolution();
        glm::vec2 mouse_tc = glm::vec2(mouse_pos.x / res.x, (res.y - mouse_pos.y) / res.y);
        glm::vec2 mouse_clip = (mouse_tc * 2.0f) - 1.0f;


        //read from the read_depth buffer
        using namespace rendering::buffer;
        geom->bind();
        float read_depth[1];
        glReadPixels(mouse_pos.x, res.y - mouse_pos.y, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, read_depth);
        geom->unbind();

        //transform to world space
        glm::vec4 mouse_homo = glm::vec4(mouse_clip.x, mouse_clip.y, linear_depth(read_depth[0], current_camera()->near, current_camera()->far), 1.0f);
        glm::vec4 world_homo = glm::inverse(current_camera()->proj) * glm::inverse(current_camera()->view) * mouse_homo;
        glm::vec3 world_pos = world_homo / world_homo.w;
        glm::vec3 dir = glm::normalize(world_pos - current_camera()->pos);

        cout << world_pos << endl;
         */
        glm::vec3 world_pos = InputHandler::hover_pos;
        if(world_pos == glm::vec3(0))
            return;
        glm::vec3 dir = glm::normalize(world_pos - current_camera()->pos);
        static float projectile_radius = 0.25f;
        cout << current_camera()->pos << endl;
        cout << world_pos << endl;
        cout << dir << endl;
        projectile = Projectile(object_draws[PROJECTILE], current_camera()->pos, projectile_radius, dir);
        projectile.duration.begin();
        shot_amount--;
    }
}