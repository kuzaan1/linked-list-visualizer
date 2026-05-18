#include "InputHandler.h"
#include <iostream>
#include <limits>

int InputHandler::getInt() {
    int value = 0;
    while (true) {
        std::cin >> value;
        if (!std::cin.fail()) {
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            return value;
        }
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "  Invalid input. Please enter an integer: ";
    }
}