#include <iostream>
#include "UNO.h"

int main() {
    UNOGame game(2);
    game.initialize();
    std::cout << game.getState() << std::endl;
    game.playTurn();
    std::cout << game.getState() << std::endl;
    game.playTurn();
    std::cout << game.getState() << std::endl;
    game.playTurn();
    std::cout << game.getState() << std::endl;
    return 0;
}
