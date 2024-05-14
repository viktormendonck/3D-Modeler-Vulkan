# Vulkan engine 3d modeler

This is a very basic 3d modeler based in a Vulkan engine.
this is made for the course graphics programming 2 in dae Howest.


## controls 

wasd for basic movement
q and e for up and down movement
shift for sprint

hold control to select a vertex
hold shift to have multiselect

when having verts selected and not pressing ctrl you are able to move the selected vertices 
arrow keys for the xy plane
Right shift and right control keys for the z


## build instructions

### easy route
double click the bat file
and then the .exe will be in build/bin/Debug

### the manual way

open the location of the project in a cmd file and fill out these commands:

1. mkdir build  
2. cd build    
3. cmake ..    
4. cmake --build .

after this is done the .exe of the project will appear in build/bin/Debug
