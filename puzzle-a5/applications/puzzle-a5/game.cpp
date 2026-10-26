#include "game.h"
#include "gui.h"
#include "input_handler.h"

shared_ptr<Game> the_game;

Game::Game() : state(GAME_MENU), void_height(-4), level_index(0) {
    Gui::loadGui();
    coin_counts = vector<unsigned int>(3, 0);
    max_coin_counts = vector<unsigned int>(3, 0);
}

void Game::loadLevel(const string& fullFilePath) {
    cout << "loading" << endl;
    level = make_shared<GameLevel>();
    level->load(fullFilePath);
    curLevel = fullFilePath;
    cout << "done loading" << endl;
}

void Game::toMenu() {
    state = GAME_MENU;
}

void Game::toLvL() {
    state = GAME_LVL;
}

void Game::startLevel(unsigned int index) {
    state = GAME_ACTIVE;
    level_index = index;
    max_coin_counts[level_index] = level->max_coins;
    customLevel = false;
}

void Game::Resume() {
    state = GAME_ACTIVE;
}

void Game::startCustomLevel() {
    state = GAME_ACTIVE;
    customLevel = true;
}

void Game::startBuildMode() {
    state = GAME_BUILD;
}

void Game::Win(){
    if (!customLevel) {
        unsigned int coin_count = level->coin_amount;
        if (coin_count > coin_counts[level_index])
            coin_counts[level_index] = coin_count;
    }
    state = GAME_WIN;
}

void Game::Lost(){
    state = GAME_LOST;
}

void Game::Pause(){
    state = GAME_PAUSE;
}

void Game::update() {
    //cout << "start game update" << endl;
    if (state == GAME_ACTIVE) {
        InputHandler::active_input();
        InputHandler::player_input();
        for(auto& enemy : the_game->level->enemies){
            enemy.second->gravity_timer.resume();
            enemy.second->movement_timer.resume();
        }
        the_game->level->player->gravity_timer.resume();
        level->update();
        if(the_game->level->player->get_position().y < void_height || the_game->level->player->hp <= 0)
            Lost();
        if(the_game->level->star_amount != 0){
            if (!customLevel){
                if(level_index == 0)
                    unlocked2 = true;
                else if(level_index == 1)
                    unlocked3 = true;
            }
            Win();
        }
    }
    else if (state == GAME_BUILD) {
        //InputHandler::build_input();
        CameraImpl::setCenter(level->level_center, level->level_center);
    } else if(state == GAME_PAUSE){
        for(auto& enemy : the_game->level->enemies){
            enemy.second->gravity_timer.pause();
            enemy.second->movement_timer.pause();
        }
        the_game->level->player->gravity_timer.pause();
        //evtl gravitytimer von movable block noch pausen
    }
    //cout << "end game update" << endl;
}

void Game::draw(Renderer &renderer) {
    //cout << "start game draw" << endl;
    if (state == GAME_MENU)
    {
        Gui::drawMenu();
    }
    if (state == GAME_LVL)
    {
        Gui::drawLvl();
    }
    else if (state == GAME_ACTIVE)
    {
        Gui::drawGUI();
        // draw level
        level->draw(renderer);
    }
    else if (state == GAME_BUILD)
    {
        Gui::drawBuildMode();
        level->raw_draw(renderer);
    } else if(state == GAME_WIN)
    {
        level->draw(renderer);
        Gui::drawWin();
    } else if(state == GAME_LOST){
        level->draw(renderer);
        Gui::drawGameOver();
    } else if(state == GAME_PAUSE){
        // pause timer
        level->draw(renderer);
        Gui::drawGUI();
        Gui::drawPause();
    }
    //cout << "end game draw" << endl;
}

void Game::drawTemporaries(Renderer &renderer) const {
    if (state == GAME_BUILD)
        InputHandler::drawTmpBlock(renderer);
}

void Game::restart(){
    loadLevel(curLevel);
    if(!customLevel)
        startLevel(level_index);
    else
        startCustomLevel();
}