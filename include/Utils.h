#pragma once
#include <raylib.h>
#include <deque>

bool ElementInDeque(const Vector2 &element, const std::deque<Vector2> &dq);
bool EventTriggered(double interval, double &lastUpdateTime);
