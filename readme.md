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

**Note:** Changing the structure of state data will cuase memory faults. Hot reloading will only work for logic changes. You will
need to re-launch for changes to game state.


# Setup

## MinGW

### Installing MinGW and MSYS
Download and Install MinGW using MinGW GUI
http://www.mingw.org/wiki/Getting_Started

Add packages:
* mingw32-gcc-g++
* mingw32-base

Install MSYS (linked on getting started page)

Launch MSYS using the shortcut placed on your desktop 

### Installing GLEW
Download latest release of glew zip (not win32)

Extract to a folder you can get to in MSYS/MinGW CLI

Navigate to that folder and run the following commands line by line
	mkdir lib/
	mkdir bin/

	gcc -DGLEW_NO_GLU -O2 -Wall -W -Iinclude  -DGLEW_BUILD -o src/glew.o -c src/glew.c
	
	gcc -nostdlib -shared -Wl,-soname,libglew32.dll -Wl,--out-implib,lib/libglew32.dll.a    -o lib/glew32.dll src/glew.o -L/mingw/lib -lglu32 -lopengl32 -lgdi32 -luser32 -lkernel32

	# Create library file: lib/libglew32.dll.a
	ar cr lib/libglew32.a src/glew.o
	
	#create pkg-config file
	sed \
                -e "s|@prefix@|/usr|g" \
                -e "s|@libdir@|/usr/lib|g" \
                -e "s|@exec_prefix@|/usr/bin|g" \
                -e "s|@includedir@|/usr/include/GL|g" \
                -e "s|@version@|1.6.0|g" \
                -e "s|@cflags@||g" \
                -e "s|@libname@|GLEW|g" \
                < glew.pc.in > glew.pc

	
	gcc -DGLEW_NO_GLU -DGLEW_MX -O2 -Wall -W -Iinclude  -DGLEW_BUILD -o src/glew.mx.o -c src/glew.c

	gcc -shared -Wl,-soname,libglew32mx.dll -Wl,--out-implib,lib/libglew32mx.dll.a -o lib/glew32mx.dll src/glew.mx.o -L/mingw/lib -lglu32 -lopengl32 -lgdi32 -luser32 -lkernel32 -nostdlib

	# Create library file: lib/libglew32mx.dll.a
	ar cr lib/libglew32mx.a src/glew.mx.o

	# Create pkg-config file
	sed \
                -e "s|@prefix@|/usr|g" \
                -e "s|@libdir@|/usr/lib|g" \
                -e "s|@exec_prefix@|/usr/bin|g" \
                -e "s|@includedir@|/usr/include/GL|g" \
                -e "s|@version@|1.6.0|g" \
                -e "s|@cflags@|-DGLEW_MX|g" \
                -e "s|@libname@|GLEWmx|g" \
                < glew.pc.in > glewmx.pc

    #Make the glew visualinfo program
    gcc -c -O2 -Wall -W -Iinclude  -o src/glewinfo.o src/glewinfo.c
    gcc -O2 -Wall -W -Iinclude  -o bin/glewinfo.exe src/glewinfo.o -Llib  -lglew32 -L/mingw/lib -lglu32 -lopengl32 -lgdi32 -luser32 -lkernel32
    gcc -c -O2 -Wall -W -Iinclude  -o src/visualinfo.o src/visualinfo.c
    gcc -O2 -Wall -W -Iinclude  -o bin/visualinfo.exe src/visualinfo.o -Llib  -lglew32 -L/mingw/lib -lglu32 -lopengl32 -lgdi32 -luser32 -lkernel32

if you look into the lib/ subdirectory you should find:
* libglew32.a
* libglew32.dll.a
* glew32.dll
* + some others

copy libglew32.a and libglew32.dll.a into the folder C:/MinGW/lib/
copy glew32.dll into C:/Windows/

**Note:** MinGW does not look in /include or other unix defaults with g++
This means header files for glew and glfw etc may not be found. To resolve this, locate the header files (such as in /include) and copy the approprate ones into C:/MinGW/include/
OpenGL headers for me were located in /inlucde/GL so I copied the entire GL folder into mingw
This solves my instance of using #include "GL/glew.h"

### Installing GLFW

GLFW uses CMake
Install it if you do not have it already. https://cmake.org/

Download the GLFW source files http://www.glfw.org/
Extract it to some accessible location
Go to the glfw folder and run the following commands
	
	cmake .

This will produce a visual studios solution (.sln) for you to open.
Run 'Build Solution' in visual studios. Once it has complete you'll find a dll in src/Debug/

copy the include/GLFW directory into C:/MinGW/include
copy the dll src/debug/glfw3.dll into C:/MinGW/lib/ and rename it to glfw3.dll.a

GLFW is a piece of shit to work with so eventually I got it working on Windows by doing the following:
- Donwload the precompiled binaries (glfw-3.2.1.bin.WIN32.zip) from: http://www.glfw.org/download.html
- Copy the libglfw3.a and libglfw3dll.a into C:\MinGW\lib (or whereever else MinGW is)
- Copy the glfw3.dll into the projects build directory.

Then update the mingw_compile script change -lglfw3 to -lglfw3dll.
Fingers crossed it now builds and runs. Run it from MSYS ./build/main.exe

### Installing GLM

GLM is a header implementation, so you only need to include these files in your project. 
To do this you'll need to add glm to your includes folder (C:/MingGW/include)
Download the latest  release and copy the glm/  folder (Note: not the whole repo) into the include folder.


### Installing Assimp

Download the latest release and export it.
https://github.com/assimp/assimp/releases

#### Building from cmd(MSYS)

open MSYS and cd into the assimp folder
create a directory called build and cd into that.
build assimp using the following commands:

	cmake -G "MSYS Makefiles" ../

	make

There will now be a number of files and folders.
Copy the libassimp.dll into Mingw/lib.
Copy libassimp.dll into the build directory.
 
Find the include/ folder and copy the assimp folder into your MingGW includes folder (C:/MinGW/include/) 


#### Building using Visual Studio
using MSYS cd into the assimp directory and run
	
	make CMakeLists.txt

This will produce a solution (.sln) for your version of visual studios.
Once this has completed, open the .sln and build it

Place the lib in the appropriate spots n you're good

