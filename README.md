# SpriteForge

SpriteForge is a custom 2D Game Engine written in C, using SDL2 and CMake! 

## Engine Development Checklist

Building an engine is a marathon, not a sprint. Here are the 10 core systems we need to build for a complete 2D game engine, in roughly the order we should tackle them:

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

- [ ] **7. State / Scene Management**
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