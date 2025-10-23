#include "Food.h"
#include <raylib.h>
#include <deque>
#include "Utils.h"

Food::Food(int cellCount) : cellCount(cellCount), position({0,0}) {}

void Food::Draw(int offset, int cellSize) const {
    float x = offset + position.x * cellSize;
    float y = offset + position.y * cellSize;
    Rectangle rect = {x, y, (float)cellSize, (float)cellSize};

    DrawRectangleRounded(rect, 0.4f, 8, {36, 48, 94, 255});
    DrawRectangleRoundedLines(rect, 0.4f, 8, {120, 140, 220, 200});
}

Vector2 Food::GetPosition() const { return position; }

Vector2 Food::GenerateRandomCell() const {
    float x = GetRandomValue(0, cellCount - 1);
    float y = GetRandomValue(0, cellCount - 1);
    return Vector2{x, y};
}

Vector2 Food::GenerateRandomPos(const std::deque<Vector2> &snakeBody) const {
    Vector2 pos = GenerateRandomCell();
    int guard = 0;
    while (ElementInDeque(pos, snakeBody) && guard < 1000) {
        pos = GenerateRandomCell();
        guard++;
    }
    return pos;
}

void Food::Respawn(const std::deque<Vector2> &snakeBody) {
    position = GenerateRandomPos(snakeBody);
}
