#pragma once
#include <raylib.h>

class Entity {
public:
    virtual ~Entity() = default;
    virtual void Draw(int offset, int cellSize) const = 0;
};
