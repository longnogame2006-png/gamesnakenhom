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

    // ✅ High scores (vector để có thể sort & push_back)
    std::vector<std::pair<std::string, int>> highScores;

    // Audio
    Sound eatSound;
    Sound wallSound;
    bool eatSoundLoaded;
    bool wallSoundLoaded;

    // Trạng thái game
    GameState state = GameState::MENU;

    // Nhập tên người chơi
    std::string playerName;
    static constexpr size_t MAX_NAME_LENGTH = 12;

    // Các hàm xử lý
    void HandleInput();
    void Update();
    void DrawGame() const;       // Vẽ khi đang chơi
    void DrawMenu();             // Menu chính
    void DrawHighScores();       // Bảng xếp hạng
    void DrawEnterName();        // Màn hình nhập tên
    bool EventTriggered();
    void CheckCollisions();
    void GameOver();
    void LoadHighScores();
    void SaveHighScores();
};
