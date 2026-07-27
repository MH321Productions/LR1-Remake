#include <iostream>

#include <LR1-remake/app.hpp>

int main() {
    std::cout << "Hello world" << std::endl;

    LR1_Remake::Main main;
    return main.onExecute();
}