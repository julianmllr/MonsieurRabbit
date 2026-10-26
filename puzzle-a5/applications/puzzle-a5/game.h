#pragma once

#include <cppgl.h>
#include "game_level.h"

using namespace std;

// Represents the current state of the game
enum GameState {
    GAME_MENU,
    GAME_LVL,
    GAME_ACTIVE,
    GAME_BUILD,
    GAME_LOST,
    GAME_PAUSE,
    GAME_WIN
};

class Game
{
public:
    // game state
    GameState state;
    // constructor/destructor
    Game();
    // getters
    void loadLevel(const string& fullFilePath);
    void toMenu();
    void toLvL();
    void startLevel(unsigned int index);
    void Resume();
    void startCustomLevel();
    void startBuildMode();
    void Win();
    void Lost();
    void Pause();
    void leftMouseClick();
    // game loop
    void update();
    void draw(Renderer &renderer);
    void drawTemporaries(Renderer &renderer) const;
    void restart();

    shared_ptr<GameLevel> level;
    string curLevel;
    int void_height;
    unsigned int level_index;
    vector<unsigned int> max_coin_counts;
    vector<unsigned int> coin_counts;
    bool customLevel;

    bool unlocked2 = false;
    bool unlocked3 = false;
};

extern shared_ptr<Game> the_game;