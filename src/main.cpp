#include <iostream>

#include <LR1-remake/app.hpp>

int main() {
    std::cout << "Hello world" << std::endl;

    LR1_Remake::Main main(std::filesystem::current_path()); //TODO: Determine current path using a more robust algorithm
    return main.onExecute();
}