#include "Utils.h"
#include <raylib.h>
#include "raymath.h"


bool ElementInDeque(const Vector2 &element, const std::deque<Vector2> &dq) {
    for (auto &v : dq) if (Vector2Equals(v, element)) return true;
    return false;
}

bool EventTriggered(double interval, double &lastUpdateTime) {
    double current = GetTime();
    if (current - lastUpdateTime >= interval) { lastUpdateTime = current; return true; }
    return false;
}
