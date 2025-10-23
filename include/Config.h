#pragma once
#include <string>

struct Config {
    int cellSize = 30;
    int cellCount = 25;
    int offset = 75;
    double tickInterval = 0.2;
    std::string playerName = "Player";
    std::string scoreFile = "scores.txt";

    static Config LoadFromFile(const std::string &path);
    void SaveToFile(const std::string &path) const;
};
