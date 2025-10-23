#include "Snake.h"
#include <raylib.h>
#include "raymath.h"

Snake::Snake() {
    body = { Vector2{6,9}, Vector2{5,9}, Vector2{4,9} };
    direction = {1,0};
}

void Snake::Draw(int offset, int cellSize) const {
    Color darkGreen = {43,51,24,255};
    for (auto &segment : body) {
        Rectangle rect = { offset + segment.x*cellSize, offset + segment.y*cellSize, (float)cellSize, (float)cellSize };
        DrawRectangleRounded(rect, 0.5f, 6, {0, 80, 255, 255}); 
        DrawRectangleRoundedLines(rect, 0.5f, 6, {100, 200, 255, 180});

    }
}

void Snake::Update(bool grow) {
    body.push_front(Vector2Add(body.front(), direction));
    if (!grow) body.pop_back();
}

void Snake::Reset() {
    body = { Vector2{6,9}, Vector2{5,9}, Vector2{4,9} };
    direction = {1,0};
}

const std::deque<Vector2>& Snake::Body() const { return body; }
void Snake::SetDirection(const Vector2 &dir) { direction = dir; }
Vector2 Snake::Head() const { return body.front(); }
