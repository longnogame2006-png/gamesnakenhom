#include "Config.h"
#include "Exceptions.h"
#include <fstream>
#include <sstream>

Config Config::LoadFromFile(const std::string &path) {
    Config cfg;
    std::ifstream in(path);
    if (!in) return cfg; // missing -> defaults
    std::string line;
    while (std::getline(in, line)) {
        if (line.empty() || line[0]=='#') continue;
        auto pos = line.find('=');
        if (pos==std::string::npos) continue;
        std::string key = line.substr(0,pos);
        std::string val = line.substr(pos+1);
        if (key=="cellSize") cfg.cellSize = std::stoi(val);
        else if (key=="cellCount") cfg.cellCount = std::stoi(val);
        else if (key=="offset") cfg.offset = std::stoi(val);
        else if (key=="tickInterval") cfg.tickInterval = std::stod(val);
        else if (key=="playerName") cfg.playerName = val;
        else if (key=="scoreFile") cfg.scoreFile = val;
    }
    return cfg;
}

void Config::SaveToFile(const std::string &path) const {
    std::ofstream out(path);
    if (!out) throw FileIOException("Cannot open config file for writing");
    out << "cellSize=" << cellSize << "\n";
    out << "cellCount=" << cellCount << "\n";
    out << "offset=" << offset << "\n";
    out << "tickInterval=" << tickInterval << "\n";
    out << "playerName=" << playerName << "\n";
    out << "scoreFile=" << scoreFile << "\n";
}
