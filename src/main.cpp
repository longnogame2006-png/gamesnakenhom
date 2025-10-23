#include <iostream>
#include "Game.h"
#include "Config.h"
#include "Exceptions.h"

int main() {
    std::cout << "=== DEBUG: STARTING GAME ===" << std::endl;
    
    try {
        std::cout << "1. Loading config..." << std::endl;
        Config cfg = Config::LoadFromFile("config.cfg");
        std::cout << "   Config loaded: " << cfg.cellSize << "x" << cfg.cellCount << std::endl;
        
        std::cout << "2. Creating game instance..." << std::endl;
        Game game(cfg);
        std::cout << "   Game instance created successfully" << std::endl;
        
        std::cout << "3. Running game loop..." << std::endl;
        game.Run();
        
        std::cout << "4. Game exited normally" << std::endl;
    }
    catch (const FileIOException &ex) {
        std::cerr << "ERROR: File I/O: " << ex.what() << std::endl;
        return 2;
    }
    catch (const std::exception &ex) {
        std::cerr << "ERROR: " << ex.what() << std::endl;
        return 3;
    }
    
    std::cout << "=== DEBUG: GAME ENDED ===" << std::endl;
    return 0;
}