#include "Game.h"
#include "Utils.h"
#include "raylib.h"
#include "raymath.h"
#include <iostream>
#include <fstream>
#include <tuple>
#include <vector>
#include <stdexcept>
#include <algorithm>
#include <sstream>

// ---------------- Exception cho lỗi file I/O ----------------
class FileIOException : public std::runtime_error {
public:
    explicit FileIOException(const std::string &msg)
        : std::runtime_error(msg) {}
};

// ---------------- Constructor ----------------
Game::Game(const Config &cfg)
    : config(cfg), food(cfg.cellCount)
{
    std::cout << "Game Constructor: Starting...\n";

    int screenWidth = 2 * config.offset + config.cellSize * config.cellCount;
    int screenHeight = 2 * config.offset + config.cellSize * config.cellCount + 80;

    InitWindow(screenWidth, screenHeight, "Retro Snake");
    SetTargetFPS(60);
    SetExitKey(KEY_NULL);

    SetRandomSeed((unsigned int)GetTime());

    // Audio setup
    InitAudioDevice();
    if (!IsAudioDeviceReady())
        std::cerr << "⚠️ Warning: Audio device init failed!\n";

    eatSoundLoaded = wallSoundLoaded = false;

    std::string soundFolder = "Sounds/";
    std::string eatPath = soundFolder + "eat.mp3";
    std::string wallPath = soundFolder + "wall.mp3";

    if (FileExists(eatPath.c_str())) {
        eatSound = LoadSound(eatPath.c_str());
        eatSoundLoaded = true;
        std::cout << "✅ Loaded eat.mp3\n";
    } else {
        std::cerr << "❌ Missing: " << eatPath << "\n";
    }

    if (FileExists(wallPath.c_str())) {
        wallSound = LoadSound(wallPath.c_str());
        wallSoundLoaded = true;
        std::cout << "✅ Loaded wall.mp3\n";
    } else {
        std::cerr << "❌ Missing: " << wallPath << "\n";
    }

    LoadHighScores();

    running = false;
    allowMove = true;
    paused = false;
    score = 0;
    playerName.clear();

    std::cout << "Game Constructor: COMPLETED\n";
}

// ---------------- Destructor ----------------
Game::~Game()
{
    if (eatSoundLoaded) UnloadSound(eatSound);
    if (wallSoundLoaded) UnloadSound(wallSound);
    CloseAudioDevice();
    CloseWindow();
    std::cout << "Game Destructor: DONE\n";
}

// ---------------- Run ----------------
void Game::Run()
{
    static std::vector<std::tuple<int, int, Color>> stars;
    if (stars.empty()) {
        stars.reserve(80);
        for (int i = 0; i < 80; ++i) {
            Color c{
                (unsigned char)GetRandomValue(150, 255),
                (unsigned char)GetRandomValue(150, 255),
                255,
                255
            };
            stars.emplace_back(GetRandomValue(0, GetScreenWidth() - 1),
                               GetRandomValue(0, GetScreenHeight() - 1),
                               c);
        }
    }

    while (!WindowShouldClose()) {
        HandleInput();

        if (EventTriggered()) {
            allowMove = true;
            if (state == GameState::PLAYING)
                Update();
        }

        BeginDrawing();
        ClearBackground({10, 10, 20, 255});

        for (auto &s : stars)
            DrawPixel(std::get<0>(s), std::get<1>(s), std::get<2>(s));

        switch (state) {
            case GameState::MENU:
                DrawMenu();
                break;
            case GameState::ENTER_NAME:
                DrawEnterName();
                break;
            case GameState::PLAYING:
                DrawGame();
                break;
            case GameState::HIGHSCORES:
                DrawHighScores();
                break;
        }

        EndDrawing();
    }
}

// ---------------- Input ----------------
void Game::HandleInput()
{
    if (state != GameState::PLAYING) return;

    Vector2 dir = snake.Direction();

    if (IsKeyPressed(KEY_UP) && dir.y != 1 && allowMove) {
        snake.SetDirection({0, -1});
        running = true;
        allowMove = false;
    }
    if (IsKeyPressed(KEY_DOWN) && dir.y != -1 && allowMove) {
        snake.SetDirection({0, 1});
        running = true;
        allowMove = false;
    }
    if (IsKeyPressed(KEY_LEFT) && dir.x != 1 && allowMove) {
        snake.SetDirection({-1, 0});
        running = true;
        allowMove = false;
    }
    if (IsKeyPressed(KEY_RIGHT) && dir.x != -1 && allowMove) {
        snake.SetDirection({1, 0});
        running = true;
        allowMove = false;
    }

    if (IsKeyPressed(KEY_P) && running)
        paused = !paused;

    if (IsKeyPressed(KEY_ESCAPE))
        state = GameState::MENU;
}

// ---------------- Update ----------------
void Game::Update()
{
    if (!running || paused) return;

    Vector2 nextHead = Vector2Add(snake.Head(), snake.Direction());

    if (nextHead.x >= config.cellCount || nextHead.x < 0 ||
        nextHead.y >= config.cellCount || nextHead.y < 0) {
        GameOver();
        return;
    }

    bool grow = Vector2Equals(nextHead, food.GetPosition());
    snake.Update(grow);

    if (grow) {
        food.Respawn(snake.Body());
        score++;
        if (eatSoundLoaded) PlaySound(eatSound);
    }

    Vector2 h = snake.Head();
    std::deque<Vector2> rest = snake.Body();
    rest.pop_front();
    if (ElementInDeque(h, rest))
        GameOver();
}

// ---------------- Draw Game ----------------
void Game::DrawGame() const
{
    DrawRectangleLinesEx(
        {(float)config.offset - 5, (float)config.offset - 5,
         (float)config.cellSize * config.cellCount + 10,
         (float)config.cellSize * config.cellCount + 10},
        5, {0, 120, 255, 255});

    food.Draw(config.offset, config.cellSize);
    snake.Draw(config.offset, config.cellSize);

    DrawText(TextFormat("Score: %i", score),
             config.offset,
             config.offset + config.cellSize * config.cellCount + 10,
             30, WHITE);

    if (paused)
        DrawText("PAUSED", GetScreenWidth()/2 - MeasureText("PAUSED", 60)/2,
                 GetScreenHeight()/2 - 30, 60, YELLOW);
}

// ---------------- Draw Menu ----------------
void Game::DrawMenu()
{
    const char* title = "SNAKE";
    int titleWidth = MeasureText(title, 100);
    DrawText(title, GetScreenWidth()/2 - titleWidth/2, 100, 100, {0, 120, 255, 255});

    Rectangle playBtn  = { (float)(GetScreenWidth()/2 - 100), 300.0f, 200.0f, 60.0f };
    Rectangle scoreBtn = { (float)(GetScreenWidth()/2 - 100), 380.0f, 200.0f, 60.0f };
    Rectangle quitBtn  = { (float)(GetScreenWidth()/2 - 100), 460.0f, 200.0f, 60.0f };

    Vector2 mouse = GetMousePosition();

    auto DrawButton = [&](Rectangle rect, const char* text) {
        bool hover = CheckCollisionPointRec(mouse, rect);
        DrawRectangleRounded(rect, 0.3f, 6, hover ? Color{30,144,255,255} : Color{0,80,200,255});
        int w = MeasureText(text, 30);
        DrawText(text, rect.x + rect.width/2 - w/2, rect.y + 15, 30, WHITE);
    };

    DrawButton(playBtn, "Play");
    DrawButton(scoreBtn, "High Scores");
    DrawButton(quitBtn, "Quit");

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        if (CheckCollisionPointRec(mouse, playBtn)) {
            state = GameState::ENTER_NAME;
            playerName.clear();
        }
        else if (CheckCollisionPointRec(mouse, scoreBtn)) {
            state = GameState::HIGHSCORES;
        }
        else if (CheckCollisionPointRec(mouse, quitBtn)) {
            CloseWindow();
        }
    }
}

// ---------------- Draw Enter Name ----------------
void Game::DrawEnterName()
{
    const char* title = "ENTER YOUR NAME";
    DrawText(title, GetScreenWidth()/2 - MeasureText(title, 50)/2, 150, 50, {0, 120, 255, 255});

    int boxWidth = 400;
    int boxHeight = 60;
    Rectangle inputBox = { (float)(GetScreenWidth()/2 - boxWidth/2), 250.0f, (float)boxWidth, (float)boxHeight };
    DrawRectangleRounded(inputBox, 0.2f, 8, DARKBLUE);
    DrawRectangleLinesEx(inputBox, 2, WHITE);

    std::string display = playerName.empty() ? std::string("_") : playerName;
    DrawText(display.c_str(), (int)inputBox.x + 20, (int)inputBox.y + 15, 30, WHITE);

    DrawText("Press ENTER to start (name saved). BACKSPACE to delete.", GetScreenWidth()/2 - 260, 350, 18, GRAY);

    int key = GetCharPressed();
    while (key > 0) {
        if (key >= 32 && key <= 126 && playerName.length() < MAX_NAME_LENGTH)
            playerName += (char)key;
        key = GetCharPressed();
    }

    if (IsKeyPressed(KEY_BACKSPACE) && !playerName.empty())
        playerName.pop_back();

    if (IsKeyPressed(KEY_ENTER) && !playerName.empty()) {
        running = false;
        paused = false;
        score = 0;
        snake.Reset();
        food.Respawn(snake.Body());
        state = GameState::PLAYING;
    }
}

// ---------------- Draw High Scores ----------------
void Game::DrawHighScores()
{
    DrawText("HIGH SCORES", GetScreenWidth()/2 - MeasureText("HIGH SCORES", 50)/2, 80, 50, {0,120,255,255});

    int y = 160;
    int rank = 1;
    for (auto &entry : highScores) {
        std::string line = std::to_string(rank++) + ". " + entry.first + " - " + std::to_string(entry.second);
        DrawText(line.c_str(), GetScreenWidth()/2 - MeasureText(line.c_str(), 30)/2, y, 30, WHITE);
        y += 40;
        if (rank > 10) break;
    }

    if (highScores.empty()) {
        DrawText("No scores yet!", GetScreenWidth()/2 - MeasureText("No scores yet!", 30)/2, y, 30, GRAY);
        y += 40;
    }

    const char* hint = "Press ESC to return";
    int hintWidth = MeasureText(hint, 25);
    DrawText(hint, GetScreenWidth()/2 - hintWidth/2, y + 30, 25, GRAY);

    if (IsKeyPressed(KEY_ESCAPE)) {
        state = GameState::MENU;
    }
}

// ---------------- Event Trigger ----------------
bool Game::EventTriggered()
{
    return ::EventTriggered(config.tickInterval, lastUpdateTime);
}

// ---------------- Game Over (ĐÃ SỬA) ----------------
void Game::GameOver()
{
    if (wallSoundLoaded) PlaySound(wallSound);

    // Nếu đã nhập tên thì lưu điểm
    if (!playerName.empty()) {
        bool found = false;
        for (auto &e : highScores) {
            if (e.first == playerName) {
                if (score > e.second) e.second = score;
                found = true;
                break;
            }
        }
        if (!found) highScores.push_back({playerName, score});
        SaveHighScores();
    }

    // Reset game
    snake.Reset();
    score = 0;
    running = false;
    paused = false;

    // Quay lại menu chính
    state = GameState::MENU;
}

// ---------------- High Score I/O ----------------
void Game::LoadHighScores()
{
    highScores.clear();
    std::ifstream in(config.scoreFile);
    if (!in) {
        std::ofstream create(config.scoreFile);
        return;
    }
    std::string name;
    int sc;
    while (in >> name >> sc)
        highScores.push_back({name, sc});
    in.close();

    std::sort(highScores.begin(), highScores.end(), [](auto &a, auto &b){ return a.second > b.second; });
    if (highScores.size() > 10) highScores.resize(10);
}

void Game::SaveHighScores()
{
    std::sort(highScores.begin(), highScores.end(), [](auto &a, auto &b){ return a.second > b.second; });
    if (highScores.size() > 10) highScores.resize(10);
    std::ofstream out(config.scoreFile);
    if (!out) throw FileIOException("Unable to write scores file");
    for (auto &p : highScores) out << p.first << " " << p.second << "\n";
    out.close();
}
