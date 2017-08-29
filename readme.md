# Off The Rail

Off the rail is our sweet train based game for our Computer Graphics Course

## Structure
This project uses ASSIMP, OpenGL, and GLFW. 

The code is structured as follows:

### main.cpp
Uses glfw as the platform layer, takes input from the platform and holds the application state
It expects a dynamic library called game.[dylib|so|dll], opens this library and makes update calls
to it, passing in the current state of the game. 
main.cpp has headers describing the state data of the game.

### game.cpp 
Game is a dynamic library compiled separately to our main loop (in main.cpp). 
It takes the current state of the game, updates it, and outputs render data to the graphics card using OpenGL


## Building
You must have an ASSIMP binary available on your system. 
you can build using one of the compile scripts 'linux_compile' etc. If one does not exist for your specific platform
create it based on the details of the others.

our compilation is done in two steps:  
* compile dynamic game library
* compile main source code

## Hot Code Reloading
When the game is running, you can modify game logic, re-run the compile script, and this will automatically be loaded in. 
This swaps out the previous game library implementation. 

*Note:* Changing the structure of state data will cuase memory faults. Hot reloading will only work for logic changes. You will
need to re-launch for changes to game state.