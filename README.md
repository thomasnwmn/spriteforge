# SpriteForge

SpriteForge is a custom 2D Game Engine written in C, using SDL2 and CMake! 

## Installation & Setup

To run SpriteForge on your machine (specifically targeting Windows), you will need to install a few development tools. Follow these step-by-step instructions to get up and running:

### 1. Prerequisites
Before you start, you must install the following software if you do not have them already:
- **Visual Studio**: Download and install [Visual Studio Community](https://visualstudio.microsoft.com/vs/community/). During installation, make sure to select the **"Desktop development with C++"** workload.
- **Git**: Download and install [Git for Windows](https://git-scm.com/download/win).
- **CMake**: Download and install [CMake](https://cmake.org/download/). Make sure to check the option to "Add CMake to the system PATH" during installation.

### 2. Clone the Repository
Open a terminal (such as PowerShell or Command Prompt) and clone the engine to your computer:
```bash
git clone https://github.com/your-username/spriteforge.git
cd spriteforge
```

### 3. Setup VCPKG & Install Dependencies
This project uses **vcpkg** as its package manager to handle SDL2 and Lua. 
If the `vcpkg` folder in this repository is empty, initialize it by running:
```bash
git clone https://github.com/microsoft/vcpkg.git
.\vcpkg\bootstrap-vcpkg.bat
```
*(If vcpkg is already fully present in the folder, just run the bootstrap script above).*

Now, use vcpkg to install the engine's required libraries (SDL2, SDL_image, SDL_ttf, and Lua) for 64-bit Windows:
```bash
.\vcpkg\vcpkg install sdl2 sdl2-image sdl2-ttf lua --triplet=x64-windows
```

### 4. Build the Engine
With the dependencies installed, you can now use CMake to generate the build files and compile the engine. Run the following commands from the root `spriteforge` directory:

```bash
# Generate the build files inside a "build" folder, pointing CMake to vcpkg
cmake -B build -S . -DCMAKE_TOOLCHAIN_FILE="vcpkg/scripts/buildsystems/vcpkg.cmake"

# Compile the engine!
cmake --build build
```

### 5. Run SpriteForge!
Once compilation is complete, your executable will be located in the `build/Debug` (or `build/Release`) directory. Because we set up a CMake post-build step, your game project folders (`example_projects` and `user_projects`) are automatically copied to the build folder!

To launch the engine, simply run:
```bash
.\build\Debug\spriteforge.exe
```


## Engine Development Checklist

- [x] **1. Core Application & Game Loop**
  - Window creation and management using SDL2.
  - The main game loop (Process Input -> Update -> Render).
  - Time management (calculating Delta Time to ensure the game runs at the same speed on all computers).

- [x] **2. Input System**
  - Polling keyboard, mouse, and controller state.
  - Abstracting input (e.g., mapping `SPACE` or `Gamepad A` to a logical `JUMP` action).

- [x] **3. Rendering System (2D)**
  - Clearing the screen and swapping buffers.
  - Drawing textures (Sprites) to the screen.
  - A 2D Camera system (panning and zooming).
  - *Future:* Sprite batching for rendering thousands of sprites efficiently.

- [x] **4. Resource / Asset Manager**
  - Loading assets from disk (images, fonts, audio files).
  - Caching assets in a hash map/dictionary so they aren't loaded into memory multiple times.

- [x] **5. Entity & Game Object System**
  - A base Entity struct or class to represent objects.
  - Spawning and destroying entities.

- [x] **6. Physics & Collision Detection**
  - AABB (Axis-Aligned Bounding Box) collision detection.
  - Collision shapes (AABB - Axis-Aligned Bounding Boxes, Circles).
  - Detecting overlaps and resolving them (stopping players from walking through walls).

- [x] **7. State / Scene Management**
  - Handling different "screens" of the game (Main Menu, Loading, Gameplay, Game Over).
  - Safely loading and unloading scenes.

- [ ] **8. Audio System**
  - Loading and playing short sound effects (SFX).
  - Streaming longer background music (BGM).
  - Audio mixing and volume control.

- [ ] **9. UI (User Interface) System**
  - Rendering text to the screen using TTF (TrueType Fonts).
  - Basic interactive elements (Clickable buttons, health bars).

- [ ] **10. Event / Messaging System**
  - Allowing different parts of the engine to communicate without being tangled together (e.g., the collision system broadcasts a "PlayerHit" event, and the audio system listens for it to play a sound).

## Future plans

- [ ] **World generation**
  - For a minecraft style game maybe?
  - Horror coop style game (preset maps but randomly generated each level to make each level different/harder) (factory/forest, randomly generated each time)

- [ ] **NPC Pathfinding**
  - Entities following a predefined route?
  - Entities tracking player (enemies?)
