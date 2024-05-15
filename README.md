# Vulkan engine 3d modeler

This is a very basic 3d modeler based in a Vulkan engine.
this is made for the course graphics programming 2 in dae Howest.


## controls 

wasd for basic movement
q and e for up and down movement
shift for sprint
left click + mouse movement for rotation

to select vertices you hold control

ctrl + a: select all vertices
ctrl + l: select all vertices that are facing the camera
ctrl + left click: select vertices at mouse location

hold shift to have multiselect

ctrl + alt + a: deselect all vertices
ctrl + alt + l: deselect all vertices that are facing the camera
ctrl + alt + left click: deselect vertices at mouse location

when having verts selected and not pressing ctrl you are able to move the selected vertices 
arrow keys for the xy plane
Right shift and right control keys for the z axis


## build instructions

### easy route
double click the bat file
do know that windows will probably complain about it being an unknown file, just click open anyway
and then the .exe will be in build/bin/Debug

### the manual way

open the location of the project in a cmd file and fill out these commands:

1. mkdir build  
2. cd build    
3. cmake ..    
4. cmake --build .

after this is done the .exe of the project will appear in build/bin/Debug

## extras

the random floating spheres are pointlight in the scene


## credits

credit where credit is due, patrick nellessen helped a lot by making the template project to start in.
and Brendan Galea, Who's youtube tutorial i followed to learn how to make a vulkan renderer

