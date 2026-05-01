//=====================================================================
//
// GameLib.SDL.h - A single-header cross-platform game library
//
// Homepage: https://github.com/skywind3000/GameLib
// Copyright (c) 2026 skywind3000 (Lin Wei)
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
// Compile command (MinGW / Dev C++):
//     g++ -o game main.cpp -lSDL2 -lSDL2_image -lSDL2_ttf -lSDL2_mixer
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

#define GAMELIB_SDL_VERSION_MAJOR 1
#define GAMELIB_SDL_VERSION_MINOR 9
#define GAMELIB_SDL_VERSION_PATCH 7

#include <stdint.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <stdarg.h>

#include <vector>
#include <string>
#include <unordered_map>

// SDL headers and extension library detection (moved here from
// GAMELIB_SDL_IMPLEMENTATION so that the GameLib class declaration
// below can use the real SDL types instead of fragile forward
// declarations that break across different SDL builds).

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

#if GAMELIB_SDL_HAS_MIXER && (defined(__EMSCRIPTEN__) || defined(GAMELIB_SDL_MIXER_CHANNEL))
#define GAMELIB_SDL_USE_MIXER_CHANNELS 1
#else
#define GAMELIB_SDL_USE_MIXER_CHANNELS 0
#endif

// Forward declarations for extension types when their headers are not available.
// These match the official SDL struct tag naming so they won't conflict if the
// headers are later included from elsewhere.
#if !GAMELIB_SDL_HAS_TTF
typedef struct _TTF_Font TTF_Font;
#endif
#if !GAMELIB_SDL_HAS_MIXER
typedef struct Mix_Chunk Mix_Chunk;
typedef struct _Mix_Music Mix_Music;
#endif

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
#define KEY_ADD       0x6E    // Numpad + (VK_ADD equivalent)
#define KEY_SUBTRACT  0x6D    // Numpad - (VK_SUBTRACT equivalent)

#define MOUSE_LEFT    0
#define MOUSE_RIGHT   1
#define MOUSE_MIDDLE  2

#define MESSAGEBOX_OK           0
#define MESSAGEBOX_YESNO        1
#define MESSAGEBOX_RESULT_OK    1
#define MESSAGEBOX_RESULT_YES   2
#define MESSAGEBOX_RESULT_NO    3

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

    int Open(int width, int height, const char *title, bool center = false, bool resizable = false);
    bool IsClosed() const;
    void Update();
    void WaitFrame(int fps);
    double GetDeltaTime() const;
    double GetFPS() const;
    double GetTime() const;
    int GetWidth() const;
    int GetHeight() const;
    uint32_t *GetFramebuffer();
    void WinResize(int width, int height);
    void SetMaximized(bool maximized);
    void SetTitle(const char *title);
    void ShowFps(bool show);
    void ShowMouse(bool show);
    void AspectLock(bool lock, uint32_t color = COLOR_BLACK);
    int ShowMessage(const char *text, const char *title = NULL, int buttons = MESSAGEBOX_OK);

    void Clear(uint32_t color = COLOR_BLACK);
    void SetPixel(int x, int y, uint32_t color);
    uint32_t GetPixel(int x, int y) const;
    void SetClip(int x, int y, int w, int h);
    void ClearClip();
    void GetClip(int *x, int *y, int *w, int *h) const;
    int GetClipX() const;
    int GetClipY() const;
    int GetClipW() const;
    int GetClipH() const;
    void Screenshot(const char *filename);

    void DrawLine(int x1, int y1, int x2, int y2, uint32_t color);
    void DrawRect(int x, int y, int w, int h, uint32_t color);
    void FillRect(int x, int y, int w, int h, uint32_t color);
    void DrawCircle(int cx, int cy, int r, uint32_t color);
    void FillCircle(int cx, int cy, int r, uint32_t color);
    void DrawEllipse(int cx, int cy, int rx, int ry, uint32_t color);
    void FillEllipse(int cx, int cy, int rx, int ry, uint32_t color);
    void DrawTriangle(int x1, int y1, int x2, int y2, int x3, int y3, uint32_t color);
    void FillTriangle(int x1, int y1, int x2, int y2, int x3, int y3, uint32_t color);

    void DrawText(int x, int y, const char *text, uint32_t color);
    void DrawNumber(int x, int y, int number, uint32_t color);
    void DrawTextScale(int x, int y, const char *text, uint32_t color, int w, int h);
    void DrawPrintf(int x, int y, uint32_t color, const char *fmt, ...);
    void DrawPrintfScale(int x, int y, uint32_t color, int w, int h, const char *fmt, ...);

    void DrawTextFont(int x, int y, const char *text, uint32_t color, const char *fontName, int fontSize);
    void DrawTextFont(int x, int y, const char *text, uint32_t color, int fontSize);
    void DrawPrintfFont(int x, int y, uint32_t color, const char *fontName, int fontSize, const char *fmt, ...);
    void DrawPrintfFont(int x, int y, uint32_t color, int fontSize, const char *fmt, ...);
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
    void DrawSpriteRotated(int id, int cx, int cy, double angleDeg, int flags = 0);
    void DrawSpriteFrame(int id, int x, int y, int frameW, int frameH, int frameIndex, int flags = 0);
    void DrawSpriteFrameScaled(int id, int x, int y, int frameW, int frameH, int frameIndex, int w, int h, int flags = 0);
    void DrawSpriteFrameRotated(int id, int cx, int cy, int frameW, int frameH, int frameIndex,
                                double angleDeg, int flags = 0);
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

    int PlayBeep(int frequency, int duration, int repeat = 1, int volume = 1000);
    int PlayWAV(const char *filename, int repeat = 1, int volume = 1000);
    int PlayPCM(const int16_t *pcm, int nchannels, int nsamples, int sample_rate, int repeat = 1, int volume = 1000);
    int StopWAV(int channel);
    int IsPlaying(int channel);
    int SetVolume(int channel, int volume);
    void StopAll();
    int SetMasterVolume(int volume);
    int GetMasterVolume() const;
    bool PlayMusic(const char *filename, bool loop = true);
    void StopMusic();
    bool IsMusicPlaying() const;
	
    int CreateTilemap(int cols, int rows, int tileSize, int tilesetId);
    bool SaveTilemap(const char *filename, int mapId) const;
    int LoadTilemap(const char *filename, int tilesetId);
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
	
    void DrawGrid(int x, int y, int rows, int cols, int cellSize, uint32_t color);
    void FillCell(int gridX, int gridY, int row, int col, int cellSize, uint32_t color);

    // -------- UI System --------
    bool Button(int x, int y, int w, int h, const char *text, uint32_t color);
    bool Checkbox(int x, int y, const char *text, bool *checked);
    bool RadioBox(int x, int y, const char *text, int *value, int index);
    bool ToggleButton(int x, int y, int w, int h, const char *text, bool *toggled, uint32_t color);

    // -------- Scene Management --------
    void SetScene(int scene);
    int GetScene() const;
    bool IsSceneChanged() const;
    int GetPreviousScene() const;

    // -------- Save / Load Data --------
    static bool SaveInt(const char *filename, const char *key, int value);
    static bool SaveFloat(const char *filename, const char *key, float value);
    static bool SaveString(const char *filename, const char *key, const char *value);
    static int LoadInt(const char *filename, const char *key, int defaultValue = 0);
    static float LoadFloat(const char *filename, const char *key, float defaultValue = 0.0f);
    static const char *LoadString(const char *filename, const char *key,
                                  const char *defaultValue = "");
    static bool HasSaveKey(const char *filename, const char *key);
    static bool DeleteSaveKey(const char *filename, const char *key);
    static bool DeleteSave(const char *filename);

    static int Random(int minVal, int maxVal);
    static bool RectOverlap(int x1, int y1, int w1, int h1,
                            int x2, int y2, int w2, int h2);
    static bool CircleOverlap(int cx1, int cy1, int r1,
                              int cx2, int cy2, int r2);
    static bool PointInRect(int px, int py, int x, int y, int w, int h);
    static float Distance(int x1, int y1, int x2, int y2);

private:
    GameLib(const GameLib &);
    GameLib &operator=(const GameLib &);

    void _DestroyWindowResources();
    void _UpdateTitleFps();
    void _UpdateWindowSize();
    void _SetMouseFromWindowCoords(int x, int y);
    void _SyncInputState();
    void _SetPixelFast(int x, int y, uint32_t color);
    void _DrawHLine(int x1, int x2, int y, uint32_t color);
    bool _ClipRectToCurrentClip(int *x0, int *y0, int *x1, int *y1) const;
    int _AllocSpriteSlot();
    void _DrawSpriteAreaFast(int id, int x, int y, int sx, int sy, int sw, int sh, int flags);
    void _DrawSpriteAreaScaled(int id, int x, int y, int sx, int sy, int sw, int sh,
                               int dw, int dh, int flags);
    void _DrawSpriteAreaRotated(int id, int cx, int cy, int sx, int sy, int sw, int sh,
                                double angleDeg, int flags);
    int _AllocTilemapSlot();
    int _GetTilesetTileCount(int tilesetId, int tileSize) const;
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
    bool _mouseVisible;
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
    int _windowWidth;
    int _windowHeight;
    int _clipX;
    int _clipY;
    int _clipW;
    int _clipH;
    bool _resizable;

    // aspect lock
    bool _aspectLock;
    uint32_t _aspectColor;
    int _aspectOffsetX;
    int _aspectOffsetY;
    int _aspectContentW;
    int _aspectContentH;

    uint32_t *_framebuffer;

    // text scale lookup tables (dynamic, max 1024)
    int *_textSrcRowMap;
    int *_textSrcColMap;

    int _keys[512];
    int _keys_prev[512];
    int _mouseX;
    int _mouseY;
    int _mouseButtons[3];
    int _mouseButtons_prev[3];
    int _mouseWheelDelta;
    uint32_t _uiActiveId;

    uint64_t _timeStartCounter;
    uint64_t _timePrevCounter;
    uint64_t _fpsTimeCounter;
    uint64_t _frameStartCounter;
    uint64_t _perfFrequency;
    double _deltaTime;
    double _fps;
    double _fpsAccum;

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

    // Multi-channel audio mixer state
    struct _WavData {
        uint8_t *buffer;
        uint32_t size;
        uint32_t sample_rate;
        uint16_t channels;
        uint16_t bits_per_sample;
        int ref_count;
        bool temporary;
        _WavData() : buffer(NULL), size(0), sample_rate(0), channels(0),
                     bits_per_sample(0), ref_count(0), temporary(false) {}
        ~_WavData() { if (buffer) delete[] buffer; }
    };
    struct _Channel {
        int id;
        _WavData *wav;
        int volume;
        int repeat;
        uint32_t position;
        bool is_playing;
    };
    std::unordered_map<std::string, _WavData*> _wav_cache;
    std::unordered_map<int, _Channel*> _audio_channels;
    int64_t _next_channel_id;
    bool _audio_initialized;
    int _master_volume;
    SDL_AudioDeviceID _audioDevice;
    SDL_AudioSpec _audioSpec;
    bool _audioSelfInit;
    static const int _MAX_CHANNELS = 32;
    static const int _AUDIO_BUFFER_FRAMES = 2048;
    static const int _AUDIO_OUTPUT_CHANNELS = 2;
    static const int _AUDIO_BUFFER_TOTAL = _AUDIO_BUFFER_FRAMES * _AUDIO_OUTPUT_CHANNELS;
    static const int _AUDIO_BUFFER_BYTES = _AUDIO_BUFFER_TOTAL * sizeof(int16_t);
    int32_t _mix_buffer[_AUDIO_BUFFER_TOTAL];

    bool _InitAudioBackend();
    void _ShutdownAudioBackend();
    static void _SDLAudioCallback(void *userdata, Uint8 *stream, int len);
    void _MixAudio(int16_t *output, int sample_count);
    void _ClampAndConvert(int32_t *input, int16_t *output, int count);
    _WavData *_LoadWAVFromFile(const char *filename);
    _WavData *_ConvertToTargetFormat(_WavData *src);
    _WavData *_LoadOrCacheWAV(const char *filename);
    int _AllocateChannel();
    void _ReleaseChannel(int channel_id);

Mix_Music *_currentMusic;
    bool _musicPlaying;
    int _mixerInitFlags;
#if GAMELIB_SDL_USE_MIXER_CHANNELS
    std::unordered_map<std::string, Mix_Chunk*> _chunk_cache;
    std::unordered_map<int, Mix_Chunk*> _temp_chunks;
    static void _MixerChannelFinishedCallback(int channel);
#endif

    // scene state
    int _scene;
    int _pendingScene;
    bool _hasPendingScene;
    bool _sceneChanged;
    int _previousScene;

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

#if defined(__EMSCRIPTEN__)
#include <emscripten.h>
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

static int _gamelib_round_to_int(double value)
{
    if (value >= 0.0) return (int)(value + 0.5);
    return (int)(value - 0.5);
}

static uint32_t _gamelib_alpha_blend(uint32_t dst, uint32_t src)
{
    uint32_t sa = COLOR_GET_A(src);
    if (sa == 0) return dst;
    if (sa == 255) return src;

    uint32_t ia = 255 - sa;
    uint32_t or_ = (sa * COLOR_GET_R(src) + ia * COLOR_GET_R(dst)) / 255;
    uint32_t og = (sa * COLOR_GET_G(src) + ia * COLOR_GET_G(dst)) / 255;
    uint32_t ob = (sa * COLOR_GET_B(src) + ia * COLOR_GET_B(dst)) / 255;
    return COLOR_ARGB(255, or_, og, ob);
}

static void _gamelib_blend_pixel(uint32_t *dst, uint32_t src)
{
    if (!dst) return;

    uint32_t sa = COLOR_GET_A(src);
    if (sa == 0) return;
    if (sa == 255) {
        *dst = src;
        return;
    }

    *dst = _gamelib_alpha_blend(*dst, src);
}

static uint32_t _gamelib_ui_lighten(uint32_t color, int amount);
static uint32_t _gamelib_ui_darken(uint32_t color, int amount);
static int _gamelib_ui_text_width(const char *text);
static int _gamelib_ui_text_height(const char *text);
static uint32_t _gamelib_ui_button_text_color(uint32_t color);
static uint32_t _gamelib_ui_make_id(uint32_t kind, int x, int y, int w, int h, const char *text);
static void _gamelib_ui_draw_bevel_rect(GameLib *game, int x, int y, int w, int h,
                                        uint32_t face, bool pressed);
static void _gamelib_ui_draw_text_with_shadow(GameLib *game, int x, int y, const char *text,
                                              uint32_t color, uint32_t shadow);
static void _gamelib_ui_draw_checkbox_mark(GameLib *game, int x, int y, int size,
                                           bool pressed, uint32_t color);

static void _gamelib_plot_circle_points_unique(GameLib *game,
                                               int cx, int cy, int x, int y,
                                               uint32_t color)
{
    if (!game) return;

    game->SetPixel(cx + x, cy + y, color);
    if (x != 0) game->SetPixel(cx - x, cy + y, color);
    if (y != 0) {
        game->SetPixel(cx + x, cy - y, color);
        if (x != 0) game->SetPixel(cx - x, cy - y, color);
    }

    if (x == y) return;

    game->SetPixel(cx + y, cy + x, color);
    game->SetPixel(cx - y, cy + x, color);
    if (x != 0) {
        game->SetPixel(cx + y, cy - x, color);
        game->SetPixel(cx - y, cy - x, color);
    }
}

static void _gamelib_plot_ellipse_points_unique(GameLib *game,
                                                int cx, int cy, int x, int y,
                                                uint32_t color)
{
    if (!game) return;

    game->SetPixel(cx + x, cy + y, color);
    if (x != 0) game->SetPixel(cx - x, cy + y, color);
    if (y != 0) {
        game->SetPixel(cx + x, cy - y, color);
        if (x != 0) game->SetPixel(cx - x, cy - y, color);
    }
}

static bool _gamelib_rw_read_text_line(SDL_RWops *rw, std::string &line)
{
    line.clear();
    if (!rw) return false;

    char ch = 0;
    while (SDL_RWread(rw, &ch, 1, 1) == 1) {
        if (ch == '\n') break;
        line.push_back(ch);
    }

    if (line.empty() && ch != '\n') return false;
    if (!line.empty() && line[line.size() - 1] == '\r') line.resize(line.size() - 1);
    return true;
}

static void _gamelib_strip_utf8_bom(std::string &line)
{
    if (line.size() >= 3 &&
        (unsigned char)line[0] == 0xEF &&
        (unsigned char)line[1] == 0xBB &&
        (unsigned char)line[2] == 0xBF) {
        line.erase(0, 3);
    }
}

static bool _gamelib_parse_int_tokens(const std::string &line, int *values, int maxCount, int *outCount)
{
    if (outCount) *outCount = 0;
    if (maxCount < 0) return false;

    const char *cursor = line.c_str();
    int count = 0;
    while (*cursor) {
        while (*cursor == ' ' || *cursor == '\t') cursor++;
        if (!*cursor) break;
        if (count >= maxCount) break;

        char *endPtr = NULL;
        long value = strtol(cursor, &endPtr, 10);
        if (endPtr == cursor) return false;
        if (value < (long)INT_MIN || value > (long)INT_MAX) return false;
        if (*endPtr && *endPtr != ' ' && *endPtr != '\t') return false;

        values[count++] = (int)value;
        cursor = endPtr;
    }

    if (outCount) *outCount = count;
    return true;
}

static bool _gamelib_rw_write_text(SDL_RWops *rw, const char *text, size_t len)
{
    if (!rw) return false;
    if (!text) return len == 0;
    if (len == 0) return true;
    return SDL_RWwrite(rw, text, 1, len) == len;
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
    case SDL_SCANCODE_KP_PLUS: return KEY_ADD;
    case SDL_SCANCODE_KP_MINUS: return KEY_SUBTRACT;
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
    _mouseVisible = true;
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
    _windowWidth = 0;
    _windowHeight = 0;
    _clipX = 0;
    _clipY = 0;
    _clipW = 0;
    _clipH = 0;
    _resizable = false;
    _aspectLock = false;
    _aspectColor = COLOR_BLACK;
    _aspectOffsetX = 0;
    _aspectOffsetY = 0;
    _aspectContentW = 0;
    _aspectContentH = 0;
    _framebuffer = NULL;
    _textSrcRowMap = NULL;
    _textSrcColMap = NULL;
    memset(_keys, 0, sizeof(_keys));
    memset(_keys_prev, 0, sizeof(_keys_prev));
    _mouseX = 0;
    _mouseY = 0;
    memset(_mouseButtons, 0, sizeof(_mouseButtons));
    memset(_mouseButtons_prev, 0, sizeof(_mouseButtons_prev));
    _mouseWheelDelta = 0;
    _uiActiveId = 0;
    _timeStartCounter = 0;
    _timePrevCounter = 0;
    _fpsTimeCounter = 0;
    _frameStartCounter = 0;
    _perfFrequency = 0;
    _deltaTime = 0.0;
    _fps = 0.0;
    _fpsAccum = 0.0;
    _next_channel_id = 1;
    _audio_initialized = false;
    _master_volume = 1000;
    _audioDevice = 0;
    _audioSelfInit = false;
    _currentMusic = NULL;
    _musicPlaying = false;
    _mixerInitFlags = 0;
    _scene = 0;
    _pendingScene = 0;
    _hasPendingScene = false;
    _sceneChanged = true;
    _previousScene = 0;

    if (!_srandDone) {
        srand((unsigned int)time(NULL));
        _srandDone = true;
    }
}

GameLib::~GameLib()
{
    StopMusic();
    _ShutdownAudioBackend();

    // Clean up channels and WAV cache (audio device closed, no lock needed)
    for (std::unordered_map<int, _Channel*>::iterator it = _audio_channels.begin();
         it != _audio_channels.end(); ++it) {
        delete it->second;
    }
    _audio_channels.clear();
    for (std::unordered_map<std::string, _WavData*>::iterator it = _wav_cache.begin();
         it != _wav_cache.end(); ++it) {
        delete it->second;
    }
    _wav_cache.clear();

#if GAMELIB_SDL_USE_MIXER_CHANNELS
    for (std::unordered_map<std::string, Mix_Chunk*>::iterator it = _chunk_cache.begin();
         it != _chunk_cache.end(); ++it) {
        Mix_FreeChunk(it->second);
    }
    _chunk_cache.clear();
    for (std::unordered_map<int, Mix_Chunk*>::iterator it = _temp_chunks.begin();
         it != _temp_chunks.end(); ++it) {
        Mix_FreeChunk(it->second);
    }
    _temp_chunks.clear();
#endif

    if (_sdlReady || SDL_WasInit(SDL_INIT_VIDEO)) {
        SDL_ShowCursor(SDL_ENABLE);
    }

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

    free(_textSrcRowMap);
    _textSrcRowMap = NULL;
    free(_textSrcColMap);
    _textSrcColMap = NULL;

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
        _mixerInitFlags = 0;
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
    _windowWidth = 0;
    _windowHeight = 0;
    _clipX = 0;
    _clipY = 0;
    _clipW = 0;
    _clipH = 0;
    _resizable = false;
    _aspectOffsetX = 0;
    _aspectOffsetY = 0;
    _aspectContentW = 0;
    _aspectContentH = 0;
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
    flags |= MIX_INIT_FLAC;
    flags |= MIX_INIT_MID;
    flags |= MIX_INIT_MOD;
    flags |= MIX_INIT_MP3;
    flags |= MIX_INIT_OGG;
    flags |= MIX_INIT_OPUS;
    _mixerInitFlags = Mix_Init(flags);
    if (Mix_OpenAudio(44100, AUDIO_S16SYS, 2, 1024) != 0) return false;
#if GAMELIB_SDL_USE_MIXER_CHANNELS
    Mix_AllocateChannels(32);
    Mix_ChannelFinished(_MixerChannelFinishedCallback);
#else
    Mix_AllocateChannels(8);
#endif
    _mixerReady = true;
    return true;
#else
    return false;
#endif
}

#if GAMELIB_SDL_USE_MIXER_CHANNELS
static GameLib *_gamelib_sdl_instance;

void GameLib::_MixerChannelFinishedCallback(int channel)
{
    if (!_gamelib_sdl_instance) return;
    std::unordered_map<int, Mix_Chunk*>::iterator it = _gamelib_sdl_instance->_temp_chunks.find(channel);
    if (it != _gamelib_sdl_instance->_temp_chunks.end()) {
        Mix_FreeChunk(it->second);
        _gamelib_sdl_instance->_temp_chunks.erase(it);
    }
}
#endif

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

#if defined(_WIN32)
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
#endif

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

#elif defined(__EMSCRIPTEN__)
    // WASM: fonts must be bundled into the virtual filesystem (MEMFS/preload)
    // No system font directories exist; rely on explicit file paths only.
    // The "default" and common family names map to bundled fonts if provided.
    if (normalized == "arial" || normalized == "sans" || normalized == "default") {
        _gamelib_push_font_candidate(candidates, "assets/fonts/Arial.ttf");
        _gamelib_push_font_candidate(candidates, "assets/fonts/FreeSans.ttf");
    }
    if (normalized == "microsoftyahei" || normalized == "yahei" || normalized == "simhei" ||
        normalized == "heiti" || normalized == "notosanscjk" || normalized == "notosanscjksc") {
        _gamelib_push_font_candidate(candidates, "assets/fonts/NotoSansCJK-Regular.ttc");
        _gamelib_push_font_candidate(candidates, "assets/fonts/msyh.ttc");
    }
    if (normalized == "couriernew" || normalized == "courier" || normalized == "monospace") {
        _gamelib_push_font_candidate(candidates, "assets/fonts/CourierNew.ttf");
        _gamelib_push_font_candidate(candidates, "assets/fonts/FreeMono.ttf");
    }
    if (normalized == "timesnewroman" || normalized == "times" || normalized == "serif") {
        _gamelib_push_font_candidate(candidates, "assets/fonts/TimesNewRoman.ttf");
        _gamelib_push_font_candidate(candidates, "assets/fonts/FreeSerif.ttf");
    }

    _gamelib_push_font_candidate(candidates, "assets/fonts/NotoSansCJK-Regular.ttc");
    _gamelib_push_font_candidate(candidates, "assets/fonts/FreeSans.ttf");

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
    if (!_ClipRectToCurrentClip(&x0, &y0, &x1, &y1)) return;

    for (int py = y0; py < y1; py++) {
        const uint32_t *srcRow = (const uint32_t*)((const unsigned char*)surface->pixels + (py - y) * surface->pitch);
        uint32_t *dstRow = _framebuffer + py * _width;
        for (int px = x0; px < x1; px++) {
            _gamelib_blend_pixel(dstRow + px, srcRow[px - x]);
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

void GameLib::_UpdateWindowSize()
{
    if (!_window) {
        _windowWidth = 0;
        _windowHeight = 0;
        return;
    }

    SDL_GetWindowSize(_window, &_windowWidth, &_windowHeight);
}

void GameLib::_SetMouseFromWindowCoords(int x, int y)
{
    if (_width <= 0 || _height <= 0 || _windowWidth <= 0 || _windowHeight <= 0) {
        _mouseX = 0;
        _mouseY = 0;
        return;
    }

    if (x < 0) x = 0;
    if (y < 0) y = 0;
    if (x >= _windowWidth) x = _windowWidth - 1;
    if (y >= _windowHeight) y = _windowHeight - 1;

    if (!_aspectLock) {
        _mouseX = (int)(((long long)x * (long long)_width) / (long long)_windowWidth);
        _mouseY = (int)(((long long)y * (long long)_height) / (long long)_windowHeight);
    } else {
        int scaleW = _windowWidth;
        int scaleH = (int)(((long long)_windowWidth * (long long)_height) / (long long)_width);
        if (scaleH > _windowHeight) {
            scaleH = _windowHeight;
            scaleW = (int)(((long long)_windowHeight * (long long)_width) / (long long)_height);
        }
        int offsetX = (_windowWidth - scaleW) / 2;
        int offsetY = (_windowHeight - scaleH) / 2;
        int cx = x - offsetX;
        int cy = y - offsetY;
        if (cx < 0) cx = 0;
        else if (cx >= scaleW) cx = scaleW - 1;
        if (cy < 0) cy = 0;
        else if (cy >= scaleH) cy = scaleH - 1;
        _mouseX = (int)(((long long)cx * (long long)_width) / (long long)scaleW);
        _mouseY = (int)(((long long)cy * (long long)_height) / (long long)scaleH);
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
    _SetMouseFromWindowCoords(mx, my);
    _mouseButtons[MOUSE_LEFT] = (mouseState & SDL_BUTTON(SDL_BUTTON_LEFT)) ? 1 : 0;
    _mouseButtons[MOUSE_RIGHT] = (mouseState & SDL_BUTTON(SDL_BUTTON_RIGHT)) ? 1 : 0;
    _mouseButtons[MOUSE_MIDDLE] = (mouseState & SDL_BUTTON(SDL_BUTTON_MIDDLE)) ? 1 : 0;
}

int GameLib::Open(int width, int height, const char *title, bool center, bool resizable)
{
#if GAMELIB_SDL_USE_MIXER_CHANNELS
    _gamelib_sdl_instance = this;
#endif
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
    _windowWidth = width;
    _windowHeight = height;
    _resizable = resizable;
    _title = title ? title : "";
    _closing = false;
    _focused = true;
    _minimized = false;
    _active = true;

    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0");

    int pos = center ? SDL_WINDOWPOS_CENTERED : SDL_WINDOWPOS_UNDEFINED;
    Uint32 windowFlags = SDL_WINDOW_SHOWN;
    if (resizable) windowFlags |= SDL_WINDOW_RESIZABLE;
    _window = SDL_CreateWindow(_title.c_str(), pos, pos, width, height, windowFlags);
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
    _uiActiveId = 0;
    ClearClip();
    _UpdateWindowSize();

    _perfFrequency = (uint64_t)SDL_GetPerformanceFrequency();
    if (_perfFrequency == 0) _perfFrequency = 1;
    _timeStartCounter = (uint64_t)SDL_GetPerformanceCounter();
    _timePrevCounter = _timeStartCounter;
    _fpsTimeCounter = _timeStartCounter;
    _frameStartCounter = _timeStartCounter;
    _deltaTime = 0.0;
    _fps = 0.0;
    _fpsAccum = 0.0;
    _SyncInputState();
    SDL_ShowCursor(_mouseVisible ? SDL_ENABLE : SDL_DISABLE);
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
            case SDL_WINDOWEVENT_RESIZED:
            case SDL_WINDOWEVENT_SIZE_CHANGED:
                _windowWidth = event.window.data1;
                _windowHeight = event.window.data2;
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
            case SDL_WINDOWEVENT_MAXIMIZED:
            case SDL_WINDOWEVENT_RESTORED:
            case SDL_WINDOWEVENT_SHOWN:
                _minimized = false;
                _UpdateWindowSize();
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
    _UpdateWindowSize();
    _SyncInputState();

    uint64_t now = (uint64_t)SDL_GetPerformanceCounter();
    uint64_t deltaCounter = now - _timePrevCounter;
    _deltaTime = (double)deltaCounter / (double)_perfFrequency;
    _timePrevCounter = now;

    _fpsAccum += 1.0;
    uint64_t fpsDelta = now - _fpsTimeCounter;
    if (fpsDelta >= _perfFrequency) {
        _fps = _fpsAccum * (double)_perfFrequency / (double)fpsDelta;
        _fpsAccum = 0.0;
        _fpsTimeCounter = now;
        _UpdateTitleFps();
    }

    // Process pending scene change
    if (_hasPendingScene) {
        _previousScene = _scene;
        _scene = _pendingScene;
        _sceneChanged = true;
        _hasPendingScene = false;
    } else {
        _sceneChanged = false;
    }

    SDL_UpdateTexture(_frameTexture, NULL, _framebuffer, _width * (int)sizeof(uint32_t));

    if (_aspectLock && (_windowWidth != _width || _windowHeight != _height)) {
        int scaleW = _windowWidth;
        int scaleH = (int)(((long long)_windowWidth * (long long)_height) / (long long)_width);
        if (scaleH > _windowHeight) {
            scaleH = _windowHeight;
            scaleW = (int)(((long long)_windowHeight * (long long)_width) / (long long)_height);
        }
        _aspectContentW = scaleW;
        _aspectContentH = scaleH;
        _aspectOffsetX = (_windowWidth - scaleW) / 2;
        _aspectOffsetY = (_windowHeight - scaleH) / 2;

        uint8_t r = (_aspectColor >> 16) & 0xff;
        uint8_t g = (_aspectColor >> 8) & 0xff;
        uint8_t b = _aspectColor & 0xff;
        SDL_SetRenderDrawColor(_renderer, r, g, b, 255);
        SDL_RenderClear(_renderer);
        SDL_Rect dstRect;
        dstRect.x = _aspectOffsetX;
        dstRect.y = _aspectOffsetY;
        dstRect.w = _aspectContentW;
        dstRect.h = _aspectContentH;
        SDL_RenderCopy(_renderer, _frameTexture, NULL, &dstRect);
    } else if (_windowWidth == _width && _windowHeight == _height) {
        SDL_RenderClear(_renderer);
        SDL_RenderCopy(_renderer, _frameTexture, NULL, NULL);
    } else if (_windowWidth > 0 && _windowHeight > 0) {
        SDL_RenderClear(_renderer);
        SDL_Rect dstRect;
        dstRect.x = 0;
        dstRect.y = 0;
        dstRect.w = _windowWidth;
        dstRect.h = _windowHeight;
        SDL_RenderCopy(_renderer, _frameTexture, NULL, &dstRect);
    } else {
        SDL_RenderClear(_renderer);
    }
    SDL_RenderPresent(_renderer);
}

void GameLib::WaitFrame(int fps)
{
    if (_perfFrequency == 0) return;
    if (fps <= 0) fps = 60;

    uint64_t frameTime = (uint64_t)((double)_perfFrequency / (double)fps);
    if (frameTime == 0) frameTime = 1;

    if (_frameStartCounter == 0) {
        _frameStartCounter = (uint64_t)SDL_GetPerformanceCounter();
    }

    uint64_t target = _frameStartCounter + frameTime;
    uint64_t now = (uint64_t)SDL_GetPerformanceCounter();

#if defined(__EMSCRIPTEN__)
    if (now < target) {
        uint64_t remaining = target - now;
        double remainingMs = (double)remaining * 1000.0 / (double)_perfFrequency;
        if (remainingMs > 0.0) {
            emscripten_sleep((int)remainingMs);
        }
    }
#else
    if (now >= target) {
        _frameStartCounter = now;
        return;
    }

    for (;;) {
        now = (uint64_t)SDL_GetPerformanceCounter();
        if (now >= target) break;

        uint64_t remaining = target - now;
        double remainingMs = (double)remaining * 1000.0 / (double)_perfFrequency;

        if (remainingMs > 1.5) {
            SDL_Delay(1);
        } else if (remainingMs > 0.3) {
            SDL_Delay(0);
        }
    }
#endif

    _frameStartCounter = target;
}

double GameLib::GetDeltaTime() const { return _deltaTime; }
double GameLib::GetFPS() const { return _fps; }
double GameLib::GetTime() const
{
    if (_perfFrequency == 0 || _timeStartCounter == 0) return 0.0;
    uint64_t now = (uint64_t)SDL_GetPerformanceCounter();
    return (double)(now - _timeStartCounter) / (double)_perfFrequency;
}
int GameLib::GetWidth() const { return _width; }
int GameLib::GetHeight() const { return _height; }
uint32_t *GameLib::GetFramebuffer() { return _framebuffer; }

void GameLib::WinResize(int width, int height)
{
    if (!_window || width <= 0 || height <= 0) return;

    if (_resizable) {
        Uint32 flags = SDL_GetWindowFlags(_window);
        if (flags & SDL_WINDOW_MAXIMIZED) {
            SDL_RestoreWindow(_window);
        }
    }

    SDL_SetWindowSize(_window, width, height);
    _UpdateWindowSize();
}

void GameLib::SetMaximized(bool maximized)
{
    if (!_window || !_resizable) return;
    if (maximized) {
        SDL_MaximizeWindow(_window);
    } else {
        SDL_RestoreWindow(_window);
    }
    _UpdateWindowSize();
}

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

void GameLib::ShowMouse(bool show)
{
    _mouseVisible = show;
    if (_sdlReady || SDL_WasInit(SDL_INIT_VIDEO)) {
        SDL_ShowCursor(_mouseVisible ? SDL_ENABLE : SDL_DISABLE);
    }
}

void GameLib::AspectLock(bool lock, uint32_t color)
{
    _aspectLock = lock;
    _aspectColor = color;
}

int GameLib::ShowMessage(const char *text, const char *title, int buttons)
{
    const char *messageText = text ? text : "";
    const char *messageTitle = title;
    if (!messageTitle || !messageTitle[0]) {
        messageTitle = _title.empty() ? "GameLib" : _title.c_str();
    }

    bool startedVideo = false;
    if (SDL_WasInit(SDL_INIT_VIDEO) == 0) {
        SDL_SetMainReady();
        if (SDL_Init(SDL_INIT_VIDEO) == 0) {
            startedVideo = true;
        }
    }

    bool restoreMouseHidden = !_mouseVisible && (SDL_WasInit(SDL_INIT_VIDEO) != 0);
    if (restoreMouseHidden) {
        SDL_ShowCursor(SDL_ENABLE);
    }

    int result = (buttons == MESSAGEBOX_YESNO) ? MESSAGEBOX_RESULT_NO : MESSAGEBOX_RESULT_OK;
    SDL_Window *parentWindow = _window;

    if (buttons == MESSAGEBOX_YESNO) {
        SDL_MessageBoxButtonData buttonData[2];
        memset(buttonData, 0, sizeof(buttonData));
        buttonData[0].flags = SDL_MESSAGEBOX_BUTTON_RETURNKEY_DEFAULT;
        buttonData[0].buttonid = MESSAGEBOX_RESULT_YES;
        buttonData[0].text = "Yes";
        buttonData[1].flags = SDL_MESSAGEBOX_BUTTON_ESCAPEKEY_DEFAULT;
        buttonData[1].buttonid = MESSAGEBOX_RESULT_NO;
        buttonData[1].text = "No";

        SDL_MessageBoxData boxData;
        memset(&boxData, 0, sizeof(boxData));
        boxData.flags = SDL_MESSAGEBOX_INFORMATION;
        boxData.window = parentWindow;
        boxData.title = messageTitle;
        boxData.message = messageText;
        boxData.numbuttons = 2;
        boxData.buttons = buttonData;

        int buttonId = MESSAGEBOX_RESULT_NO;
        if (SDL_ShowMessageBox(&boxData, &buttonId) == 0) {
            if (buttonId == MESSAGEBOX_RESULT_YES || buttonId == MESSAGEBOX_RESULT_NO) {
                result = buttonId;
            }
        }
    } else {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, messageTitle, messageText, parentWindow);
    }

    if (restoreMouseHidden) {
        SDL_ShowCursor(SDL_DISABLE);
    }
    if (startedVideo) {
        SDL_QuitSubSystem(SDL_INIT_VIDEO);
    }

    return result;
}

void GameLib::Screenshot(const char *filename)
{
    if (!filename) return;
    if (!_framebuffer || _width <= 0 || _height <= 0) return;

    SDL_RWops *rw = SDL_RWFromFile(filename, "wb");
    if (!rw) return;

    int rowSize = ((_width * 3 + 3) / 4) * 4;
    int imageSize = rowSize * _height;
    int fileSize = 14 + 40 + imageSize;

    // BMP file header (14 bytes)
    unsigned char fh[14];
    memset(fh, 0, 14);
    fh[0] = 'B'; fh[1] = 'M';
    fh[2] = (unsigned char)(fileSize);      fh[3] = (unsigned char)(fileSize >> 8);
    fh[4] = (unsigned char)(fileSize >> 16); fh[5] = (unsigned char)(fileSize >> 24);
    fh[10] = 14 + 40;

    // BMP info header (40 bytes): BITMAPINFOHEADER layout
    unsigned char ih[40];
    memset(ih, 0, 40);
    ih[0]  = 40;   // biSize
    ih[4]  = (unsigned char)(_width);       ih[5]  = (unsigned char)(_width >> 8);
    ih[6]  = (unsigned char)(_width >> 16);  ih[7]  = (unsigned char)(_width >> 24);
    ih[8]  = (unsigned char)(_height);      ih[9]  = (unsigned char)(_height >> 8);
    ih[10] = (unsigned char)(_height >> 16); ih[11] = (unsigned char)(_height >> 24);
    ih[12] = 1; ih[14] = 24;  // biPlanes=1, biBitCount=24
    ih[20] = (unsigned char)(imageSize);     ih[21] = (unsigned char)(imageSize >> 8);
    ih[22] = (unsigned char)(imageSize >> 16); ih[23] = (unsigned char)(imageSize >> 24);

    if (SDL_RWwrite(rw, fh, 14, 1) != 1 || SDL_RWwrite(rw, ih, 40, 1) != 1) {
        SDL_RWclose(rw);
        return;
    }

    unsigned char *rowBuf = (unsigned char*)malloc((size_t)rowSize);
    if (!rowBuf) {
        SDL_RWclose(rw);
        return;
    }

    for (int y = _height - 1; y >= 0; y--) {
        const uint32_t *src = _framebuffer + (size_t)y * _width;
        memset(rowBuf, 0, (size_t)rowSize);
        for (int x = 0; x < _width; x++) {
            rowBuf[x * 3 + 0] = (unsigned char)COLOR_GET_B(src[x]);
            rowBuf[x * 3 + 1] = (unsigned char)COLOR_GET_G(src[x]);
            rowBuf[x * 3 + 2] = (unsigned char)COLOR_GET_R(src[x]);
        }
        if (SDL_RWwrite(rw, rowBuf, (size_t)rowSize, 1) != 1) {
            free(rowBuf);
            SDL_RWclose(rw);
            return;
        }
    }

    free(rowBuf);
    SDL_RWclose(rw);
}

void GameLib::Clear(uint32_t color)
{
    if (!_framebuffer || _clipW <= 0 || _clipH <= 0) return;
    int clipY1 = _clipY + _clipH;
    uint32_t *firstRow = _framebuffer + _clipY * _width + _clipX;
    for (int x = 0; x < _clipW; x++) {
        firstRow[x] = color;
    }
    size_t rowBytes = (size_t)_clipW * sizeof(uint32_t);
    for (int y = _clipY + 1; y < clipY1; y++) {
        uint32_t *row = _framebuffer + y * _width + _clipX;
        memcpy(row, firstRow, rowBytes);
    }
}

void GameLib::SetPixel(int x, int y, uint32_t color)
{
    if (!_framebuffer) return;
    if (x >= _clipX && x < _clipX + _clipW && y >= _clipY && y < _clipY + _clipH) {
        _gamelib_blend_pixel(_framebuffer + y * _width + x, color);
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

void GameLib::SetClip(int x, int y, int w, int h)
{
    if (_width <= 0 || _height <= 0 || w <= 0 || h <= 0) {
        _clipX = 0;
        _clipY = 0;
        _clipW = 0;
        _clipH = 0;
        return;
    }

    int64_t x0 = (int64_t)x;
    int64_t y0 = (int64_t)y;
    int64_t x1 = x0 + (int64_t)w;
    int64_t y1 = y0 + (int64_t)h;

    if (x0 < 0) x0 = 0;
    if (y0 < 0) y0 = 0;
    if (x1 > _width) x1 = _width;
    if (y1 > _height) y1 = _height;

    if (x0 >= x1 || y0 >= y1) {
        _clipX = 0;
        _clipY = 0;
        _clipW = 0;
        _clipH = 0;
        return;
    }

    _clipX = (int)x0;
    _clipY = (int)y0;
    _clipW = (int)(x1 - x0);
    _clipH = (int)(y1 - y0);
}

void GameLib::ClearClip()
{
    if (_width <= 0 || _height <= 0) {
        _clipX = 0;
        _clipY = 0;
        _clipW = 0;
        _clipH = 0;
        return;
    }
    _clipX = 0;
    _clipY = 0;
    _clipW = _width;
    _clipH = _height;
}

void GameLib::GetClip(int *x, int *y, int *w, int *h) const
{
    if (x) *x = _clipX;
    if (y) *y = _clipY;
    if (w) *w = _clipW;
    if (h) *h = _clipH;
}

int GameLib::GetClipX() const { return _clipX; }
int GameLib::GetClipY() const { return _clipY; }
int GameLib::GetClipW() const { return _clipW; }
int GameLib::GetClipH() const { return _clipH; }

bool GameLib::_ClipRectToCurrentClip(int *x0, int *y0, int *x1, int *y1) const
{
    if (_clipW <= 0 || _clipH <= 0) return false;

    int clipX1 = _clipX + _clipW;
    int clipY1 = _clipY + _clipH;
    if (*x0 < _clipX) *x0 = _clipX;
    if (*y0 < _clipY) *y0 = _clipY;
    if (*x1 > clipX1) *x1 = clipX1;
    if (*y1 > clipY1) *y1 = clipY1;
    return *x0 < *x1 && *y0 < *y1;
}

void GameLib::_SetPixelFast(int x, int y, uint32_t color)
{
    _gamelib_blend_pixel(_framebuffer + y * _width + x, color);
}

enum {
    _GAMELIB_LINE_CLIP_LEFT   = 1,
    _GAMELIB_LINE_CLIP_RIGHT  = 2,
    _GAMELIB_LINE_CLIP_TOP    = 4,
    _GAMELIB_LINE_CLIP_BOTTOM = 8
};

static int _gamelib_line_clip_code(int x, int y, int left, int top, int right, int bottom)
{
    int code = 0;
    if (x < left) code |= _GAMELIB_LINE_CLIP_LEFT;
    else if (x > right) code |= _GAMELIB_LINE_CLIP_RIGHT;
    if (y < top) code |= _GAMELIB_LINE_CLIP_TOP;
    else if (y > bottom) code |= _GAMELIB_LINE_CLIP_BOTTOM;
    return code;
}

static bool _gamelib_clip_line_to_rect(int left, int top, int right, int bottom,
                                       int *x1, int *y1, int *x2, int *y2)
{
    int ax = *x1;
    int ay = *y1;
    int bx = *x2;
    int by = *y2;
    int codeA = _gamelib_line_clip_code(ax, ay, left, top, right, bottom);
    int codeB = _gamelib_line_clip_code(bx, by, left, top, right, bottom);

    for (;;) {
        if ((codeA | codeB) == 0) {
            *x1 = ax;
            *y1 = ay;
            *x2 = bx;
            *y2 = by;
            return true;
        }
        if ((codeA & codeB) != 0) {
            return false;
        }

        int outCode = codeA ? codeA : codeB;
        int nx = ax;
        int ny = ay;

        if (outCode & _GAMELIB_LINE_CLIP_TOP) {
            if (by == ay) return false;
            nx = ax + (int)(((int64_t)(bx - ax) * (top - ay)) / (by - ay));
            ny = top;
        } else if (outCode & _GAMELIB_LINE_CLIP_BOTTOM) {
            if (by == ay) return false;
            nx = ax + (int)(((int64_t)(bx - ax) * (bottom - ay)) / (by - ay));
            ny = bottom;
        } else if (outCode & _GAMELIB_LINE_CLIP_RIGHT) {
            if (bx == ax) return false;
            ny = ay + (int)(((int64_t)(by - ay) * (right - ax)) / (bx - ax));
            nx = right;
        } else {
            if (bx == ax) return false;
            ny = ay + (int)(((int64_t)(by - ay) * (left - ax)) / (bx - ax));
            nx = left;
        }

        if (outCode == codeA) {
            ax = nx;
            ay = ny;
            codeA = _gamelib_line_clip_code(ax, ay, left, top, right, bottom);
        } else {
            bx = nx;
            by = ny;
            codeB = _gamelib_line_clip_code(bx, by, left, top, right, bottom);
        }
    }
}

void GameLib::DrawLine(int x1, int y1, int x2, int y2, uint32_t color)
{
    if (_clipW <= 0 || _clipH <= 0) return;
    if (!_gamelib_clip_line_to_rect(_clipX, _clipY, _clipX + _clipW - 1, _clipY + _clipH - 1,
                                    &x1, &y1, &x2, &y2)) return;

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
    if (!_framebuffer || _clipW <= 0 || _clipH <= 0) return;
    if (y < _clipY || y >= _clipY + _clipH) return;
    if (x1 > x2) { int t = x1; x1 = x2; x2 = t; }
    if (x1 < _clipX) x1 = _clipX;
    if (x2 >= _clipX + _clipW) x2 = _clipX + _clipW - 1;
    if (x1 > x2) return;
    uint32_t *row = _framebuffer + y * _width;
    if (COLOR_GET_A(color) == 255) {
        for (int x = x1; x <= x2; x++) {
            row[x] = color;
        }
    } else {
        for (int x = x1; x <= x2; x++) {
            _gamelib_blend_pixel(row + x, color);
        }
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
    if (!_ClipRectToCurrentClip(&x1, &y1, &x2, &y2)) return;

    int rw = x2 - x1;
    size_t rowBytes = (size_t)rw * sizeof(uint32_t);
    if (COLOR_GET_A(color) == 255) {
        uint32_t *firstRow = _framebuffer + y1 * _width + x1;
        for (int i = 0; i < rw; i++) firstRow[i] = color;
        for (int j = y1 + 1; j < y2; j++) {
            memcpy(_framebuffer + j * _width + x1, firstRow, rowBytes);
        }
    } else {
        for (int j = y1; j < y2; j++) {
            uint32_t *row = _framebuffer + j * _width;
            for (int i = x1; i < x2; i++) {
                _gamelib_blend_pixel(row + i, color);
            }
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
        _gamelib_plot_circle_points_unique(this, cx, cy, x, y, color);
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
    FillEllipse(cx, cy, r, r, color);
}

void GameLib::DrawEllipse(int cx, int cy, int rx, int ry, uint32_t color)
{
    if (rx < 0 || ry < 0) return;
    if (rx == 0 && ry == 0) {
        SetPixel(cx, cy, color);
        return;
    }
    if (rx == 0) {
        DrawLine(cx, cy - ry, cx, cy + ry, color);
        return;
    }
    if (ry == 0) {
        DrawLine(cx - rx, cy, cx + rx, cy, color);
        return;
    }

    int64_t rx2 = (int64_t)rx * (int64_t)rx;
    int64_t ry2 = (int64_t)ry * (int64_t)ry;
    int64_t twoRx2 = 2 * rx2;
    int64_t twoRy2 = 2 * ry2;
    int64_t x = 0;
    int64_t y = ry;
    int64_t px = 0;
    int64_t py = twoRx2 * y;

    double p = (double)ry2 - (double)rx2 * (double)ry + 0.25 * (double)rx2;
    while (px < py) {
        _gamelib_plot_ellipse_points_unique(this, cx, cy, (int)x, (int)y, color);
        x++;
        px += twoRy2;
        if (p < 0.0) {
            p += (double)ry2 + (double)px;
        } else {
            y--;
            py -= twoRx2;
            p += (double)ry2 + (double)px - (double)py;
        }
    }

    p = (double)ry2 * ((double)x + 0.5) * ((double)x + 0.5)
      + (double)rx2 * ((double)y - 1.0) * ((double)y - 1.0)
      - (double)rx2 * (double)ry2;
    while (y >= 0) {
        _gamelib_plot_ellipse_points_unique(this, cx, cy, (int)x, (int)y, color);
        y--;
        py -= twoRx2;
        if (p > 0.0) {
            p += (double)rx2 - (double)py;
        } else {
            x++;
            px += twoRy2;
            p += (double)rx2 - (double)py + (double)px;
        }
    }
}

void GameLib::FillEllipse(int cx, int cy, int rx, int ry, uint32_t color)
{
    if (rx < 0 || ry < 0) return;
    if (rx == 0 && ry == 0) {
        SetPixel(cx, cy, color);
        return;
    }
    if (rx == 0) {
        DrawLine(cx, cy - ry, cx, cy + ry, color);
        return;
    }
    if (ry == 0) {
        DrawLine(cx - rx, cy, cx + rx, cy, color);
        return;
    }

    double ry2 = (double)ry * (double)ry;
    for (int y = -ry; y <= ry; y++) {
        double ratio = 1.0 - ((double)y * (double)y) / ry2;
        if (ratio < 0.0) ratio = 0.0;
        int x = _gamelib_round_to_int(sqrt(ratio) * (double)rx);
        _DrawHLine(cx - x, cx + x, cy + y, color);
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
            if (bits == 0) continue;
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

void GameLib::DrawTextScale(int x, int y, const char *text, uint32_t color, int w, int h)
{
    if (!text || w <= 0 || h <= 0 || w > 1024 || h > 1024) return;
    if (w == 8 && h == 8) { DrawText(x, y, text, color); return; }
    if (!_textSrcRowMap) {
        _textSrcRowMap = (int*)malloc(1024 * sizeof(int));
        _textSrcColMap = (int*)malloc(1024 * sizeof(int));
        if (!_textSrcRowMap || !_textSrcColMap) return;
    }
    for (int i = 0; i < h; i++) _textSrcRowMap[i] = i * 8 / h;
    for (int i = 0; i < w; i++) _textSrcColMap[i] = i * 8 / w;
    if (!_framebuffer) return;
    bool opaque = COLOR_GET_A(color) == 255;
    int clipX1 = _clipX + _clipW;
    int clipY1 = _clipY + _clipH;
    int ox = x;
    for (const char *p = text; *p; p++) {
        unsigned char ch = (unsigned char)*p;
        if (ch == '\n') {
            x = ox;
            y += h + h / 4;
            continue;
        }
        if (ch < 32 || ch > 126) continue;
        const unsigned char *glyph = _gamelib_font8x8[ch - 32];
        for (int row = 0; row < h; row++) {
            int dy = y + row;
            if (dy < _clipY || dy >= clipY1) continue;
            unsigned char bits = glyph[_textSrcRowMap[row]];
            if (bits == 0) continue;
            uint32_t *dstRow = _framebuffer + (size_t)dy * _width;
            for (int col = 0; col < w; col++) {
                int dx = x + col;
                if (dx < _clipX || dx >= clipX1) continue;
                if (bits & (0x80 >> _textSrcColMap[col])) {
                    if (opaque) dstRow[dx] = color;
                    else _gamelib_blend_pixel(dstRow + dx, color);
                }
            }
        }
        x += w;
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

void GameLib::DrawPrintfScale(int x, int y, uint32_t color, int w, int h, const char *fmt, ...)
{
    char buf[1024];
    va_list args;
    va_start(args, fmt);
    vsnprintf(buf, sizeof(buf), fmt, args);
    va_end(args);
    buf[sizeof(buf) - 1] = '\0';
    DrawTextScale(x, y, buf, color, w, h);
}

bool GameLib::Button(int x, int y, int w, int h, const char *text, uint32_t color)
{
    if (w <= 0 || h <= 0) return false;

    uint32_t id = _gamelib_ui_make_id(0x42544E31u, x, y, w, h, text);
    bool hovered = PointInRect(_mouseX, _mouseY, x, y, w, h);
    bool mousePressed = IsMousePressed(MOUSE_LEFT);
    bool mouseReleased = IsMouseReleased(MOUSE_LEFT);
    bool mouseDown = IsMouseDown(MOUSE_LEFT);

    if (mousePressed && hovered) {
        _uiActiveId = id;
    }

    bool pressed = (_uiActiveId == id) && mouseDown && hovered;
    uint32_t face = color;
    if (pressed) face = _gamelib_ui_darken(color, 36);
    else if (hovered) face = _gamelib_ui_lighten(color, 46);

    _gamelib_ui_draw_bevel_rect(this, x, y, w, h, face, pressed);

    int textWidth = _gamelib_ui_text_width(text);
    int textHeight = _gamelib_ui_text_height(text);
    int textX = x + ((w - textWidth) > 0 ? (w - textWidth) / 2 : 0);
    int textY = y + ((h - textHeight) > 0 ? (h - textHeight) / 2 : 0);
    if (pressed) {
        textX += 1;
        textY += 1;
    }

    uint32_t textColor = _gamelib_ui_button_text_color(face);
    uint32_t shadowColor = (textColor == COLOR_WHITE)
        ? COLOR_ARGB(160, 0, 0, 0)
        : COLOR_ARGB(112, 255, 255, 255);
    _gamelib_ui_draw_text_with_shadow(this, textX, textY, text, textColor, shadowColor);

    bool clicked = mouseReleased && hovered && (_uiActiveId == id);
    if (mouseReleased && _uiActiveId == id) {
        _uiActiveId = 0;
    }
    return clicked;
}

bool GameLib::Checkbox(int x, int y, const char *text, bool *checked)
{
    if (!checked) return false;

    const int boxSize = 16;
    const int gap = (text && text[0]) ? 6 : 0;
    int labelWidth = _gamelib_ui_text_width(text);
    int labelHeight = _gamelib_ui_text_height(text);
    int controlWidth = boxSize + gap + labelWidth;
    int controlHeight = boxSize;
    if (labelHeight > controlHeight) controlHeight = labelHeight;
    if (controlWidth <= 0) controlWidth = boxSize;

    int boxY = y + (controlHeight - boxSize) / 2;
    int labelY = y + (controlHeight - labelHeight) / 2;
    if (labelHeight <= 0) labelY = y + (controlHeight - 8) / 2;

    uint32_t id = _gamelib_ui_make_id(0x43484B31u, x, y, controlWidth, controlHeight, text);
    bool hovered = PointInRect(_mouseX, _mouseY, x, y, controlWidth, controlHeight);
    bool mousePressed = IsMousePressed(MOUSE_LEFT);
    bool mouseReleased = IsMouseReleased(MOUSE_LEFT);
    bool mouseDown = IsMouseDown(MOUSE_LEFT);

    if (mousePressed && hovered) {
        _uiActiveId = id;
    }

    bool pressed = (_uiActiveId == id) && mouseDown && hovered;
    uint32_t boxFace = hovered
        ? _gamelib_ui_lighten(COLOR_RGB(182, 182, 182), 32)
        : COLOR_RGB(182, 182, 182);
    if (pressed) boxFace = _gamelib_ui_darken(boxFace, 28);
    _gamelib_ui_draw_bevel_rect(this, x, boxY, boxSize, boxSize, boxFace, pressed);

    if (*checked) {
        uint32_t markColor = hovered ? COLOR_BLACK : COLOR_DARK_GRAY;
        _gamelib_ui_draw_checkbox_mark(this, x, boxY, boxSize, pressed, markColor);
    }

    if (text && text[0]) {
        uint32_t labelColor = hovered ? COLOR_WHITE : COLOR_LIGHT_GRAY;
        _gamelib_ui_draw_text_with_shadow(this, x + boxSize + gap, labelY, text,
                                          labelColor, COLOR_ARGB(160, 0, 0, 0));
    }

    bool changed = false;
    if (mouseReleased && _uiActiveId == id) {
        if (hovered) {
            *checked = !*checked;
            changed = true;
        }
        _uiActiveId = 0;
    }
    return changed;
}

static void _gamelib_ui_draw_radio_circle(GameLib *game, int x, int y, int size,
                                           bool selected, bool pressed, uint32_t face,
                                           uint32_t dotColor)
{
    if (!game || size <= 0) return;

    int cx = x + size / 2;
    int cy = y + size / 2;
    int r = size / 2;
    if (r < 4) r = 4;

    if (pressed) {
        cx += 1;
        cy += 1;
    }

    game->FillCircle(cx, cy, r, face);

    uint32_t lightColor = _gamelib_ui_lighten(face, 112);
    uint32_t darkColor = _gamelib_ui_darken(face, 112);
    if (pressed) {
        uint32_t tmp = lightColor; lightColor = darkColor; darkColor = tmp;
    }
    game->DrawCircle(cx, cy, r, darkColor);
    game->DrawCircle(cx, cy, r - 1, lightColor);

    if (selected) {
        int dotR = r / 2;
        if (dotR < 3) dotR = 3;
        game->FillCircle(cx, cy, dotR, dotColor);
    }
}

bool GameLib::RadioBox(int x, int y, const char *text, int *value, int index)
{
    if (!value) return false;

    const int boxSize = 16;
    const int gap = (text && text[0]) ? 6 : 0;
    int labelWidth = _gamelib_ui_text_width(text);
    int labelHeight = _gamelib_ui_text_height(text);
    int controlWidth = boxSize + gap + labelWidth;
    int controlHeight = boxSize;
    if (labelHeight > controlHeight) controlHeight = labelHeight;
    if (controlWidth <= 0) controlWidth = boxSize;

    int boxY = y + (controlHeight - boxSize) / 2;
    int labelY = y + (controlHeight - labelHeight) / 2;
    if (labelHeight <= 0) labelY = y + (controlHeight - 8) / 2;

    uint32_t id = _gamelib_ui_make_id(0x52444F31u, x, y, controlWidth, controlHeight, text);
    bool hovered = PointInRect(_mouseX, _mouseY, x, y, controlWidth, controlHeight);
    bool mousePressed = IsMousePressed(MOUSE_LEFT);
    bool mouseReleased = IsMouseReleased(MOUSE_LEFT);
    bool mouseDown = IsMouseDown(MOUSE_LEFT);

    if (mousePressed && hovered) {
        _uiActiveId = id;
    }

    bool pressed = (_uiActiveId == id) && mouseDown && hovered;

    bool willSelect = (mouseReleased && hovered && _uiActiveId == id) ? true : (*value == index);
    uint32_t face = hovered
        ? _gamelib_ui_lighten(COLOR_RGB(182, 182, 182), 32)
        : COLOR_RGB(182, 182, 182);
    if (pressed) face = _gamelib_ui_darken(face, 28);
    uint32_t dotColor = hovered ? COLOR_BLACK : COLOR_DARK_GRAY;

    _gamelib_ui_draw_radio_circle(this, x, boxY, boxSize, willSelect, pressed, face, dotColor);

    if (text && text[0]) {
        uint32_t labelColor = hovered ? COLOR_WHITE : COLOR_LIGHT_GRAY;
        _gamelib_ui_draw_text_with_shadow(this, x + boxSize + gap, labelY, text,
                                          labelColor, COLOR_ARGB(160, 0, 0, 0));
    }

    bool changed = false;
    if (mouseReleased && _uiActiveId == id) {
        if (hovered && *value != index) {
            *value = index;
            changed = true;
        }
        _uiActiveId = 0;
    }
    return changed;
}

bool GameLib::ToggleButton(int x, int y, int w, int h, const char *text,
                           bool *toggled, uint32_t color)
{
    if (w <= 0 || h <= 0 || !toggled) return false;

    uint32_t id = _gamelib_ui_make_id(0x54474231u, x, y, w, h, text);
    bool hovered = PointInRect(_mouseX, _mouseY, x, y, w, h);
    bool mousePressed = IsMousePressed(MOUSE_LEFT);
    bool mouseReleased = IsMouseReleased(MOUSE_LEFT);
    bool mouseDown = IsMouseDown(MOUSE_LEFT);

    if (mousePressed && hovered) {
        _uiActiveId = id;
    }

    bool pressed = (_uiActiveId == id) && mouseDown && hovered;

    bool willToggle = (mouseReleased && hovered && _uiActiveId == id);
    bool on = willToggle ? !*toggled : *toggled;

    bool bevelPressed = pressed || on;
    uint32_t face = color;
    if (on && !pressed) face = _gamelib_ui_darken(color, 24);
    else if (pressed) face = _gamelib_ui_darken(color, 36);
    else if (hovered) face = _gamelib_ui_lighten(color, 46);

    _gamelib_ui_draw_bevel_rect(this, x, y, w, h, face, bevelPressed);

    int textWidth = _gamelib_ui_text_width(text);
    int textHeight = _gamelib_ui_text_height(text);
    int textX = x + ((w - textWidth) > 0 ? (w - textWidth) / 2 : 0);
    int textY = y + ((h - textHeight) > 0 ? (h - textHeight) / 2 : 0);
    if (bevelPressed) {
        textX += 1;
        textY += 1;
    }

    uint32_t textColor = _gamelib_ui_button_text_color(face);
    uint32_t shadowColor = (textColor == COLOR_WHITE)
        ? COLOR_ARGB(160, 0, 0, 0)
        : COLOR_ARGB(112, 255, 255, 255);
    _gamelib_ui_draw_text_with_shadow(this, textX, textY, text, textColor, shadowColor);

    bool changed = false;
    if (mouseReleased && _uiActiveId == id) {
        if (hovered) {
            *toggled = !*toggled;
            changed = true;
        }
        _uiActiveId = 0;
    }
    return changed;
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

void GameLib::DrawPrintfFont(int x, int y, uint32_t color, const char *fontName, int fontSize, const char *fmt, ...)
{
    char buf[1024];
    va_list args;
    va_start(args, fmt);
    vsnprintf(buf, sizeof(buf), fmt, args);
    va_end(args);
    buf[sizeof(buf) - 1] = '\0';
    DrawTextFont(x, y, buf, color, fontName, fontSize);
}

void GameLib::DrawPrintfFont(int x, int y, uint32_t color, int fontSize, const char *fmt, ...)
{
    char buf[1024];
    va_list args;
    va_start(args, fmt);
    vsnprintf(buf, sizeof(buf), fmt, args);
    va_end(args);
    buf[sizeof(buf) - 1] = '\0';
    DrawTextFont(x, y, buf, color, GAMELIB_SDL_DEFAULT_FONT, fontSize);
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
#if defined(__EMSCRIPTEN__)
        SDL_RWops *rw = SDL_RWFromFile(filename, "rb");
        SDL_Surface *loaded = NULL;
        if (rw) {
            loaded = IMG_Load_RW(rw, 1);
        }
#else
        SDL_Surface *loaded = IMG_Load(filename);
#endif
        if (loaded) {
            SDL_Surface *argb = SDL_ConvertSurfaceFormat(loaded, SDL_PIXELFORMAT_ARGB8888, 0);
            SDL_FreeSurface(loaded);
            if (!argb) return -1;

            if (argb->w <= 0 || argb->h <= 0 || argb->w > 16384 || argb->h > 16384) {
                SDL_FreeSurface(argb);
                return -1;
            }

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
    int clipX0 = _clipX;
    int clipY0 = _clipY;
    int clipX1 = _clipX + _clipW;
    int clipY1 = _clipY + _clipH;

    if (_clipW <= 0 || _clipH <= 0) return;
    if (x < clipX0) localX0 = clipX0 - x;
    if (y < clipY0) localY0 = clipY0 - y;
    if (x + sw > clipX1) localX1 = clipX1 - x;
    if (y + sh > clipY1) localY1 = clipY1 - y;

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

                int dx = x + localX;
                _gamelib_blend_pixel(dstRow + dx, c);
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
                dstRow[x + localX] = c;
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

    if (!_ClipRectToCurrentClip(&dx0, &dy0, &dx1, &dy1)) return;
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

            if (!useAlpha) {
                dstRow[dx] = c;
            } else {
                uint32_t sa = COLOR_GET_A(c);
                if (sa == 0) continue;
                _gamelib_blend_pixel(dstRow + dx, c);
            }
        }
    }
}

void GameLib::_DrawSpriteAreaRotated(int id, int cx, int cy, int sx, int sy, int sw, int sh,
                                     double angleDeg, int flags)
{
    if (id < 0 || id >= (int)_sprites.size()) return;
    if (!_sprites[id].used || !_framebuffer) return;
    if (sw <= 0 || sh <= 0) return;
    if (_clipW <= 0 || _clipH <= 0) return;

    GameSprite &spr = _sprites[id];
    double normalizedAngle = fmod(angleDeg, 360.0);
    if (normalizedAngle < 0.0) normalizedAngle += 360.0;
    if (normalizedAngle == 0.0) {
        _DrawSpriteAreaFast(id, cx - sw / 2, cy - sh / 2, sx, sy, sw, sh, flags);
        return;
    }

    bool flipH = (flags & SPRITE_FLIP_H) != 0;
    bool flipV = (flags & SPRITE_FLIP_V) != 0;
    bool useAlpha = (flags & SPRITE_ALPHA) != 0;
    bool useColorKey = (flags & SPRITE_COLORKEY) != 0;
    uint32_t colorKey = spr.colorKey;

    const double radians = normalizedAngle * (3.14159265358979323846 / 180.0);
    const double cosA = cos(radians);
    const double sinA = sin(radians);
    const double halfW = (double)sw * 0.5;
    const double halfH = (double)sh * 0.5;
    const double srcCenterX = (double)(sw - 1) * 0.5;
    const double srcCenterY = (double)(sh - 1) * 0.5;

    double cornerX[4] = { -halfW, halfW, halfW, -halfW };
    double cornerY[4] = { -halfH, -halfH, halfH, halfH };
    double minX = 0.0, maxX = 0.0, minY = 0.0, maxY = 0.0;
    for (int i = 0; i < 4; i++) {
        double rx = cornerX[i] * cosA - cornerY[i] * sinA;
        double ry = cornerX[i] * sinA + cornerY[i] * cosA;
        if (i == 0 || rx < minX) minX = rx;
        if (i == 0 || rx > maxX) maxX = rx;
        if (i == 0 || ry < minY) minY = ry;
        if (i == 0 || ry > maxY) maxY = ry;
    }

    int dx0 = (int)floor((double)cx + minX);
    int dy0 = (int)floor((double)cy + minY);
    int dx1 = (int)ceil((double)cx + maxX);
    int dy1 = (int)ceil((double)cy + maxY);
    if (!_ClipRectToCurrentClip(&dx0, &dy0, &dx1, &dy1)) return;

    for (int dy = dy0; dy < dy1; dy++) {
        double localY = (double)(dy - cy);
        uint32_t *dstRow = _framebuffer + dy * _width;

        for (int dx = dx0; dx < dx1; dx++) {
            double localX = (double)(dx - cx);
            int srcLocalX = _gamelib_round_to_int(localX * cosA + localY * sinA + srcCenterX);
            int srcLocalY = _gamelib_round_to_int(-localX * sinA + localY * cosA + srcCenterY);
            if (srcLocalX < 0 || srcLocalX >= sw || srcLocalY < 0 || srcLocalY >= sh) continue;

            if (flipH) srcLocalX = sw - 1 - srcLocalX;
            if (flipV) srcLocalY = sh - 1 - srcLocalY;

            int srcX = sx + srcLocalX;
            int srcY = sy + srcLocalY;
            if (srcX < 0 || srcX >= spr.width || srcY < 0 || srcY >= spr.height) continue;

            uint32_t c = spr.pixels[srcY * spr.width + srcX];
            if (useColorKey && c == colorKey) continue;

            if (!useAlpha) {
                dstRow[dx] = c;
            } else {
                uint32_t sa = COLOR_GET_A(c);
                if (sa == 0) continue;
                _gamelib_blend_pixel(dstRow + dx, c);
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

void GameLib::DrawSpriteRotated(int id, int cx, int cy, double angleDeg, int flags)
{
    if (id < 0 || id >= (int)_sprites.size()) return;
    if (!_sprites[id].used) return;
    _DrawSpriteAreaRotated(id, cx, cy, 0, 0, _sprites[id].width, _sprites[id].height,
                           angleDeg, flags);
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

void GameLib::DrawSpriteFrameRotated(int id, int cx, int cy, int frameW, int frameH, int frameIndex,
                                     double angleDeg, int flags)
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
    _DrawSpriteAreaRotated(id, cx, cy, sx, sy, frameW, frameH, angleDeg, flags);
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


//=====================================================================
// Tilemap System
//=====================================================================
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

int GameLib::_GetTilesetTileCount(int tilesetId, int tileSize) const
{
    if (tileSize <= 0) return 0;
    if (tilesetId < 0 || tilesetId >= (int)_sprites.size()) return 0;
    if (!_sprites[tilesetId].used) return 0;

    int cols = _sprites[tilesetId].width / tileSize;
    int rows = _sprites[tilesetId].height / tileSize;
    if (cols <= 0 || rows <= 0) return 0;
    return cols * rows;
}

int GameLib::CreateTilemap(int cols, int rows, int tileSize, int tilesetId)
{
    if (cols <= 0 || rows <= 0 || tileSize <= 0) return -1;
    if (tilesetId < 0 || tilesetId >= (int)_sprites.size()) return -1;
    if (!_sprites[tilesetId].used) return -1;
    if (cols > 4096 || rows > 4096) return -1;
    int tileCount = _GetTilesetTileCount(tilesetId, tileSize);
    if (tileCount <= 0) return -1;

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

bool GameLib::SaveTilemap(const char *filename, int mapId) const
{
    if (!filename) return false;
    if (mapId < 0 || mapId >= (int)_tilemaps.size()) return false;
    if (!_tilemaps[mapId].used) return false;

    SDL_RWops *rw = SDL_RWFromFile(filename, "wb");
    if (!rw) return false;

    const GameTilemap &tm = _tilemaps[mapId];
    char buffer[64];
    int headerLen = snprintf(buffer, sizeof(buffer), "GLM1\n%d %d %d\n", tm.tileSize, tm.rows, tm.cols);
    if (headerLen <= 0 || !_gamelib_rw_write_text(rw, buffer, (size_t)headerLen)) {
        SDL_RWclose(rw);
        return false;
    }

    for (int row = 0; row < tm.rows; row++) {
        std::string line;
        for (int col = 0; col < tm.cols; col++) {
            if (col > 0) line.push_back(' ');
            int valueLen = snprintf(buffer, sizeof(buffer), "%d", tm.tiles[row * tm.cols + col]);
            if (valueLen <= 0) {
                SDL_RWclose(rw);
                return false;
            }
            line.append(buffer, (size_t)valueLen);
        }
        line.push_back('\n');
        if (!_gamelib_rw_write_text(rw, line.c_str(), line.size())) {
            SDL_RWclose(rw);
            return false;
        }
    }

    return SDL_RWclose(rw) == 0;
}

int GameLib::LoadTilemap(const char *filename, int tilesetId)
{
    if (!filename) return -1;

    SDL_RWops *rw = SDL_RWFromFile(filename, "rb");
    if (!rw) return -1;

    std::string line;
    if (!_gamelib_rw_read_text_line(rw, line)) {
        SDL_RWclose(rw);
        return -1;
    }
    _gamelib_strip_utf8_bom(line);
    if (line != "GLM1") {
        SDL_RWclose(rw);
        return -1;
    }

    int header[3];
    int headerCount = 0;
    if (!_gamelib_rw_read_text_line(rw, line) ||
        !_gamelib_parse_int_tokens(line, header, 3, &headerCount) ||
        headerCount < 3) {
        SDL_RWclose(rw);
        return -1;
    }

    int tileSize = header[0];
    int rows = header[1];
    int cols = header[2];
    int mapId = CreateTilemap(cols, rows, tileSize, tilesetId);
    if (mapId < 0) {
        SDL_RWclose(rw);
        return -1;
    }

    for (int row = 0; row < rows; row++) {
        if (!_gamelib_rw_read_text_line(rw, line)) break;

        int count = 0;
        int *rowPtr = _tilemaps[mapId].tiles + row * cols;
        if (!_gamelib_parse_int_tokens(line, rowPtr, cols, &count)) {
            FreeTilemap(mapId);
            SDL_RWclose(rw);
            return -1;
        }
        for (int col = 0; col < count; col++) {
            if (rowPtr[col] < -1) {
                FreeTilemap(mapId);
                SDL_RWclose(rw);
                return -1;
            }
        }
    }

    SDL_RWclose(rw);
    return mapId;
}

void GameLib::FreeTilemap(int mapId)
{
    if (mapId < 0 || mapId >= (int)_tilemaps.size()) return;
    if (!_tilemaps[mapId].used) return;
    if (_tilemaps[mapId].tiles) {
        free(_tilemaps[mapId].tiles);
        _tilemaps[mapId].tiles = NULL;
    }
    _tilemaps[mapId].tilesetId = -1;
    _tilemaps[mapId].tilesetCols = 0;
    _tilemaps[mapId].used = false;
}

void GameLib::SetTile(int mapId, int col, int row, int tileId)
{
    if (mapId < 0 || mapId >= (int)_tilemaps.size()) return;
    if (!_tilemaps[mapId].used) return;
    if (col < 0 || col >= _tilemaps[mapId].cols) return;
    if (row < 0 || row >= _tilemaps[mapId].rows) return;
    if (tileId < -1) return;
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
    if (tileId < -1) return;

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
    if (tileId < -1) return;

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
    int tsCols = tset.width / ts;
    int tileCount = _GetTilesetTileCount(tsId, ts);
    tm.tilesetCols = tsCols;
    if (tsCols <= 0 || tileCount <= 0 || _clipW <= 0 || _clipH <= 0) return;

    int clipX0 = _clipX;
    int clipY0 = _clipY;
    int clipX1 = _clipX + _clipW;
    int clipY1 = _clipY + _clipH;

    int col0 = (clipX0 - x) / ts;
    int row0 = (clipY0 - y) / ts;
    int col1 = (clipX1 - 1 - x) / ts + 1;
    int row1 = (clipY1 - 1 - y) / ts + 1;
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
            if (tid < 0 || tid >= tileCount) continue;

            int srcCol = tid % tsCols;
            int srcRow = tid / tsCols;
            int srcX0 = srcCol * ts;
            int srcY0 = srcRow * ts;

            int dstX0 = x + c * ts;
            int dstY0 = y + r * ts;

            if (canMemcpyTiles) {
                int ix0 = 0, iy0 = 0, ix1 = ts, iy1 = ts;
                if (dstX0 < clipX0) ix0 = clipX0 - dstX0;
                if (dstY0 < clipY0) iy0 = clipY0 - dstY0;
                if (dstX0 + ix1 > clipX1) ix1 = clipX1 - dstX0;
                if (dstY0 + iy1 > clipY1) iy1 = clipY1 - dstY0;
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


//=====================================================================
// Keyboard & Mouse
//=====================================================================
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

#if GAMELIB_SDL_HAS_MIXER
static char _gamelib_sdl_music_ascii_tolower(char ch)
{
    if (ch >= 'A' && ch <= 'Z') return (char)(ch - 'A' + 'a');
    return ch;
}

static bool _gamelib_sdl_path_has_music_extension(const char *filename, const char *extension)
{
    if (!filename || !extension || !extension[0]) return false;

    const char *dot = strrchr(filename, '.');
    if (!dot || !dot[1]) return false;

    const char *lhs = dot + 1;
    const char *rhs = extension;
    while (*lhs && *rhs) {
        if (_gamelib_sdl_music_ascii_tolower(*lhs) != _gamelib_sdl_music_ascii_tolower(*rhs)) return false;
        lhs++;
        rhs++;
    }
    return *lhs == '\0' && *rhs == '\0';
}

static bool _gamelib_sdl_is_midi_music_path(const char *filename)
{
    return _gamelib_sdl_path_has_music_extension(filename, "mid") ||
           _gamelib_sdl_path_has_music_extension(filename, "midi");
}

static bool _gamelib_sdl_is_mp3_music_path(const char *filename)
{
    return _gamelib_sdl_path_has_music_extension(filename, "mp3");
}

static bool _gamelib_sdl_is_ogg_music_path(const char *filename)
{
    return _gamelib_sdl_path_has_music_extension(filename, "ogg");
}

static bool _gamelib_sdl_is_flac_music_path(const char *filename)
{
    return _gamelib_sdl_path_has_music_extension(filename, "flac");
}

static bool _gamelib_sdl_check_music_format_supported(const char *filename, int mixerInitFlags)
{
    if (_gamelib_sdl_is_midi_music_path(filename)) {
        return (mixerInitFlags & MIX_INIT_MID) != 0;
    }
    if (_gamelib_sdl_is_mp3_music_path(filename)) {
        return (mixerInitFlags & MIX_INIT_MP3) != 0;
    }
    if (_gamelib_sdl_is_ogg_music_path(filename)) {
        return (mixerInitFlags & MIX_INIT_OGG) != 0;
    }
    if (_gamelib_sdl_is_flac_music_path(filename)) {
        return (mixerInitFlags & MIX_INIT_FLAC) != 0;
    }
    // WAV and other formats are always supported by SDL_mixer
    return true;
}
#endif

bool GameLib::PlayMusic(const char *filename, bool loop)
{
#if GAMELIB_SDL_HAS_MIXER
    if (!filename) return false;

    if (!_EnsureMixerReady()) return false;

    if (!_gamelib_sdl_check_music_format_supported(filename, _mixerInitFlags)) {
        StopMusic();
        return false;
    }

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

void GameLib::SetScene(int scene)
{
    _pendingScene = scene;
    _hasPendingScene = true;
}

int GameLib::GetScene() const { return _scene; }
bool GameLib::IsSceneChanged() const { return _sceneChanged; }
int GameLib::GetPreviousScene() const { return _previousScene; }

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



static uint32_t _gamelib_ui_lighten(uint32_t color, int amount)
{
    if (amount <= 0) return color;
    if (amount > 255) amount = 255;

    int r = COLOR_GET_R(color);
    int g = COLOR_GET_G(color);
    int b = COLOR_GET_B(color);

    r += ((255 - r) * amount) / 255;
    g += ((255 - g) * amount) / 255;
    b += ((255 - b) * amount) / 255;

    return COLOR_ARGB(COLOR_GET_A(color), r, g, b);
}

static uint32_t _gamelib_ui_darken(uint32_t color, int amount)
{
    if (amount <= 0) return color;
    if (amount > 255) amount = 255;

    int scale = 255 - amount;
    int r = (COLOR_GET_R(color) * scale) / 255;
    int g = (COLOR_GET_G(color) * scale) / 255;
    int b = (COLOR_GET_B(color) * scale) / 255;

    return COLOR_ARGB(COLOR_GET_A(color), r, g, b);
}

static int _gamelib_ui_text_width(const char *text)
{
    if (!text || !text[0]) return 0;

    int lineWidth = 0;
    int maxWidth = 0;
    for (const char *p = text; *p; ++p) {
        unsigned char ch = (unsigned char)*p;
        if (ch == '\n') {
            if (lineWidth > maxWidth) maxWidth = lineWidth;
            lineWidth = 0;
            continue;
        }
        if (ch >= 32 && ch <= 126) lineWidth += 8;
    }
    if (lineWidth > maxWidth) maxWidth = lineWidth;
    return maxWidth;
}

static int _gamelib_ui_text_height(const char *text)
{
    if (!text || !text[0]) return 0;

    int lines = 1;
    for (const char *p = text; *p; ++p) {
        if (*p == '\n') lines++;
    }
    return lines * 8 + (lines - 1) * 2;
}

static uint32_t _gamelib_ui_button_text_color(uint32_t color)
{
    int r = COLOR_GET_R(color);
    int g = COLOR_GET_G(color);
    int b = COLOR_GET_B(color);
    int luma = r * 299 + g * 587 + b * 114;
    return (luma >= 140000) ? COLOR_BLACK : COLOR_WHITE;
}

static uint32_t _gamelib_ui_make_id(uint32_t kind, int x, int y, int w, int h, const char *text)
{
    uint32_t hash = 2166136261u;
    hash ^= kind; hash *= 16777619u;
    hash ^= (uint32_t)x; hash *= 16777619u;
    hash ^= (uint32_t)y; hash *= 16777619u;
    hash ^= (uint32_t)w; hash *= 16777619u;
    hash ^= (uint32_t)h; hash *= 16777619u;

    if (text) {
        for (const unsigned char *p = (const unsigned char *)text; *p; ++p) {
            hash ^= (uint32_t)(*p);
            hash *= 16777619u;
        }
    }

    if (hash == 0) hash = kind ? kind : 1u;
    return hash;
}

static void _gamelib_ui_draw_bevel_rect(GameLib *game, int x, int y, int w, int h,
                                        uint32_t face, bool pressed)
{
    if (!game || w <= 0 || h <= 0) return;

    game->FillRect(x, y, w, h, face);

    uint32_t lightOuter = _gamelib_ui_lighten(face, 112);
    uint32_t lightInner = _gamelib_ui_lighten(face, 56);
    uint32_t darkOuter = _gamelib_ui_darken(face, 112);
    uint32_t darkInner = _gamelib_ui_darken(face, 56);

    if (pressed) {
        uint32_t tmp = lightOuter; lightOuter = darkOuter; darkOuter = tmp;
        tmp = lightInner; lightInner = darkInner; darkInner = tmp;
    }

    game->DrawLine(x, y, x + w - 1, y, lightOuter);
    game->DrawLine(x, y, x, y + h - 1, lightOuter);
    game->DrawLine(x, y + h - 1, x + w - 1, y + h - 1, darkOuter);
    game->DrawLine(x + w - 1, y, x + w - 1, y + h - 1, darkOuter);

    if (w > 2 && h > 2) {
        game->DrawLine(x + 1, y + 1, x + w - 2, y + 1, lightInner);
        game->DrawLine(x + 1, y + 1, x + 1, y + h - 2, lightInner);
        game->DrawLine(x + 1, y + h - 2, x + w - 2, y + h - 2, darkInner);
        game->DrawLine(x + w - 2, y + 1, x + w - 2, y + h - 2, darkInner);
    }
}

static void _gamelib_ui_draw_text_with_shadow(GameLib *game, int x, int y, const char *text,
                                              uint32_t color, uint32_t shadow)
{
    if (!game || !text || !text[0]) return;
    if (COLOR_GET_A(shadow) != 0) game->DrawText(x + 1, y + 1, text, shadow);
    game->DrawText(x, y, text, color);
}

static void _gamelib_ui_draw_checkbox_mark(GameLib *game, int x, int y, int size,
                                           bool pressed, uint32_t color)
{
    if (!game || size <= 0) return;

    int markSize = size / 2;
    if (markSize < 6) markSize = 6;
    if (markSize > size - 4) markSize = size - 4;
    int markX = x + (size - markSize) / 2;
    int markY = y + (size - markSize) / 2;
    if (pressed) {
        markX += 1;
        markY += 1;
    }
    game->FillRect(markX, markY, markSize, markSize, color);
}
//=====================================================================
// Save / Load Data
//=====================================================================

#define _GAMELIB_SAVE_MAGIC      "GAMELIB_SAVE"
#define _GAMELIB_SAVE_MAX_ENTRIES 256
#define _GAMELIB_SAVE_MAX_KEY    64
#define _GAMELIB_SAVE_MAX_VALUE  1024
#define _GAMELIB_SAVE_MAX_LINE   1200

struct _gamelib_save_entry {
    char key[_GAMELIB_SAVE_MAX_KEY];
    char value[_GAMELIB_SAVE_MAX_VALUE];
};

static void _gamelib_save_escape(const char *src, char *dst, int dstSize)
{
    int j = 0;
    for (int i = 0; src[i] && j < dstSize - 2; i++) {
        if (src[i] == '\\')      { dst[j++] = '\\'; dst[j++] = '\\'; }
        else if (src[i] == '\n') { dst[j++] = '\\'; dst[j++] = 'n'; }
        else                     { dst[j++] = src[i]; }
    }
    dst[j] = '\0';
}

static void _gamelib_save_unescape(const char *src, char *dst, int dstSize)
{
    int j = 0;
    for (int i = 0; src[i] && j < dstSize - 1; i++) {
        if (src[i] == '\\' && src[i + 1] == '\\')     { dst[j++] = '\\'; i++; }
        else if (src[i] == '\\' && src[i + 1] == 'n')  { dst[j++] = '\n'; i++; }
        else                                            { dst[j++] = src[i]; }
    }
    dst[j] = '\0';
}

static int _gamelib_save_find_key(const _gamelib_save_entry *entries, int count,
                                  const char *key)
{
    for (int i = 0; i < count; i++) {
        if (strcmp(entries[i].key, key) == 0) return i;
    }
    return -1;
}

static bool _gamelib_save_key_is_valid(const char *key)
{
    if (!key || !key[0]) return false;

    for (const char *p = key; *p; p++) {
        if (*p == '=' || *p == '\r' || *p == '\n') return false;
    }
    return true;
}

static int _gamelib_save_read_all(const char *filename,
                                  _gamelib_save_entry *entries, int maxEntries)
{
    FILE *fp = fopen(filename, "r");
    if (!fp) return 0;

    char line[_GAMELIB_SAVE_MAX_LINE];
    int count = 0;

    // Read and verify magic header
    if (!fgets(line, sizeof(line), fp)) { fclose(fp); return 0; }
    size_t len = strlen(line);
    while (len > 0 && (line[len - 1] == '\n' || line[len - 1] == '\r')) line[--len] = '\0';
    if (strcmp(line, _GAMELIB_SAVE_MAGIC) != 0) { fclose(fp); return 0; }

    while (count < maxEntries && fgets(line, sizeof(line), fp)) {
        len = strlen(line);
        while (len > 0 && (line[len - 1] == '\n' || line[len - 1] == '\r')) line[--len] = '\0';
        if (len == 0) continue;

        char *eq = strchr(line, '=');
        if (!eq) continue;

        size_t keyLen = (size_t)(eq - line);
        if (keyLen == 0 || keyLen >= _GAMELIB_SAVE_MAX_KEY) continue;

        strncpy(entries[count].key, line, keyLen);
        entries[count].key[keyLen] = '\0';
        strncpy(entries[count].value, eq + 1, _GAMELIB_SAVE_MAX_VALUE - 1);
        entries[count].value[_GAMELIB_SAVE_MAX_VALUE - 1] = '\0';
        count++;
    }

    fclose(fp);
    return count;
}

static bool _gamelib_save_write_all(const char *filename,
                                    const _gamelib_save_entry *entries, int count)
{
    FILE *fp = fopen(filename, "w");
    if (!fp) return false;

    fprintf(fp, "%s\n", _GAMELIB_SAVE_MAGIC);
    for (int i = 0; i < count; i++) {
        fprintf(fp, "%s=%s\n", entries[i].key, entries[i].value);
    }
    fclose(fp);
    return true;
}

static bool _gamelib_save_write_key(const char *filename, const char *key,
                                    const char *rawValue)
{
    if (!filename || !_gamelib_save_key_is_valid(key) || !rawValue) return false;

    _gamelib_save_entry entries[_GAMELIB_SAVE_MAX_ENTRIES];
    int count = _gamelib_save_read_all(filename, entries, _GAMELIB_SAVE_MAX_ENTRIES);

    int idx = _gamelib_save_find_key(entries, count, key);
    if (idx >= 0) {
        strncpy(entries[idx].value, rawValue, _GAMELIB_SAVE_MAX_VALUE - 1);
        entries[idx].value[_GAMELIB_SAVE_MAX_VALUE - 1] = '\0';
    } else {
        if (count >= _GAMELIB_SAVE_MAX_ENTRIES) return false;
        strncpy(entries[count].key, key, _GAMELIB_SAVE_MAX_KEY - 1);
        entries[count].key[_GAMELIB_SAVE_MAX_KEY - 1] = '\0';
        strncpy(entries[count].value, rawValue, _GAMELIB_SAVE_MAX_VALUE - 1);
        entries[count].value[_GAMELIB_SAVE_MAX_VALUE - 1] = '\0';
        count++;
    }

    return _gamelib_save_write_all(filename, entries, count);
}

static const char *_gamelib_save_read_key(const char *filename, const char *key)
{
    if (!filename || !_gamelib_save_key_is_valid(key)) return NULL;

    static _gamelib_save_entry entries[_GAMELIB_SAVE_MAX_ENTRIES];
    int count = _gamelib_save_read_all(filename, entries, _GAMELIB_SAVE_MAX_ENTRIES);

    int idx = _gamelib_save_find_key(entries, count, key);
    if (idx < 0) return NULL;
    return entries[idx].value;
}

bool GameLib::SaveInt(const char *filename, const char *key, int value)
{
    char buf[64];
    snprintf(buf, sizeof(buf), "%d", value);
    return _gamelib_save_write_key(filename, key, buf);
}

bool GameLib::SaveFloat(const char *filename, const char *key, float value)
{
    char buf[64];
    snprintf(buf, sizeof(buf), "%g", value);
    return _gamelib_save_write_key(filename, key, buf);
}

bool GameLib::SaveString(const char *filename, const char *key, const char *value)
{
    if (!value) return false;
    char escaped[_GAMELIB_SAVE_MAX_VALUE];
    _gamelib_save_escape(value, escaped, _GAMELIB_SAVE_MAX_VALUE);
    return _gamelib_save_write_key(filename, key, escaped);
}

int GameLib::LoadInt(const char *filename, const char *key, int defaultValue)
{
    const char *raw = _gamelib_save_read_key(filename, key);
    if (!raw) return defaultValue;
    return atoi(raw);
}

float GameLib::LoadFloat(const char *filename, const char *key, float defaultValue)
{
    const char *raw = _gamelib_save_read_key(filename, key);
    if (!raw) return defaultValue;
    return (float)atof(raw);
}

const char *GameLib::LoadString(const char *filename, const char *key,
                                const char *defaultValue)
{
    static char _saveStringBuf[_GAMELIB_SAVE_MAX_VALUE];
    const char *raw = _gamelib_save_read_key(filename, key);
    if (!raw) return defaultValue;
    _gamelib_save_unescape(raw, _saveStringBuf, _GAMELIB_SAVE_MAX_VALUE);
    return _saveStringBuf;
}

bool GameLib::HasSaveKey(const char *filename, const char *key)
{
    return _gamelib_save_read_key(filename, key) != NULL;
}

bool GameLib::DeleteSaveKey(const char *filename, const char *key)
{
    if (!filename || !_gamelib_save_key_is_valid(key)) return false;

    _gamelib_save_entry entries[_GAMELIB_SAVE_MAX_ENTRIES];
    int count = _gamelib_save_read_all(filename, entries, _GAMELIB_SAVE_MAX_ENTRIES);

    int idx = _gamelib_save_find_key(entries, count, key);
    if (idx < 0) return false;

    for (int i = idx; i < count - 1; i++) {
        entries[i] = entries[i + 1];
    }
    count--;

    return _gamelib_save_write_all(filename, entries, count);
}

bool GameLib::DeleteSave(const char *filename)
{
    if (!filename) return false;
    return remove(filename) == 0;
}


//=====================================================================
// Software Mixer (SDL audio callback backend)
//=====================================================================

bool GameLib::_InitAudioBackend()
{
    // Initialize SDL audio subsystem if not already done
    if (SDL_WasInit(SDL_INIT_AUDIO) == 0) {
        if (SDL_InitSubSystem(SDL_INIT_AUDIO) != 0) return false;
        _audioSelfInit = true;
    }

    SDL_AudioSpec desired;
    memset(&desired, 0, sizeof(desired));
    desired.freq = 44100;
    desired.format = AUDIO_S16SYS;
    desired.channels = _AUDIO_OUTPUT_CHANNELS;
    desired.samples = _AUDIO_BUFFER_FRAMES;
    desired.callback = _SDLAudioCallback;
    desired.userdata = this;

    _audioDevice = SDL_OpenAudioDevice(NULL, 0, &desired, &_audioSpec, 0);
    if (_audioDevice == 0) return false;

    SDL_PauseAudioDevice(_audioDevice, 0);
    return true;
}

void GameLib::_ShutdownAudioBackend()
{
#if GAMELIB_SDL_USE_MIXER_CHANNELS
    _audio_initialized = false;
#else
    if (_audioDevice != 0) {
        SDL_CloseAudioDevice(_audioDevice);
        _audioDevice = 0;
    }
    if (_audioSelfInit && SDL_WasInit(SDL_INIT_AUDIO)) {
        // Don't quit audio subsystem here - SDL_mixer may still need it
        // It will be quit in the destructor after SDL_mixer cleanup
    }
    _audio_initialized = false;
#endif
}

void GameLib::_SDLAudioCallback(void *userdata, Uint8 *stream, int len)
{
    GameLib *game = (GameLib*)userdata;
    int total_samples = len / sizeof(int16_t);

    // SDL may request more samples than _mix_buffer can hold, so mix in chunks
    int16_t *out = (int16_t*)stream;
    int offset = 0;
    int chunk_samples = _AUDIO_BUFFER_TOTAL;
    while (offset < total_samples) {
        int to_mix = chunk_samples;
        if (offset + to_mix > total_samples) {
            to_mix = total_samples - offset;
        }
        game->_MixAudio(out + offset, to_mix);
        offset += to_mix;
    }
}

void GameLib::_ClampAndConvert(int32_t *input, int16_t *output, int count)
{
    for (int i = 0; i < count; i++) {
        int32_t sample = input[i];
        if (sample > 32767) sample = 32767;
        else if (sample < -32768) sample = -32768;
        output[i] = (int16_t)sample;
    }
}

void GameLib::_MixAudio(int16_t *output_buffer, int sample_count)
{
    for (int i = 0; i < sample_count; i++) {
        _mix_buffer[i] = 0;
    }

    // No lock needed inside callback - SDL_LockAudioDevice guarantees safety
    // Public APIs lock before modifying _audio_channels

    std::unordered_map<int, _Channel*>::iterator it = _audio_channels.begin();
    while (it != _audio_channels.end()) {
        _Channel *ch = it->second;
        if (!ch->is_playing) {
            ++it;
            continue;
        }

        float vol = (ch->volume / 1000.0f) * (_master_volume / 1000.0f);
        int bytes_per_sample = ch->wav->bits_per_sample / 8;
        int bytes_per_frame = bytes_per_sample * ch->wav->channels;
        int frames_to_mix = sample_count / ch->wav->channels;

        uint32_t remaining_bytes = ch->wav->size - ch->position;
        uint32_t remaining_frames = remaining_bytes / bytes_per_frame;

        if (remaining_frames == 0) {
            frames_to_mix = 0;
        } else if ((uint32_t)frames_to_mix > remaining_frames) {
            frames_to_mix = (int)remaining_frames;
        }

        for (int frame = 0; frame < frames_to_mix; frame++) {
            uint32_t frame_start = ch->position + frame * bytes_per_frame;
            for (uint16_t ch_idx = 0; ch_idx < ch->wav->channels; ch_idx++) {
                uint32_t sample_pos = frame_start + ch_idx * bytes_per_sample;
                int16_t sample = 0;
                if (sample_pos + 1 < ch->wav->size) {
                    sample = (int16_t)((uint16_t)(uint8_t)ch->wav->buffer[sample_pos] |
                                       ((uint16_t)(uint8_t)ch->wav->buffer[sample_pos + 1] << 8));
                }
                int out_idx = frame * ch->wav->channels + ch_idx;
                _mix_buffer[out_idx] += (int32_t)(sample * vol);
            }
        }
        ch->position += frames_to_mix * bytes_per_frame;

        if (ch->position >= ch->wav->size) {
            if (ch->repeat <= 0) {
                // <=0: infinite loop
                ch->position = 0;
            } else if (ch->repeat > 1) {
                // >1: decrement and loop
                ch->position = 0;
                ch->repeat--;
            } else {
                // 1: last play, stop now
                if (ch->wav) {
                    ch->wav->ref_count--;
                    if (ch->wav->temporary) delete ch->wav;
                }
                delete ch;
                it = _audio_channels.erase(it);
                continue;
            }
        }
        ++it;
    }

    _ClampAndConvert(_mix_buffer, output_buffer, sample_count);
}

GameLib::_WavData *GameLib::_LoadWAVFromFile(const char *filename)
{
    FILE *f = fopen(filename, "rb");
    if (!f) return NULL;

    char header[44];
    if (fread(header, 1, 44, f) != 44) {
        fclose(f);
        return NULL;
    }

    if (header[0] != 'R' || header[1] != 'I' || header[2] != 'F' || header[3] != 'F') {
        fclose(f);
        return NULL;
    }
    if (header[8] != 'W' || header[9] != 'A' || header[10] != 'V' || header[11] != 'E') {
        fclose(f);
        return NULL;
    }

    uint16_t audio_format = (uint16_t)(header[20] | (header[21] << 8));
    if (audio_format != 1) {
        fclose(f);
        return NULL;
    }

    _WavData *wav = new _WavData();
    wav->channels = (uint16_t)((uint8_t)header[22] | ((uint8_t)header[23] << 8));
    wav->sample_rate = (uint32_t)((uint8_t)header[24] | ((uint8_t)header[25] << 8) |
                                   ((uint8_t)header[26] << 16) | ((uint8_t)header[27] << 24));
    wav->bits_per_sample = (uint16_t)((uint8_t)header[34] | ((uint8_t)header[35] << 8));

    if (wav->channels == 0 || wav->channels > 2) {
        delete wav; fclose(f); return NULL;
    }
    if (wav->sample_rate == 0) {
        delete wav; fclose(f); return NULL;
    }
    if (wav->bits_per_sample != 8 && wav->bits_per_sample != 16) {
        delete wav; fclose(f); return NULL;
    }

    // Find data chunk
    fseek(f, 12, SEEK_SET);
    bool found_data = false;
    while (!found_data) {
        char chunk_id[4];
        uint32_t chunk_size = 0;
        if (fread(chunk_id, 1, 4, f) != 4) break;
        if (fread(&chunk_size, 4, 1, f) != 1) break;
        if (chunk_id[0] == 'd' && chunk_id[1] == 'a' &&
            chunk_id[2] == 't' && chunk_id[3] == 'a') {
            found_data = true;
            wav->size = chunk_size;
        } else {
            uint32_t skip = chunk_size + (chunk_size % 2);
            fseek(f, skip, SEEK_CUR);
        }
    }

    if (!found_data || wav->size == 0 || wav->size > 100 * 1024 * 1024) {
        delete wav;
        fclose(f);
        return NULL;
    }

    wav->buffer = new uint8_t[wav->size];
    if (fread(wav->buffer, 1, wav->size, f) != wav->size) {
        delete wav;
        fclose(f);
        return NULL;
    }

    fclose(f);

    _WavData *converted = _ConvertToTargetFormat(wav);
    delete wav;
    return converted;
}

GameLib::_WavData *GameLib::_ConvertToTargetFormat(_WavData *src)
{
    if (!src || !src->buffer || src->size == 0 ||
        src->sample_rate == 0 || src->channels == 0) {
        return NULL;
    }

    const uint32_t target_rate = 44100;
    const uint16_t target_channels = 2;
    const uint16_t target_bps = 16;

    uint32_t bytes_per_sample = src->bits_per_sample / 8;
    uint32_t total_samples = src->size / bytes_per_sample;
    uint32_t samples_per_channel = total_samples / src->channels;

    // Step 1: Decode to 16-bit array
    int16_t *decoded = new int16_t[total_samples];
    for (uint32_t i = 0; i < total_samples; i++) {
        if (src->bits_per_sample == 16) {
            decoded[i] = (int16_t)((uint16_t)(uint8_t)src->buffer[i * 2] |
                                    ((uint16_t)(uint8_t)src->buffer[i * 2 + 1] << 8));
        } else if (src->bits_per_sample == 8) {
            decoded[i] = (int16_t)((src->buffer[i] - 128) << 8);
        }
    }

    // Step 2: Resample (linear interpolation)
    double ratio = (double)target_rate / src->sample_rate;
    double step = (double)src->sample_rate / target_rate;
    uint32_t new_samples_per_ch = (uint32_t)(samples_per_channel * ratio);
    uint32_t new_total_samples = new_samples_per_ch * src->channels;

    int16_t *resampled = new int16_t[new_total_samples];
    for (uint16_t ch = 0; ch < src->channels; ch++) {
        double src_index = 0;
        for (uint32_t i = 0; i < new_samples_per_ch; i++) {
            uint32_t idx = (uint32_t)src_index;
            if (idx >= samples_per_channel) idx = samples_per_channel - 1;
            double frac = src_index - idx;

            int16_t s0 = decoded[idx * src->channels + ch];
            int16_t s1 = (idx + 1 < samples_per_channel) ?
                         decoded[(idx + 1) * src->channels + ch] : s0;

            resampled[i * src->channels + ch] = (int16_t)(s0 * (1.0 - frac) + s1 * frac);
            src_index += step;
        }
    }
    delete[] decoded;

    // Step 3: Convert mono to stereo
    int16_t *stereo = NULL;
    uint32_t stereo_samples = 0;

    if (src->channels == 1) {
        stereo_samples = new_samples_per_ch * 2;
        stereo = new int16_t[stereo_samples];
        for (uint32_t i = 0; i < new_samples_per_ch; i++) {
            stereo[i * 2] = resampled[i];
            stereo[i * 2 + 1] = resampled[i];
        }
        delete[] resampled;
    } else {
        stereo = resampled;
        stereo_samples = new_total_samples;
    }

    // Step 4: Create new WavData
    _WavData *dst = new _WavData();
    dst->sample_rate = target_rate;
    dst->channels = target_channels;
    dst->bits_per_sample = target_bps;
    dst->size = stereo_samples * sizeof(int16_t);
    dst->buffer = new uint8_t[dst->size];
    memcpy(dst->buffer, stereo, dst->size);

    delete[] stereo;
    return dst;
}

GameLib::_WavData *GameLib::_LoadOrCacheWAV(const char *filename)
{
    std::string key(filename);
    std::unordered_map<std::string, _WavData*>::iterator it = _wav_cache.find(key);
    if (it != _wav_cache.end()) {
        it->second->ref_count++;
        return it->second;
    }

    _WavData *wav = _LoadWAVFromFile(filename);
    if (wav) {
        wav->ref_count = 1;
        _wav_cache[key] = wav;
    }
    return wav;
}

int GameLib::_AllocateChannel()
{
    if ((int)_audio_channels.size() >= _MAX_CHANNELS) {
        return 0;
    }
    if (_next_channel_id > 32700) {
        _next_channel_id = 1;
    }
    while (_audio_channels.count((int)_next_channel_id)) {
        _next_channel_id++;
        if (_next_channel_id > 32700) {
            _next_channel_id = 1;
        }
    }
    int id = (int)_next_channel_id;
    _next_channel_id++;
    return id;
}

void GameLib::_ReleaseChannel(int channel_id)
{
    std::unordered_map<int, _Channel*>::iterator it = _audio_channels.find(channel_id);
    if (it != _audio_channels.end()) {
        if (it->second->wav) {
            it->second->wav->ref_count--;
            if (it->second->wav->temporary) delete it->second->wav;
        }
        delete it->second;
        _audio_channels.erase(it);
    }
}


//=====================================================================
// Public Audio API
//=====================================================================

int GameLib::PlayWAV(const char *filename, int repeat, int volume)
{
    if (!filename) return -1;
#if GAMELIB_SDL_USE_MIXER_CHANNELS
    if (!_EnsureMixerReady()) return -2;

    std::string key(filename);
    std::unordered_map<std::string, Mix_Chunk*>::iterator it = _chunk_cache.find(key);
    Mix_Chunk *chunk;
    if (it != _chunk_cache.end()) {
        chunk = it->second;
    } else {
        chunk = Mix_LoadWAV(filename);
        if (!chunk) return -1;
        _chunk_cache[key] = chunk;
    }

    int v = (volume < 0) ? 0 : (volume > 1000 ? 1000 : volume);
    chunk->volume = (Uint8)(v * 128 / 1000);

    int loops = (repeat <= 0) ? -1 : (repeat - 1);
    int ch = Mix_PlayChannel(-1, chunk, loops);
    return ch;
#else
    if (!_audio_initialized) {
        _audio_initialized = _InitAudioBackend();
        if (!_audio_initialized) return -2;
    }

    _WavData *wav = _LoadOrCacheWAV(filename);
    if (!wav) return -1;

    SDL_LockAudioDevice(_audioDevice);
    int ch_id = _AllocateChannel();
    if (ch_id == 0) {
        SDL_UnlockAudioDevice(_audioDevice);
        return -4;
    }

    _Channel *ch = new _Channel();
    ch->id = ch_id;
    ch->wav = wav;
    ch->position = 0;
    ch->repeat = repeat;
    ch->volume = (volume < 0) ? 0 : (volume > 1000 ? 1000 : volume);
    ch->is_playing = true;
    _audio_channels[ch_id] = ch;
    SDL_UnlockAudioDevice(_audioDevice);

    return ch_id;
#endif
}

int GameLib::PlayPCM(const int16_t *pcm, int nchannels, int nsamples, int sample_rate, int repeat, int volume)
{
    if (!pcm || nchannels < 1 || nchannels > 2 || nsamples <= 0 || sample_rate <= 0) return -1;
#if GAMELIB_SDL_USE_MIXER_CHANNELS
    if (!_EnsureMixerReady()) return -2;

    _WavData src;
    src.sample_rate = (uint32_t)sample_rate;
    src.channels = (uint16_t)nchannels;
    src.bits_per_sample = 16;
    src.size = (uint32_t)(nsamples * sizeof(int16_t));
    src.buffer = (uint8_t*)pcm;

    _WavData *wav = _ConvertToTargetFormat(&src);
    src.buffer = NULL;

    if (!wav) return -1;

    Mix_Chunk *chunk = new Mix_Chunk();
    chunk->allocated = 1;
    chunk->abuf = wav->buffer;
    chunk->alen = wav->size;
    int v = (volume < 0) ? 0 : (volume > 1000 ? 1000 : volume);
    chunk->volume = (Uint8)(v * 128 / 1000);

    wav->buffer = NULL;
    delete wav;

    int loops = (repeat <= 0) ? -1 : (repeat - 1);
    int ch = Mix_PlayChannel(-1, chunk, loops);
    if (ch < 0) {
        if (chunk->abuf) free(chunk->abuf);
        delete chunk;
        return -1;
    }

    _temp_chunks[ch] = chunk;
    return ch;
#else
    if (!_audio_initialized) {
        _audio_initialized = _InitAudioBackend();
        if (!_audio_initialized) return -2;
    }

    _WavData src;
    src.sample_rate = (uint32_t)sample_rate;
    src.channels = (uint16_t)nchannels;
    src.bits_per_sample = 16;
    src.size = (uint32_t)(nsamples * sizeof(int16_t));
    src.buffer = (uint8_t*)pcm;

    _WavData *wav = _ConvertToTargetFormat(&src);
    src.buffer = NULL;

    if (!wav) return -1;
    wav->temporary = true;

    SDL_LockAudioDevice(_audioDevice);
    int ch_id = _AllocateChannel();
    if (ch_id == 0) {
        SDL_UnlockAudioDevice(_audioDevice);
        delete wav;
        return -4;
    }

    _Channel *ch = new _Channel();
    ch->id = ch_id;
    ch->wav = wav;
    ch->position = 0;
    ch->repeat = repeat;
    ch->volume = (volume < 0) ? 0 : (volume > 1000 ? 1000 : volume);
    ch->is_playing = true;
    _audio_channels[ch_id] = ch;
    SDL_UnlockAudioDevice(_audioDevice);

    return ch_id;
#endif
}

int GameLib::StopWAV(int channel)
{
#if GAMELIB_SDL_USE_MIXER_CHANNELS
    if (channel < 0) return 0;
    std::unordered_map<int, Mix_Chunk*>::iterator it = _temp_chunks.find(channel);
    if (it != _temp_chunks.end()) {
        Mix_FreeChunk(it->second);
        _temp_chunks.erase(it);
    }
    Mix_HaltChannel(channel);
    return 1;
#else
    if (_audioDevice == 0) return 0;
    SDL_LockAudioDevice(_audioDevice);
    std::unordered_map<int, _Channel*>::iterator it = _audio_channels.find(channel);
    if (it == _audio_channels.end()) {
        SDL_UnlockAudioDevice(_audioDevice);
        return 0;
    }
    _ReleaseChannel(channel);
    SDL_UnlockAudioDevice(_audioDevice);
    return 1;
#endif
}

int GameLib::IsPlaying(int channel)
{
#if GAMELIB_SDL_USE_MIXER_CHANNELS
    if (channel < 0) return 0;
    return Mix_Playing(channel) ? 1 : 0;
#else
    if (_audioDevice == 0) return 0;
    SDL_LockAudioDevice(_audioDevice);
    std::unordered_map<int, _Channel*>::iterator it = _audio_channels.find(channel);
    int result = 0;
    if (it != _audio_channels.end()) {
        result = it->second->is_playing ? 1 : 0;
    }
    SDL_UnlockAudioDevice(_audioDevice);
    return result;
#endif
}

int GameLib::SetVolume(int channel, int volume)
{
#if GAMELIB_SDL_USE_MIXER_CHANNELS
    if (channel < 0) return -1;
    int v = (volume < 0) ? 0 : (volume > 1000 ? 1000 : volume);
    Mix_Volume(channel, v * 128 / 1000);
    return v;
#else
    if (_audioDevice == 0) return -1;
    SDL_LockAudioDevice(_audioDevice);
    std::unordered_map<int, _Channel*>::iterator it = _audio_channels.find(channel);
    int result = -1;
    if (it != _audio_channels.end()) {
        it->second->volume = (volume < 0) ? 0 : (volume > 1000 ? 1000 : volume);
        result = it->second->volume;
    }
    SDL_UnlockAudioDevice(_audioDevice);
    return result;
#endif
}

void GameLib::StopAll()
{
#if GAMELIB_SDL_USE_MIXER_CHANNELS
    Mix_HaltChannel(-1);
    for (std::unordered_map<int, Mix_Chunk*>::iterator it = _temp_chunks.begin();
         it != _temp_chunks.end(); ++it) {
        Mix_FreeChunk(it->second);
    }
    _temp_chunks.clear();
#else
    if (_audioDevice == 0) return;
    SDL_LockAudioDevice(_audioDevice);
    std::vector<int> channel_ids;
    for (std::unordered_map<int, _Channel*>::iterator it = _audio_channels.begin();
         it != _audio_channels.end(); ++it) {
        channel_ids.push_back(it->first);
    }
    for (size_t i = 0; i < channel_ids.size(); i++) {
        _ReleaseChannel(channel_ids[i]);
    }
    _audio_channels.clear();
    SDL_UnlockAudioDevice(_audioDevice);
#endif
}

int GameLib::SetMasterVolume(int volume)
{
    _master_volume = (volume < 0) ? 0 : (volume > 1000 ? 1000 : volume);
    return _master_volume;
}

int GameLib::GetMasterVolume() const
{
    return _master_volume;
}


int GameLib::PlayBeep(int frequency, int duration, int repeat, int volume)
{
    if (frequency <= 0 || duration <= 0) return -1;
#if GAMELIB_SDL_USE_MIXER_CHANNELS
    if (!_EnsureMixerReady()) return -2;
#else
    if (!_audio_initialized) {
        _audio_initialized = _InitAudioBackend();
        if (!_audio_initialized) return -2;
    }
#endif

    const int sample_rate = 44100;
    const int nchannels = 1;
    int total_samples = (int)((double)sample_rate * duration / 1000.0);
    if (total_samples <= 0) return -1;

    int16_t *pcm = new int16_t[total_samples];
    double phase = 0.0;
    double step = 2.0 * 3.14159265358979323846 * frequency / sample_rate;
    int fade_samples = sample_rate / 100;
    if (fade_samples > total_samples) fade_samples = total_samples;
    for (int i = 0; i < total_samples; i++) {
        pcm[i] = (int16_t)(32767.0 * 0.5 * sin(phase));
        phase += step;
    }
    for (int i = 0; i < fade_samples; i++) {
        double fade = 1.0 - (double)i / fade_samples;
        pcm[total_samples - fade_samples + i] =
            (int16_t)(pcm[total_samples - fade_samples + i] * fade);
    }

    int ch_id = PlayPCM(pcm, nchannels, total_samples, sample_rate, repeat, volume);
    delete[] pcm;
    return ch_id;
}


#endif // GAMELIB_SDL_IMPLEMENTATION

#endif // GAMELIB_SDL_H
