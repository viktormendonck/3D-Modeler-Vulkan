
//vulkan 3d modeling software made by viktor mendonck in the second year of the game developent course of howest
//this engine was made with the tutorial from Brendan galea's youtube series
#include <App/ModelingApp.hpp>

//std includes 
#include <iostream>
#include <stdexcept>
#include <cstdlib>

int main() {
    VE::ModelingApp app{};
    try{
        app.run();
    } catch (const std::exception& e){
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    
    return EXIT_SUCCESS;
}
