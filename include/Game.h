#pragma once
#include "Config.h"
#include "Snake.h"
#include "Food.h"
#include <vector>
#include <deque>
#include <string>
#include "raylib.h"

enum class GameState {
    MENU,
    PLAYING,
    HIGHSCORES,
    ENTER_NAME
};

class Game {
public:
    Game(const Config &cfg);
    ~Game();

    void Run();

private:
    Config config;
    Snake snake;
    Food food;

    int score = 0;
    bool running = false;
    bool allowMove = true;
    bool paused = false;
    double lastUpdateTime = 0.0;

    std::vector<std::pair<std::string, int>> highScores;

    Sound eatSound;
    Sound wallSound;
    bool eatSoundLoaded;
    bool wallSoundLoaded;

    GameState state = GameState::MENU;

    std::string playerName;
    static constexpr size_t MAX_NAME_LENGTH = 12;

    void HandleInput();
    void Update();
    void DrawGame() const;
    void DrawMenu();
    void DrawHighScores();
    void DrawEnterName();
    bool EventTriggered();
    void CheckCollisions();
    void GameOver();
    void LoadHighScores();
    void SaveHighScores();
};
