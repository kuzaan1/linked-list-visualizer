#ifndef INPUT_HANDLER_H
#define INPUT_HANDLER_H

#include <limits>
#include <type_traits>
#include <iostream>

class InputHandler {
public:
    static int getInt();
    template <typename T>
    static T getValue();

private:
    InputHandler() = delete;
};

template <typename T>
T InputHandler::getValue() {
    if constexpr (std::is_same_v<T, int>) {
        return getInt();
    } else {
        T value;
        std::cin >> value;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        return value;
    }
}

#endif