# GameLib.h

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE) [![C++ Standard](https://img.shields.io/badge/C%2B%2B-11-blue.svg)](https://en.cppreference.com/w/cpp/11) [![Platform](https://img.shields.io/badge/Platform-Windows%20%7C%20Linux%20%7C%20macOS-0078d4.svg)](https://github.com/skywind3000/GameLib) [![Header Only](https://img.shields.io/badge/Header--Only-single%20file-green.svg)](GameLib.h) [![No Dependencies](https://img.shields.io/badge/Dependencies-zero-brightgreen.svg)](https://github.com/skywind3000/GameLib) [![GCC 4.9+](https://img.shields.io/badge/GCC-4.9%2B-orange.svg?logo=gnu)](https://gcc.gnu.org/)

A game development library for C++ beginners. Just one header file, no installation needed — write `#include "GameLib.h"` and start making games.

Many C++ beginners report that, unlike other languages where you can get something running in a few lines, the most painful experience is spending weeks learning C++ and still only being able to `cout` a line of text in a black console window. Building "a real program" feels impossibly far away, with almost zero positive feedback. Want to use SDL or SFML? Just setting up the environment and linking libraries is enough to exhaust you — before you've even written your first line of game logic, you already want to give up.

GameLib was born to eliminate that pain. Drop `GameLib.h` into your project folder and, in ten lines of code, draw a colorful circle controlled by the keyboard — windows, drawing, keyboard, mouse, sprites, sound, collision detection, all included. No SDK to download, no paths to configure, no libraries to link. It's like PyGame for the C++ world: hiding all the complexity and leaving only the fun of making games.

![](assets/demo.png)


## What Does It Look Like?

```cpp
#include "GameLib.h"

int main()
{
    GameLib game;
    game.Open(640, 480, "My Game", true);

    int x = 320, y = 240;

    while (!game.IsClosed()) {
        if (game.IsKeyDown(KEY_LEFT))  x -= 3;
        if (game.IsKeyDown(KEY_RIGHT)) x += 3;
        if (game.IsKeyDown(KEY_UP))    y -= 3;
        if (game.IsKeyDown(KEY_DOWN))  y += 3;

        game.Clear(COLOR_BLACK);
        game.FillCircle(x, y, 15, COLOR_CYAN);
        game.DrawText(10, 10, "Up/Down/Left/Right to move!", COLOR_WHITE);
        game.Update();
        game.WaitFrame(60);
    }
    return 0;
}
```

Compile:

```bash
g++ main.cpp -o game.exe
```

Or with an optional (recommended) flag:

```bash
g++ main.cpp -o game.exe -mwindows
```

That's it. No project configuration, no SDK downloads, no IDE settings to tweak, and no manually writing `-lgdi32 -lwinmm -lgdiplus -lole32` linker flags. Run it and control the ball with arrow keys:

![](https://skywind3000.github.io/images/p/gamelib/demo1.png)

The `-mwindows` flag (which you can even omit) launches the program as a standard Windows GUI application without popping up a console window. When creating a Windows Application project in Dev-Cpp, this flag is usually added automatically.



## Why Build This?

Existing game libraries (SDL, SFML, raylib) are all great, but for C++ beginners:

- SDL requires configuring header paths and linking a dozen DLLs, with complex usage
- SFML requires CMake
- raylib requires linear algebra knowledge, 3D basics, C/C++ proficiency, and compilation/installation

**GameLib's goal is zero barrier to entry**: copy `GameLib.h` into your project folder, write a `.cpp` file, hit compile, and your game runs.

It's specifically designed for **Dev C++** (the IDE used in many school programming courses), compatible with its bundled GCC 4.9.2 compiler. Of course, any Windows compiler supporting C++11 will work.


## Features

### Zero Configuration

- Single header file `GameLib.h`, copy and use
- No dependency on SDL / SFML / DirectX / OpenGL
- No need for `-lgdi32 -lwinmm -lgdiplus -lole32` linker flags; `-mwindows` recommended
- Compatible with Dev C++'s bundled GCC 4.9.2
- The repository also includes a standalone `GameLib.SDL.h` version, depending on SDL2 libraries, targeting cross-platform scenarios (Linux / macOS).

### Ready-to-Use Drawing

- Draw pixels, lines, rectangles, circles, triangles (stroke and fill)
- Built-in 8x8 pixel bitmap font supporting all printable ASCII characters
- `DrawTextFont` supports scalable fonts and UTF-8 text output
- `DrawPrintf` formats output on screen just like `printf`
- `ShowFps(true)` displays real-time FPS in the title bar
- All graphics algorithms implemented internally (Bresenham line, midpoint circle, scanline fill)

### Sprite System

- Load PNG, JPG, BMP, GIF and more
- Support for 8-bit palette, 24-bit, 32-bit images (auto-converted to 32-bit ARGB)
- 24-bit images automatically get full alpha channel (set to opaque)
- Flip, Color Key transparency, Alpha blending, region clipping, scaling, frame-based drawing
- Managed by integer IDs — no need to understand pointers or object lifetimes

### Keyboard and Mouse

- `IsKeyDown` — held-down detection
- `IsKeyPressed` — single press detection (triggers once on press)
- `IsKeyReleased` / `IsMouseReleased` — single release detection
- Mouse position, three-button state, and scroll wheel delta
- `IsActive()` checks whether the window has focus, handy for pausing games
- Predefined constants for all common keys: `KEY_A`\~`KEY_Z`, arrow keys, F1\~F12

### Sound

- `PlayWAV` — play sound effects (WAV format, asynchronous, returns success status)
- `PlayMusic` / `StopMusic` — play background music (MP3/MIDI/WAV, explicit MCI type by extension; MIDI looping uses notify replay)
- `IsMusicPlaying()` — query current background music playback status
- Sound effects and music on independent channels, no interference

### Game Utilities

- `Random(min, max)` — random numbers
- `RectOverlap` / `CircleOverlap` — collision detection
- `Distance` — distance between two points
- `DrawGrid` / `FillCell` — grid drawing (great for Tetris, board games)
- `GetDeltaTime` / `GetFPS` — frame time and frame rate

### Tilemap System

- `CreateTilemap` — create tile maps from tileset sprites
- `SaveTilemap` / `LoadTilemap` — save or load plain-text `.glm` map data
- `SetTile` / `GetTile` / `FillTileRect` / `ClearTilemap` — set tiles individually or in bulk
- `GetTilemapCols` / `GetTilemapRows` / `GetTileSize` — read map metadata
- `WorldToTileCol` / `WorldToTileRow` / `GetTileAtPixel` — convert pixel coordinates to tile coordinates
- `DrawTilemap` — render maps with opaque, Color Key, or Alpha modes
- Only draws tiles visible on screen — no lag even with large maps
- Easily implement side-scrolling and parallax with camera offset




## Quick Start

### Step 1: Download

Place `GameLib.h` in your project folder.

### Step 2: Write Code

Create a `main.cpp`:

```cpp
#include "GameLib.h"

int main()
{
    GameLib game;
    game.Open(800, 600, "Hello GameLib", true);

    while (!game.IsClosed()) {
        game.Clear(COLOR_DARK_BLUE);
        game.DrawTextScale(200, 250, "Hello, World!", COLOR_GOLD, 3);
        game.DrawText(280, 320, "Press ESC to exit", COLOR_GRAY);

        if (game.IsKeyPressed(KEY_ESCAPE))
            break;

        game.Update();
        game.WaitFrame(60);
    }
    return 0;
}
```

### Step 3: Compile and Run

**Dev C++**: New Project > Add `main.cpp` > Compile and Run.

**Command line**:

```bash
g++ -o game.exe main.cpp -mwindows
```




## More Examples

### Bouncing Ball

```cpp
#include "GameLib.h"

int main()
{
    GameLib game;
    game.Open(640, 480, "Bouncing Ball", true);

    float x = 320, y = 240;
    float vx = 4, vy = 3;
    int r = 20;

    while (!game.IsClosed()) {
        // Move
        x += vx;
        y += vy;

        // Bounce off walls
        if (x - r < 0 || x + r > game.GetWidth())  vx = -vx;
        if (y - r < 0 || y + r > game.GetHeight()) vy = -vy;

        // Draw
        game.Clear(COLOR_BLACK);
        game.FillCircle((int)x, (int)y, r, COLOR_RED);
        game.DrawPrintf(10, 10, COLOR_WHITE, "FPS: %.0f", game.GetFPS());
        game.Update();
        game.WaitFrame(60);
    }
    return 0;
}
```

### Keyboard Control + Mouse Tracking

```cpp
#include "GameLib.h"

int main()
{
    GameLib game;
    game.Open(640, 480, "Input Demo", true);

    int px = 320, py = 240;

    while (!game.IsClosed()) {
        // Keyboard controls the square
        if (game.IsKeyDown(KEY_A)) px -= 3;
        if (game.IsKeyDown(KEY_D)) px += 3;
        if (game.IsKeyDown(KEY_W)) py -= 3;
        if (game.IsKeyDown(KEY_S)) py += 3;

        // Mouse position
        int mx = game.GetMouseX();
        int my = game.GetMouseY();

        game.Clear(COLOR_DARK_GRAY);

        // Square
        game.FillRect(px - 15, py - 15, 30, 30, COLOR_GREEN);

        // Mouse crosshair
        game.DrawLine(mx - 20, my, mx + 20, my, COLOR_YELLOW);
        game.DrawLine(mx, my - 20, mx, my + 20, COLOR_YELLOW);

        // Draw circle on mouse click
        if (game.IsMouseDown(MOUSE_LEFT)) {
            game.FillCircle(mx, my, 10, COLOR_RED);
        }

        game.DrawPrintf(10, 10, COLOR_WHITE, "Player: %d, %d", px, py);
        game.DrawPrintf(10, 22, COLOR_WHITE, "Mouse:  %d, %d", mx, my);
        game.Update();
        game.WaitFrame(60);
    }
    return 0;
}
```

### Random Starfield

```cpp
#include "GameLib.h"

int main()
{
    GameLib game;
    game.Open(800, 600, "Starfield", true);

    // Generate 200 stars
    int sx[200], sy[200], speed[200];
    uint32_t colors[] = {COLOR_WHITE, COLOR_LIGHT_GRAY, COLOR_YELLOW, COLOR_CYAN};
    for (int i = 0; i < 200; i++) {
        sx[i] = GameLib::Random(0, 799);
        sy[i] = GameLib::Random(0, 599);
        speed[i] = GameLib::Random(1, 5);
    }

    while (!game.IsClosed()) {
        game.Clear(COLOR_BLACK);

        for (int i = 0; i < 200; i++) {
            sx[i] -= speed[i];
            if (sx[i] < 0) {
                sx[i] = 800;
                sy[i] = GameLib::Random(0, 599);
            }
            game.SetPixel(sx[i], sy[i], colors[speed[i] % 4]);
        }

        game.DrawText(250, 290, "Press ESC to exit", COLOR_GRAY);
        if (game.IsKeyPressed(KEY_ESCAPE)) break;

        game.Update();
        game.WaitFrame(60);
    }
    return 0;
}
```

More examples in the `examples/` directory.




## Example Programs

The `examples/` directory contains 19 progressive examples that demonstrate GameLib's features step by step, covering windows, graphics, input, sprites, sound, tilemap, clip rectangles, font text, and scaled drawing.

Compile any example:

```bash
g++ -o demo.exe examples/01_hello.cpp -mwindows
```

### Getting Started

| Example | Description | What You'll Learn |
|-|-|-|
| `01_hello.cpp` | Hello World | Game loop, window creation, text drawing |
| `02_moving_box.cpp` | Keyboard-controlled moving box | Keyboard input, real-time updates |
| `03_shapes.cpp` | All shape drawing showcase | Lines, rectangles, circles, triangles (stroke & fill) |
| `04_bouncing_ball.cpp` | Bouncing ball | Float-based motion, wall bounce, DrawPrintf |

### Interactive Intermediate

| Example | Description | What You'll Learn |
|-|-|-|
| `05_paint.cpp` | Simple paint program | Mouse input, scroll wheel brush size, focus-loss pause |
| `06_catch_fruit.cpp` | Fruit catching mini-game | Random, collision detection, scoring |
| `07_shooting_stars.cpp` | Simple shooter | Array-based multi-object management, bullet firing, collision destruction |

### Sprites and Sound

| Example | Description | What You'll Learn |
|-|-|-|
| `08_sprite_demo.cpp` | Sprite loading and drawing | LoadSpriteBMP, flipping, region clipping |
| `09_sprite_animation.cpp` | Sprite frame animation | DrawSpriteRegion for sprite sheet animation |
| `10_sound_demo.cpp` | Sound playback demo | PlayWAV effects, PlayMusic background music |

### Complete Mini-Games

| Example | Description | What You'll Learn |
|-|-|-|
| `11_snake.cpp` | Snake | DrawGrid/FillCell, game state machine |
| `12_breakout.cpp` | Breakout | Advanced collision detection, multi-object management |
| `13_space_shooter.cpp` | Space Shooter | Comprehensive: sprites + sound + collision + scrolling background + scoring |

### Tilemap Scrolling

| Example | Description | What You'll Learn |
|-|-|-|
| `14_tilemap.cpp` | Dual-layer scrolling | FillTileRect/ClearTilemap, pixel-to-tile conversion, parallax scrolling |
| `18_tilemap_file.cpp` | Tilemap file I/O | SaveTilemap, LoadTilemap, the plain-text `.glm` format |
| `19_clip_tilemap.cpp` | Clipped scrolling windows | SetClip/ClearClip, dual-layer tilemaps, clipped sprites / text / shapes |

### Fonts and Additional Demos

| Example | Description | What You'll Learn |
|-|-|-|
| `15_font_text.cpp` | Scalable fonts | DrawTextFont, Chinese text output, different font sizes |
| `16_playsound.cpp` | Simple sound playback | PlayWAV, key-triggered sound effects |
| `17_sprite_scaling.cpp` | Sprite scaled drawing | LoadSprite, DrawSpriteScaled / DrawSpriteFrameScaled, scroll wheel zoom |




## API Reference

### Window

| Function | Description |
|-|-|
| `Open(w, h, title, center)` | Create window, center=true to center on screen |
| `IsClosed()` | Whether the window is closed |
| `Update()` | Refresh display and process input |
| `WaitFrame(fps)` | Frame rate control |
| `GetWidth()` / `GetHeight()` | Window dimensions |
| `GetDeltaTime()` | Frame interval (seconds) |
| `GetFPS()` | Current frame rate |
| `GetTime()` | Total elapsed time (seconds) |
| `SetTitle(title)` | Change window title |
| `ShowFps(show)` | Show real-time FPS in title bar |

### Drawing

| Function | Description |
|-|-|
| `Clear(color)` | Clear screen |
| `SetPixel(x, y, color)` | Draw pixel |
| `GetPixel(x, y)` | Read pixel |
| `DrawLine(x1, y1, x2, y2, color)` | Draw line |
| `DrawRect(x, y, w, h, color)` | Rectangle outline |
| `FillRect(x, y, w, h, color)` | Filled rectangle |
| `DrawCircle(cx, cy, r, color)` | Circle outline |
| `FillCircle(cx, cy, r, color)` | Filled circle |
| `DrawTriangle(...)` | Triangle outline |
| `FillTriangle(...)` | Filled triangle |

### Text

| Function | Description |
|-|-|
| `DrawText(x, y, text, color)` | Draw text |
| `DrawNumber(x, y, number, color)` | Draw integer |
| `DrawTextScale(x, y, text, color, scale)` | Draw scaled text |
| `DrawPrintf(x, y, color, fmt, ...)` | Formatted output |
| `DrawTextFont(x, y, text, color, size)` | Draw text with default font |
| `DrawTextFont(x, y, text, color, font, size)` | Draw text with specified font |
| `GetTextWidthFont(...)` / `GetTextHeightFont(...)` | Measure font text dimensions |

### Sprites

| Function | Description |
|-|-|
| `CreateSprite(w, h)` | Create blank sprite, returns ID |
| `LoadSprite(filename)` | Load image sprite (PNG/JPG/BMP/GIF/TIFF, UTF-8 path) |
| `LoadSpriteBMP(filename)` | Load sprite from BMP (8/24/32-bit, UTF-8 path) |
| `FreeSprite(id)` | Free sprite |
| `DrawSprite(id, x, y)` | Draw sprite |
| `DrawSpriteEx(id, x, y, flags)` | Draw with flip/transparency/alpha blending |
| `DrawSpriteRegion(id, x, y, sx, sy, sw, sh)` | Draw sprite sub-region |
| `DrawSpriteRegionEx(...)` | Draw sprite sub-region with flags |
| `DrawSpriteScaled(id, x, y, w, h, flags)` | Draw sprite scaled to target size |
| `DrawSpriteFrame(...)` | Draw frame from sprite sheet by frame number |
| `DrawSpriteFrameScaled(...)` | Draw scaled frame from sprite sheet by frame number |
| `SetSpritePixel(id, x, y, color)` | Modify sprite pixel |
| `GetSpritePixel(id, x, y)` | Read sprite pixel |
| `SetSpriteColorKey(id, color)` | Set sprite's Color Key |
| `GetSpriteColorKey(id)` | Get sprite's Color Key |

Sprite flags: `SPRITE_FLIP_H` (horizontal flip), `SPRITE_FLIP_V` (vertical flip), `SPRITE_COLORKEY` (transparent by sprite's current Color Key, default magenta), `SPRITE_ALPHA` (alpha blending)

The default `DrawSprite(id, x, y)` uses the opaque fast path. If your assets rely on transparent holes, explicitly pass `SPRITE_COLORKEY` or `SPRITE_ALPHA`.

### Input

| Function | Description |
|-|-|
| `IsKeyDown(key)` | Whether key is held down |
| `IsKeyPressed(key)` | Whether key was just pressed (single trigger) |
| `IsKeyReleased(key)` | Whether key was just released (single trigger) |
| `GetMouseX()` / `GetMouseY()` | Mouse position |
| `IsMouseDown(button)` | Whether mouse button is pressed |
| `IsMousePressed(button)` | Whether mouse button was just pressed (single trigger) |
| `IsMouseReleased(button)` | Whether mouse button was just released (single trigger) |
| `GetMouseWheelDelta()` | Accumulated scroll wheel delta since last `Update()` |
| `IsActive()` | Whether the window is currently active |

### Sound

| Function | Description |
|-|-|
| `PlayWAV(filename, loop)` | Play sound effect, returns `true` on success |
| `StopWAV()` | Stop sound effects |
| `PlayMusic(filename, loop)` | Play background music (MP3/MIDI/WAV, explicit MCI type by extension; MIDI looping uses notify replay), returns `true` on success |
| `StopMusic()` | Stop background music |
| `IsMusicPlaying()` | Whether music is currently playing |
| `PlayBeep(freq, duration)` | Beep |

### Utilities

| Function | Description |
|-|-|
| `Random(min, max)` | Random number |
| `RectOverlap(...)` | Rectangle collision detection |
| `CircleOverlap(...)` | Circle collision detection |
| `PointInRect(px, py, x, y, w, h)` | Point inside rectangle |
| `Distance(x1, y1, x2, y2)` | Distance between two points |
| `DrawGrid(x, y, rows, cols, size, color)` | Draw grid |
| `FillCell(gx, gy, row, col, size, color)` | Fill grid cell |

### Tilemap

| Function | Description |
|-|-|
| `CreateTilemap(cols, rows, tileSize, tilesetId)` | Create tile map, returns ID |
| `SaveTilemap(filename, mapId)` | Save map to a plain-text `.glm` file |
| `LoadTilemap(filename, tilesetId)` | Load map from a plain-text `.glm` file |
| `FreeTilemap(mapId)` | Free map |
| `SetTile(mapId, col, row, tileId)` | Set tile (-1=empty) |
| `GetTile(mapId, col, row)` | Read tile |
| `GetTilemapCols(mapId)` / `GetTilemapRows(mapId)` | Read map column/row count |
| `GetTileSize(mapId)` | Read map tile size |
| `WorldToTileCol(mapId, x)` / `WorldToTileRow(mapId, y)` | Convert pixel to tile coordinates |
| `GetTileAtPixel(mapId, x, y)` | Read tile at pixel position |
| `FillTileRect(mapId, col, row, cols, rows, tileId)` | Batch fill rectangular area |
| `ClearTilemap(mapId, tileId)` | Fill entire map with specified tile (default `-1` to clear) |
| `DrawTilemap(mapId, x, y, flags)` | Draw map (supports ColorKey/Alpha) |

A tileset is a regular sprite (`LoadSprite` / `CreateSprite`), automatically sliced by `tileSize`. `WorldToTileCol/Row` uses floor division even for negative coordinates. `DrawTilemap` defaults to the opaque fast path, drawing only visible tiles; pass `(-cameraX, -cameraY)` for scrolling, and explicitly pass `SPRITE_COLORKEY` or `SPRITE_ALPHA` for transparent holes. `.glm` is a plain-text format: line 1 is `GLM1`, line 2 is `tileSize rows cols`, and each following line stores one tile row separated by spaces or tabs. When loading, values beyond `cols` on a row are ignored, missing values are filled with `-1`, extra rows beyond `rows` are ignored, and missing rows stay `-1`. The file does not store a tileset path; the caller provides `tilesetId` when loading.

### Color Constants

```
COLOR_BLACK    COLOR_WHITE     COLOR_RED       COLOR_GREEN     COLOR_BLUE
COLOR_YELLOW   COLOR_CYAN      COLOR_MAGENTA   COLOR_ORANGE    COLOR_PINK
COLOR_PURPLE   COLOR_GRAY      COLOR_DARK_GRAY COLOR_LIGHT_GRAY
COLOR_DARK_RED COLOR_DARK_GREEN COLOR_DARK_BLUE COLOR_SKY_BLUE
COLOR_BROWN    COLOR_GOLD      COLOR_TRANSPARENT
```

Custom colors: `COLOR_RGB(r, g, b)` or `COLOR_ARGB(a, r, g, b)`




## What Can You Build?

- Space Shooter
- Side-Scrolling Platformer
- Tetris
- Snake
- Breakout
- Maze
- Fruit Catcher
- Bullet Hell
- Paint Program
- Any 2D mini-game you can think of




## Technical Details

- **Double Buffering**: All drawing writes to an in-memory framebuffer (`uint32_t*` ARGB); `Update()` flushes to the window via DIB Section + `BitBlt`, flicker-free
- **Self-Implemented Graphics**: Does not call GDI primitives like `LineTo` or `Ellipse`; line, circle, and triangle algorithms are all implemented within the library
- **Font Backend**: Scalable font text currently implemented via GDI's `CreateFontW` / `TextOutW` / `GetTextExtentPoint32W`
- **Precise Window Size**: `Open()` guarantees the client area exactly matches the requested dimensions (with high-DPI secondary correction)
- **stb-Style Single Header**: Include enables implementation by default; multi-file projects can use `GAMELIB_NO_IMPLEMENTATION` to control this




## Requirements

- Windows 7 or later
- Any compiler supporting C++11 (GCC 4.9+, MSVC 2015+, Clang)
- Recommended IDEs: Dev C++, Visual Studio, Code::Blocks

## Resources

Looking for game assets? Check out these sites:

- Classic game sprite resources: https://www.spriters-resource.com
- Pixel art resources: http://www.charas-project.net/resources.php?lang=en
- More pixel assets: https://craftpix.net/tags/pixel-art-game-assets-download

You can also visit PyGame's [Resources](https://www.pygame.org/wiki/resources) page for free images, audio, and font resources.


## License

MIT License. Use it however you want.
