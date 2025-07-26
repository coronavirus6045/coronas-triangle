#include "common.hpp"
#include "renderer.hpp"

//Yall love CHICKEN?
Renderer renderer;
int main() {
    try {
        std::cout << "[INIT]: Starting the triangle.....\n";

        renderer.initialize();
        renderer.boot();
    } catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

