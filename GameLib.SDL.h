//=====================================================================
//
// GameLib.SDL.h - A single-header cross-platform game library
//
// Uses SDL2 as the platform backend and keeps a GameLib-style API.
// The current scaffold already supports window creation, input,
// framebuffer rendering, built-in 8x8 text, software sprite drawing,
// and tilemaps. SDL_image / SDL_ttf / SDL_mixer features are enabled
// automatically when their headers are available at compile time.
//
// How to use (single file project, most common):
//
//     #include "GameLib.SDL.h"
//
//     int main() {
//         GameLib game;
//         game.Open(640, 480, "My Game", true);
//         while (!game.IsClosed()) {
//             game.Clear(COLOR_BLACK);
//             game.DrawText(10, 10, "Hello SDL", COLOR_WHITE);
//             game.Update();
//             game.WaitFrame(60);
//         }
//         return 0;
//     }
//
// Multi-file project: add this line before #include in the main .cpp file
//     #define GAMELIB_SDL_IMPLEMENTATION
//     #include "GameLib.SDL.h"
// In other .cpp files, add this line
//     #define GAMELIB_SDL_NO_IMPLEMENTATION
//     #include "GameLib.SDL.h"
//
//=====================================================================
#ifndef GAMELIB_SDL_H
#define GAMELIB_SDL_H

#ifdef GAMELIB_H
#error GameLib.h and GameLib.SDL.h cannot be included in the same translation unit.
#endif

#ifndef SDL_MAIN_HANDLED
#define SDL_MAIN_HANDLED
#endif

#ifndef GAMELIB_SDL_NO_IMPLEMENTATION
#ifndef GAMELIB_SDL_IMPLEMENTATION
#define GAMELIB_SDL_IMPLEMENTATION
#endif
#endif

#define GAMELIB_SDL_VERSION_MAJOR 0
#define GAMELIB_SDL_VERSION_MINOR 1
#define GAMELIB_SDL_VERSION_PATCH 0

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <stdarg.h>

#include <vector>
#include <string>

struct SDL_Window;
struct SDL_Renderer;
struct SDL_Texture;
struct SDL_Surface;
typedef struct TTF_Font TTF_Font;
typedef struct Mix_Chunk Mix_Chunk;
typedef struct Mix_Music Mix_Music;

// Color constants (ARGB format: 0xAARRGGBB)
#define COLOR_BLACK       0xFF000000
#define COLOR_WHITE       0xFFFFFFFF
#define COLOR_RED         0xFFFF0000
#define COLOR_GREEN       0xFF00FF00
#define COLOR_BLUE        0xFF0000FF
#define COLOR_YELLOW      0xFFFFFF00
#define COLOR_CYAN        0xFF00FFFF
#define COLOR_MAGENTA     0xFFFF00FF
#define COLOR_ORANGE      0xFFFF8800
#define COLOR_PINK        0xFFFF88CC
#define COLOR_PURPLE      0xFF8800FF
#define COLOR_GRAY        0xFF888888
#define COLOR_DARK_GRAY   0xFF444444
#define COLOR_LIGHT_GRAY  0xFFCCCCCC
#define COLOR_DARK_RED    0xFF880000
#define COLOR_DARK_GREEN  0xFF008800
#define COLOR_DARK_BLUE   0xFF000088
#define COLOR_SKY_BLUE    0xFF87CEEB
#define COLOR_BROWN       0xFF8B4513
#define COLOR_GOLD        0xFFFFD700
#define COLOR_TRANSPARENT 0x00000000

#define COLOR_RGB(r, g, b)     ((uint32_t)(0xFF000000 | (((r) & 0xFF) << 16) | (((g) & 0xFF) << 8) | ((b) & 0xFF)))
#define COLOR_ARGB(a, r, g, b) ((uint32_t)((((a) & 0xFF) << 24) | (((r) & 0xFF) << 16) | (((g) & 0xFF) << 8) | ((b) & 0xFF)))

#define COLOR_GET_A(c)    (((c) >> 24) & 0xFF)
#define COLOR_GET_R(c)    (((c) >> 16) & 0xFF)
#define COLOR_GET_G(c)    (((c) >> 8) & 0xFF)
#define COLOR_GET_B(c)    ((c) & 0xFF)

// Keyboard constants (kept source-compatible with the Win32 GameLib values)
#define KEY_LEFT      0x25
#define KEY_RIGHT     0x27
#define KEY_UP        0x26
#define KEY_DOWN      0x28
#define KEY_SPACE     0x20
#define KEY_ENTER     0x0D
#define KEY_ESCAPE    0x1B
#define KEY_TAB       0x09
#define KEY_SHIFT     0x10
#define KEY_CONTROL   0x11
#define KEY_BACK      0x08

#define KEY_A         0x41
#define KEY_B         0x42
#define KEY_C         0x43
#define KEY_D         0x44
#define KEY_E         0x45
#define KEY_F         0x46
#define KEY_G         0x47
#define KEY_H         0x48
#define KEY_I         0x49
#define KEY_J         0x4A
#define KEY_K         0x4B
#define KEY_L         0x4C
#define KEY_M         0x4D
#define KEY_N         0x4E
#define KEY_O         0x4F
#define KEY_P         0x50
#define KEY_Q         0x51
#define KEY_R         0x52
#define KEY_S         0x53
#define KEY_T         0x54
#define KEY_U         0x55
#define KEY_V         0x56
#define KEY_W         0x57
#define KEY_X         0x58
#define KEY_Y         0x59
#define KEY_Z         0x5A

#define KEY_0         0x30
#define KEY_1         0x31
#define KEY_2         0x32
#define KEY_3         0x33
#define KEY_4         0x34
#define KEY_5         0x35
#define KEY_6         0x36
#define KEY_7         0x37
#define KEY_8         0x38
#define KEY_9         0x39

#define KEY_F1        0x70
#define KEY_F2        0x71
#define KEY_F3        0x72
#define KEY_F4        0x73
#define KEY_F5        0x74
#define KEY_F6        0x75
#define KEY_F7        0x76
#define KEY_F8        0x77
#define KEY_F9        0x78
#define KEY_F10       0x79
#define KEY_F11       0x7A
#define KEY_F12       0x7B

#define MOUSE_LEFT    0
#define MOUSE_RIGHT   1
#define MOUSE_MIDDLE  2

#define SPRITE_FLIP_H     1
#define SPRITE_FLIP_V     2
#define SPRITE_COLORKEY   4
#define SPRITE_ALPHA      8

#ifndef COLORKEY_DEFAULT
#define COLORKEY_DEFAULT  0xFFFF00FF
#endif

#ifndef GAMELIB_SDL_DEFAULT_FONT
#define GAMELIB_SDL_DEFAULT_FONT ""
#endif

// Optional SDL extension backends are auto-enabled when their headers are present.
// Define these macros to 1 before including GameLib.SDL.h if you want to force-disable
// a backend and avoid linking the corresponding extension library.
#ifndef GAMELIB_SDL_DISABLE_IMAGE
#define GAMELIB_SDL_DISABLE_IMAGE 0
#endif

#ifndef GAMELIB_SDL_DISABLE_TTF
#define GAMELIB_SDL_DISABLE_TTF 0
#endif

#ifndef GAMELIB_SDL_DISABLE_MIXER
#define GAMELIB_SDL_DISABLE_MIXER 0
#endif

// Windows DPI behavior: match GameLib.h by default.
// "unaware" lets Windows scale the whole window with the system scale factor,
// so a logical 800x600 window stays comfortably readable on high-DPI displays.
// Override this macro before including GameLib.SDL.h if you want SDL's DPI-aware modes.
#ifndef GAMELIB_SDL_WINDOWS_DPI_AWARENESS
#define GAMELIB_SDL_WINDOWS_DPI_AWARENESS "unaware"
#endif

class GameLib
{
public:
    GameLib();
    ~GameLib();

    int Open(int width, int height, const char *title, bool center = false);
    bool IsClosed() const;
    void Update();
    void WaitFrame(int fps);
    float GetDeltaTime() const;
    float GetFPS() const;
    float GetTime() const;
    int GetWidth() const;
    int GetHeight() const;
    void SetTitle(const char *title);
    void ShowFps(bool show);

    void Clear(uint32_t color = COLOR_BLACK);
    void SetPixel(int x, int y, uint32_t color);
    uint32_t GetPixel(int x, int y) const;

    void DrawLine(int x1, int y1, int x2, int y2, uint32_t color);
    void DrawRect(int x, int y, int w, int h, uint32_t color);
    void FillRect(int x, int y, int w, int h, uint32_t color);
    void DrawCircle(int cx, int cy, int r, uint32_t color);
    void FillCircle(int cx, int cy, int r, uint32_t color);
    void DrawTriangle(int x1, int y1, int x2, int y2, int x3, int y3, uint32_t color);
    void FillTriangle(int x1, int y1, int x2, int y2, int x3, int y3, uint32_t color);

    void DrawText(int x, int y, const char *text, uint32_t color);
    void DrawNumber(int x, int y, int number, uint32_t color);
    void DrawTextScale(int x, int y, const char *text, uint32_t color, int scale);
    void DrawPrintf(int x, int y, uint32_t color, const char *fmt, ...);

    void DrawTextFont(int x, int y, const char *text, uint32_t color, const char *fontName, int fontSize);
    void DrawTextFont(int x, int y, const char *text, uint32_t color, int fontSize);
    int GetTextWidthFont(const char *text, const char *fontName, int fontSize);
    int GetTextWidthFont(const char *text, int fontSize);
    int GetTextHeightFont(const char *text, const char *fontName, int fontSize);
    int GetTextHeightFont(const char *text, int fontSize);

    int CreateSprite(int width, int height);
    int LoadSpriteBMP(const char *filename);
    int LoadSprite(const char *filename);
    void FreeSprite(int id);
    void DrawSprite(int id, int x, int y);
    void DrawSpriteEx(int id, int x, int y, int flags);
    void DrawSpriteRegion(int id, int x, int y, int sx, int sy, int sw, int sh);
    void DrawSpriteRegionEx(int id, int x, int y, int sx, int sy, int sw, int sh, int flags = 0);
    void DrawSpriteScaled(int id, int x, int y, int w, int h, int flags = 0);
    void DrawSpriteFrame(int id, int x, int y, int frameW, int frameH, int frameIndex, int flags = 0);
    void DrawSpriteFrameScaled(int id, int x, int y, int frameW, int frameH, int frameIndex, int w, int h, int flags = 0);
    void SetSpritePixel(int id, int x, int y, uint32_t color);
    uint32_t GetSpritePixel(int id, int x, int y) const;
    int GetSpriteWidth(int id) const;
    int GetSpriteHeight(int id) const;
    void SetSpriteColorKey(int id, uint32_t color);
    uint32_t GetSpriteColorKey(int id) const;

    bool IsKeyDown(int key) const;
    bool IsKeyPressed(int key) const;
    bool IsKeyReleased(int key) const;
    int GetMouseX() const;
    int GetMouseY() const;
    bool IsMouseDown(int button) const;
    bool IsMousePressed(int button) const;
    bool IsMouseReleased(int button) const;
    int GetMouseWheelDelta() const;
    bool IsActive() const;

    void PlayBeep(int frequency, int duration);
    bool PlayWAV(const char *filename, bool loop = false);
    void StopWAV();
    bool PlayMusic(const char *filename, bool loop = true);
    void StopMusic();
    bool IsMusicPlaying() const;

    static int Random(int minVal, int maxVal);
    static bool RectOverlap(int x1, int y1, int w1, int h1,
                            int x2, int y2, int w2, int h2);
    static bool CircleOverlap(int cx1, int cy1, int r1,
                              int cx2, int cy2, int r2);
    static bool PointInRect(int px, int py, int x, int y, int w, int h);
    static float Distance(int x1, int y1, int x2, int y2);

    void DrawGrid(int x, int y, int rows, int cols, int cellSize, uint32_t color);
    void FillCell(int gridX, int gridY, int row, int col, int cellSize, uint32_t color);

    int CreateTilemap(int cols, int rows, int tileSize, int tilesetId);
    void FreeTilemap(int mapId);
    void SetTile(int mapId, int col, int row, int tileId);
    int GetTile(int mapId, int col, int row) const;
    int GetTilemapCols(int mapId) const;
    int GetTilemapRows(int mapId) const;
    int GetTileSize(int mapId) const;
    int WorldToTileCol(int mapId, int x) const;
    int WorldToTileRow(int mapId, int y) const;
    int GetTileAtPixel(int mapId, int x, int y) const;
    void FillTileRect(int mapId, int col, int row, int cols, int rows, int tileId);
    void ClearTilemap(int mapId, int tileId = -1);
    void DrawTilemap(int mapId, int x, int y, int flags = 0);

private:
    GameLib(const GameLib &);
    GameLib &operator=(const GameLib &);

    void _DestroyWindowResources();
    void _UpdateTitleFps();
    void _SyncInputState();
    void _SetPixelFast(int x, int y, uint32_t color);
    void _DrawHLine(int x1, int x2, int y, uint32_t color);
    int _AllocSpriteSlot();
    void _DrawSpriteAreaFast(int id, int x, int y, int sx, int sy, int sw, int sh, int flags);
    void _DrawSpriteAreaScaled(int id, int x, int y, int sx, int sy, int sw, int sh,
                               int dw, int dh, int flags);
    int _AllocTilemapSlot();
    bool _EnsureImageReady();
    bool _EnsureTtfReady();
    bool _EnsureMixerReady();
    TTF_Font *_GetFont(const char *fontName, int fontSize);
    void _BlendSurfaceToFramebuffer(int x, int y, SDL_Surface *surface);

private:
    SDL_Window *_window;
    SDL_Renderer *_renderer;
    SDL_Texture *_frameTexture;
    bool _closing;
    bool _active;
    bool _showFps;
    bool _focused;
    bool _minimized;
    bool _sdlReady;
    bool _imageReady;
    bool _ttfReady;
    bool _mixerReady;
    bool _audioReady;
    std::string _title;
    int _width;
    int _height;

    uint32_t *_framebuffer;

    int _keys[512];
    int _keys_prev[512];
    int _mouseX;
    int _mouseY;
    int _mouseButtons[3];
    int _mouseButtons_prev[3];
    int _mouseWheelDelta;

    uint64_t _timeStartCounter;
    uint64_t _timePrevCounter;
    uint64_t _fpsTimeCounter;
    uint64_t _perfFrequency;
    float _deltaTime;
    float _fps;
    float _fpsAccum;

    struct GameSprite {
        int width;
        int height;
        uint32_t *pixels;
        uint32_t colorKey;
        bool used;
    };
    std::vector<GameSprite> _sprites;

    struct GameTilemap {
        int cols;
        int rows;
        int tileSize;
        int tilesetId;
        int tilesetCols;
        int *tiles;
        bool used;
    };
    std::vector<GameTilemap> _tilemaps;

    struct GameFontCacheEntry {
        std::string key;
        TTF_Font *font;
    };
    std::vector<GameFontCacheEntry> _fontCache;

    Mix_Chunk *_currentWav;
    int _wavChannel;
    Mix_Music *_currentMusic;
    bool _musicPlaying;

    static bool _srandDone;
};

// Classic 8x8 bitmap font, 8 bytes per char, one byte per row, MSB on left
static const unsigned char _gamelib_font8x8[95][8] = {
    { 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00 },
    { 0x18,0x3C,0x3C,0x18,0x18,0x00,0x18,0x00 },
    { 0x6C,0x6C,0x24,0x00,0x00,0x00,0x00,0x00 },
    { 0x6C,0x6C,0xFE,0x6C,0xFE,0x6C,0x6C,0x00 },
    { 0x18,0x7E,0xC0,0x7C,0x06,0xFC,0x18,0x00 },
    { 0x00,0xC6,0xCC,0x18,0x30,0x66,0xC6,0x00 },
    { 0x38,0x6C,0x38,0x76,0xDC,0xCC,0x76,0x00 },
    { 0x18,0x18,0x30,0x00,0x00,0x00,0x00,0x00 },
    { 0x0C,0x18,0x30,0x30,0x30,0x18,0x0C,0x00 },
    { 0x30,0x18,0x0C,0x0C,0x0C,0x18,0x30,0x00 },
    { 0x00,0x66,0x3C,0xFF,0x3C,0x66,0x00,0x00 },
    { 0x00,0x18,0x18,0x7E,0x18,0x18,0x00,0x00 },
    { 0x00,0x00,0x00,0x00,0x00,0x18,0x18,0x30 },
    { 0x00,0x00,0x00,0x7E,0x00,0x00,0x00,0x00 },
    { 0x00,0x00,0x00,0x00,0x00,0x18,0x18,0x00 },
    { 0x06,0x0C,0x18,0x30,0x60,0xC0,0x80,0x00 },
    { 0x7C,0xCE,0xDE,0xF6,0xE6,0xC6,0x7C,0x00 },
    { 0x18,0x38,0x78,0x18,0x18,0x18,0x7E,0x00 },
    { 0x7C,0xC6,0x06,0x1C,0x70,0xC6,0xFE,0x00 },
    { 0x7C,0xC6,0x06,0x3C,0x06,0xC6,0x7C,0x00 },
    { 0x1C,0x3C,0x6C,0xCC,0xFE,0x0C,0x1E,0x00 },
    { 0xFE,0xC0,0xFC,0x06,0x06,0xC6,0x7C,0x00 },
    { 0x38,0x60,0xC0,0xFC,0xC6,0xC6,0x7C,0x00 },
    { 0xFE,0xC6,0x0C,0x18,0x30,0x30,0x30,0x00 },
    { 0x7C,0xC6,0xC6,0x7C,0xC6,0xC6,0x7C,0x00 },
    { 0x7C,0xC6,0xC6,0x7E,0x06,0x0C,0x78,0x00 },
    { 0x00,0x18,0x18,0x00,0x00,0x18,0x18,0x00 },
    { 0x00,0x18,0x18,0x00,0x00,0x18,0x18,0x30 },
    { 0x0C,0x18,0x30,0x60,0x30,0x18,0x0C,0x00 },
    { 0x00,0x00,0x7E,0x00,0x7E,0x00,0x00,0x00 },
    { 0x60,0x30,0x18,0x0C,0x18,0x30,0x60,0x00 },
    { 0x7C,0xC6,0x0C,0x18,0x18,0x00,0x18,0x00 },
    { 0x7C,0xC6,0xDE,0xDE,0xDC,0xC0,0x7C,0x00 },
    { 0x38,0x6C,0xC6,0xC6,0xFE,0xC6,0xC6,0x00 },
    { 0xFC,0xC6,0xC6,0xFC,0xC6,0xC6,0xFC,0x00 },
    { 0x7C,0xC6,0xC0,0xC0,0xC0,0xC6,0x7C,0x00 },
    { 0xF8,0xCC,0xC6,0xC6,0xC6,0xCC,0xF8,0x00 },
    { 0xFE,0xC0,0xC0,0xFC,0xC0,0xC0,0xFE,0x00 },
    { 0xFE,0xC0,0xC0,0xFC,0xC0,0xC0,0xC0,0x00 },
    { 0x7C,0xC6,0xC0,0xCE,0xC6,0xC6,0x7E,0x00 },
    { 0xC6,0xC6,0xC6,0xFE,0xC6,0xC6,0xC6,0x00 },
    { 0x3C,0x18,0x18,0x18,0x18,0x18,0x3C,0x00 },
    { 0x1E,0x0C,0x0C,0x0C,0xCC,0xCC,0x78,0x00 },
    { 0xC6,0xCC,0xD8,0xF0,0xD8,0xCC,0xC6,0x00 },
    { 0xC0,0xC0,0xC0,0xC0,0xC0,0xC0,0xFE,0x00 },
    { 0xC6,0xEE,0xFE,0xD6,0xC6,0xC6,0xC6,0x00 },
    { 0xC6,0xE6,0xF6,0xDE,0xCE,0xC6,0xC6,0x00 },
    { 0x7C,0xC6,0xC6,0xC6,0xC6,0xC6,0x7C,0x00 },
    { 0xFC,0xC6,0xC6,0xFC,0xC0,0xC0,0xC0,0x00 },
    { 0x7C,0xC6,0xC6,0xC6,0xD6,0xDE,0x7C,0x06 },
    { 0xFC,0xC6,0xC6,0xFC,0xD8,0xCC,0xC6,0x00 },
    { 0x7C,0xC6,0xC0,0x7C,0x06,0xC6,0x7C,0x00 },
    { 0x7E,0x18,0x18,0x18,0x18,0x18,0x18,0x00 },
    { 0xC6,0xC6,0xC6,0xC6,0xC6,0xC6,0x7C,0x00 },
    { 0xC6,0xC6,0xC6,0xC6,0x6C,0x38,0x10,0x00 },
    { 0xC6,0xC6,0xC6,0xD6,0xFE,0xEE,0xC6,0x00 },
    { 0xC6,0xC6,0x6C,0x38,0x6C,0xC6,0xC6,0x00 },
    { 0x66,0x66,0x66,0x3C,0x18,0x18,0x18,0x00 },
    { 0xFE,0x06,0x0C,0x18,0x30,0x60,0xFE,0x00 },
    { 0x3C,0x30,0x30,0x30,0x30,0x30,0x3C,0x00 },
    { 0xC0,0x60,0x30,0x18,0x0C,0x06,0x02,0x00 },
    { 0x3C,0x0C,0x0C,0x0C,0x0C,0x0C,0x3C,0x00 },
    { 0x10,0x38,0x6C,0xC6,0x00,0x00,0x00,0x00 },
    { 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFE },
    { 0x30,0x18,0x0C,0x00,0x00,0x00,0x00,0x00 },
    { 0x00,0x00,0x78,0x0C,0x7C,0xCC,0x76,0x00 },
    { 0xC0,0xC0,0xFC,0xC6,0xC6,0xC6,0xFC,0x00 },
    { 0x00,0x00,0x7C,0xC6,0xC0,0xC6,0x7C,0x00 },
    { 0x06,0x06,0x7E,0xC6,0xC6,0xC6,0x7E,0x00 },
    { 0x00,0x00,0x7C,0xC6,0xFE,0xC0,0x7C,0x00 },
    { 0x1C,0x36,0x30,0x7C,0x30,0x30,0x30,0x00 },
    { 0x00,0x00,0x7E,0xC6,0xC6,0x7E,0x06,0x7C },
    { 0xC0,0xC0,0xFC,0xC6,0xC6,0xC6,0xC6,0x00 },
    { 0x18,0x00,0x38,0x18,0x18,0x18,0x3C,0x00 },
    { 0x0C,0x00,0x1C,0x0C,0x0C,0x0C,0xCC,0x78 },
    { 0xC0,0xC0,0xCC,0xD8,0xF0,0xD8,0xCC,0x00 },
    { 0x38,0x18,0x18,0x18,0x18,0x18,0x3C,0x00 },
    { 0x00,0x00,0xCC,0xFE,0xD6,0xC6,0xC6,0x00 },
    { 0x00,0x00,0xFC,0xC6,0xC6,0xC6,0xC6,0x00 },
    { 0x00,0x00,0x7C,0xC6,0xC6,0xC6,0x7C,0x00 },
    { 0x00,0x00,0xFC,0xC6,0xC6,0xFC,0xC0,0xC0 },
    { 0x00,0x00,0x7E,0xC6,0xC6,0x7E,0x06,0x06 },
    { 0x00,0x00,0xDC,0xE6,0xC0,0xC0,0xC0,0x00 },
    { 0x00,0x00,0x7E,0xC0,0x7C,0x06,0xFC,0x00 },
    { 0x30,0x30,0x7C,0x30,0x30,0x36,0x1C,0x00 },
    { 0x00,0x00,0xC6,0xC6,0xC6,0xC6,0x7E,0x00 },
    { 0x00,0x00,0xC6,0xC6,0xC6,0x6C,0x38,0x00 },
    { 0x00,0x00,0xC6,0xC6,0xD6,0xFE,0x6C,0x00 },
    { 0x00,0x00,0xC6,0x6C,0x38,0x6C,0xC6,0x00 },
    { 0x00,0x00,0xC6,0xC6,0xC6,0x7E,0x06,0x7C },
    { 0x00,0x00,0xFE,0x0C,0x38,0x60,0xFE,0x00 },
    { 0x0E,0x18,0x18,0x70,0x18,0x18,0x0E,0x00 },
    { 0x18,0x18,0x18,0x00,0x18,0x18,0x18,0x00 },
    { 0x70,0x18,0x18,0x0E,0x18,0x18,0x70,0x00 },
    { 0x76,0xDC,0x00,0x00,0x00,0x00,0x00,0x00 }
};

#ifdef GAMELIB_SDL_IMPLEMENTATION

#if defined(__has_include)
#if __has_include(<SDL.h>)
#include <SDL.h>
#elif __has_include(<SDL2/SDL.h>)
#include <SDL2/SDL.h>
#else
#error SDL2 headers not found. Install SDL2 and add its include path.
#endif
#else
#include <SDL.h>
#endif

#if GAMELIB_SDL_DISABLE_IMAGE
#define GAMELIB_SDL_HAS_IMAGE 0
#elif defined(__has_include)
#if __has_include(<SDL_image.h>)
#include <SDL_image.h>
#define GAMELIB_SDL_HAS_IMAGE 1
#elif __has_include(<SDL2/SDL_image.h>)
#include <SDL2/SDL_image.h>
#define GAMELIB_SDL_HAS_IMAGE 1
#else
#define GAMELIB_SDL_HAS_IMAGE 0
#endif
#else
#define GAMELIB_SDL_HAS_IMAGE 0
#endif

#if GAMELIB_SDL_DISABLE_TTF
#define GAMELIB_SDL_HAS_TTF 0
#elif defined(__has_include)
#if __has_include(<SDL_ttf.h>)
#include <SDL_ttf.h>
#define GAMELIB_SDL_HAS_TTF 1
#elif __has_include(<SDL2/SDL_ttf.h>)
#include <SDL2/SDL_ttf.h>
#define GAMELIB_SDL_HAS_TTF 1
#else
#define GAMELIB_SDL_HAS_TTF 0
#endif
#else
#define GAMELIB_SDL_HAS_TTF 0
#endif

#if GAMELIB_SDL_DISABLE_MIXER
#define GAMELIB_SDL_HAS_MIXER 0
#elif defined(__has_include)
#if __has_include(<SDL_mixer.h>)
#include <SDL_mixer.h>
#define GAMELIB_SDL_HAS_MIXER 1
#elif __has_include(<SDL2/SDL_mixer.h>)
#include <SDL2/SDL_mixer.h>
#define GAMELIB_SDL_HAS_MIXER 1
#else
#define GAMELIB_SDL_HAS_MIXER 0
#endif
#else
#define GAMELIB_SDL_HAS_MIXER 0
#endif

bool GameLib::_srandDone = false;

static int _gamelib_floor_div(int value, int divisor)
{
    if (divisor <= 0) return 0;
    int q = value / divisor;
    int r = value % divisor;
    if (r != 0 && ((r > 0) != (divisor > 0))) q--;
    return q;
}

static void _gamelib_generate_beep_pcm(std::vector<int16_t> &samples,
                                       int sampleRate, int channels,
                                       int frequency, int durationMs)
{
    samples.clear();
    if (sampleRate <= 0 || channels <= 0 || durationMs <= 0) return;

    if (frequency < 37) frequency = 37;
    int nyquistLimit = sampleRate / 2 - 1;
    if (nyquistLimit < 37) nyquistLimit = 37;
    if (frequency > nyquistLimit) frequency = nyquistLimit;

    int sampleCount = (durationMs * sampleRate) / 1000;
    if (sampleCount <= 0) sampleCount = 1;

    samples.resize((size_t)sampleCount * (size_t)channels);

    const double pi = 3.14159265358979323846;
    const double phaseStep = 2.0 * pi * (double)frequency / (double)sampleRate;
    const double amplitude = 12000.0;

    int edgeSamples = sampleRate / 200; // About 5ms fade-in/out to reduce clicks.
    if (edgeSamples < 1) edgeSamples = 1;
    if (edgeSamples * 2 > sampleCount) edgeSamples = sampleCount / 2;

    double phase = 0.0;
    for (int i = 0; i < sampleCount; i++) {
        double envelope = 1.0;
        if (edgeSamples > 0) {
            if (i < edgeSamples) {
                envelope = (double)(i + 1) / (double)edgeSamples;
            } else if (i >= sampleCount - edgeSamples) {
                envelope = (double)(sampleCount - i) / (double)edgeSamples;
            }
        }

        int16_t value = (int16_t)(sin(phase) * amplitude * envelope);
        phase += phaseStep;

        size_t base = (size_t)i * (size_t)channels;
        for (int ch = 0; ch < channels; ch++) {
            samples[base + (size_t)ch] = value;
        }
    }
}

static int _gamelib_sdl_map_scancode(SDL_Scancode scancode)
{
    switch (scancode) {
    case SDL_SCANCODE_LEFT: return KEY_LEFT;
    case SDL_SCANCODE_RIGHT: return KEY_RIGHT;
    case SDL_SCANCODE_UP: return KEY_UP;
    case SDL_SCANCODE_DOWN: return KEY_DOWN;
    case SDL_SCANCODE_SPACE: return KEY_SPACE;
    case SDL_SCANCODE_RETURN:
    case SDL_SCANCODE_KP_ENTER: return KEY_ENTER;
    case SDL_SCANCODE_ESCAPE: return KEY_ESCAPE;
    case SDL_SCANCODE_TAB: return KEY_TAB;
    case SDL_SCANCODE_LSHIFT:
    case SDL_SCANCODE_RSHIFT: return KEY_SHIFT;
    case SDL_SCANCODE_LCTRL:
    case SDL_SCANCODE_RCTRL: return KEY_CONTROL;
    case SDL_SCANCODE_BACKSPACE: return KEY_BACK;
    case SDL_SCANCODE_A: return KEY_A;
    case SDL_SCANCODE_B: return KEY_B;
    case SDL_SCANCODE_C: return KEY_C;
    case SDL_SCANCODE_D: return KEY_D;
    case SDL_SCANCODE_E: return KEY_E;
    case SDL_SCANCODE_F: return KEY_F;
    case SDL_SCANCODE_G: return KEY_G;
    case SDL_SCANCODE_H: return KEY_H;
    case SDL_SCANCODE_I: return KEY_I;
    case SDL_SCANCODE_J: return KEY_J;
    case SDL_SCANCODE_K: return KEY_K;
    case SDL_SCANCODE_L: return KEY_L;
    case SDL_SCANCODE_M: return KEY_M;
    case SDL_SCANCODE_N: return KEY_N;
    case SDL_SCANCODE_O: return KEY_O;
    case SDL_SCANCODE_P: return KEY_P;
    case SDL_SCANCODE_Q: return KEY_Q;
    case SDL_SCANCODE_R: return KEY_R;
    case SDL_SCANCODE_S: return KEY_S;
    case SDL_SCANCODE_T: return KEY_T;
    case SDL_SCANCODE_U: return KEY_U;
    case SDL_SCANCODE_V: return KEY_V;
    case SDL_SCANCODE_W: return KEY_W;
    case SDL_SCANCODE_X: return KEY_X;
    case SDL_SCANCODE_Y: return KEY_Y;
    case SDL_SCANCODE_Z: return KEY_Z;
    case SDL_SCANCODE_0: return KEY_0;
    case SDL_SCANCODE_1: return KEY_1;
    case SDL_SCANCODE_2: return KEY_2;
    case SDL_SCANCODE_3: return KEY_3;
    case SDL_SCANCODE_4: return KEY_4;
    case SDL_SCANCODE_5: return KEY_5;
    case SDL_SCANCODE_6: return KEY_6;
    case SDL_SCANCODE_7: return KEY_7;
    case SDL_SCANCODE_8: return KEY_8;
    case SDL_SCANCODE_9: return KEY_9;
    case SDL_SCANCODE_F1: return KEY_F1;
    case SDL_SCANCODE_F2: return KEY_F2;
    case SDL_SCANCODE_F3: return KEY_F3;
    case SDL_SCANCODE_F4: return KEY_F4;
    case SDL_SCANCODE_F5: return KEY_F5;
    case SDL_SCANCODE_F6: return KEY_F6;
    case SDL_SCANCODE_F7: return KEY_F7;
    case SDL_SCANCODE_F8: return KEY_F8;
    case SDL_SCANCODE_F9: return KEY_F9;
    case SDL_SCANCODE_F10: return KEY_F10;
    case SDL_SCANCODE_F11: return KEY_F11;
    case SDL_SCANCODE_F12: return KEY_F12;
    default: return -1;
    }
}

GameLib::GameLib()
{
    _window = NULL;
    _renderer = NULL;
    _frameTexture = NULL;
    _closing = false;
    _active = true;
    _showFps = false;
    _focused = true;
    _minimized = false;
    _sdlReady = false;
    _imageReady = false;
    _ttfReady = false;
    _mixerReady = false;
    _audioReady = false;
    _title = "";
    _width = 0;
    _height = 0;
    _framebuffer = NULL;
    memset(_keys, 0, sizeof(_keys));
    memset(_keys_prev, 0, sizeof(_keys_prev));
    _mouseX = 0;
    _mouseY = 0;
    memset(_mouseButtons, 0, sizeof(_mouseButtons));
    memset(_mouseButtons_prev, 0, sizeof(_mouseButtons_prev));
    _mouseWheelDelta = 0;
    _timeStartCounter = 0;
    _timePrevCounter = 0;
    _fpsTimeCounter = 0;
    _perfFrequency = 0;
    _deltaTime = 0.0f;
    _fps = 0.0f;
    _fpsAccum = 0.0f;
    _currentWav = NULL;
    _wavChannel = 0;
    _currentMusic = NULL;
    _musicPlaying = false;

    if (!_srandDone) {
        srand((unsigned int)time(NULL));
        _srandDone = true;
    }
}

GameLib::~GameLib()
{
    StopMusic();
    StopWAV();

#if GAMELIB_SDL_HAS_TTF
    for (size_t i = 0; i < _fontCache.size(); i++) {
        if (_fontCache[i].font) {
            TTF_CloseFont(_fontCache[i].font);
            _fontCache[i].font = NULL;
        }
    }
#endif
    _fontCache.clear();

    for (size_t i = 0; i < _sprites.size(); i++) {
        if (_sprites[i].pixels) {
            free(_sprites[i].pixels);
            _sprites[i].pixels = NULL;
        }
        _sprites[i].used = false;
    }
    for (size_t i = 0; i < _tilemaps.size(); i++) {
        if (_tilemaps[i].tiles) {
            free(_tilemaps[i].tiles);
            _tilemaps[i].tiles = NULL;
        }
        _tilemaps[i].used = false;
    }

    _DestroyWindowResources();

#if GAMELIB_SDL_HAS_IMAGE
    if (_imageReady) {
        IMG_Quit();
        _imageReady = false;
    }
#endif

#if GAMELIB_SDL_HAS_TTF
    if (_ttfReady) {
        TTF_Quit();
        _ttfReady = false;
    }
#endif

#if GAMELIB_SDL_HAS_MIXER
    if (_mixerReady) {
        Mix_CloseAudio();
        Mix_Quit();
        _mixerReady = false;
    }
#endif

    if (_audioReady) {
        SDL_QuitSubSystem(SDL_INIT_AUDIO);
        _audioReady = false;
    }
    if (_sdlReady) {
        SDL_QuitSubSystem(SDL_INIT_VIDEO | SDL_INIT_TIMER);
        _sdlReady = false;
    }
}

void GameLib::_DestroyWindowResources()
{
    if (_frameTexture) {
        SDL_DestroyTexture(_frameTexture);
        _frameTexture = NULL;
    }
    if (_renderer) {
        SDL_DestroyRenderer(_renderer);
        _renderer = NULL;
    }
    if (_window) {
        SDL_DestroyWindow(_window);
        _window = NULL;
    }
    if (_framebuffer) {
        free(_framebuffer);
        _framebuffer = NULL;
    }
    _width = 0;
    _height = 0;
}

bool GameLib::_EnsureImageReady()
{
#if GAMELIB_SDL_HAS_IMAGE
    if (_imageReady) return true;
    int flags = 0;
#ifdef IMG_INIT_PNG
    flags |= IMG_INIT_PNG;
#endif
#ifdef IMG_INIT_JPG
    flags |= IMG_INIT_JPG;
#endif
#ifdef IMG_INIT_TIF
    flags |= IMG_INIT_TIF;
#endif
#ifdef IMG_INIT_WEBP
    flags |= IMG_INIT_WEBP;
#endif
    if (flags != 0) IMG_Init(flags);
    _imageReady = true;
    return true;
#else
    return false;
#endif
}

bool GameLib::_EnsureTtfReady()
{
#if GAMELIB_SDL_HAS_TTF
    if (_ttfReady) return true;
    if (TTF_Init() != 0) return false;
    _ttfReady = true;
    return true;
#else
    return false;
#endif
}

bool GameLib::_EnsureMixerReady()
{
#if GAMELIB_SDL_HAS_MIXER
    if (_mixerReady) return true;
    if (!_audioReady) {
        if (SDL_InitSubSystem(SDL_INIT_AUDIO) != 0) return false;
        _audioReady = true;
    }
    int flags = 0;
#ifdef MIX_INIT_OGG
    flags |= MIX_INIT_OGG;
#endif
#ifdef MIX_INIT_MP3
    flags |= MIX_INIT_MP3;
#endif
#ifdef MIX_INIT_FLAC
    flags |= MIX_INIT_FLAC;
#endif
#ifdef MIX_INIT_MID
    flags |= MIX_INIT_MID;
#endif
    if (flags != 0) Mix_Init(flags);
    if (Mix_OpenAudio(44100, AUDIO_S16SYS, 2, 1024) != 0) return false;
    Mix_AllocateChannels(8);
    _wavChannel = 0;
    _mixerReady = true;
    return true;
#else
    return false;
#endif
}

#if GAMELIB_SDL_HAS_TTF

static char _gamelib_ascii_tolower(char ch)
{
    if (ch >= 'A' && ch <= 'Z') return (char)(ch - 'A' + 'a');
    return ch;
}

static std::string _gamelib_normalize_font_name(const char *text)
{
    std::string normalized;
    if (!text) return normalized;

    for (const unsigned char *p = (const unsigned char*)text; *p; p++) {
        unsigned char ch = *p;
        if (ch >= 'A' && ch <= 'Z') {
            normalized.push_back((char)(ch - 'A' + 'a'));
        } else if ((ch >= 'a' && ch <= 'z') || (ch >= '0' && ch <= '9')) {
            normalized.push_back((char)ch);
        }
    }
    return normalized;
}

static bool _gamelib_has_font_extension(const char *text)
{
    if (!text) return false;
    const char *dot = strrchr(text, '.');
    if (!dot || !dot[1]) return false;

    std::string ext;
    for (const char *p = dot + 1; *p; p++) {
        ext.push_back(_gamelib_ascii_tolower(*p));
    }
    return ext == "ttf" || ext == "ttc" || ext == "otf" || ext == "otc";
}

static bool _gamelib_is_path_like_font_name(const char *text)
{
    if (!text || !text[0]) return false;
    for (const char *p = text; *p; p++) {
        if (*p == '/' || *p == '\\' || *p == ':') return true;
    }
    return _gamelib_has_font_extension(text);
}

static void _gamelib_push_font_candidate(std::vector<std::string> &candidates, const std::string &value)
{
    if (value.empty()) return;
    for (size_t i = 0; i < candidates.size(); i++) {
        if (candidates[i] == value) return;
    }
    candidates.push_back(value);
}

static bool _gamelib_font_file_exists(const std::string &path)
{
    if (path.empty()) return false;
    SDL_RWops *rw = SDL_RWFromFile(path.c_str(), "rb");
    if (!rw) return false;
    SDL_RWclose(rw);
    return true;
}

static void _gamelib_push_joined_font_candidate(std::vector<std::string> &candidates,
                                                const std::string &dir,
                                                const char *fileName)
{
    if (!fileName || !fileName[0]) return;
    if (dir.empty()) {
        _gamelib_push_font_candidate(candidates, fileName);
        return;
    }

    std::string path = dir;
    char tail = path[path.size() - 1];
    if (tail != '/' && tail != '\\') path += '/';
    path += fileName;
    _gamelib_push_font_candidate(candidates, path);
}

static void _gamelib_append_platform_font_candidates(std::vector<std::string> &candidates,
                                                     const char *fontName)
{
    std::string normalized = _gamelib_normalize_font_name(fontName);

    if (fontName && fontName[0] && _gamelib_is_path_like_font_name(fontName)) {
        _gamelib_push_font_candidate(candidates, fontName);
    }

#if defined(_WIN32)
    std::string fontDir = "C:/Windows/Fonts";
    const char *winDir = SDL_getenv("WINDIR");
    if (winDir && winDir[0]) fontDir = std::string(winDir) + "/Fonts";

    if (normalized == "microsoftyahei" || normalized == "yahei" || normalized == "default") {
        _gamelib_push_joined_font_candidate(candidates, fontDir, "msyh.ttc");
        _gamelib_push_joined_font_candidate(candidates, fontDir, "msyh.ttf");
        _gamelib_push_joined_font_candidate(candidates, fontDir, "msyhbd.ttc");
    }
    if (normalized == "simhei" || normalized == "heiti") {
        _gamelib_push_joined_font_candidate(candidates, fontDir, "simhei.ttf");
    }
    if (normalized == "arial") {
        _gamelib_push_joined_font_candidate(candidates, fontDir, "arial.ttf");
    }
    if (normalized == "timesnewroman" || normalized == "times") {
        _gamelib_push_joined_font_candidate(candidates, fontDir, "times.ttf");
    }
    if (normalized == "couriernew" || normalized == "courier" || normalized == "monospace") {
        _gamelib_push_joined_font_candidate(candidates, fontDir, "cour.ttf");
    }
    if (normalized == "msgothic" || normalized == "gothic") {
        _gamelib_push_joined_font_candidate(candidates, fontDir, "msgothic.ttc");
        _gamelib_push_joined_font_candidate(candidates, fontDir, "msgothic.ttf");
    }
    if (normalized == "meiryo") {
        _gamelib_push_joined_font_candidate(candidates, fontDir, "meiryo.ttc");
        _gamelib_push_joined_font_candidate(candidates, fontDir, "meiryo.ttf");
    }

    _gamelib_push_joined_font_candidate(candidates, fontDir, "msyh.ttc");
    _gamelib_push_joined_font_candidate(candidates, fontDir, "segoeui.ttf");
    _gamelib_push_joined_font_candidate(candidates, fontDir, "arial.ttf");
    _gamelib_push_joined_font_candidate(candidates, fontDir, "tahoma.ttf");
    _gamelib_push_joined_font_candidate(candidates, fontDir, "simhei.ttf");
    _gamelib_push_joined_font_candidate(candidates, fontDir, "msgothic.ttc");
    _gamelib_push_joined_font_candidate(candidates, fontDir, "meiryo.ttc");

#elif defined(__APPLE__)
    if (normalized == "pingfang" || normalized == "pingfangsc" || normalized == "sans" || normalized == "default") {
        _gamelib_push_font_candidate(candidates, "/System/Library/Fonts/PingFang.ttc");
    }
    if (normalized == "arial") {
        _gamelib_push_font_candidate(candidates, "/System/Library/Fonts/Supplemental/Arial.ttf");
        _gamelib_push_font_candidate(candidates, "/Library/Fonts/Arial.ttf");
    }
    if (normalized == "timesnewroman" || normalized == "times" || normalized == "serif") {
        _gamelib_push_font_candidate(candidates, "/System/Library/Fonts/Supplemental/Times New Roman.ttf");
        _gamelib_push_font_candidate(candidates, "/Library/Fonts/Times New Roman.ttf");
    }
    if (normalized == "couriernew" || normalized == "courier" || normalized == "monospace") {
        _gamelib_push_font_candidate(candidates, "/System/Library/Fonts/Supplemental/Courier New.ttf");
        _gamelib_push_font_candidate(candidates, "/Library/Fonts/Courier New.ttf");
    }

    _gamelib_push_font_candidate(candidates, "/System/Library/Fonts/PingFang.ttc");
    _gamelib_push_font_candidate(candidates, "/System/Library/Fonts/Helvetica.ttc");
    _gamelib_push_font_candidate(candidates, "/System/Library/Fonts/Supplemental/Arial.ttf");
    _gamelib_push_font_candidate(candidates, "/System/Library/Fonts/Supplemental/Courier New.ttf");

#else
    if (normalized == "arial" || normalized == "sans" || normalized == "default" || normalized == "dejavusans") {
        _gamelib_push_font_candidate(candidates, "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf");
        _gamelib_push_font_candidate(candidates, "/usr/share/fonts/truetype/liberation2/LiberationSans-Regular.ttf");
        _gamelib_push_font_candidate(candidates, "/usr/share/fonts/truetype/freefont/FreeSans.ttf");
    }
    if (normalized == "timesnewroman" || normalized == "times" || normalized == "serif") {
        _gamelib_push_font_candidate(candidates, "/usr/share/fonts/truetype/dejavu/DejaVuSerif.ttf");
        _gamelib_push_font_candidate(candidates, "/usr/share/fonts/truetype/liberation2/LiberationSerif-Regular.ttf");
    }
    if (normalized == "couriernew" || normalized == "courier" || normalized == "monospace") {
        _gamelib_push_font_candidate(candidates, "/usr/share/fonts/truetype/dejavu/DejaVuSansMono.ttf");
        _gamelib_push_font_candidate(candidates, "/usr/share/fonts/truetype/liberation2/LiberationMono-Regular.ttf");
    }
    if (normalized == "microsoftyahei" || normalized == "simhei" || normalized == "msgothic" ||
        normalized == "meiryo" || normalized == "notosanscjk" || normalized == "notosanscjksc") {
        _gamelib_push_font_candidate(candidates, "/usr/share/fonts/opentype/noto/NotoSansCJK-Regular.ttc");
        _gamelib_push_font_candidate(candidates, "/usr/share/fonts/opentype/noto/NotoSansCJK-Regular.otf");
        _gamelib_push_font_candidate(candidates, "/usr/share/fonts/truetype/wqy/wqy-zenhei.ttc");
        _gamelib_push_font_candidate(candidates, "/usr/share/fonts/truetype/wqy/wqy-zenhei.ttf");
    }

    _gamelib_push_font_candidate(candidates, "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf");
    _gamelib_push_font_candidate(candidates, "/usr/share/fonts/truetype/dejavu/DejaVuSansMono.ttf");
    _gamelib_push_font_candidate(candidates, "/usr/share/fonts/truetype/liberation2/LiberationSans-Regular.ttf");
    _gamelib_push_font_candidate(candidates, "/usr/share/fonts/truetype/freefont/FreeSans.ttf");
    _gamelib_push_font_candidate(candidates, "/usr/share/fonts/opentype/noto/NotoSans-Regular.ttf");
    _gamelib_push_font_candidate(candidates, "/usr/share/fonts/opentype/noto/NotoSansCJK-Regular.ttc");
#endif
}

static std::string _gamelib_resolve_font_path(const char *fontName)
{
    std::vector<std::string> candidates;

    if (fontName && fontName[0]) {
        _gamelib_append_platform_font_candidates(candidates, fontName);
    }

    if (GAMELIB_SDL_DEFAULT_FONT[0]) {
        if (!fontName || strcmp(fontName, GAMELIB_SDL_DEFAULT_FONT) != 0) {
            _gamelib_append_platform_font_candidates(candidates, GAMELIB_SDL_DEFAULT_FONT);
        }
    }

    _gamelib_append_platform_font_candidates(candidates, "default");

    for (size_t i = 0; i < candidates.size(); i++) {
        if (_gamelib_font_file_exists(candidates[i])) return candidates[i];
    }
    return "";
}

#endif

TTF_Font *GameLib::_GetFont(const char *fontName, int fontSize)
{
#if GAMELIB_SDL_HAS_TTF
    if (fontSize <= 0) return NULL;
    if (!_EnsureTtfReady()) return NULL;

    std::string resolved = _gamelib_resolve_font_path(fontName);
    if (resolved.empty()) return NULL;

    char sizeBuf[32];
    snprintf(sizeBuf, sizeof(sizeBuf), "%d", fontSize);
    std::string key = resolved + "#" + sizeBuf;
    for (size_t i = 0; i < _fontCache.size(); i++) {
        if (_fontCache[i].key == key) return _fontCache[i].font;
    }

    TTF_Font *font = TTF_OpenFont(resolved.c_str(), fontSize);
    if (!font) return NULL;

    GameFontCacheEntry entry;
    entry.key = key;
    entry.font = font;
    _fontCache.push_back(entry);
    return font;
#else
    (void)fontName;
    (void)fontSize;
    return NULL;
#endif
}

void GameLib::_BlendSurfaceToFramebuffer(int x, int y, SDL_Surface *surface)
{
    if (!_framebuffer || !surface) return;

    int x0 = x;
    int y0 = y;
    int x1 = x + surface->w;
    int y1 = y + surface->h;
    if (x0 < 0) x0 = 0;
    if (y0 < 0) y0 = 0;
    if (x1 > _width) x1 = _width;
    if (y1 > _height) y1 = _height;
    if (x0 >= x1 || y0 >= y1) return;

    for (int py = y0; py < y1; py++) {
        const uint32_t *srcRow = (const uint32_t*)((const unsigned char*)surface->pixels + (py - y) * surface->pitch);
        uint32_t *dstRow = _framebuffer + py * _width;
        for (int px = x0; px < x1; px++) {
            uint32_t c = srcRow[px - x];
            uint32_t sa = COLOR_GET_A(c);
            if (sa == 0) continue;
            if (sa == 255) {
                dstRow[px] = c;
            } else {
                uint32_t dc = dstRow[px];
                uint32_t ia = 255 - sa;
                uint32_t or_ = (sa * COLOR_GET_R(c) + ia * COLOR_GET_R(dc)) / 255;
                uint32_t og = (sa * COLOR_GET_G(c) + ia * COLOR_GET_G(dc)) / 255;
                uint32_t ob = (sa * COLOR_GET_B(c) + ia * COLOR_GET_B(dc)) / 255;
                dstRow[px] = COLOR_ARGB(255, or_, og, ob);
            }
        }
    }
}

void GameLib::_UpdateTitleFps()
{
    if (!_window) return;
    if (_showFps) {
        char buf[512];
        snprintf(buf, sizeof(buf), "%s (FPS: %.1f)", _title.c_str(), _fps);
        buf[sizeof(buf) - 1] = '\0';
        SDL_SetWindowTitle(_window, buf);
    } else {
        SDL_SetWindowTitle(_window, _title.c_str());
    }
}

void GameLib::_SyncInputState()
{
    const Uint8 *state = SDL_GetKeyboardState(NULL);
    memset(_keys, 0, sizeof(_keys));

    for (int sc = (int)SDL_SCANCODE_UNKNOWN; sc < (int)SDL_NUM_SCANCODES; sc++) {
        int key = _gamelib_sdl_map_scancode((SDL_Scancode)sc);
        if (key >= 0) {
            _keys[key & 511] = state[sc] ? 1 : 0;
        }
    }

    int mx = 0, my = 0;
    Uint32 mouseState = SDL_GetMouseState(&mx, &my);
    _mouseX = mx;
    _mouseY = my;
    _mouseButtons[MOUSE_LEFT] = (mouseState & SDL_BUTTON(SDL_BUTTON_LEFT)) ? 1 : 0;
    _mouseButtons[MOUSE_RIGHT] = (mouseState & SDL_BUTTON(SDL_BUTTON_RIGHT)) ? 1 : 0;
    _mouseButtons[MOUSE_MIDDLE] = (mouseState & SDL_BUTTON(SDL_BUTTON_MIDDLE)) ? 1 : 0;
}

int GameLib::Open(int width, int height, const char *title, bool center)
{
    if (width <= 0 || height <= 0 || width > 16384 || height > 16384) return -9;

    if (!_sdlReady) {
        SDL_SetMainReady();
#if defined(_WIN32)
#ifdef SDL_HINT_WINDOWS_DPI_AWARENESS
    SDL_SetHint(SDL_HINT_WINDOWS_DPI_AWARENESS, GAMELIB_SDL_WINDOWS_DPI_AWARENESS);
#else
    SDL_SetHint("SDL_WINDOWS_DPI_AWARENESS", GAMELIB_SDL_WINDOWS_DPI_AWARENESS);
#endif
#endif
        if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0) return -1;
        _sdlReady = true;
    }

    _DestroyWindowResources();

    _width = width;
    _height = height;
    _title = title ? title : "";
    _closing = false;
    _focused = true;
    _minimized = false;
    _active = true;

    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0");

    int pos = center ? SDL_WINDOWPOS_CENTERED : SDL_WINDOWPOS_UNDEFINED;
    _window = SDL_CreateWindow(_title.c_str(), pos, pos, width, height, SDL_WINDOW_SHOWN);
    if (!_window) {
        _DestroyWindowResources();
        return -2;
    }

    _renderer = SDL_CreateRenderer(_window, -1, SDL_RENDERER_ACCELERATED);
    if (!_renderer) {
        _renderer = SDL_CreateRenderer(_window, -1, SDL_RENDERER_SOFTWARE);
    }
    if (!_renderer) {
        _DestroyWindowResources();
        return -3;
    }
    SDL_SetRenderDrawColor(_renderer, 0, 0, 0, 255);

    _frameTexture = SDL_CreateTexture(_renderer, SDL_PIXELFORMAT_ARGB8888,
                                      SDL_TEXTUREACCESS_STREAMING, width, height);
    if (!_frameTexture) {
        _DestroyWindowResources();
        return -4;
    }

    _framebuffer = (uint32_t*)malloc((size_t)width * height * sizeof(uint32_t));
    if (!_framebuffer) {
        _DestroyWindowResources();
        return -5;
    }
    memset(_framebuffer, 0, (size_t)width * height * sizeof(uint32_t));

    memset(_keys, 0, sizeof(_keys));
    memset(_keys_prev, 0, sizeof(_keys_prev));
    memset(_mouseButtons, 0, sizeof(_mouseButtons));
    memset(_mouseButtons_prev, 0, sizeof(_mouseButtons_prev));
    _mouseWheelDelta = 0;

    _perfFrequency = (uint64_t)SDL_GetPerformanceFrequency();
    if (_perfFrequency == 0) _perfFrequency = 1;
    _timeStartCounter = (uint64_t)SDL_GetPerformanceCounter();
    _timePrevCounter = _timeStartCounter;
    _fpsTimeCounter = _timeStartCounter;
    _deltaTime = 0.0f;
    _fps = 0.0f;
    _fpsAccum = 0.0f;
    _SyncInputState();
    _UpdateTitleFps();

    return 0;
}

bool GameLib::IsClosed() const
{
    return _closing;
}

void GameLib::Update()
{
    if (!_window || !_renderer || !_frameTexture || !_framebuffer) return;

    memcpy(_keys_prev, _keys, sizeof(_keys));
    memcpy(_mouseButtons_prev, _mouseButtons, sizeof(_mouseButtons));
    _mouseWheelDelta = 0;

    Uint32 windowId = SDL_GetWindowID(_window);
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
        case SDL_QUIT:
            _closing = true;
            break;

        case SDL_WINDOWEVENT:
            if (event.window.windowID != windowId) break;
            switch (event.window.event) {
            case SDL_WINDOWEVENT_CLOSE:
                _closing = true;
                break;
            case SDL_WINDOWEVENT_FOCUS_GAINED:
                _focused = true;
                break;
            case SDL_WINDOWEVENT_FOCUS_LOST:
                _focused = false;
                break;
            case SDL_WINDOWEVENT_MINIMIZED:
            case SDL_WINDOWEVENT_HIDDEN:
                _minimized = true;
                break;
            case SDL_WINDOWEVENT_RESTORED:
            case SDL_WINDOWEVENT_SHOWN:
                _minimized = false;
                break;
            default:
                break;
            }
            break;

        case SDL_MOUSEWHEEL:
            if (event.wheel.windowID == windowId) {
                int dy = event.wheel.y;
#ifdef SDL_MOUSEWHEEL_FLIPPED
                if (event.wheel.direction == SDL_MOUSEWHEEL_FLIPPED) dy = -dy;
#endif
                _mouseWheelDelta += dy * 120;
            }
            break;

        default:
            break;
        }
    }

    _active = _focused && !_minimized;
    _SyncInputState();

    uint64_t now = (uint64_t)SDL_GetPerformanceCounter();
    uint64_t deltaCounter = now - _timePrevCounter;
    _deltaTime = (float)((double)deltaCounter / (double)_perfFrequency);
    _timePrevCounter = now;

    _fpsAccum += 1.0f;
    uint64_t fpsDelta = now - _fpsTimeCounter;
    if (fpsDelta >= _perfFrequency) {
        _fps = (float)((double)_fpsAccum * (double)_perfFrequency / (double)fpsDelta);
        _fpsAccum = 0.0f;
        _fpsTimeCounter = now;
        _UpdateTitleFps();
    }

    SDL_UpdateTexture(_frameTexture, NULL, _framebuffer, _width * (int)sizeof(uint32_t));
    SDL_RenderClear(_renderer);
    SDL_RenderCopy(_renderer, _frameTexture, NULL, NULL);
    SDL_RenderPresent(_renderer);
}

void GameLib::WaitFrame(int fps)
{
    if (_perfFrequency == 0) return;
    if (fps <= 0) fps = 60;

    uint64_t target = (uint64_t)((double)_perfFrequency / (double)fps);
    if (target == 0) return;

    for (;;) {
        uint64_t now = (uint64_t)SDL_GetPerformanceCounter();
        uint64_t elapsed = now - _timePrevCounter;
        if (elapsed >= target) break;

        uint64_t remaining = target - elapsed;
        double remainingMs = (double)remaining * 1000.0 / (double)_perfFrequency;
        if (remainingMs > 2.0) {
            SDL_Delay(1);
        } else {
            SDL_Delay(0);
        }
    }
}

float GameLib::GetDeltaTime() const { return _deltaTime; }
float GameLib::GetFPS() const { return _fps; }
float GameLib::GetTime() const
{
    if (_perfFrequency == 0 || _timeStartCounter == 0) return 0.0f;
    uint64_t now = (uint64_t)SDL_GetPerformanceCounter();
    return (float)((double)(now - _timeStartCounter) / (double)_perfFrequency);
}
int GameLib::GetWidth() const { return _width; }
int GameLib::GetHeight() const { return _height; }

void GameLib::SetTitle(const char *title)
{
    _title = title ? title : "";
    _UpdateTitleFps();
}

void GameLib::ShowFps(bool show)
{
    _showFps = show;
    _UpdateTitleFps();
}

void GameLib::Clear(uint32_t color)
{
    if (!_framebuffer) return;
    size_t count = (size_t)_width * _height;
    for (size_t i = 0; i < count; i++) _framebuffer[i] = color;
}

void GameLib::SetPixel(int x, int y, uint32_t color)
{
    if (!_framebuffer) return;
    if (x >= 0 && x < _width && y >= 0 && y < _height) {
        _framebuffer[y * _width + x] = color;
    }
}

uint32_t GameLib::GetPixel(int x, int y) const
{
    if (!_framebuffer) return 0;
    if (x >= 0 && x < _width && y >= 0 && y < _height) {
        return _framebuffer[y * _width + x];
    }
    return 0;
}

void GameLib::_SetPixelFast(int x, int y, uint32_t color)
{
    _framebuffer[y * _width + x] = color;
}

void GameLib::DrawLine(int x1, int y1, int x2, int y2, uint32_t color)
{
    int dx = abs(x2 - x1);
    int dy = abs(y2 - y1);
    int sx = (x1 < x2) ? 1 : -1;
    int sy = (y1 < y2) ? 1 : -1;
    int err = dx - dy;

    while (true) {
        SetPixel(x1, y1, color);
        if (x1 == x2 && y1 == y2) break;
        int e2 = err * 2;
        if (e2 > -dy) { err -= dy; x1 += sx; }
        if (e2 < dx)  { err += dx; y1 += sy; }
    }
}

void GameLib::_DrawHLine(int x1, int x2, int y, uint32_t color)
{
    if (!_framebuffer) return;
    if (y < 0 || y >= _height) return;
    if (x1 > x2) { int t = x1; x1 = x2; x2 = t; }
    if (x1 < 0) x1 = 0;
    if (x2 >= _width) x2 = _width - 1;
    for (int x = x1; x <= x2; x++) {
        _SetPixelFast(x, y, color);
    }
}

void GameLib::DrawRect(int x, int y, int w, int h, uint32_t color)
{
    if (w <= 0 || h <= 0) return;
    _DrawHLine(x, x + w - 1, y, color);
    _DrawHLine(x, x + w - 1, y + h - 1, color);
    for (int j = y + 1; j < y + h - 1; j++) {
        SetPixel(x, j, color);
        SetPixel(x + w - 1, j, color);
    }
}

void GameLib::FillRect(int x, int y, int w, int h, uint32_t color)
{
    if (!_framebuffer || w <= 0 || h <= 0) return;

    int x1 = x;
    int y1 = y;
    int x2 = x + w;
    int y2 = y + h;
    if (x1 < 0) x1 = 0;
    if (y1 < 0) y1 = 0;
    if (x2 > _width) x2 = _width;
    if (y2 > _height) y2 = _height;
    if (x1 >= x2 || y1 >= y2) return;

    for (int j = y1; j < y2; j++) {
        uint32_t *row = _framebuffer + j * _width;
        for (int i = x1; i < x2; i++) {
            row[i] = color;
        }
    }
}

void GameLib::DrawCircle(int cx, int cy, int r, uint32_t color)
{
    if (r < 0) return;
    int x = 0;
    int y = r;
    int d = 1 - r;
    while (x <= y) {
        SetPixel(cx + x, cy + y, color);
        SetPixel(cx - x, cy + y, color);
        SetPixel(cx + x, cy - y, color);
        SetPixel(cx - x, cy - y, color);
        SetPixel(cx + y, cy + x, color);
        SetPixel(cx - y, cy + x, color);
        SetPixel(cx + y, cy - x, color);
        SetPixel(cx - y, cy - x, color);
        if (d < 0) {
            d += 2 * x + 3;
        } else {
            d += 2 * (x - y) + 5;
            y--;
        }
        x++;
    }
}

void GameLib::FillCircle(int cx, int cy, int r, uint32_t color)
{
    if (r < 0) return;
    int x = 0;
    int y = r;
    int d = 1 - r;
    while (x <= y) {
        _DrawHLine(cx - x, cx + x, cy + y, color);
        _DrawHLine(cx - x, cx + x, cy - y, color);
        _DrawHLine(cx - y, cx + y, cy + x, color);
        _DrawHLine(cx - y, cx + y, cy - x, color);
        if (d < 0) {
            d += 2 * x + 3;
        } else {
            d += 2 * (x - y) + 5;
            y--;
        }
        x++;
    }
}

void GameLib::DrawTriangle(int x1, int y1, int x2, int y2, int x3, int y3, uint32_t color)
{
    DrawLine(x1, y1, x2, y2, color);
    DrawLine(x2, y2, x3, y3, color);
    DrawLine(x3, y3, x1, y1, color);
}

void GameLib::FillTriangle(int x1, int y1, int x2, int y2, int x3, int y3, uint32_t color)
{
    if (y1 > y2) { int t; t = x1; x1 = x2; x2 = t; t = y1; y1 = y2; y2 = t; }
    if (y1 > y3) { int t; t = x1; x1 = x3; x3 = t; t = y1; y1 = y3; y3 = t; }
    if (y2 > y3) { int t; t = x2; x2 = x3; x3 = t; t = y2; y2 = y3; y3 = t; }

    if (y3 == y1) {
        int minX = x1;
        int maxX = x1;
        if (x2 < minX) minX = x2;
        if (x2 > maxX) maxX = x2;
        if (x3 < minX) minX = x3;
        if (x3 > maxX) maxX = x3;
        _DrawHLine(minX, maxX, y1, color);
        return;
    }

    for (int y = y1; y <= y3; y++) {
        int xa = x1 + (int)((int64_t)(x3 - x1) * (y - y1) / (y3 - y1));
        int xb;
        if (y < y2) {
            if (y2 != y1) {
                xb = x1 + (int)((int64_t)(x2 - x1) * (y - y1) / (y2 - y1));
            } else {
                xb = x1;
            }
        } else {
            if (y3 != y2) {
                xb = x2 + (int)((int64_t)(x3 - x2) * (y - y2) / (y3 - y2));
            } else {
                xb = x2;
            }
        }
        _DrawHLine(xa, xb, y, color);
    }
}

void GameLib::DrawText(int x, int y, const char *text, uint32_t color)
{
    if (!text) return;
    int ox = x;
    for (const char *p = text; *p; p++) {
        unsigned char ch = (unsigned char)*p;
        if (ch == '\n') {
            x = ox;
            y += 10;
            continue;
        }
        if (ch < 32 || ch > 126) continue;
        const unsigned char *glyph = _gamelib_font8x8[ch - 32];
        for (int row = 0; row < 8; row++) {
            unsigned char bits = glyph[row];
            for (int col = 0; col < 8; col++) {
                if (bits & (0x80 >> col)) {
                    SetPixel(x + col, y + row, color);
                }
            }
        }
        x += 8;
    }
}

void GameLib::DrawNumber(int x, int y, int number, uint32_t color)
{
    char buf[32];
    snprintf(buf, sizeof(buf), "%d", number);
    DrawText(x, y, buf, color);
}

void GameLib::DrawTextScale(int x, int y, const char *text, uint32_t color, int scale)
{
    if (!text || scale <= 0) return;
    int ox = x;
    for (const char *p = text; *p; p++) {
        unsigned char ch = (unsigned char)*p;
        if (ch == '\n') {
            x = ox;
            y += (8 + 2) * scale;
            continue;
        }
        if (ch < 32 || ch > 126) continue;
        const unsigned char *glyph = _gamelib_font8x8[ch - 32];
        for (int row = 0; row < 8; row++) {
            unsigned char bits = glyph[row];
            for (int col = 0; col < 8; col++) {
                if (bits & (0x80 >> col)) {
                    FillRect(x + col * scale, y + row * scale, scale, scale, color);
                }
            }
        }
        x += 8 * scale;
    }
}

void GameLib::DrawPrintf(int x, int y, uint32_t color, const char *fmt, ...)
{
    char buf[1024];
    va_list args;
    va_start(args, fmt);
    vsnprintf(buf, sizeof(buf), fmt, args);
    va_end(args);
    buf[sizeof(buf) - 1] = '\0';
    DrawText(x, y, buf, color);
}

void GameLib::DrawTextFont(int x, int y, const char *text, uint32_t color, const char *fontName, int fontSize)
{
#if GAMELIB_SDL_HAS_TTF
    if (!text || fontSize <= 0) return;
    TTF_Font *font = _GetFont(fontName, fontSize);
    if (!font) return;

    int lineHeight = TTF_FontHeight(font);
    if (lineHeight <= 0) lineHeight = fontSize;

    SDL_Color sdlColor;
    sdlColor.r = (Uint8)COLOR_GET_R(color);
    sdlColor.g = (Uint8)COLOR_GET_G(color);
    sdlColor.b = (Uint8)COLOR_GET_B(color);
    sdlColor.a = (Uint8)COLOR_GET_A(color);

    std::string allText(text);
    size_t start = 0;
    int penY = y;
    while (start <= allText.size()) {
        size_t end = allText.find('\n', start);
        size_t len = (end == std::string::npos) ? (allText.size() - start) : (end - start);
        if (len > 0 && allText[start + len - 1] == '\r') len--;
        std::string line = allText.substr(start, len);

        if (!line.empty()) {
            SDL_Surface *rendered = TTF_RenderUTF8_Blended(font, line.c_str(), sdlColor);
            if (rendered) {
                SDL_Surface *argb = SDL_ConvertSurfaceFormat(rendered, SDL_PIXELFORMAT_ARGB8888, 0);
                SDL_FreeSurface(rendered);
                if (argb) {
                    _BlendSurfaceToFramebuffer(x, penY, argb);
                    if (argb->h > 0) lineHeight = argb->h;
                    SDL_FreeSurface(argb);
                }
            }
        }

        penY += lineHeight;
        if (end == std::string::npos) break;
        start = end + 1;
    }
#else
    (void)x;
    (void)y;
    (void)text;
    (void)color;
    (void)fontName;
    (void)fontSize;
#endif
}

void GameLib::DrawTextFont(int x, int y, const char *text, uint32_t color, int fontSize)
{
    DrawTextFont(x, y, text, color, GAMELIB_SDL_DEFAULT_FONT, fontSize);
}

int GameLib::GetTextWidthFont(const char *text, const char *fontName, int fontSize)
{
#if GAMELIB_SDL_HAS_TTF
    if (!text || fontSize <= 0) return 0;
    TTF_Font *font = _GetFont(fontName, fontSize);
    if (!font) return 0;

    std::string allText(text);
    size_t start = 0;
    int maxWidth = 0;
    while (start <= allText.size()) {
        size_t end = allText.find('\n', start);
        size_t len = (end == std::string::npos) ? (allText.size() - start) : (end - start);
        if (len > 0 && allText[start + len - 1] == '\r') len--;
        std::string line = allText.substr(start, len);

        int lineWidth = 0;
        if (!line.empty()) {
            TTF_SizeUTF8(font, line.c_str(), &lineWidth, NULL);
        }
        if (lineWidth > maxWidth) maxWidth = lineWidth;

        if (end == std::string::npos) break;
        start = end + 1;
    }
    return maxWidth;
#else
    (void)text;
    (void)fontName;
    (void)fontSize;
    return 0;
#endif
}

int GameLib::GetTextWidthFont(const char *text, int fontSize)
{
    return GetTextWidthFont(text, GAMELIB_SDL_DEFAULT_FONT, fontSize);
}

int GameLib::GetTextHeightFont(const char *text, const char *fontName, int fontSize)
{
#if GAMELIB_SDL_HAS_TTF
    if (!text || fontSize <= 0) return 0;
    TTF_Font *font = _GetFont(fontName, fontSize);
    if (!font) return 0;

    int lineHeight = TTF_FontHeight(font);
    if (lineHeight <= 0) lineHeight = fontSize;
    int totalHeight = 0;

    const char *cursor = text;
    for (;;) {
        totalHeight += lineHeight;
        const char *newline = strchr(cursor, '\n');
        if (!newline) break;
        cursor = newline + 1;
    }
    return totalHeight;
#else
    (void)text;
    (void)fontName;
    (void)fontSize;
    return 0;
#endif
}

int GameLib::GetTextHeightFont(const char *text, int fontSize)
{
    return GetTextHeightFont(text, GAMELIB_SDL_DEFAULT_FONT, fontSize);
}

int GameLib::_AllocSpriteSlot()
{
    for (size_t i = 0; i < _sprites.size(); i++) {
        if (!_sprites[i].used) return (int)i;
    }
    GameSprite spr;
    spr.width = 0;
    spr.height = 0;
    spr.pixels = NULL;
    spr.colorKey = COLORKEY_DEFAULT;
    spr.used = false;
    _sprites.push_back(spr);
    return (int)(_sprites.size() - 1);
}

int GameLib::CreateSprite(int width, int height)
{
    if (width <= 0 || height <= 0 || width > 16384 || height > 16384) return -1;
    int id = _AllocSpriteSlot();
    uint32_t *pixels = (uint32_t*)malloc((size_t)width * height * sizeof(uint32_t));
    if (!pixels) return -1;
    memset(pixels, 0, (size_t)width * height * sizeof(uint32_t));

    _sprites[id].width = width;
    _sprites[id].height = height;
    _sprites[id].pixels = pixels;
    _sprites[id].colorKey = COLORKEY_DEFAULT;
    _sprites[id].used = true;
    return id;
}

int GameLib::LoadSpriteBMP(const char *filename)
{
    if (!filename) return -1;

    SDL_Surface *loaded = SDL_LoadBMP(filename);
    if (!loaded) return -1;

    SDL_Surface *argb = SDL_ConvertSurfaceFormat(loaded, SDL_PIXELFORMAT_ARGB8888, 0);
    SDL_FreeSurface(loaded);
    if (!argb) return -1;

    int id = CreateSprite(argb->w, argb->h);
    if (id < 0) {
        SDL_FreeSurface(argb);
        return -1;
    }

    for (int y = 0; y < argb->h; y++) {
        const uint32_t *src = (const uint32_t*)((const unsigned char*)argb->pixels + y * argb->pitch);
        uint32_t *dst = _sprites[id].pixels + y * argb->w;
        memcpy(dst, src, (size_t)argb->w * sizeof(uint32_t));
    }

    SDL_FreeSurface(argb);
    return id;
}

int GameLib::LoadSprite(const char *filename)
{
    if (!filename) return -1;

#if GAMELIB_SDL_HAS_IMAGE
    if (_EnsureImageReady()) {
        SDL_Surface *loaded = IMG_Load(filename);
        if (loaded) {
            SDL_Surface *argb = SDL_ConvertSurfaceFormat(loaded, SDL_PIXELFORMAT_ARGB8888, 0);
            SDL_FreeSurface(loaded);
            if (!argb) return -1;

            int id = CreateSprite(argb->w, argb->h);
            if (id < 0) {
                SDL_FreeSurface(argb);
                return -1;
            }

            for (int y = 0; y < argb->h; y++) {
                const uint32_t *src = (const uint32_t*)((const unsigned char*)argb->pixels + y * argb->pitch);
                uint32_t *dst = _sprites[id].pixels + y * argb->w;
                memcpy(dst, src, (size_t)argb->w * sizeof(uint32_t));
            }

            SDL_FreeSurface(argb);
            return id;
        }
    }
#endif

    return LoadSpriteBMP(filename);
}

void GameLib::FreeSprite(int id)
{
    if (id < 0 || id >= (int)_sprites.size()) return;
    if (!_sprites[id].used) return;
    if (_sprites[id].pixels) {
        free(_sprites[id].pixels);
        _sprites[id].pixels = NULL;
    }
    _sprites[id].width = 0;
    _sprites[id].height = 0;
    _sprites[id].colorKey = COLORKEY_DEFAULT;
    _sprites[id].used = false;
}

void GameLib::_DrawSpriteAreaFast(int id, int x, int y, int sx, int sy, int sw, int sh, int flags)
{
    if (id < 0 || id >= (int)_sprites.size()) return;
    if (!_sprites[id].used || !_framebuffer) return;
    if (sw <= 0 || sh <= 0) return;

    GameSprite &spr = _sprites[id];
    bool flipH = (flags & SPRITE_FLIP_H) != 0;
    bool flipV = (flags & SPRITE_FLIP_V) != 0;
    bool useAlpha = (flags & SPRITE_ALPHA) != 0;
    bool useColorKey = (flags & SPRITE_COLORKEY) != 0;
    uint32_t colorKey = spr.colorKey;
    bool canMemcpyRows = !useAlpha && !useColorKey && !flipH && !flipV;

    int localX0 = 0, localX1 = sw;
    int localY0 = 0, localY1 = sh;

    if (x < 0) localX0 = -x;
    if (y < 0) localY0 = -y;
    if (x + sw > _width) localX1 = _width - x;
    if (y + sh > _height) localY1 = _height - y;

    if (!flipH) {
        if (sx < 0) localX0 = (localX0 > -sx) ? localX0 : -sx;
        if (sx + sw > spr.width) {
            int bound = spr.width - sx;
            localX1 = (localX1 < bound) ? localX1 : bound;
        }
    } else {
        if (sx < 0) {
            int bound = sx + sw;
            localX1 = (localX1 < bound) ? localX1 : bound;
        }
        if (sx + sw > spr.width) {
            int bound = sx + sw - spr.width;
            localX0 = (localX0 > bound) ? localX0 : bound;
        }
    }

    if (!flipV) {
        if (sy < 0) localY0 = (localY0 > -sy) ? localY0 : -sy;
        if (sy + sh > spr.height) {
            int bound = spr.height - sy;
            localY1 = (localY1 < bound) ? localY1 : bound;
        }
    } else {
        if (sy < 0) {
            int bound = sy + sh;
            localY1 = (localY1 < bound) ? localY1 : bound;
        }
        if (sy + sh > spr.height) {
            int bound = sy + sh - spr.height;
            localY0 = (localY0 > bound) ? localY0 : bound;
        }
    }

    if (localX0 >= localX1 || localY0 >= localY1) return;

    int srcXStep = flipH ? -1 : 1;

    if (canMemcpyRows) {
        int copyPixels = localX1 - localX0;
        int dstX0 = x + localX0;
        for (int localY = localY0; localY < localY1; localY++) {
            int srcY = flipV ? (sy + sh - 1 - localY) : (sy + localY);
            const uint32_t *srcRow = spr.pixels + srcY * spr.width;
            uint32_t *dstRow = _framebuffer + (y + localY) * _width;
            memcpy(dstRow + dstX0, srcRow + sx + localX0, (size_t)copyPixels * sizeof(uint32_t));
        }
        return;
    }

    if (useAlpha) {
        for (int localY = localY0; localY < localY1; localY++) {
            int srcY = flipV ? (sy + sh - 1 - localY) : (sy + localY);
            const uint32_t *srcRow = spr.pixels + srcY * spr.width;
            uint32_t *dstRow = _framebuffer + (y + localY) * _width;
            int srcX = flipH ? (sx + sw - 1 - localX0) : (sx + localX0);

            for (int localX = localX0; localX < localX1; localX++, srcX += srcXStep) {
                uint32_t c = srcRow[srcX];
                if (useColorKey && c == colorKey) continue;

                uint32_t sa = COLOR_GET_A(c);
                if (sa == 0) continue;

                int dx = x + localX;
                if (sa == 255) {
                    dstRow[dx] = c;
                } else {
                    uint32_t dc = dstRow[dx];
                    uint32_t ia = 255 - sa;
                    uint32_t or_ = (sa * COLOR_GET_R(c) + ia * COLOR_GET_R(dc)) / 255;
                    uint32_t og = (sa * COLOR_GET_G(c) + ia * COLOR_GET_G(dc)) / 255;
                    uint32_t ob = (sa * COLOR_GET_B(c) + ia * COLOR_GET_B(dc)) / 255;
                    dstRow[dx] = COLOR_ARGB(255, or_, og, ob);
                }
            }
        }
    } else if (useColorKey) {
        for (int localY = localY0; localY < localY1; localY++) {
            int srcY = flipV ? (sy + sh - 1 - localY) : (sy + localY);
            const uint32_t *srcRow = spr.pixels + srcY * spr.width;
            uint32_t *dstRow = _framebuffer + (y + localY) * _width;
            int srcX = flipH ? (sx + sw - 1 - localX0) : (sx + localX0);

            for (int localX = localX0; localX < localX1; localX++, srcX += srcXStep) {
                uint32_t c = srcRow[srcX];
                if (c != colorKey) dstRow[x + localX] = c;
            }
        }
    } else {
        for (int localY = localY0; localY < localY1; localY++) {
            int srcY = flipV ? (sy + sh - 1 - localY) : (sy + localY);
            const uint32_t *srcRow = spr.pixels + srcY * spr.width;
            uint32_t *dstRow = _framebuffer + (y + localY) * _width;
            int srcX = flipH ? (sx + sw - 1 - localX0) : (sx + localX0);

            for (int localX = localX0; localX < localX1; localX++, srcX += srcXStep) {
                uint32_t c = srcRow[srcX];
                if (COLOR_GET_A(c) > 0) dstRow[x + localX] = c;
            }
        }
    }
}

void GameLib::_DrawSpriteAreaScaled(int id, int x, int y, int sx, int sy, int sw, int sh,
                                    int dw, int dh, int flags)
{
    if (id < 0 || id >= (int)_sprites.size()) return;
    if (!_sprites[id].used || !_framebuffer) return;
    if (sw <= 0 || sh <= 0 || dw <= 0 || dh <= 0) return;

    GameSprite &spr = _sprites[id];
    int dx0 = x;
    int dy0 = y;
    int dx1 = x + dw;
    int dy1 = y + dh;

    if (dx0 < 0) dx0 = 0;
    if (dy0 < 0) dy0 = 0;
    if (dx1 > _width) dx1 = _width;
    if (dy1 > _height) dy1 = _height;
    if (dx0 >= dx1 || dy0 >= dy1) return;

    bool flipH = (flags & SPRITE_FLIP_H) != 0;
    bool flipV = (flags & SPRITE_FLIP_V) != 0;
    bool useAlpha = (flags & SPRITE_ALPHA) != 0;
    bool useColorKey = (flags & SPRITE_COLORKEY) != 0;
    uint32_t colorKey = spr.colorKey;

    for (int dy = dy0; dy < dy1; dy++) {
        int localY = dy - y;
        int srcY = (int)(((int64_t)localY * sh) / dh);
        if (flipV) srcY = sh - 1 - srcY;
        srcY += sy;
        if (srcY < 0 || srcY >= spr.height) continue;

        const uint32_t *srcRow = spr.pixels + srcY * spr.width;
        uint32_t *dstRow = _framebuffer + dy * _width;

        for (int dx = dx0; dx < dx1; dx++) {
            int localX = dx - x;
            int srcX = (int)(((int64_t)localX * sw) / dw);
            if (flipH) srcX = sw - 1 - srcX;
            srcX += sx;
            if (srcX < 0 || srcX >= spr.width) continue;

            uint32_t c = srcRow[srcX];
            if (useColorKey && c == colorKey) continue;

            uint32_t sa = COLOR_GET_A(c);
            if (sa == 0) continue;

            if (!useAlpha || sa == 255) {
                dstRow[dx] = c;
            } else {
                uint32_t dc = dstRow[dx];
                uint32_t ia = 255 - sa;
                uint32_t or_ = (sa * COLOR_GET_R(c) + ia * COLOR_GET_R(dc)) / 255;
                uint32_t og = (sa * COLOR_GET_G(c) + ia * COLOR_GET_G(dc)) / 255;
                uint32_t ob = (sa * COLOR_GET_B(c) + ia * COLOR_GET_B(dc)) / 255;
                dstRow[dx] = COLOR_ARGB(255, or_, og, ob);
            }
        }
    }
}

void GameLib::DrawSprite(int id, int x, int y)
{
    DrawSpriteEx(id, x, y, 0);
}

void GameLib::DrawSpriteEx(int id, int x, int y, int flags)
{
    if (id < 0 || id >= (int)_sprites.size()) return;
    if (!_sprites[id].used) return;
    _DrawSpriteAreaFast(id, x, y, 0, 0, _sprites[id].width, _sprites[id].height, flags);
}

void GameLib::DrawSpriteRegion(int id, int x, int y, int sx, int sy, int sw, int sh)
{
    DrawSpriteRegionEx(id, x, y, sx, sy, sw, sh, 0);
}

void GameLib::DrawSpriteRegionEx(int id, int x, int y, int sx, int sy, int sw, int sh, int flags)
{
    _DrawSpriteAreaFast(id, x, y, sx, sy, sw, sh, flags);
}

void GameLib::DrawSpriteScaled(int id, int x, int y, int w, int h, int flags)
{
    if (id < 0 || id >= (int)_sprites.size()) return;
    if (!_sprites[id].used) return;
    if (w == _sprites[id].width && h == _sprites[id].height)
        _DrawSpriteAreaFast(id, x, y, 0, 0, _sprites[id].width, _sprites[id].height, flags);
    else
        _DrawSpriteAreaScaled(id, x, y, 0, 0, _sprites[id].width, _sprites[id].height, w, h, flags);
}

void GameLib::DrawSpriteFrame(int id, int x, int y, int frameW, int frameH, int frameIndex, int flags)
{
    if (id < 0 || id >= (int)_sprites.size()) return;
    if (!_sprites[id].used) return;
    if (frameW <= 0 || frameH <= 0 || frameIndex < 0) return;

    GameSprite &spr = _sprites[id];
    int cols = spr.width / frameW;
    int rows = spr.height / frameH;
    if (cols <= 0 || rows <= 0) return;

    int totalFrames = cols * rows;
    if (frameIndex >= totalFrames) return;

    int sx = (frameIndex % cols) * frameW;
    int sy = (frameIndex / cols) * frameH;
    DrawSpriteRegionEx(id, x, y, sx, sy, frameW, frameH, flags);
}

void GameLib::DrawSpriteFrameScaled(int id, int x, int y, int frameW, int frameH, int frameIndex,
                                    int w, int h, int flags)
{
    if (id < 0 || id >= (int)_sprites.size()) return;
    if (!_sprites[id].used) return;
    if (frameW <= 0 || frameH <= 0 || frameIndex < 0 || w <= 0 || h <= 0) return;

    GameSprite &spr = _sprites[id];
    int cols = spr.width / frameW;
    int rows = spr.height / frameH;
    if (cols <= 0 || rows <= 0) return;

    int totalFrames = cols * rows;
    if (frameIndex >= totalFrames) return;

    int sx = (frameIndex % cols) * frameW;
    int sy = (frameIndex / cols) * frameH;
    if (w == frameW && h == frameH)
        _DrawSpriteAreaFast(id, x, y, sx, sy, frameW, frameH, flags);
    else
        _DrawSpriteAreaScaled(id, x, y, sx, sy, frameW, frameH, w, h, flags);
}

void GameLib::SetSpritePixel(int id, int x, int y, uint32_t color)
{
    if (id < 0 || id >= (int)_sprites.size()) return;
    if (!_sprites[id].used) return;
    if (x < 0 || x >= _sprites[id].width) return;
    if (y < 0 || y >= _sprites[id].height) return;
    _sprites[id].pixels[y * _sprites[id].width + x] = color;
}

uint32_t GameLib::GetSpritePixel(int id, int x, int y) const
{
    if (id < 0 || id >= (int)_sprites.size()) return 0;
    if (!_sprites[id].used) return 0;
    if (x < 0 || x >= _sprites[id].width) return 0;
    if (y < 0 || y >= _sprites[id].height) return 0;
    return _sprites[id].pixels[y * _sprites[id].width + x];
}

int GameLib::GetSpriteWidth(int id) const
{
    if (id < 0 || id >= (int)_sprites.size()) return 0;
    if (!_sprites[id].used) return 0;
    return _sprites[id].width;
}

int GameLib::GetSpriteHeight(int id) const
{
    if (id < 0 || id >= (int)_sprites.size()) return 0;
    if (!_sprites[id].used) return 0;
    return _sprites[id].height;
}

void GameLib::SetSpriteColorKey(int id, uint32_t color)
{
    if (id < 0 || id >= (int)_sprites.size()) return;
    if (!_sprites[id].used) return;
    _sprites[id].colorKey = color;
}

uint32_t GameLib::GetSpriteColorKey(int id) const
{
    if (id < 0 || id >= (int)_sprites.size()) return COLORKEY_DEFAULT;
    if (!_sprites[id].used) return COLORKEY_DEFAULT;
    return _sprites[id].colorKey;
}

bool GameLib::IsKeyDown(int key) const
{
    return _keys[key & 511] != 0;
}

bool GameLib::IsKeyPressed(int key) const
{
    int k = key & 511;
    return (_keys[k] != 0) && (_keys_prev[k] == 0);
}

bool GameLib::IsKeyReleased(int key) const
{
    int k = key & 511;
    return (_keys[k] == 0) && (_keys_prev[k] != 0);
}

int GameLib::GetMouseX() const { return _mouseX; }
int GameLib::GetMouseY() const { return _mouseY; }

bool GameLib::IsMouseDown(int button) const
{
    if (button < 0 || button > 2) return false;
    return _mouseButtons[button] != 0;
}

bool GameLib::IsMousePressed(int button) const
{
    if (button < 0 || button > 2) return false;
    return (_mouseButtons[button] != 0) && (_mouseButtons_prev[button] == 0);
}

bool GameLib::IsMouseReleased(int button) const
{
    if (button < 0 || button > 2) return false;
    return (_mouseButtons[button] == 0) && (_mouseButtons_prev[button] != 0);
}

int GameLib::GetMouseWheelDelta() const
{
    return _mouseWheelDelta;
}

bool GameLib::IsActive() const
{
    return _active;
}

void GameLib::PlayBeep(int frequency, int duration)
{
    if (duration <= 0) return;
    if (frequency <= 0) frequency = 440;

#if GAMELIB_SDL_HAS_MIXER
    if (_EnsureMixerReady()) {
        int sampleRate = 0;
        int channels = 0;
        Uint16 format = 0;
        if (Mix_QuerySpec(&sampleRate, &format, &channels) != 0 &&
            sampleRate > 0 && channels > 0 && format == AUDIO_S16SYS) {
            std::vector<int16_t> toneData;
            _gamelib_generate_beep_pcm(toneData, sampleRate, channels, frequency, duration);

            if (!toneData.empty()) {
                Mix_Chunk *chunk = Mix_QuickLoad_RAW((Uint8*)toneData.data(),
                    (Uint32)(toneData.size() * sizeof(int16_t)));
                if (chunk) {
                    int channel = Mix_PlayChannel(1, chunk, 0);
                    if (channel >= 0) {
                        Uint32 timeout = SDL_GetTicks() + (Uint32)duration + 1000U;
                        while (Mix_Playing(channel) != 0 &&
                               !SDL_TICKS_PASSED(SDL_GetTicks(), timeout)) {
                            SDL_Delay(1);
                        }
                        Mix_HaltChannel(channel);
                        Mix_FreeChunk(chunk);
                        return;
                    }
                    Mix_FreeChunk(chunk);
                }
            }
        }
    }
#endif

    if (!_audioReady) {
        if (SDL_InitSubSystem(SDL_INIT_AUDIO) != 0) return;
        _audioReady = true;
    }

    SDL_AudioSpec want;
    SDL_AudioSpec have;
    SDL_zero(want);
    SDL_zero(have);
    want.freq = 44100;
    want.format = AUDIO_S16SYS;
    want.channels = 2;
    want.samples = 1024;
    want.callback = NULL;

    SDL_AudioDeviceID device = SDL_OpenAudioDevice(
        NULL, 0, &want, &have,
        SDL_AUDIO_ALLOW_FREQUENCY_CHANGE | SDL_AUDIO_ALLOW_CHANNELS_CHANGE);
    if (device == 0) return;

    if (have.freq <= 0 || have.channels <= 0 || have.format != AUDIO_S16SYS) {
        SDL_CloseAudioDevice(device);
        return;
    }

    std::vector<int16_t> toneData;
    _gamelib_generate_beep_pcm(toneData, have.freq, have.channels, frequency, duration);
    if (toneData.empty()) {
        SDL_CloseAudioDevice(device);
        return;
    }

    SDL_ClearQueuedAudio(device);
    if (SDL_QueueAudio(device, toneData.data(),
                       (Uint32)(toneData.size() * sizeof(int16_t))) == 0) {
        Uint32 timeout = SDL_GetTicks() + (Uint32)duration + 500U;
        SDL_PauseAudioDevice(device, 0);
        while (SDL_GetQueuedAudioSize(device) > 0 &&
               !SDL_TICKS_PASSED(SDL_GetTicks(), timeout)) {
            SDL_Delay(1);
        }
        SDL_Delay(8);
    }

    SDL_CloseAudioDevice(device);
}

bool GameLib::PlayWAV(const char *filename, bool loop)
{
#if GAMELIB_SDL_HAS_MIXER
    if (!filename) return false;
    if (!_EnsureMixerReady()) return false;

    StopWAV();
    _currentWav = Mix_LoadWAV(filename);
    if (!_currentWav) return false;

    _wavChannel = Mix_PlayChannel(0, _currentWav, loop ? -1 : 0);
    if (_wavChannel < 0) {
        Mix_FreeChunk(_currentWav);
        _currentWav = NULL;
        return false;
    }
    return true;
#else
    (void)filename;
    (void)loop;
    return false;
#endif
}

void GameLib::StopWAV()
{
#if GAMELIB_SDL_HAS_MIXER
    if (_mixerReady) {
        Mix_HaltChannel(0);
        if (_currentWav) {
            Mix_FreeChunk(_currentWav);
            _currentWav = NULL;
        }
        _wavChannel = 0;
    }
#endif
}

bool GameLib::PlayMusic(const char *filename, bool loop)
{
#if GAMELIB_SDL_HAS_MIXER
    if (!filename) return false;
    if (!_EnsureMixerReady()) return false;

    StopMusic();
    _currentMusic = Mix_LoadMUS(filename);
    if (!_currentMusic) return false;
    if (Mix_PlayMusic(_currentMusic, loop ? -1 : 0) != 0) {
        Mix_FreeMusic(_currentMusic);
        _currentMusic = NULL;
        return false;
    }
    _musicPlaying = true;
    return true;
#else
    (void)filename;
    (void)loop;
    return false;
#endif
}

void GameLib::StopMusic()
{
#if GAMELIB_SDL_HAS_MIXER
    if (_mixerReady) {
        Mix_HaltMusic();
        if (_currentMusic) {
            Mix_FreeMusic(_currentMusic);
            _currentMusic = NULL;
        }
    }
#endif
    _musicPlaying = false;
}

bool GameLib::IsMusicPlaying() const
{
#if GAMELIB_SDL_HAS_MIXER
    if (!_mixerReady || !_currentMusic) return false;
    return _musicPlaying && Mix_PlayingMusic() != 0;
#else
    return false;
#endif
}

int GameLib::Random(int minVal, int maxVal)
{
    if (minVal > maxVal) { int t = minVal; minVal = maxVal; maxVal = t; }
    if (minVal == maxVal) return minVal;
    return minVal + rand() % (maxVal - minVal + 1);
}

bool GameLib::RectOverlap(int x1, int y1, int w1, int h1,
                          int x2, int y2, int w2, int h2)
{
    return !(x1 + w1 <= x2 || x2 + w2 <= x1 || y1 + h1 <= y2 || y2 + h2 <= y1);
}

bool GameLib::CircleOverlap(int cx1, int cy1, int r1,
                            int cx2, int cy2, int r2)
{
    int64_t dx = cx1 - cx2;
    int64_t dy = cy1 - cy2;
    int64_t distSq = dx * dx + dy * dy;
    int64_t rSum = r1 + r2;
    return distSq <= rSum * rSum;
}

bool GameLib::PointInRect(int px, int py, int x, int y, int w, int h)
{
    return px >= x && px < x + w && py >= y && py < y + h;
}

float GameLib::Distance(int x1, int y1, int x2, int y2)
{
    float dx = (float)(x1 - x2);
    float dy = (float)(y1 - y2);
    return sqrtf(dx * dx + dy * dy);
}

void GameLib::DrawGrid(int x, int y, int rows, int cols, int cellSize, uint32_t color)
{
    for (int r = 0; r <= rows; r++) {
        DrawLine(x, y + r * cellSize, x + cols * cellSize, y + r * cellSize, color);
    }
    for (int c = 0; c <= cols; c++) {
        DrawLine(x + c * cellSize, y, x + c * cellSize, y + rows * cellSize, color);
    }
}

void GameLib::FillCell(int gridX, int gridY, int row, int col, int cellSize, uint32_t color)
{
    FillRect(gridX + col * cellSize + 1, gridY + row * cellSize + 1,
             cellSize - 1, cellSize - 1, color);
}

int GameLib::_AllocTilemapSlot()
{
    for (size_t i = 0; i < _tilemaps.size(); i++) {
        if (!_tilemaps[i].used) return (int)i;
    }

    GameTilemap tm;
    tm.cols = 0;
    tm.rows = 0;
    tm.tileSize = 0;
    tm.tilesetId = -1;
    tm.tilesetCols = 0;
    tm.tiles = NULL;
    tm.used = false;
    _tilemaps.push_back(tm);
    return (int)(_tilemaps.size() - 1);
}

int GameLib::CreateTilemap(int cols, int rows, int tileSize, int tilesetId)
{
    if (cols <= 0 || rows <= 0 || tileSize <= 0) return -1;
    if (tilesetId < 0 || tilesetId >= (int)_sprites.size()) return -1;
    if (!_sprites[tilesetId].used) return -1;
    if (cols > 4096 || rows > 4096) return -1;

    int id = _AllocTilemapSlot();
    int *tiles = (int*)malloc((size_t)cols * rows * sizeof(int));
    if (!tiles) return -1;
    for (int i = 0; i < cols * rows; i++) tiles[i] = -1;

    _tilemaps[id].cols = cols;
    _tilemaps[id].rows = rows;
    _tilemaps[id].tileSize = tileSize;
    _tilemaps[id].tilesetId = tilesetId;
    _tilemaps[id].tilesetCols = _sprites[tilesetId].width / tileSize;
    _tilemaps[id].tiles = tiles;
    _tilemaps[id].used = true;
    return id;
}

void GameLib::FreeTilemap(int mapId)
{
    if (mapId < 0 || mapId >= (int)_tilemaps.size()) return;
    if (!_tilemaps[mapId].used) return;
    if (_tilemaps[mapId].tiles) {
        free(_tilemaps[mapId].tiles);
        _tilemaps[mapId].tiles = NULL;
    }
    _tilemaps[mapId].used = false;
}

void GameLib::SetTile(int mapId, int col, int row, int tileId)
{
    if (mapId < 0 || mapId >= (int)_tilemaps.size()) return;
    if (!_tilemaps[mapId].used) return;
    if (col < 0 || col >= _tilemaps[mapId].cols) return;
    if (row < 0 || row >= _tilemaps[mapId].rows) return;
    _tilemaps[mapId].tiles[row * _tilemaps[mapId].cols + col] = tileId;
}

int GameLib::GetTile(int mapId, int col, int row) const
{
    if (mapId < 0 || mapId >= (int)_tilemaps.size()) return -1;
    if (!_tilemaps[mapId].used) return -1;
    if (col < 0 || col >= _tilemaps[mapId].cols) return -1;
    if (row < 0 || row >= _tilemaps[mapId].rows) return -1;
    return _tilemaps[mapId].tiles[row * _tilemaps[mapId].cols + col];
}

int GameLib::GetTilemapCols(int mapId) const
{
    if (mapId < 0 || mapId >= (int)_tilemaps.size()) return 0;
    if (!_tilemaps[mapId].used) return 0;
    return _tilemaps[mapId].cols;
}

int GameLib::GetTilemapRows(int mapId) const
{
    if (mapId < 0 || mapId >= (int)_tilemaps.size()) return 0;
    if (!_tilemaps[mapId].used) return 0;
    return _tilemaps[mapId].rows;
}

int GameLib::GetTileSize(int mapId) const
{
    if (mapId < 0 || mapId >= (int)_tilemaps.size()) return 0;
    if (!_tilemaps[mapId].used) return 0;
    return _tilemaps[mapId].tileSize;
}

int GameLib::WorldToTileCol(int mapId, int x) const
{
    if (mapId < 0 || mapId >= (int)_tilemaps.size()) return 0;
    if (!_tilemaps[mapId].used) return 0;
    return _gamelib_floor_div(x, _tilemaps[mapId].tileSize);
}

int GameLib::WorldToTileRow(int mapId, int y) const
{
    if (mapId < 0 || mapId >= (int)_tilemaps.size()) return 0;
    if (!_tilemaps[mapId].used) return 0;
    return _gamelib_floor_div(y, _tilemaps[mapId].tileSize);
}

int GameLib::GetTileAtPixel(int mapId, int x, int y) const
{
    return GetTile(mapId, WorldToTileCol(mapId, x), WorldToTileRow(mapId, y));
}

void GameLib::FillTileRect(int mapId, int col, int row, int cols, int rows, int tileId)
{
    if (mapId < 0 || mapId >= (int)_tilemaps.size()) return;
    if (!_tilemaps[mapId].used) return;
    if (cols <= 0 || rows <= 0) return;

    GameTilemap &tm = _tilemaps[mapId];
    int col0 = col;
    int row0 = row;
    int col1 = col + cols;
    int row1 = row + rows;

    if (col0 < 0) col0 = 0;
    if (row0 < 0) row0 = 0;
    if (col1 > tm.cols) col1 = tm.cols;
    if (row1 > tm.rows) row1 = tm.rows;
    if (col0 >= col1 || row0 >= row1) return;

    for (int r = row0; r < row1; r++) {
        int *rowPtr = tm.tiles + r * tm.cols;
        for (int c = col0; c < col1; c++) {
            rowPtr[c] = tileId;
        }
    }
}

void GameLib::ClearTilemap(int mapId, int tileId)
{
    if (mapId < 0 || mapId >= (int)_tilemaps.size()) return;
    if (!_tilemaps[mapId].used) return;

    GameTilemap &tm = _tilemaps[mapId];
    int count = tm.cols * tm.rows;
    for (int i = 0; i < count; i++) {
        tm.tiles[i] = tileId;
    }
}

void GameLib::DrawTilemap(int mapId, int x, int y, int flags)
{
    if (mapId < 0 || mapId >= (int)_tilemaps.size()) return;
    if (!_tilemaps[mapId].used) return;

    GameTilemap &tm = _tilemaps[mapId];
    int tsId = tm.tilesetId;
    if (tsId < 0 || tsId >= (int)_sprites.size()) return;
    if (!_sprites[tsId].used) return;

    GameSprite &tset = _sprites[tsId];
    int ts = tm.tileSize;
    int tsCols = tm.tilesetCols;
    if (tsCols <= 0) return;

    int col0 = (-x) / ts;
    int row0 = (-y) / ts;
    int col1 = (-x + _width - 1) / ts + 1;
    int row1 = (-y + _height - 1) / ts + 1;
    if (col0 < 0) col0 = 0;
    if (row0 < 0) row0 = 0;
    if (col1 > tm.cols) col1 = tm.cols;
    if (row1 > tm.rows) row1 = tm.rows;

    bool useAlpha = (flags & SPRITE_ALPHA) != 0;
    bool useColorKey = (flags & SPRITE_COLORKEY) != 0;
    int tileFlags = flags & (SPRITE_ALPHA | SPRITE_COLORKEY);
    bool canMemcpyTiles = !useAlpha && !useColorKey;

    for (int r = row0; r < row1; r++) {
        for (int c = col0; c < col1; c++) {
            int tid = tm.tiles[r * tm.cols + c];
            if (tid < 0) continue;

            int srcCol = tid % tsCols;
            int srcRow = tid / tsCols;
            int srcX0 = srcCol * ts;
            int srcY0 = srcRow * ts;

            int dstX0 = x + c * ts;
            int dstY0 = y + r * ts;

            if (canMemcpyTiles) {
                int ix0 = 0, iy0 = 0, ix1 = ts, iy1 = ts;
                if (dstX0 < 0) ix0 = -dstX0;
                if (dstY0 < 0) iy0 = -dstY0;
                if (dstX0 + ix1 > _width) ix1 = _width - dstX0;
                if (dstY0 + iy1 > _height) iy1 = _height - dstY0;
                if (ix0 >= ix1 || iy0 >= iy1) continue;

                int copyPixels = ix1 - ix0;
                int dstX = dstX0 + ix0;
                for (int iy = iy0; iy < iy1; iy++) {
                    const uint32_t *srcRow_ = tset.pixels + (srcY0 + iy) * tset.width;
                    uint32_t *dstRow_ = _framebuffer + (dstY0 + iy) * _width;
                    memcpy(dstRow_ + dstX, srcRow_ + srcX0 + ix0, (size_t)copyPixels * sizeof(uint32_t));
                }
            } else {
                _DrawSpriteAreaFast(tsId, dstX0, dstY0, srcX0, srcY0, ts, ts, tileFlags);
            }
        }
    }
}

#endif // GAMELIB_SDL_IMPLEMENTATION

#endif // GAMELIB_SDL_H