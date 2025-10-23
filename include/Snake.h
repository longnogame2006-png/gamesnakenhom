#pragma once
#include "Entity.h"
#include <deque>
#include <raylib.h>

class Snake : public Entity {
public:
    Snake();
    void Draw(int offset, int cellSize) const override;
    void Update(bool grow = false);
    void Reset();
    const std::deque<Vector2>& Body() const;
    void SetDirection(const Vector2 &dir);
    Vector2 Head() const;
    Vector2 Direction() const { return direction; }

private:
    std::deque<Vector2> body;
    Vector2 direction;
};
