
#include <Core/FirstApp.hpp>


//std includes 
#include <iostream>
#include <stdexcept>
#include <cstdlib>

int main() {
    VE::FirstApp app{};
    try{
        app.run();
    } catch (const std::exception& e){
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    
    return EXIT_SUCCESS;
}
