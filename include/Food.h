#pragma once
#include <raylib.h>
#include <deque>

class Food {
private:
    int cellCount{};
    Vector2 position{};

public:
    Food() = default;
    Food(int cellCount);
    ~Food() = default;

    void Draw(int offset, int cellSize) const;
    Vector2 GetPosition() const;
    void Respawn(const std::deque<Vector2> &snakeBody);

private:
    Vector2 GenerateRandomCell() const;
    Vector2 GenerateRandomPos(const std::deque<Vector2> &snakeBody) const;
};
