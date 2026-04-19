//=====================================================================
//
// GameLib.h - A single-header game library for beginners
//
// Homepage: https://github.com/skywind3000/GameLib
// Copyright (c) 2026 skywind3000 (Lin Wei)
//
// Based on Win32 GDI, no SDL or other third-party libraries needed.
// Works with Dev C++ (GCC 4.9.2+), can make shooting games, Tetris, 
// maze games, etc.
//
// How to use (single file project, most common):
//
//     #include "GameLib.h"
//
//     int main() {
//         GameLib game;
//         game.Open(640, 480, "My Game", true);
//
//         int x = 320, y = 240;
//
//         while (!game.IsClosed()) {
//             if (game.IsKeyDown(KEY_UP))    y -= 3;
//             if (game.IsKeyDown(KEY_DOWN))  y += 3;
//             if (game.IsKeyDown(KEY_LEFT))  x -= 3;
//             if (game.IsKeyDown(KEY_RIGHT)) x += 3;
//
//             game.Clear(COLOR_BLACK);
//             game.FillRect(x - 10, y - 10, 20, 20, COLOR_RED);
//             game.DrawText(10, 10, "Move the box!", COLOR_WHITE);
//             game.Update();
//
//             game.WaitFrame(60);
//         }
//         return 0;
//     }
//
// Multi-file project: add this line before #include in the main .cpp file
//     #define GAMELIB_IMPLEMENTATION
//     #include "GameLib.h"
// In other .cpp files, add this line
//     #define GAMELIB_NO_IMPLEMENTATION
//     #include "GameLib.h"
//
// Compile command (MinGW / Dev C++):
//     g++ -o game.exe main.cpp -mwindows
//
// Last Modified: 2026/04/16
//
//=====================================================================
#ifndef GAMELIB_H
#define GAMELIB_H

// Default behavior: include enables implementation (good for single file projects)
#ifndef GAMELIB_NO_IMPLEMENTATION
#ifndef GAMELIB_IMPLEMENTATION
#define GAMELIB_IMPLEMENTATION
#endif
#endif

// Version Info
#define GAMELIB_VERSION_MAJOR     1
#define GAMELIB_VERSION_MINOR     9
#define GAMELIB_VERSION_PATCH     0


//---------------------------------------------------------------------
// System header files
//---------------------------------------------------------------------
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>
#include <mmsystem.h>

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


//---------------------------------------------------------------------
// Link library: only needs -mwindows, gdi32 and winmm are loaded
// via LoadLibrary
//---------------------------------------------------------------------
#ifdef _MSC_VER
#pragma comment(lib, "user32.lib")
#endif


//---------------------------------------------------------------------
// Dynamically loaded function pointer types
//---------------------------------------------------------------------

// gdi32.dll
typedef int (WINAPI *PFN_SetDIBitsToDevice)(
    HDC, int, int, DWORD, DWORD, int, int, UINT, UINT,
    const void*, const BITMAPINFO*, UINT);
typedef HGDIOBJ (WINAPI *PFN_GetStockObject)(int);
typedef HDC (WINAPI *PFN_CreateCompatibleDC)(HDC);
typedef BOOL (WINAPI *PFN_DeleteDC)(HDC);
typedef HBITMAP (WINAPI *PFN_CreateDIBSection)(HDC, const BITMAPINFO*, UINT, void**, HANDLE, DWORD);
typedef HGDIOBJ (WINAPI *PFN_SelectObject)(HDC, HGDIOBJ);
typedef BOOL (WINAPI *PFN_DeleteObject)(HGDIOBJ);
typedef BOOL (WINAPI *PFN_BitBlt)(HDC, int, int, int, int, HDC, int, int, DWORD);
typedef BOOL (WINAPI *PFN_StretchBlt)(HDC, int, int, int, int, HDC, int, int, int, int, DWORD);
typedef int (WINAPI *PFN_SetStretchBltMode)(HDC, int);
typedef HFONT (WINAPI *PFN_CreateFontW)(int, int, int, int, int, DWORD, DWORD, DWORD, DWORD, DWORD, DWORD, DWORD, DWORD, const WCHAR*);
typedef BOOL (WINAPI *PFN_TextOutW)(HDC, int, int, LPCWSTR, int);
typedef COLORREF (WINAPI *PFN_SetTextColor)(HDC, COLORREF);
typedef int (WINAPI *PFN_SetBkMode)(HDC, int);
typedef BOOL (WINAPI *PFN_GetTextExtentPoint32W)(HDC, LPCWSTR, int, SIZE*);
typedef BOOL (WINAPI *PFN_GdiFlush)(void);

// winmm.dll (timer + MCI)
typedef DWORD   (WINAPI *PFN_timeBeginPeriod)(UINT);
typedef DWORD   (WINAPI *PFN_timeEndPeriod)(UINT);
typedef UINT (WINAPI *PFN_timeSetEvent)(UINT, UINT, DWORD_PTR, DWORD_PTR, UINT);
typedef UINT (WINAPI *PFN_timeKillEvent)(UINT);
typedef MCIERROR (WINAPI *PFN_mciSendStringW)(LPCWSTR, LPWSTR, UINT, HWND);

// winmm.dll (waveOut)
typedef MMRESULT (WINAPI *PFN_waveOutOpen)(LPHWAVEOUT, UINT, LPCWAVEFORMATEX, DWORD_PTR, DWORD_PTR, DWORD);
typedef MMRESULT (WINAPI *PFN_waveOutClose)(HWAVEOUT);
typedef MMRESULT (WINAPI *PFN_waveOutPrepareHeader)(HWAVEOUT, LPWAVEHDR, UINT);
typedef MMRESULT (WINAPI *PFN_waveOutUnprepareHeader)(HWAVEOUT, LPWAVEHDR, UINT);
typedef MMRESULT (WINAPI *PFN_waveOutWrite)(HWAVEOUT, LPWAVEHDR, UINT);
typedef MMRESULT (WINAPI *PFN_waveOutReset)(HWAVEOUT);

// gdiplus.dll
typedef int  (WINAPI *PFN_GdiplusStartup)(ULONG_PTR*, void*, void*);
typedef void (WINAPI *PFN_GdiplusShutdown)(ULONG_PTR);
typedef int  (WINAPI *PFN_GdipCreateBitmapFromStream)(void*, void**);
typedef int  (WINAPI *PFN_GdipGetImageWidth)(void*, UINT*);
typedef int  (WINAPI *PFN_GdipGetImageHeight)(void*, UINT*);
typedef int  (WINAPI *PFN_GdipBitmapLockBits)(void*, const int*, UINT, int, void*);
typedef int  (WINAPI *PFN_GdipBitmapUnlockBits)(void*, void*);
typedef int  (WINAPI *PFN_GdipDisposeImage)(void*);

// ole32.dll
typedef HRESULT (WINAPI *PFN_CreateStreamOnHGlobal)(HGLOBAL, BOOL, void**);


//=====================================================================
// Part 1: Constants
//=====================================================================

//---------------------------------------------------------------------
// Color constants (ARGB format: 0xAARRGGBB)
//---------------------------------------------------------------------
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

// Color helper macros
#define COLOR_RGB(r, g, b)     ((uint32_t)(0xFF000000 | (((r) & 0xFF) << 16) | (((g) & 0xFF) << 8) | ((b) & 0xFF)))
#define COLOR_ARGB(a, r, g, b) ((uint32_t)((((a) & 0xFF) << 24) | (((r) & 0xFF) << 16) | (((g) & 0xFF) << 8) | ((b) & 0xFF)))

// Color component extraction
#define COLOR_GET_A(c)    (((c) >> 24) & 0xFF)
#define COLOR_GET_R(c)    (((c) >> 16) & 0xFF)
#define COLOR_GET_G(c)    (((c) >> 8) & 0xFF)
#define COLOR_GET_B(c)    ((c) & 0xFF)

//---------------------------------------------------------------------
// Keyboard constants (using Windows Virtual Key Code)
//---------------------------------------------------------------------
#define KEY_LEFT      VK_LEFT
#define KEY_RIGHT     VK_RIGHT
#define KEY_UP        VK_UP
#define KEY_DOWN      VK_DOWN
#define KEY_SPACE     VK_SPACE
#define KEY_ENTER     VK_RETURN
#define KEY_ESCAPE    VK_ESCAPE
#define KEY_TAB       VK_TAB
#define KEY_SHIFT     VK_SHIFT
#define KEY_CONTROL   VK_CONTROL
#define KEY_BACK      VK_BACK

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

#define KEY_F1        VK_F1
#define KEY_F2        VK_F2
#define KEY_F3        VK_F3
#define KEY_F4        VK_F4
#define KEY_F5        VK_F5
#define KEY_F6        VK_F6
#define KEY_F7        VK_F7
#define KEY_F8        VK_F8
#define KEY_F9        VK_F9
#define KEY_F10       VK_F10
#define KEY_F11       VK_F11
#define KEY_F12       VK_F12
#define KEY_ADD       VK_ADD
#define KEY_SUBTRACT  VK_SUBTRACT

//---------------------------------------------------------------------
// Mouse button constants
//---------------------------------------------------------------------
#define MOUSE_LEFT    0
#define MOUSE_RIGHT   1
#define MOUSE_MIDDLE  2

// Message box button and result constants
#define MESSAGEBOX_OK           0
#define MESSAGEBOX_YESNO        1
#define MESSAGEBOX_RESULT_OK    1
#define MESSAGEBOX_RESULT_YES   2
#define MESSAGEBOX_RESULT_NO    3

//---------------------------------------------------------------------
// Sprite drawing flags
//---------------------------------------------------------------------
#define SPRITE_FLIP_H     1    // flip horizontal
#define SPRITE_FLIP_V     2    // flip vertical
#define SPRITE_COLORKEY   4    // enable transparent color
#define SPRITE_ALPHA      8    // enable alpha blending

// Default Color Key: magenta (255, 0, 255), common transparent color in 2D games
#ifndef COLORKEY_DEFAULT
#define COLORKEY_DEFAULT  0xFFFF00FF
#endif

#ifndef GAMELIB_DEFAULT_FONT_NAME
#define GAMELIB_DEFAULT_FONT_NAME "Microsoft YaHei"
#endif


//=====================================================================
// Part 2: Class Declaration
//=====================================================================

//---------------------------------------------------------------------
// GameLib Main Class
//---------------------------------------------------------------------
class GameLib
{
public:
    GameLib();
    ~GameLib();

    // -------- Window and Main Loop --------
    int Open(int width, int height, const char *title, bool center = false, bool resizable = false);
    bool IsClosed() const;
    void Update();
    void WaitFrame(int fps);
    double GetDeltaTime() const;
    double GetFPS() const;
    double GetTime() const;
    int GetWidth() const;
    int GetHeight() const;
    void WinResize(int width, int height);
    void SetMaximized(bool maximized);
    void SetTitle(const char *title);
    void ShowFps(bool show);
    void ShowMouse(bool show);
    int ShowMessage(const char *text, const char *title = NULL, int buttons = MESSAGEBOX_OK);

    // -------- Frame Buffer --------
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

    // -------- Drawing --------
    void DrawLine(int x1, int y1, int x2, int y2, uint32_t color);
    void DrawRect(int x, int y, int w, int h, uint32_t color);
    void FillRect(int x, int y, int w, int h, uint32_t color);
    void DrawCircle(int cx, int cy, int r, uint32_t color);
    void FillCircle(int cx, int cy, int r, uint32_t color);
    void DrawEllipse(int cx, int cy, int rx, int ry, uint32_t color);
    void FillEllipse(int cx, int cy, int rx, int ry, uint32_t color);
    void DrawTriangle(int x1, int y1, int x2, int y2, int x3, int y3, uint32_t color);
    void FillTriangle(int x1, int y1, int x2, int y2, int x3, int y3, uint32_t color);

    // -------- Text Rendering (built-in 8x8 font) --------
    void DrawText(int x, int y, const char *text, uint32_t color);
    void DrawNumber(int x, int y, int number, uint32_t color);
    void DrawTextScale(int x, int y, const char *text, uint32_t color, int scale);
    void DrawPrintf(int x, int y, uint32_t color, const char *fmt, ...);
    void DrawPrintfScale(int x, int y, uint32_t color, int scale, const char *fmt, ...);

    // -------- Font Text Rendering (scalable fonts, Unicode support) --------
    void DrawTextFont(int x, int y, const char *text, uint32_t color, const char *fontName, int fontSize);
    void DrawTextFont(int x, int y, const char *text, uint32_t color, int fontSize);
    void DrawPrintfFont(int x, int y, uint32_t color, const char *fontName, int fontSize, const char *fmt, ...);
    void DrawPrintfFont(int x, int y, uint32_t color, int fontSize, const char *fmt, ...);
    int GetTextWidthFont(const char *text, const char *fontName, int fontSize);
    int GetTextWidthFont(const char *text, int fontSize);
    int GetTextHeightFont(const char *text, const char *fontName, int fontSize);
    int GetTextHeightFont(const char *text, int fontSize);

    // -------- Sprite System (managed by integer ID) --------
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
	
    // -------- Tilemap System --------
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
	
    // -------- Grid Helpers --------
    void DrawGrid(int x, int y, int rows, int cols, int cellSize, uint32_t color);
    void FillCell(int gridX, int gridY, int row, int col, int cellSize, uint32_t color);
	
    // -------- Input --------
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

    // -------- Sound --------
    void PlayBeep(int frequency, int duration);
    int PlayWAV(const char *filename, int repeat = 1, int volume = 1000);
    int StopWAV(int channel);
    int IsPlaying(int channel);
    int SetVolume(int channel, int volume);
    void StopAll();
    int SetMasterVolume(int volume);
    int GetMasterVolume() const;
    bool PlayMusic(const char *filename, bool loop = true);
    void StopMusic();
    bool IsMusicPlaying() const;

    // -------- Scene Management --------
    void SetScene(int scene);
    int GetScene() const;
    bool IsSceneChanged() const;
    int GetPreviousScene() const;

    // -------- UI Helpers (built-in 8x8 font) --------
    bool Button(int x, int y, int w, int h, const char *text, uint32_t color);
    bool Checkbox(int x, int y, const char *text, bool *checked);
    bool RadioBox(int x, int y, const char *text, int *value, int index);
    bool ToggleButton(int x, int y, int w, int h, const char *text, bool *toggled, uint32_t color);

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

    // -------- Helper Functions --------
    static int Random(int minVal, int maxVal);
    static bool RectOverlap(int x1, int y1, int w1, int h1,
                            int x2, int y2, int w2, int h2);
    static bool CircleOverlap(int cx1, int cy1, int r1,
                              int cx2, int cy2, int r2);
    static bool PointInRect(int px, int py, int x, int y, int w, int h);
    static float Distance(int x1, int y1, int x2, int y2);

private:
    // disable copy
    GameLib(const GameLib &);
    GameLib &operator=(const GameLib &);

    // internal window management
    static LRESULT CALLBACK _WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
    static int _InitWindowClass();
    void _DispatchMessages();
    void _InitDIBInfo(void *ptr, int width, int height);
    void _DestroyGraphicsResources();
    void _DestroyTimingResources();
    void _PresentFrame(HDC hdc);
    void _UpdateClientSize();
    void _SetMouseFromWindowCoords(int x, int y);
    void _UpdateTitleFps();

    // internal pixel drawing (no bounds check, for fast drawing after clipping)
    void _SetPixelFast(int x, int y, uint32_t color);
    void _DrawHLine(int x1, int x2, int y, uint32_t color);
    bool _ClipRectToCurrentClip(int *x0, int *y0, int *x1, int *y1) const;

    // internal sprite management
    int _AllocSpriteSlot();
    void _DrawSpriteAreaFast(int id, int x, int y, int sx, int sy, int sw, int sh, int flags);
    void _DrawSpriteAreaScaled(int id, int x, int y, int sx, int sy, int sw, int sh,
                               int dw, int dh, int flags);
    void _DrawSpriteAreaRotated(int id, int cx, int cy, int sx, int sy, int sw, int sh,
                                double angleDeg, int flags);

    // internal tilemap management
    int _AllocTilemapSlot();
    int _GetTilesetTileCount(int tilesetId, int tileSize) const;

private:
    // window state
    HWND _hwnd;
    bool _closing;
    bool _active;
    bool _showFps;
    bool _mouseVisible;
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

    // frame buffer
    uint32_t *_framebuffer;
    uint32_t *_presentBuffer;
    int *_presentMapX;
    int *_presentMapY;
    int _presentWidth;
    int _presentHeight;

    // DIB Section (for scalable font text rendering on current backend)
    HDC _memDC;
    HBITMAP _dibSection;
    HBITMAP _oldBmp;

    // DIB info (for SetDIBitsToDevice, kept for compatibility)
    unsigned char _bmi_data[sizeof(BITMAPINFO) + 16 * sizeof(RGBQUAD)];
    unsigned char _present_bmi_data[sizeof(BITMAPINFO) + 16 * sizeof(RGBQUAD)];

    // input state
    int _keys[512];
    int _keys_prev[512];
    int _mouseX;
    int _mouseY;
    int _mouseButtons[3];
    int _mouseButtons_prev[3];
    int _mouseWheelDelta;
    uint32_t _uiActiveId;

    // timing
    uint64_t _timeStartCounter;
    uint64_t _timePrevCounter;
    uint64_t _fpsTimeCounter;
    uint64_t _frameStartCounter;
    uint64_t _perfFrequency;
    double _deltaTime;
    double _fps;
    double _fpsAccum;
    HANDLE _timerEvent;     // event signaled by multimedia timer
    UINT   _timerId;        // multimedia timer ID (from timeSetEvent)
    bool _timerPeriodActive;

    // sprite storage
    struct GameSprite { int width, height; uint32_t *pixels; uint32_t colorKey; bool used; };
    std::vector<GameSprite> _sprites;

    // tilemap storage
    struct GameTilemap {
        int cols, rows;     // map grid size
        int tileSize;       // tile size in pixels
        int tilesetId;      // tileset sprite ID
        int tilesetCols;    // tiles per row in tileset
        int *tiles;         // tile ID array (cols * rows, -1 = empty)
        bool used;          // is this slot in use
    };
    std::vector<GameTilemap> _tilemaps;

    // music state (MCI)
    bool _musicPlaying;
    bool _musicLoop;
    bool _musicIsMidi;
    std::wstring _musicAlias;

    // audio mixer state (waveOut software mixer)
    struct _WavData {
        uint8_t *buffer;
        uint32_t size;
        uint32_t sample_rate;
        uint16_t channels;
        uint16_t bits_per_sample;
        int ref_count;
        _WavData() : buffer(NULL), size(0), sample_rate(0), channels(0),
                     bits_per_sample(0), ref_count(0) {}
        ~_WavData() { if (buffer) delete[] buffer; }
    };
    struct _Channel {
        int id;
        _WavData *wav;
        uint32_t position;
        int repeat;
        int volume;
        bool is_playing;
        _Channel() : id(0), wav(NULL), position(0), repeat(1),
                     volume(1000), is_playing(false) {}
    };
    std::unordered_map<std::string, _WavData*> _wav_cache;
    std::unordered_map<int, _Channel*> _audio_channels;
    int64_t _next_channel_id;
    bool _audio_initialized;
    int _master_volume;
    HWAVEOUT _hWaveOut;
    WAVEHDR *_wave_hdr[2];
    CRITICAL_SECTION _audio_lock;
    volatile bool _audio_closing;
    static const int _MAX_CHANNELS = 32;
    static const int _AUDIO_BUFFER_FRAMES = 2048;
    static const int _AUDIO_OUTPUT_CHANNELS = 2;
    static const int _AUDIO_BUFFER_TOTAL = _AUDIO_BUFFER_FRAMES * _AUDIO_OUTPUT_CHANNELS;
    static const int _AUDIO_BUFFER_BYTES = _AUDIO_BUFFER_TOTAL * sizeof(int16_t);
    int32_t _mix_buffer[_AUDIO_BUFFER_TOTAL];

    // internal audio methods
    _WavData *_LoadWAVFromFile(const char *filename);
    _WavData *_ConvertToTargetFormat(_WavData *src);
    _WavData *_LoadOrCacheWAV(const char *filename);
    int _AllocateChannel();
    void _ReleaseChannel(int channel_id);
    void _MixAudio(int16_t *output_buffer, int sample_count);
    void _ClampAndConvert(int32_t *input, int16_t *output, int count);
    bool _InitAudioBackend();
    void _ShutdownAudioBackend();
    static void CALLBACK _WaveOutCallback(HWAVEOUT hwo, UINT uMsg,
                                          DWORD_PTR dwInstance,
                                          DWORD_PTR dwParam1, DWORD_PTR dwParam2);

    // scene state
    int _scene;
    int _pendingScene;
    bool _hasPendingScene;
    bool _sceneChanged;
    int _previousScene;

    // random seed initialized flag
    static bool _srandDone;
};


//=====================================================================
// Part 3: 8x8 Font Data (ASCII 32-126)
//=====================================================================

// Classic 8x8 bitmap font, 8 bytes per char, one byte per row, MSB on left
static const unsigned char _gamelib_font8x8[95][8] = {
    { 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00 }, // 32 ' '
    { 0x18,0x3C,0x3C,0x18,0x18,0x00,0x18,0x00 }, // 33 '!'
    { 0x6C,0x6C,0x24,0x00,0x00,0x00,0x00,0x00 }, // 34 '"'
    { 0x6C,0x6C,0xFE,0x6C,0xFE,0x6C,0x6C,0x00 }, // 35 '#'
    { 0x18,0x7E,0xC0,0x7C,0x06,0xFC,0x18,0x00 }, // 36 '$'
    { 0x00,0xC6,0xCC,0x18,0x30,0x66,0xC6,0x00 }, // 37 '%'
    { 0x38,0x6C,0x38,0x76,0xDC,0xCC,0x76,0x00 }, // 38 '&'
    { 0x18,0x18,0x30,0x00,0x00,0x00,0x00,0x00 }, // 39 '''
    { 0x0C,0x18,0x30,0x30,0x30,0x18,0x0C,0x00 }, // 40 '('
    { 0x30,0x18,0x0C,0x0C,0x0C,0x18,0x30,0x00 }, // 41 ')'
    { 0x00,0x66,0x3C,0xFF,0x3C,0x66,0x00,0x00 }, // 42 '*'
    { 0x00,0x18,0x18,0x7E,0x18,0x18,0x00,0x00 }, // 43 '+'
    { 0x00,0x00,0x00,0x00,0x00,0x18,0x18,0x30 }, // 44 ','
    { 0x00,0x00,0x00,0x7E,0x00,0x00,0x00,0x00 }, // 45 '-'
    { 0x00,0x00,0x00,0x00,0x00,0x18,0x18,0x00 }, // 46 '.'
    { 0x06,0x0C,0x18,0x30,0x60,0xC0,0x80,0x00 }, // 47 '/'
    { 0x7C,0xCE,0xDE,0xF6,0xE6,0xC6,0x7C,0x00 }, // 48 '0'
    { 0x18,0x38,0x78,0x18,0x18,0x18,0x7E,0x00 }, // 49 '1'
    { 0x7C,0xC6,0x06,0x1C,0x70,0xC6,0xFE,0x00 }, // 50 '2'
    { 0x7C,0xC6,0x06,0x3C,0x06,0xC6,0x7C,0x00 }, // 51 '3'
    { 0x1C,0x3C,0x6C,0xCC,0xFE,0x0C,0x1E,0x00 }, // 52 '4'
    { 0xFE,0xC0,0xFC,0x06,0x06,0xC6,0x7C,0x00 }, // 53 '5'
    { 0x38,0x60,0xC0,0xFC,0xC6,0xC6,0x7C,0x00 }, // 54 '6'
    { 0xFE,0xC6,0x0C,0x18,0x30,0x30,0x30,0x00 }, // 55 '7'
    { 0x7C,0xC6,0xC6,0x7C,0xC6,0xC6,0x7C,0x00 }, // 56 '8'
    { 0x7C,0xC6,0xC6,0x7E,0x06,0x0C,0x78,0x00 }, // 57 '9'
    { 0x00,0x18,0x18,0x00,0x00,0x18,0x18,0x00 }, // 58 ':'
    { 0x00,0x18,0x18,0x00,0x00,0x18,0x18,0x30 }, // 59 ';'
    { 0x0C,0x18,0x30,0x60,0x30,0x18,0x0C,0x00 }, // 60 '<'
    { 0x00,0x00,0x7E,0x00,0x7E,0x00,0x00,0x00 }, // 61 '='
    { 0x60,0x30,0x18,0x0C,0x18,0x30,0x60,0x00 }, // 62 '>'
    { 0x7C,0xC6,0x0C,0x18,0x18,0x00,0x18,0x00 }, // 63 '?'
    { 0x7C,0xC6,0xDE,0xDE,0xDC,0xC0,0x7C,0x00 }, // 64 '@'
    { 0x38,0x6C,0xC6,0xC6,0xFE,0xC6,0xC6,0x00 }, // 65 'A'
    { 0xFC,0xC6,0xC6,0xFC,0xC6,0xC6,0xFC,0x00 }, // 66 'B'
    { 0x7C,0xC6,0xC0,0xC0,0xC0,0xC6,0x7C,0x00 }, // 67 'C'
    { 0xF8,0xCC,0xC6,0xC6,0xC6,0xCC,0xF8,0x00 }, // 68 'D'
    { 0xFE,0xC0,0xC0,0xFC,0xC0,0xC0,0xFE,0x00 }, // 69 'E'
    { 0xFE,0xC0,0xC0,0xFC,0xC0,0xC0,0xC0,0x00 }, // 70 'F'
    { 0x7C,0xC6,0xC0,0xCE,0xC6,0xC6,0x7E,0x00 }, // 71 'G'
    { 0xC6,0xC6,0xC6,0xFE,0xC6,0xC6,0xC6,0x00 }, // 72 'H'
    { 0x3C,0x18,0x18,0x18,0x18,0x18,0x3C,0x00 }, // 73 'I'
    { 0x1E,0x0C,0x0C,0x0C,0xCC,0xCC,0x78,0x00 }, // 74 'J'
    { 0xC6,0xCC,0xD8,0xF0,0xD8,0xCC,0xC6,0x00 }, // 75 'K'
    { 0xC0,0xC0,0xC0,0xC0,0xC0,0xC0,0xFE,0x00 }, // 76 'L'
    { 0xC6,0xEE,0xFE,0xD6,0xC6,0xC6,0xC6,0x00 }, // 77 'M'
    { 0xC6,0xE6,0xF6,0xDE,0xCE,0xC6,0xC6,0x00 }, // 78 'N'
    { 0x7C,0xC6,0xC6,0xC6,0xC6,0xC6,0x7C,0x00 }, // 79 'O'
    { 0xFC,0xC6,0xC6,0xFC,0xC0,0xC0,0xC0,0x00 }, // 80 'P'
    { 0x7C,0xC6,0xC6,0xC6,0xD6,0xDE,0x7C,0x06 }, // 81 'Q'
    { 0xFC,0xC6,0xC6,0xFC,0xD8,0xCC,0xC6,0x00 }, // 82 'R'
    { 0x7C,0xC6,0xC0,0x7C,0x06,0xC6,0x7C,0x00 }, // 83 'S'
    { 0x7E,0x18,0x18,0x18,0x18,0x18,0x18,0x00 }, // 84 'T'
    { 0xC6,0xC6,0xC6,0xC6,0xC6,0xC6,0x7C,0x00 }, // 85 'U'
    { 0xC6,0xC6,0xC6,0xC6,0x6C,0x38,0x10,0x00 }, // 86 'V'
    { 0xC6,0xC6,0xC6,0xD6,0xFE,0xEE,0xC6,0x00 }, // 87 'W'
    { 0xC6,0xC6,0x6C,0x38,0x6C,0xC6,0xC6,0x00 }, // 88 'X'
    { 0x66,0x66,0x66,0x3C,0x18,0x18,0x18,0x00 }, // 89 'Y'
    { 0xFE,0x06,0x0C,0x18,0x30,0x60,0xFE,0x00 }, // 90 'Z'
    { 0x3C,0x30,0x30,0x30,0x30,0x30,0x3C,0x00 }, // 91 '['
    { 0xC0,0x60,0x30,0x18,0x0C,0x06,0x02,0x00 }, // 92 '\'
    { 0x3C,0x0C,0x0C,0x0C,0x0C,0x0C,0x3C,0x00 }, // 93 ']'
    { 0x10,0x38,0x6C,0xC6,0x00,0x00,0x00,0x00 }, // 94 '^'
    { 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFE }, // 95 '_'
    { 0x30,0x18,0x0C,0x00,0x00,0x00,0x00,0x00 }, // 96 '`'
    { 0x00,0x00,0x78,0x0C,0x7C,0xCC,0x76,0x00 }, // 97 'a'
    { 0xC0,0xC0,0xFC,0xC6,0xC6,0xC6,0xFC,0x00 }, // 98 'b'
    { 0x00,0x00,0x7C,0xC6,0xC0,0xC6,0x7C,0x00 }, // 99 'c'
    { 0x06,0x06,0x7E,0xC6,0xC6,0xC6,0x7E,0x00 }, //100 'd'
    { 0x00,0x00,0x7C,0xC6,0xFE,0xC0,0x7C,0x00 }, //101 'e'
    { 0x1C,0x36,0x30,0x7C,0x30,0x30,0x30,0x00 }, //102 'f'
    { 0x00,0x00,0x7E,0xC6,0xC6,0x7E,0x06,0x7C }, //103 'g'
    { 0xC0,0xC0,0xFC,0xC6,0xC6,0xC6,0xC6,0x00 }, //104 'h'
    { 0x18,0x00,0x38,0x18,0x18,0x18,0x3C,0x00 }, //105 'i'
    { 0x0C,0x00,0x1C,0x0C,0x0C,0x0C,0xCC,0x78 }, //106 'j'
    { 0xC0,0xC0,0xCC,0xD8,0xF0,0xD8,0xCC,0x00 }, //107 'k'
    { 0x38,0x18,0x18,0x18,0x18,0x18,0x3C,0x00 }, //108 'l'
    { 0x00,0x00,0xCC,0xFE,0xD6,0xC6,0xC6,0x00 }, //109 'm'
    { 0x00,0x00,0xFC,0xC6,0xC6,0xC6,0xC6,0x00 }, //110 'n'
    { 0x00,0x00,0x7C,0xC6,0xC6,0xC6,0x7C,0x00 }, //111 'o'
    { 0x00,0x00,0xFC,0xC6,0xC6,0xFC,0xC0,0xC0 }, //112 'p'
    { 0x00,0x00,0x7E,0xC6,0xC6,0x7E,0x06,0x06 }, //113 'q'
    { 0x00,0x00,0xDC,0xE6,0xC0,0xC0,0xC0,0x00 }, //114 'r'
    { 0x00,0x00,0x7E,0xC0,0x7C,0x06,0xFC,0x00 }, //115 's'
    { 0x30,0x30,0x7C,0x30,0x30,0x36,0x1C,0x00 }, //116 't'
    { 0x00,0x00,0xC6,0xC6,0xC6,0xC6,0x7E,0x00 }, //117 'u'
    { 0x00,0x00,0xC6,0xC6,0xC6,0x6C,0x38,0x00 }, //118 'v'
    { 0x00,0x00,0xC6,0xC6,0xD6,0xFE,0x6C,0x00 }, //119 'w'
    { 0x00,0x00,0xC6,0x6C,0x38,0x6C,0xC6,0x00 }, //120 'x'
    { 0x00,0x00,0xC6,0xC6,0xC6,0x7E,0x06,0x7C }, //121 'y'
    { 0x00,0x00,0xFE,0x0C,0x38,0x60,0xFE,0x00 }, //122 'z'
    { 0x0E,0x18,0x18,0x70,0x18,0x18,0x0E,0x00 }, //123 '{'
    { 0x18,0x18,0x18,0x00,0x18,0x18,0x18,0x00 }, //124 '|'
    { 0x70,0x18,0x18,0x0E,0x18,0x18,0x70,0x00 }, //125 '}'
    { 0x76,0xDC,0x00,0x00,0x00,0x00,0x00,0x00 }, //126 '~'
};


//=====================================================================
// Part 4: Implementation
//=====================================================================
#ifdef GAMELIB_IMPLEMENTATION

//---------------------------------------------------------------------
// Dynamically loaded function pointers (global, valid during process lifetime)
//---------------------------------------------------------------------
static PFN_SetDIBitsToDevice   _gl_SetDIBitsToDevice  = NULL;
static PFN_GetStockObject      _gl_GetStockObject     = NULL;
static PFN_CreateCompatibleDC  _gl_CreateCompatibleDC = NULL;
static PFN_DeleteDC            _gl_DeleteDC           = NULL;
static PFN_CreateDIBSection    _gl_CreateDIBSection   = NULL;
static PFN_SelectObject        _gl_SelectObject       = NULL;
static PFN_DeleteObject        _gl_DeleteObject       = NULL;
static PFN_BitBlt              _gl_BitBlt             = NULL;
static PFN_StretchBlt          _gl_StretchBlt         = NULL;
static PFN_SetStretchBltMode   _gl_SetStretchBltMode  = NULL;
static PFN_CreateFontW         _gl_CreateFontW        = NULL;
static PFN_TextOutW            _gl_TextOutW           = NULL;
static PFN_SetTextColor        _gl_SetTextColor       = NULL;
static PFN_SetBkMode           _gl_SetBkMode          = NULL;
static PFN_GetTextExtentPoint32W _gl_GetTextExtentPoint32W = NULL;
static PFN_GdiFlush              _gl_GdiFlush              = NULL;
static PFN_timeBeginPeriod     _gl_timeBeginPeriod    = NULL;
static PFN_timeEndPeriod       _gl_timeEndPeriod      = NULL;
static PFN_timeSetEvent        _gl_timeSetEvent       = NULL;
static PFN_timeKillEvent       _gl_timeKillEvent      = NULL;
static PFN_mciSendStringW      _gl_mciSendStringW     = NULL;

// winmm.dll (waveOut - loaded separately for software mixer)
static PFN_waveOutOpen          _gl_waveOutOpen          = NULL;
static PFN_waveOutClose         _gl_waveOutClose         = NULL;
static PFN_waveOutPrepareHeader _gl_waveOutPrepareHeader = NULL;
static PFN_waveOutUnprepareHeader _gl_waveOutUnprepareHeader = NULL;
static PFN_waveOutWrite         _gl_waveOutWrite         = NULL;
static PFN_waveOutReset         _gl_waveOutReset         = NULL;

static int _gamelib_apis_loaded = 0;

static uint64_t _gamelib_query_performance_counter()
{
    LARGE_INTEGER counter;
    counter.QuadPart = 0;
    QueryPerformanceCounter(&counter);
    return (uint64_t)counter.QuadPart;
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

template <typename T>
static T _gamelib_load_proc(HMODULE module, const char *name)
{
    FARPROC proc = GetProcAddress(module, name);
    T typed = NULL;
    if (!proc) return typed;
    if (sizeof(typed) != sizeof(proc)) return typed;
    memcpy(&typed, &proc, sizeof(typed));
    return typed;
}

static int _gamelib_load_apis()
{
    if (_gamelib_apis_loaded) return 0;

    HMODULE hGdi32 = LoadLibraryA("gdi32.dll");
    HMODULE hWinmm = LoadLibraryA("winmm.dll");
    if (!hGdi32 || !hWinmm) {
        if (hGdi32) FreeLibrary(hGdi32);
        if (hWinmm) FreeLibrary(hWinmm);
        return -1;
    }

    _gl_SetDIBitsToDevice = _gamelib_load_proc<PFN_SetDIBitsToDevice>(hGdi32, "SetDIBitsToDevice");
    _gl_GetStockObject    = _gamelib_load_proc<PFN_GetStockObject>(hGdi32, "GetStockObject");
    _gl_CreateCompatibleDC = _gamelib_load_proc<PFN_CreateCompatibleDC>(hGdi32, "CreateCompatibleDC");
    _gl_DeleteDC          = _gamelib_load_proc<PFN_DeleteDC>(hGdi32, "DeleteDC");
    _gl_CreateDIBSection  = _gamelib_load_proc<PFN_CreateDIBSection>(hGdi32, "CreateDIBSection");
    _gl_SelectObject      = _gamelib_load_proc<PFN_SelectObject>(hGdi32, "SelectObject");
    _gl_DeleteObject      = _gamelib_load_proc<PFN_DeleteObject>(hGdi32, "DeleteObject");
    _gl_BitBlt            = _gamelib_load_proc<PFN_BitBlt>(hGdi32, "BitBlt");
    _gl_StretchBlt        = _gamelib_load_proc<PFN_StretchBlt>(hGdi32, "StretchBlt");
    _gl_SetStretchBltMode = _gamelib_load_proc<PFN_SetStretchBltMode>(hGdi32, "SetStretchBltMode");
    _gl_CreateFontW       = _gamelib_load_proc<PFN_CreateFontW>(hGdi32, "CreateFontW");
    _gl_TextOutW          = _gamelib_load_proc<PFN_TextOutW>(hGdi32, "TextOutW");
    _gl_SetTextColor      = _gamelib_load_proc<PFN_SetTextColor>(hGdi32, "SetTextColor");
    _gl_SetBkMode         = _gamelib_load_proc<PFN_SetBkMode>(hGdi32, "SetBkMode");
    _gl_GetTextExtentPoint32W = _gamelib_load_proc<PFN_GetTextExtentPoint32W>(hGdi32, "GetTextExtentPoint32W");
    _gl_GdiFlush              = _gamelib_load_proc<PFN_GdiFlush>(hGdi32, "GdiFlush");

    _gl_timeBeginPeriod   = _gamelib_load_proc<PFN_timeBeginPeriod>(hWinmm, "timeBeginPeriod");
    _gl_timeEndPeriod     = _gamelib_load_proc<PFN_timeEndPeriod>(hWinmm, "timeEndPeriod");
    _gl_timeSetEvent      = _gamelib_load_proc<PFN_timeSetEvent>(hWinmm, "timeSetEvent");
    _gl_timeKillEvent     = _gamelib_load_proc<PFN_timeKillEvent>(hWinmm, "timeKillEvent");
    _gl_mciSendStringW    = _gamelib_load_proc<PFN_mciSendStringW>(hWinmm, "mciSendStringW");

    // waveOut APIs for software mixer (loaded from same winmm.dll)
    _gl_waveOutOpen          = _gamelib_load_proc<PFN_waveOutOpen>(hWinmm, "waveOutOpen");
    _gl_waveOutClose         = _gamelib_load_proc<PFN_waveOutClose>(hWinmm, "waveOutClose");
    _gl_waveOutPrepareHeader = _gamelib_load_proc<PFN_waveOutPrepareHeader>(hWinmm, "waveOutPrepareHeader");
    _gl_waveOutUnprepareHeader = _gamelib_load_proc<PFN_waveOutUnprepareHeader>(hWinmm, "waveOutUnprepareHeader");
    _gl_waveOutWrite         = _gamelib_load_proc<PFN_waveOutWrite>(hWinmm, "waveOutWrite");
    _gl_waveOutReset         = _gamelib_load_proc<PFN_waveOutReset>(hWinmm, "waveOutReset");

    if (!_gl_SetDIBitsToDevice || !_gl_GetStockObject ||
        !_gl_CreateCompatibleDC || !_gl_DeleteDC ||
        !_gl_CreateDIBSection || !_gl_SelectObject || !_gl_DeleteObject ||
        !_gl_BitBlt || !_gl_StretchBlt || !_gl_SetStretchBltMode ||
        !_gl_CreateFontW || !_gl_TextOutW ||
        !_gl_SetTextColor || !_gl_SetBkMode || !_gl_GetTextExtentPoint32W ||
        !_gl_timeBeginPeriod  || !_gl_timeEndPeriod ||
        !_gl_mciSendStringW) {
        // NULL out all pointers to prevent dangling state
        _gl_SetDIBitsToDevice = NULL; _gl_GetStockObject = NULL;
        _gl_CreateCompatibleDC = NULL; _gl_DeleteDC = NULL;
        _gl_CreateDIBSection = NULL; _gl_SelectObject = NULL;
        _gl_DeleteObject = NULL; _gl_BitBlt = NULL;
        _gl_StretchBlt = NULL; _gl_SetStretchBltMode = NULL;
        _gl_CreateFontW = NULL; _gl_TextOutW = NULL;
        _gl_SetTextColor = NULL; _gl_SetBkMode = NULL;
        _gl_GetTextExtentPoint32W = NULL; _gl_GdiFlush = NULL;
        _gl_timeBeginPeriod = NULL;
        _gl_timeEndPeriod = NULL; _gl_timeSetEvent = NULL;
        _gl_timeKillEvent = NULL;
        _gl_mciSendStringW = NULL;
        _gl_waveOutOpen = NULL; _gl_waveOutClose = NULL;
        _gl_waveOutPrepareHeader = NULL; _gl_waveOutUnprepareHeader = NULL;
        _gl_waveOutWrite = NULL; _gl_waveOutReset = NULL;
        FreeLibrary(hGdi32);
        FreeLibrary(hWinmm);
        return -1;
    }
    _gamelib_apis_loaded = 1;
    return 0;
}

static bool _gamelib_mci_play_music_alias(const wchar_t *alias, HWND callbackWindow, bool isMidi, bool loop);
static void _gamelib_close_music_alias(const wchar_t *alias);
static wchar_t *_gamelib_utf8_to_wide(const char *text, int *outLen);
static FILE *_gamelib_fopen_utf8(const char *filename, const wchar_t *mode);


//---------------------------------------------------------------------
// GDI+ dynamic loading (for PNG/JPG image loading)
//---------------------------------------------------------------------
static PFN_GdiplusStartup             _gl_GdiplusStartup = NULL;
static PFN_GdipCreateBitmapFromStream _gl_GdipCreateBitmapFromStream = NULL;
static PFN_GdipGetImageWidth          _gl_GdipGetImageWidth = NULL;
static PFN_GdipGetImageHeight         _gl_GdipGetImageHeight = NULL;
static PFN_GdipBitmapLockBits         _gl_GdipBitmapLockBits = NULL;
static PFN_GdipBitmapUnlockBits       _gl_GdipBitmapUnlockBits = NULL;
static PFN_GdipDisposeImage           _gl_GdipDisposeImage = NULL;
static PFN_CreateStreamOnHGlobal      _gl_CreateStreamOnHGlobal = NULL;

static int _gamelib_gdiplus_ready = 0;
static ULONG_PTR _gamelib_gdip_token = 0;

// Initialize GDI+: load gdiplus.dll / ole32.dll and call GdiplusStartup
static int _gamelib_gdiplus_init()
{
    if (_gamelib_gdiplus_ready) return 0;

    HMODULE hGdiPlus = LoadLibraryA("gdiplus.dll");
    if (!hGdiPlus) return -1;

    HMODULE hOle32 = LoadLibraryA("ole32.dll");
    if (!hOle32) { FreeLibrary(hGdiPlus); return -2; }

    _gl_GdiplusStartup = _gamelib_load_proc<PFN_GdiplusStartup>(hGdiPlus, "GdiplusStartup");
    _gl_GdipCreateBitmapFromStream = _gamelib_load_proc<PFN_GdipCreateBitmapFromStream>(hGdiPlus, "GdipCreateBitmapFromStream");
    _gl_GdipGetImageWidth = _gamelib_load_proc<PFN_GdipGetImageWidth>(hGdiPlus, "GdipGetImageWidth");
    _gl_GdipGetImageHeight = _gamelib_load_proc<PFN_GdipGetImageHeight>(hGdiPlus, "GdipGetImageHeight");
    _gl_GdipBitmapLockBits = _gamelib_load_proc<PFN_GdipBitmapLockBits>(hGdiPlus, "GdipBitmapLockBits");
    _gl_GdipBitmapUnlockBits = _gamelib_load_proc<PFN_GdipBitmapUnlockBits>(hGdiPlus, "GdipBitmapUnlockBits");
    _gl_GdipDisposeImage = _gamelib_load_proc<PFN_GdipDisposeImage>(hGdiPlus, "GdipDisposeImage");
    _gl_CreateStreamOnHGlobal = _gamelib_load_proc<PFN_CreateStreamOnHGlobal>(hOle32, "CreateStreamOnHGlobal");

    if (!_gl_GdiplusStartup || !_gl_GdipCreateBitmapFromStream ||
        !_gl_GdipGetImageWidth || !_gl_GdipGetImageHeight ||
        !_gl_GdipBitmapLockBits || !_gl_GdipBitmapUnlockBits ||
        !_gl_GdipDisposeImage || !_gl_CreateStreamOnHGlobal) {
        FreeLibrary(hOle32); FreeLibrary(hGdiPlus);
        return -3;
    }

    // GdiplusStartup
    struct { unsigned int ver; void *cb; BOOL noThread; BOOL noCodecs; } si;
    si.ver = 1; si.cb = NULL; si.noThread = FALSE; si.noCodecs = FALSE;

    if (_gl_GdiplusStartup(&_gamelib_gdip_token, &si, NULL) != 0)
        return -4;

    _gamelib_gdiplus_ready = 1;
    return 0;
}

// Call IUnknown::Release through COM vtable (no need to #include <ObjBase.h>)
static void _gamelib_com_release(void *obj)
{
    if (!obj) return;
    // COM object layout: first pointer points to vtable
    // IUnknown vtable: [0]=QueryInterface, [1]=AddRef, [2]=Release
    typedef unsigned long (WINAPI *PFN_Release)(void*);
    void **vtbl = *(void***)obj;
    PFN_Release release_fn = (PFN_Release)vtbl[2];
    release_fn(obj);
}

// GDI+ PixelFormat32bppARGB
#define _GL_PIXFMT_32bppARGB 2498570

// Load image from memory via GDI+ (PNG/JPG/BMP/GIF/TIFF etc.), returns ARGB pixels
static uint32_t* _gamelib_gdiplus_load(
    const void *data, long size, int *out_w, int *out_h)
{
    if (_gamelib_gdiplus_init() != 0) return NULL;

    // Allocate global memory and copy file data
    HGLOBAL hg = GlobalAlloc(GMEM_MOVEABLE, size);
    if (!hg) return NULL;

    void *pg = GlobalLock(hg);
    if (!pg) { GlobalFree(hg); return NULL; }
    memcpy(pg, data, size);
    GlobalUnlock(hg);

    // Create IStream
    void *pStream = NULL;
    if (_gl_CreateStreamOnHGlobal(hg, FALSE, &pStream) != 0 || !pStream) {
        GlobalFree(hg);
        return NULL;
    }

    // Create GDI+ Bitmap from IStream
    void *gpBitmap = NULL;
    if (_gl_GdipCreateBitmapFromStream(pStream, &gpBitmap) != 0 || !gpBitmap) {
        _gamelib_com_release(pStream);
        GlobalFree(hg);
        return NULL;
    }

    // Get image size
    UINT width = 0, height = 0;
    _gl_GdipGetImageWidth(gpBitmap, &width);
    _gl_GdipGetImageHeight(gpBitmap, &height);

    uint32_t *pixels = NULL;

    if (width == 0 || height == 0 || width > 16384 || height > 16384) goto cleanup;

    // LockBits struct (matches GDI+ BitmapData layout)
    {
        struct {
            UINT    bWidth;
            UINT    bHeight;
            INT     bStride;
            INT     bPixelFormat;
            void   *bScan0;
            ULONG_PTR bReserved;
        } bd;
        memset(&bd, 0, sizeof(bd));

        int lockRect[4] = { 0, 0, (int)width, (int)height };

        // ImageLockModeRead = 1, request 32bppARGB format
        if (_gl_GdipBitmapLockBits(gpBitmap, lockRect, 1,
                _GL_PIXFMT_32bppARGB, &bd) != 0) {
            goto cleanup;
        }

        pixels = (uint32_t*)malloc((size_t)width * height * sizeof(uint32_t));
        if (pixels) {
            for (UINT y = 0; y < height; y++) {
                uint32_t *dst = pixels + y * width;
                const char *src = (const char*)bd.bScan0 + y * bd.bStride;
                memcpy(dst, src, width * sizeof(uint32_t));
            }
            // 24-bit images may have alpha=0 after GDI+ conversion, detect and fix to 255
            {
                size_t total = (size_t)width * height;
                bool allZeroAlpha = true;
                for (size_t i = 0; i < total; i++) {
                    if (COLOR_GET_A(pixels[i]) != 0) {
                        allZeroAlpha = false;
                        break;
                    }
                }
                if (allZeroAlpha) {
                    for (size_t i = 0; i < total; i++) {
                        pixels[i] |= 0xFF000000;
                    }
                }
            }
        }

        _gl_GdipBitmapUnlockBits(gpBitmap, &bd);
    }

    if (out_w) *out_w = (int)width;
    if (out_h) *out_h = (int)height;

cleanup:
    _gl_GdipDisposeImage(gpBitmap);
    _gamelib_com_release(pStream);
    GlobalFree(hg);
    return pixels;
}


// Static member initialization
bool GameLib::_srandDone = false;


//---------------------------------------------------------------------
// Constructor
//---------------------------------------------------------------------
GameLib::GameLib()
{
    _hwnd = NULL;
    _closing = false;
    _active = false;
    _showFps = false;
    _mouseVisible = true;
    _width = 0;
    _height = 0;
    _windowWidth = 0;
    _windowHeight = 0;
    _clipX = 0;
    _clipY = 0;
    _clipW = 0;
    _clipH = 0;
    _resizable = false;
    _framebuffer = NULL;
    _presentBuffer = NULL;
    _presentMapX = NULL;
    _presentMapY = NULL;
    _presentWidth = 0;
    _presentHeight = 0;
    _memDC = NULL;
    _dibSection = NULL;
    _oldBmp = NULL;
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
    _timerEvent = NULL;
    _timerId = 0;
    _timerPeriodActive = false;
    memset(_bmi_data, 0, sizeof(_bmi_data));
    memset(_present_bmi_data, 0, sizeof(_present_bmi_data));
    _musicPlaying = false;
    _musicLoop = false;
    _musicIsMidi = false;
    _next_channel_id = 1;
    _audio_initialized = false;
    _master_volume = 1000;
    _hWaveOut = NULL;
    _wave_hdr[0] = NULL;
    _wave_hdr[1] = NULL;
    _audio_closing = false;
    memset(_mix_buffer, 0, sizeof(_mix_buffer));
    InitializeCriticalSection(&_audio_lock);
    _scene = 0;
    _pendingScene = 0;
    _hasPendingScene = false;
    _sceneChanged = true;
    _previousScene = 0;
    {
        char aliasBuffer[64];
        unsigned long long aliasValue = (unsigned long long)(uintptr_t)this;
        snprintf(aliasBuffer, sizeof(aliasBuffer), "gamelib_music_%llu", aliasValue);
        wchar_t *wideAlias = _gamelib_utf8_to_wide(aliasBuffer, NULL);
        if (wideAlias) {
            _musicAlias = wideAlias;
            free(wideAlias);
        } else {
            _musicAlias = L"gamelib_music";
        }
    }
    if (!_srandDone) {
        srand((unsigned int)time(NULL));
        _srandDone = true;
    }
    // Load core APIs (gdi32/winmm) at construction time
    // These are Windows system DLLs and should always succeed
    if (_gamelib_load_apis() != 0) {
        MessageBoxA(NULL, "GameLib: Failed to load gdi32.dll or winmm.dll",
                    "Fatal Error", MB_OK | MB_ICONERROR);
        exit(1);
    }
}


void GameLib::_DestroyGraphicsResources()
{
    if (_presentBuffer) {
        free(_presentBuffer);
        _presentBuffer = NULL;
    }
    if (_presentMapX) {
        free(_presentMapX);
        _presentMapX = NULL;
    }
    if (_presentMapY) {
        free(_presentMapY);
        _presentMapY = NULL;
    }
    if (_memDC) {
        if (_oldBmp && _gl_SelectObject) {
            _gl_SelectObject(_memDC, _oldBmp);
            _oldBmp = NULL;
        }
        if (_dibSection && _gl_DeleteObject) {
            _gl_DeleteObject(_dibSection);
            _dibSection = NULL;
        }
        if (_gl_DeleteDC) {
            _gl_DeleteDC(_memDC);
        }
        _memDC = NULL;
    }
    _framebuffer = NULL;
    _presentWidth = 0;
    _presentHeight = 0;
    _windowWidth = 0;
    _windowHeight = 0;
    _clipX = 0;
    _clipY = 0;
    _clipW = 0;
    _clipH = 0;
}


void GameLib::_PresentFrame(HDC hdc)
{
    if (!hdc || !_memDC || _width <= 0 || _height <= 0 || _windowWidth <= 0 || _windowHeight <= 0) {
        return;
    }

    if (_windowWidth == _width && _windowHeight == _height) {
        _gl_BitBlt(hdc, 0, 0, _width, _height, _memDC, 0, 0, 0x00CC0020 /* SRCCOPY */);
        if (_gl_GdiFlush) _gl_GdiFlush();
        return;
    }

    bool ready = (_presentWidth == _windowWidth) && (_presentHeight == _windowHeight) &&
                 (_presentBuffer != NULL) && (_presentMapX != NULL) && (_presentMapY != NULL);
    if (!ready) {
        uint32_t *newBuffer = (uint32_t*)realloc(_presentBuffer,
            (size_t)_windowWidth * (size_t)_windowHeight * sizeof(uint32_t));
        int *newMapX = (int*)realloc(_presentMapX, (size_t)_windowWidth * sizeof(int));
        int *newMapY = (int*)realloc(_presentMapY, (size_t)_windowHeight * sizeof(int));

        if (!newBuffer || !newMapX || !newMapY) {
            if (newBuffer) _presentBuffer = newBuffer;
            if (newMapX) _presentMapX = newMapX;
            if (newMapY) _presentMapY = newMapY;

            _gl_SetStretchBltMode(hdc, COLORONCOLOR);
            _gl_StretchBlt(hdc, 0, 0, _windowWidth, _windowHeight,
                           _memDC, 0, 0, _width, _height, 0x00CC0020 /* SRCCOPY */);
            if (_gl_GdiFlush) _gl_GdiFlush();
            return;
        }

        _presentBuffer = newBuffer;
        _presentMapX = newMapX;
        _presentMapY = newMapY;
        _presentWidth = _windowWidth;
        _presentHeight = _windowHeight;

        for (int x = 0; x < _windowWidth; x++) {
            _presentMapX[x] = (int)(((long long)x * (long long)_width) / (long long)_windowWidth);
        }
        for (int y = 0; y < _windowHeight; y++) {
            _presentMapY[y] = (int)(((long long)y * (long long)_height) / (long long)_windowHeight);
        }
        _InitDIBInfo(_present_bmi_data, _windowWidth, _windowHeight);
    }

    const int dstWidth = _windowWidth;
    const size_t rowBytes = (size_t)dstWidth * sizeof(uint32_t);
    int previousSrcY = -1;
    uint32_t *previousDstRow = NULL;

    for (int y = 0; y < _windowHeight; y++) {
        int srcY = _presentMapY[y];
        uint32_t *dstRow = _presentBuffer + (size_t)y * (size_t)dstWidth;
        if (srcY == previousSrcY && previousDstRow) {
            memcpy(dstRow, previousDstRow, rowBytes);
            continue;
        }

        const uint32_t *srcRow = _framebuffer + (size_t)srcY * (size_t)_width;
        for (int x = 0; x < dstWidth; x++) {
            dstRow[x] = srcRow[_presentMapX[x]];
        }
        previousSrcY = srcY;
        previousDstRow = dstRow;
    }

    _gl_SetDIBitsToDevice(hdc, 0, 0, (DWORD)_windowWidth, (DWORD)_windowHeight,
                          0, 0, 0, (UINT)_windowHeight,
                          _presentBuffer, (BITMAPINFO*)_present_bmi_data, DIB_RGB_COLORS);
    if (_gl_GdiFlush) _gl_GdiFlush();
}


void GameLib::_UpdateClientSize()
{
    if (!_hwnd) {
        _windowWidth = 0;
        _windowHeight = 0;
        return;
    }

    RECT clientRC;
    ::GetClientRect(_hwnd, &clientRC);
    _windowWidth = clientRC.right - clientRC.left;
    _windowHeight = clientRC.bottom - clientRC.top;
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

    _mouseX = (int)(((long long)x * (long long)_width) / (long long)_windowWidth);
    _mouseY = (int)(((long long)y * (long long)_height) / (long long)_windowHeight);
}


void GameLib::_DestroyTimingResources()
{
    if (_timerId && _gl_timeKillEvent) {
        _gl_timeKillEvent(_timerId);
        _timerId = 0;
    }
    if (_timerEvent) {
        CloseHandle(_timerEvent);
        _timerEvent = NULL;
    }
    if (_timerPeriodActive && _gl_timeEndPeriod) {
        _gl_timeEndPeriod(1);
        _timerPeriodActive = false;
    }
    _timeStartCounter = 0;
    _timePrevCounter = 0;
    _fpsTimeCounter = 0;
    _frameStartCounter = 0;
    _perfFrequency = 0;
    _deltaTime = 0.0;
    _fps = 0.0;
    _fpsAccum = 0.0;
}


//---------------------------------------------------------------------
// Destructor
//---------------------------------------------------------------------
GameLib::~GameLib()
{
    // Shutdown audio backend first (stops callbacks, prevents deadlock)
    _ShutdownAudioBackend();

    // Clean up audio channels and WAV cache (no lock needed since callbacks stopped)
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
    DeleteCriticalSection(&_audio_lock);

    // Stop music
    if ((_musicPlaying || _musicIsMidi) && _gl_mciSendStringW) {
        _musicPlaying = false;
        _musicLoop = false;
        _musicIsMidi = false;
        _gamelib_close_music_alias(_musicAlias.c_str());
    }
    // Free all sprites
    for (size_t i = 0; i < _sprites.size(); i++) {
        if (_sprites[i].used && _sprites[i].pixels) {
            free(_sprites[i].pixels);
            _sprites[i].pixels = NULL;
            _sprites[i].used = false;
        }
    }
    // Free all Tilemaps
    for (size_t i = 0; i < _tilemaps.size(); i++) {
        if (_tilemaps[i].used && _tilemaps[i].tiles) {
            free(_tilemaps[i].tiles);
            _tilemaps[i].tiles = NULL;
            _tilemaps[i].used = false;
        }
    }
    // Clean up multimedia timer
    _DestroyTimingResources();
    // Free DIB Section resources
    _DestroyGraphicsResources();
    // Destroy window
    if (_hwnd) {
        DestroyWindow(_hwnd);
        _hwnd = NULL;
    }
}


//---------------------------------------------------------------------
// Register window class (static, runs only once)
//---------------------------------------------------------------------
int GameLib::_InitWindowClass()
{
    static int initialized = 0;
    if (initialized) return 0;
    HINSTANCE inst = GetModuleHandle(NULL);
    WNDCLASSW wc;
    memset(&wc, 0, sizeof(wc));
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wc.lpszClassName = L"GameLibWindowClass";
    wc.hbrBackground = (HBRUSH)_gl_GetStockObject(BLACK_BRUSH);
    wc.hInstance = inst;
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = _WndProc;
    wc.cbWndExtra = 0;
    wc.cbClsExtra = 0;
    if (RegisterClassW(&wc) == 0) return -1;
    initialized = 1;
    return 0;
}


//---------------------------------------------------------------------
// Window procedure (static callback)
//---------------------------------------------------------------------
#define GAMELIB_REPEATED_KEYMASK (1 << 30)

LRESULT CALLBACK GameLib::_WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    GameLib *self = NULL;

    if (msg == WM_CREATE) {
        self = (GameLib*)(((LPCREATESTRUCT)lParam)->lpCreateParams);
        SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)self);
        return DefWindowProcW(hWnd, msg, wParam, lParam);
    }

    self = (GameLib*)GetWindowLongPtr(hWnd, GWLP_USERDATA);

    switch (msg) {
    case WM_DESTROY:
        if (self) self->_closing = true;
        PostQuitMessage(0);
        return 0;

    case WM_CLOSE:
        if (self) self->_closing = true;
        return 0;

    case WM_ACTIVATE:
        if (self) {
            int minimized = HIWORD(wParam);
            int active = LOWORD(wParam);
            self->_active = (!minimized && active) ? true : false;
        }
        return DefWindowProcW(hWnd, msg, wParam, lParam);

    case WM_SIZE:
        if (self) {
            self->_windowWidth = (int)LOWORD(lParam);
            self->_windowHeight = (int)HIWORD(lParam);
        }
        return 0;

    case MM_MCINOTIFY:
        if (!self || !self->_musicIsMidi) return 0;

        if (wParam == MCI_NOTIFY_SUCCESSFUL) {
            if (self->_musicLoop) {
                std::wstring seekCmd = L"seek ";
                seekCmd += self->_musicAlias;
                seekCmd += L" to start";
                if (_gl_mciSendStringW &&
                    _gl_mciSendStringW(seekCmd.c_str(), NULL, 0, NULL) == 0 &&
                    _gamelib_mci_play_music_alias(self->_musicAlias.c_str(), hWnd, true, true)) {
                    return 0;
                }
            }

            self->_musicPlaying = false;
            self->_musicLoop = false;
            self->_musicIsMidi = false;
            _gamelib_close_music_alias(self->_musicAlias.c_str());
            return 0;
        }

        if (wParam == MCI_NOTIFY_FAILURE) {
            self->_musicPlaying = false;
            self->_musicLoop = false;
            self->_musicIsMidi = false;
            _gamelib_close_music_alias(self->_musicAlias.c_str());
            return 0;
        }

        if (wParam == MCI_NOTIFY_ABORTED || wParam == MCI_NOTIFY_SUPERSEDED) {
            return 0;
        }
        return 0;

    case WM_KEYDOWN:
        if (lParam & GAMELIB_REPEATED_KEYMASK) return 0;
        if (self) self->_keys[wParam & 511] = 1;
        return 0;

    case WM_KEYUP:
        if (self) self->_keys[wParam & 511] = 0;
        return 0;

    case WM_MOUSEMOVE:
        if (self) {
            self->_SetMouseFromWindowCoords((int)(short)LOWORD(lParam),
                                            (int)(short)HIWORD(lParam));
        }
        return 0;

    case WM_SETCURSOR:
        if (self && LOWORD(lParam) == HTCLIENT) {
            if (self->_mouseVisible) {
                SetCursor(LoadCursor(NULL, IDC_ARROW));
            } else {
                SetCursor(NULL);
            }
            return TRUE;
        }
        break;

    case WM_MOUSEWHEEL:
        if (self) {
            POINT pt;
            pt.x = (int)(short)LOWORD(lParam);
            pt.y = (int)(short)HIWORD(lParam);
            ScreenToClient(hWnd, &pt);
            self->_SetMouseFromWindowCoords(pt.x, pt.y);
            self->_mouseWheelDelta += (int)(short)HIWORD(wParam);
        }
        return 0;

    case WM_LBUTTONDOWN:
        if (self) {
            self->_SetMouseFromWindowCoords((int)(short)LOWORD(lParam),
                                            (int)(short)HIWORD(lParam));
            self->_mouseButtons[MOUSE_LEFT] = 1;
        }
        return 0;
    case WM_LBUTTONUP:
        if (self) {
            self->_SetMouseFromWindowCoords((int)(short)LOWORD(lParam),
                                            (int)(short)HIWORD(lParam));
            self->_mouseButtons[MOUSE_LEFT] = 0;
        }
        return 0;
    case WM_RBUTTONDOWN:
        if (self) {
            self->_SetMouseFromWindowCoords((int)(short)LOWORD(lParam),
                                            (int)(short)HIWORD(lParam));
            self->_mouseButtons[MOUSE_RIGHT] = 1;
        }
        return 0;
    case WM_RBUTTONUP:
        if (self) {
            self->_SetMouseFromWindowCoords((int)(short)LOWORD(lParam),
                                            (int)(short)HIWORD(lParam));
            self->_mouseButtons[MOUSE_RIGHT] = 0;
        }
        return 0;
    case WM_MBUTTONDOWN:
        if (self) {
            self->_SetMouseFromWindowCoords((int)(short)LOWORD(lParam),
                                            (int)(short)HIWORD(lParam));
            self->_mouseButtons[MOUSE_MIDDLE] = 1;
        }
        return 0;
    case WM_MBUTTONUP:
        if (self) {
            self->_SetMouseFromWindowCoords((int)(short)LOWORD(lParam),
                                            (int)(short)HIWORD(lParam));
            self->_mouseButtons[MOUSE_MIDDLE] = 0;
        }
        return 0;

    case WM_PAINT: {
            PAINTSTRUCT ps;
            BeginPaint(hWnd, &ps);
            if (self) {
                self->_PresentFrame(ps.hdc);
            }
            EndPaint(hWnd, &ps);
        }
        return 0;

    default:
        return DefWindowProcW(hWnd, msg, wParam, lParam);
    }

    return DefWindowProcW(hWnd, msg, wParam, lParam);
}


//---------------------------------------------------------------------
// Initialize DIB info (32-bit ARGB)
//---------------------------------------------------------------------
void GameLib::_InitDIBInfo(void *ptr, int width, int height)
{
    BITMAPINFO *info = (BITMAPINFO*)ptr;
    memset(info, 0, sizeof(BITMAPINFOHEADER));
    info->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    info->bmiHeader.biWidth = width;
    info->bmiHeader.biHeight = -height;  // negative value means top-down
    info->bmiHeader.biPlanes = 1;
    info->bmiHeader.biBitCount = 32;
    info->bmiHeader.biCompression = BI_RGB;
    info->bmiHeader.biSizeImage = (DWORD)((size_t)width * height * 4);
}


//---------------------------------------------------------------------
// Dispatch Windows messages
//---------------------------------------------------------------------
void GameLib::_DispatchMessages()
{
    MSG msg;
    while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
        if (msg.message == WM_QUIT) {
            _closing = true;
            break;
        }
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}


//---------------------------------------------------------------------
// Open: create window and initialize
//---------------------------------------------------------------------
int GameLib::Open(int width, int height, const char *title, bool center, bool resizable)
{
    // Validate dimensions
    if (width <= 0 || height <= 0 || width > 16384 || height > 16384) return -7;

    // Destroy existing resources first
    _DestroyTimingResources();
    _DestroyGraphicsResources();
    if (_hwnd) {
        DestroyWindow(_hwnd);
        _hwnd = NULL;
    }
    {
        MSG msg;
        while (PeekMessage(&msg, NULL, WM_QUIT, WM_QUIT, PM_REMOVE)) {
        }
    }

    if (_InitWindowClass() != 0) return -1;

    _width = width;
    _height = height;
    _windowWidth = width;
    _windowHeight = height;
    _resizable = resizable;
    _title = title ? title : "";
    _closing = false;
    _active = true;

    // Initialize DIB info
    _InitDIBInfo(_bmi_data, width, height);

    // Create memory DC
    _memDC = _gl_CreateCompatibleDC(NULL);
    if (!_memDC) return -2;

    // Create DIB Section (frame buffer memory is managed by DIB Section)
    void *pBits = NULL;
    _dibSection = _gl_CreateDIBSection(_memDC, (BITMAPINFO*)_bmi_data,
                                        DIB_RGB_COLORS, &pBits, NULL, 0);
    if (!_dibSection || !pBits) {
        _gl_DeleteDC(_memDC);
        _memDC = NULL;
        return -3;
    }
    _framebuffer = (uint32_t*)pBits;
    memset(_framebuffer, 0, (size_t)width * height * sizeof(uint32_t));

    // Select DIB Section into memory DC
    _oldBmp = (HBITMAP)_gl_SelectObject(_memDC, _dibSection);
    if (!_oldBmp) {
        _gl_DeleteObject(_dibSection);
        _gl_DeleteDC(_memDC);
        _dibSection = NULL;
        _memDC = NULL;
        _framebuffer = NULL;
        return -4;
    }

    // Calculate window size (make client area equal to width x height)
    DWORD style = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;
    if (resizable) {
        style |= WS_THICKFRAME | WS_MAXIMIZEBOX;
    }
    RECT rc = { 0, 0, width, height };
    AdjustWindowRect(&rc, style, FALSE);
    int ww = rc.right - rc.left;
    int wh = rc.bottom - rc.top;

    // Window position: let system decide by default, center if requested
    int posX = CW_USEDEFAULT;
    int posY = CW_USEDEFAULT;
    if (center) {
        int screenW = GetSystemMetrics(SM_CXSCREEN);
        int screenH = GetSystemMetrics(SM_CYSCREEN);
        posX = (screenW - ww) / 2;
        posY = (screenH - wh) / 2;
    }

    // Convert UTF-8 to wide string (query required size first)
    const char *windowTitle = _title.c_str();
    int wtitleLen = MultiByteToWideChar(CP_UTF8, 0, windowTitle, -1, NULL, 0);
    if (wtitleLen <= 0) {
        _DestroyGraphicsResources();
        return -5;
    }
    wchar_t *wtitle = (wchar_t*)malloc(wtitleLen * sizeof(wchar_t));
    if (!wtitle) {
        _DestroyGraphicsResources();
        return -5;
    }
    MultiByteToWideChar(CP_UTF8, 0, windowTitle, -1, wtitle, wtitleLen);

    HINSTANCE inst = GetModuleHandle(NULL);
    _hwnd = CreateWindowW(L"GameLibWindowClass", wtitle, style,
        posX, posY, ww, wh, NULL, NULL, inst, this);

    free(wtitle);

    if (!_hwnd) {
        _DestroyGraphicsResources();
        return -6;
    }

    // Second adjustment after creation: ensure client area is exactly width x height
    // AdjustWindowRect may not be accurate in some DPI settings
    {
        RECT clientRC;
        ::GetClientRect(_hwnd, &clientRC);
        int cw = clientRC.right - clientRC.left;
        int ch = clientRC.bottom - clientRC.top;
        if (cw != width || ch != height) {
            RECT winRC;
            ::GetWindowRect(_hwnd, &winRC);
            int winW = winRC.right - winRC.left;
            int winH = winRC.bottom - winRC.top;
            int adjustW = winW + (width - cw);
            int adjustH = winH + (height - ch);
            int adjX = winRC.left;
            int adjY = winRC.top;
            if (center) {
                int screenW = GetSystemMetrics(SM_CXSCREEN);
                int screenH = GetSystemMetrics(SM_CYSCREEN);
                adjX = (screenW - adjustW) / 2;
                adjY = (screenH - adjustH) / 2;
            }
            ::MoveWindow(_hwnd, adjX, adjY, adjustW, adjustH, TRUE);
        }
    }

    _UpdateClientSize();

    ShowWindow(_hwnd, SW_SHOW);
    UpdateWindow(_hwnd);
    ShowMouse(_mouseVisible);

    // Initialize high-resolution timing.
    LARGE_INTEGER perfFrequency;
    perfFrequency.QuadPart = 0;
    QueryPerformanceFrequency(&perfFrequency);
    _perfFrequency = (perfFrequency.QuadPart > 0) ? (uint64_t)perfFrequency.QuadPart : 1;

    if (_gl_timeBeginPeriod && _gl_timeBeginPeriod(1) == 0) {
        _timerPeriodActive = true;
    }
    _timeStartCounter = _gamelib_query_performance_counter();
    _timePrevCounter = _timeStartCounter;
    _fpsTimeCounter = _timeStartCounter;
    _frameStartCounter = _timeStartCounter;
    _fpsAccum = 0.0;
    _fps = 0.0;
    _deltaTime = 0.0;

    // Create 1ms periodic multimedia timer for precise frame timing
    _timerEvent = CreateEventA(NULL, FALSE, FALSE, NULL);
    _timerId = 0;
    if (_timerEvent && _gl_timeSetEvent) {
        // TIME_PERIODIC=1, TIME_CALLBACK_EVENT_SET=0x0010
        _timerId = _gl_timeSetEvent(1, 0, (DWORD_PTR)_timerEvent, 0, 0x0001 | 0x0010);
    }

    // Initialize input
    memset(_keys, 0, sizeof(_keys));
    memset(_keys_prev, 0, sizeof(_keys_prev));
    memset(_mouseButtons, 0, sizeof(_mouseButtons));
    memset(_mouseButtons_prev, 0, sizeof(_mouseButtons_prev));
    _mouseWheelDelta = 0;
    _uiActiveId = 0;
    ClearClip();

    return 0;
}


//---------------------------------------------------------------------
// IsClosed
//---------------------------------------------------------------------
bool GameLib::IsClosed() const
{
    return _closing;
}


//---------------------------------------------------------------------
// Update: flush frame buffer to window, then process messages and update input
//---------------------------------------------------------------------
void GameLib::Update()
{
    if (!_hwnd || !_framebuffer || !_memDC) return;

    // Save previous frame key/mouse state (for edge detection)
    memcpy(_keys_prev, _keys, sizeof(_keys));
    memcpy(_mouseButtons_prev, _mouseButtons, sizeof(_mouseButtons));
    _mouseWheelDelta = 0;

    // Dispatch messages
    _DispatchMessages();

    // Draw frame buffer to window using BitBlt when sizes match, otherwise stretch to fill.
    HDC hdc = ::GetDC(_hwnd);
    if (hdc) {
        _PresentFrame(hdc);
        ::ReleaseDC(_hwnd, hdc);
    }

    // Update time with QueryPerformanceCounter.
    uint64_t now = _gamelib_query_performance_counter();
    if (now < _timePrevCounter) now = _timePrevCounter;
    uint64_t deltaCounter = now - _timePrevCounter;
    _deltaTime = (double)deltaCounter / (double)_perfFrequency;
    _timePrevCounter = now;

    // Update FPS
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
}


//---------------------------------------------------------------------
// WaitFrame: frame rate control
//---------------------------------------------------------------------
void GameLib::WaitFrame(int fps)
{
    if (fps <= 0) fps = 60;
    if (_perfFrequency == 0) return;

    uint64_t frameTime = (uint64_t)((double)_perfFrequency / (double)fps);
    if (frameTime == 0) frameTime = 1;

    if (_frameStartCounter == 0) {
        _frameStartCounter = _gamelib_query_performance_counter();
    }

    uint64_t target = _frameStartCounter + frameTime;
    uint64_t now = _gamelib_query_performance_counter();

    if (now >= target) {
        _frameStartCounter = now;
        return;
    }

    for (;;) {
        now = _gamelib_query_performance_counter();
        if (now >= target) break;

        uint64_t remaining = target - now;
        double remainingMs = (double)remaining * 1000.0 / (double)_perfFrequency;

        if (_timerEvent && _timerId && remainingMs > 1.5) {
            WaitForSingleObject(_timerEvent, 1);
        } else if (remainingMs > 1.5) {
            Sleep(1);
        } else if (remainingMs > 0.3) {
            Sleep(0);
        }
    }

    _frameStartCounter = target;
}


//---------------------------------------------------------------------
// GetDeltaTime / GetFPS / GetTime / GetWidth / GetHeight
//---------------------------------------------------------------------
double GameLib::GetDeltaTime() const { return _deltaTime; }
double GameLib::GetFPS() const { return _fps; }
double GameLib::GetTime() const
{
    if (_perfFrequency == 0 || _timeStartCounter == 0) return 0.0;
    uint64_t now = _gamelib_query_performance_counter();
    if (now < _timeStartCounter) now = _timeStartCounter;
    return (double)(now - _timeStartCounter) / (double)_perfFrequency;
}
int GameLib::GetWidth() const { return _width; }
int GameLib::GetHeight() const { return _height; }


void GameLib::WinResize(int width, int height)
{
    if (!_hwnd || width <= 0 || height <= 0) return;

    if (_resizable && IsZoomed(_hwnd)) {
        ShowWindow(_hwnd, SW_RESTORE);
    }

    RECT winRC;
    RECT clientRC;
    if (!::GetWindowRect(_hwnd, &winRC) || !::GetClientRect(_hwnd, &clientRC)) return;

    int currentClientW = clientRC.right - clientRC.left;
    int currentClientH = clientRC.bottom - clientRC.top;
    int winW = winRC.right - winRC.left;
    int winH = winRC.bottom - winRC.top;
    int targetWinW = winW + (width - currentClientW);
    int targetWinH = winH + (height - currentClientH);

    ::MoveWindow(_hwnd, winRC.left, winRC.top, targetWinW, targetWinH, TRUE);
    _UpdateClientSize();
}


void GameLib::SetMaximized(bool maximized)
{
    if (!_hwnd || !_resizable) return;
    ShowWindow(_hwnd, maximized ? SW_MAXIMIZE : SW_RESTORE);
    _UpdateClientSize();
}


//---------------------------------------------------------------------
// SetTitle
//---------------------------------------------------------------------
void GameLib::SetTitle(const char *title)
{
    _title = title ? title : "";
    if (_hwnd) {
        int wlen = MultiByteToWideChar(CP_UTF8, 0, _title.c_str(), -1, NULL, 0);
        if (wlen > 0) {
            wchar_t *wt = (wchar_t*)malloc(wlen * sizeof(wchar_t));
            if (wt) {
                MultiByteToWideChar(CP_UTF8, 0, _title.c_str(), -1, wt, wlen);
                SetWindowTextW(_hwnd, wt);
                free(wt);
            }
        }
    }
}


//---------------------------------------------------------------------
// ShowFps: whether to show FPS in title bar
//---------------------------------------------------------------------
void GameLib::ShowFps(bool show)
{
    _showFps = show;
    if (!show && _hwnd) {
        // Restore original title when turned off
        SetTitle(_title.c_str());
    }
}

void GameLib::ShowMouse(bool show)
{
    _mouseVisible = show;
    if (_hwnd) {
        if (_mouseVisible) {
            SetCursor(LoadCursor(NULL, IDC_ARROW));
        } else {
            SetCursor(NULL);
        }
    }
}

int GameLib::ShowMessage(const char *text, const char *title, int buttons)
{
    const char *messageText = text ? text : "";
    const char *messageTitle = title;
    if (!messageTitle || !messageTitle[0]) {
        messageTitle = _title.empty() ? "GameLib" : _title.c_str();
    }

    wchar_t *wideText = _gamelib_utf8_to_wide(messageText, NULL);
    wchar_t *wideTitle = _gamelib_utf8_to_wide(messageTitle, NULL);
    if (!wideText || !wideTitle) {
        if (wideText) free(wideText);
        if (wideTitle) free(wideTitle);
        return (buttons == MESSAGEBOX_YESNO) ? MESSAGEBOX_RESULT_NO : MESSAGEBOX_RESULT_OK;
    }

    UINT flags = MB_OK | MB_ICONINFORMATION;
    if (buttons == MESSAGEBOX_YESNO) {
        flags = MB_YESNO | MB_ICONQUESTION;
    }

    int nativeResult = MessageBoxW(_hwnd, wideText, wideTitle, flags);
    free(wideText);
    free(wideTitle);

    if (nativeResult == IDYES) return MESSAGEBOX_RESULT_YES;
    if (nativeResult == IDNO) return MESSAGEBOX_RESULT_NO;
    if (buttons == MESSAGEBOX_YESNO) return MESSAGEBOX_RESULT_NO;
    return MESSAGEBOX_RESULT_OK;
}


//---------------------------------------------------------------------
// _UpdateTitleFps: update title bar FPS display (internal method)
//---------------------------------------------------------------------
void GameLib::_UpdateTitleFps()
{
    if (!_showFps || !_hwnd) return;
    char buf[256];
    snprintf(buf, sizeof(buf), "%s (FPS: %.1f)", _title.c_str(), _fps);
    int wlen = MultiByteToWideChar(CP_UTF8, 0, buf, -1, NULL, 0);
    if (wlen > 0) {
        wchar_t *wt = (wchar_t*)malloc(wlen * sizeof(wchar_t));
        if (wt) {
            MultiByteToWideChar(CP_UTF8, 0, buf, -1, wt, wlen);
            SetWindowTextW(_hwnd, wt);
            free(wt);
        }
    }
}


//=====================================================================
// Frame Buffer Operations
//=====================================================================

void GameLib::Screenshot(const char *filename)
{
    if (!filename) return;
    if (!_framebuffer || _width <= 0 || _height <= 0) return;

    FILE *fp = _gamelib_fopen_utf8(filename, L"wb");
    if (!fp) return;

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

    if (fwrite(fh, 14, 1, fp) != 1 || fwrite(ih, 40, 1, fp) != 1) {
        fclose(fp);
        return;
    }

    unsigned char *rowBuf = (unsigned char*)malloc((size_t)rowSize);
    if (!rowBuf) {
        fclose(fp);
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
        if (fwrite(rowBuf, (size_t)rowSize, 1, fp) != 1) {
            free(rowBuf);
            fclose(fp);
            return;
        }
    }

    free(rowBuf);
    fclose(fp);
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
    if (_framebuffer && x >= _clipX && x < _clipX + _clipW && y >= _clipY && y < _clipY + _clipH) {
        _gamelib_blend_pixel(_framebuffer + y * _width + x, color);
    }
}

uint32_t GameLib::GetPixel(int x, int y) const
{
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


//=====================================================================
// Drawing Functions
//=====================================================================

//---------------------------------------------------------------------
// DrawLine: Bresenham's algorithm
//---------------------------------------------------------------------
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


//---------------------------------------------------------------------
// Horizontal line (internal use, with clipping)
//---------------------------------------------------------------------
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


//---------------------------------------------------------------------
// DrawRect / FillRect
//---------------------------------------------------------------------
void GameLib::DrawRect(int x, int y, int w, int h, uint32_t color)
{
    if (w <= 0 || h <= 0) return;
    _DrawHLine(x, x + w - 1, y, color);
    _DrawHLine(x, x + w - 1, y + h - 1, color);
    // Vertical edges (skip corners already drawn by _DrawHLine)
    for (int j = y + 1; j < y + h - 1; j++) {
        SetPixel(x, j, color);
        SetPixel(x + w - 1, j, color);
    }
}

void GameLib::FillRect(int x, int y, int w, int h, uint32_t color)
{
    if (!_framebuffer || w <= 0 || h <= 0) return;
    int x1 = x, y1 = y, x2 = x + w, y2 = y + h;
    if (!_ClipRectToCurrentClip(&x1, &y1, &x2, &y2)) return;

    bool opaque = COLOR_GET_A(color) == 255;
    for (int j = y1; j < y2; j++) {
        uint32_t *row = _framebuffer + j * _width;
        for (int i = x1; i < x2; i++) {
            if (opaque) {
                row[i] = color;
            } else {
                _gamelib_blend_pixel(row + i, color);
            }
        }
    }
}


//---------------------------------------------------------------------
// DrawCircle: midpoint circle algorithm
//---------------------------------------------------------------------
void GameLib::DrawCircle(int cx, int cy, int r, uint32_t color)
{
    if (r < 0) return;
    int x = 0, y = r;
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


//---------------------------------------------------------------------
// FillCircle
//---------------------------------------------------------------------
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


//---------------------------------------------------------------------
// DrawTriangle
//---------------------------------------------------------------------
void GameLib::DrawTriangle(int x1, int y1, int x2, int y2, int x3, int y3, uint32_t color)
{
    DrawLine(x1, y1, x2, y2, color);
    DrawLine(x2, y2, x3, y3, color);
    DrawLine(x3, y3, x1, y1, color);
}


//---------------------------------------------------------------------
// FillTriangle: scanline fill
//---------------------------------------------------------------------
void GameLib::FillTriangle(int x1, int y1, int x2, int y2, int x3, int y3, uint32_t color)
{
    // Sort by y: y1 <= y2 <= y3
    if (y1 > y2) { int t; t=x1; x1=x2; x2=t; t=y1; y1=y2; y2=t; }
    if (y1 > y3) { int t; t=x1; x1=x3; x3=t; t=y1; y1=y3; y3=t; }
    if (y2 > y3) { int t; t=x2; x2=x3; x3=t; t=y2; y2=y3; y3=t; }

    if (y3 == y1) {
        int minX = x1, maxX = x1;
        if (x2 < minX) minX = x2;
        if (x2 > maxX) maxX = x2;
        if (x3 < minX) minX = x3;
        if (x3 > maxX) maxX = x3;
        _DrawHLine(minX, maxX, y1, color);
        return;
    }

    for (int y = y1; y <= y3; y++) {
        int xa, xb;
        // y3 != y1 is always true here (degenerate case returned above)
        xa = x1 + (int)((int64_t)(x3 - x1) * (y - y1) / (y3 - y1));
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


//=====================================================================
// Text Rendering
//=====================================================================

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

void GameLib::DrawText(int x, int y, const char *text, uint32_t color)
{
    if (!text) return;
    int ox = x;
    for (const char *p = text; *p; p++) {
        unsigned char ch = (unsigned char)*p;
        if (ch == '\n') {
            x = ox;
            y += 10;  // 8 pixels high + 2 pixels line spacing
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

void GameLib::DrawPrintfScale(int x, int y, uint32_t color, int scale, const char *fmt, ...)
{
    char buf[1024];
    va_list args;
    va_start(args, fmt);
    vsnprintf(buf, sizeof(buf), fmt, args);
    va_end(args);
    buf[sizeof(buf) - 1] = '\0';
    DrawTextScale(x, y, buf, color, scale);
}


//=====================================================================
// Font Text Rendering (current Windows backend: GDI)
//=====================================================================

static wchar_t *_gamelib_utf8_to_wide(const char *text, int *outLen)
{
    if (outLen) *outLen = 0;
    if (!text) return NULL;

    int wideLen = MultiByteToWideChar(CP_UTF8, 0, text, -1, NULL, 0);
    if (wideLen <= 0) return NULL;

    wchar_t *wideText = (wchar_t*)malloc((size_t)wideLen * sizeof(wchar_t));
    if (!wideText) return NULL;

    if (MultiByteToWideChar(CP_UTF8, 0, text, -1, wideText, wideLen) <= 0) {
        free(wideText);
        return NULL;
    }

    if (outLen) *outLen = wideLen - 1;
    return wideText;
}

static FILE *_gamelib_fopen_utf8(const char *filename, const wchar_t *mode)
{
    if (!filename || !mode) return NULL;

    wchar_t *wideFilename = _gamelib_utf8_to_wide(filename, NULL);
    if (!wideFilename) return NULL;

    FILE *fp = _wfopen(wideFilename, mode);
    free(wideFilename);
    return fp;
}

static bool _gamelib_mci_path_is_safe(const char *filename)
{
    if (!filename) return false;

    for (const char *p = filename; *p; p++) {
        if (*p == '"' || *p == '\r' || *p == '\n') return false;
    }
    return true;
}

static char _gamelib_ascii_tolower(char ch)
{
    if (ch >= 'A' && ch <= 'Z') return (char)(ch - 'A' + 'a');
    return ch;
}

static bool _gamelib_path_has_extension(const char *filename, const char *extension)
{
    if (!filename || !extension || !extension[0]) return false;

    const char *dot = strrchr(filename, '.');
    if (!dot || !dot[1]) return false;

    const char *lhs = dot + 1;
    const char *rhs = extension;
    while (*lhs && *rhs) {
        if (_gamelib_ascii_tolower(*lhs) != _gamelib_ascii_tolower(*rhs)) return false;
        lhs++;
        rhs++;
    }
    return *lhs == '\0' && *rhs == '\0';
}

static const wchar_t *_gamelib_mci_device_type_for_path(const char *filename)
{
    if (_gamelib_path_has_extension(filename, "mp3")) return L"mpegvideo";
    if (_gamelib_path_has_extension(filename, "mid")) return L"sequencer";
    if (_gamelib_path_has_extension(filename, "midi")) return L"sequencer";
    if (_gamelib_path_has_extension(filename, "wav")) return L"waveaudio";
    return NULL;
}

static bool _gamelib_mci_is_midi_path(const char *filename)
{
    return _gamelib_path_has_extension(filename, "mid") ||
           _gamelib_path_has_extension(filename, "midi");
}

static bool _gamelib_mci_play_music_alias(const wchar_t *alias, HWND callbackWindow, bool isMidi, bool loop)
{
    if (!_gl_mciSendStringW || !alias || !alias[0]) return false;

    std::wstring playCmd = L"play ";
    playCmd += alias;

    if (isMidi) {
        playCmd += L" from 0 notify";
        return _gl_mciSendStringW(playCmd.c_str(), NULL, 0, callbackWindow) == 0;
    }

    if (loop) playCmd += L" repeat";
    return _gl_mciSendStringW(playCmd.c_str(), NULL, 0, NULL) == 0;
}

static bool _gamelib_read_text_line(FILE *fp, std::string &line)
{
    line.clear();
    if (!fp) return false;

    int ch = 0;
    while ((ch = fgetc(fp)) != EOF) {
        if (ch == '\n') break;
        line.push_back((char)ch);
    }

    if (ch == EOF && line.empty()) return false;
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

static void _gamelib_close_music_alias(const wchar_t *alias)
{
    if (!_gl_mciSendStringW || !alias || !alias[0]) return;

    std::wstring stopCmd = L"stop ";
    stopCmd += alias;
    std::wstring closeCmd = L"close ";
    closeCmd += alias;
    _gl_mciSendStringW(stopCmd.c_str(), NULL, 0, NULL);
    _gl_mciSendStringW(closeCmd.c_str(), NULL, 0, NULL);
}

static HFONT _gamelib_create_font_utf8(const char *fontName, int fontSize)
{
    const char *name = fontName ? fontName : GAMELIB_DEFAULT_FONT_NAME;
    wchar_t *wideFont = _gamelib_utf8_to_wide(name, NULL);
    if (!wideFont) return NULL;

    HFONT font = _gl_CreateFontW(
        fontSize, 0, 0, 0, FW_NORMAL,
        FALSE, FALSE, FALSE,
        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
        CLIP_DEFAULT_PRECIS,
        ANTIALIASED_QUALITY,
        DEFAULT_PITCH,
        wideFont);

    free(wideFont);
    return font;
}

static void _gamelib_measure_font_text(HDC dc, const wchar_t *wideText, int *outWidth, int *outHeight)
{
    if (outWidth) *outWidth = 0;
    if (outHeight) *outHeight = 0;
    if (!dc || !wideText || !*wideText || !_gl_GetTextExtentPoint32W) return;

    SIZE sample = { 0, 0 };
    _gl_GetTextExtentPoint32W(dc, L"Hg", 2, &sample);
    int lineHeight = sample.cy;

    int maxWidth = 0;
    int totalHeight = 0;
    const wchar_t *lineStart = wideText;
    const wchar_t *cursor = wideText;

    for (;;) {
        if (*cursor == L'\n' || *cursor == L'\0') {
            int lineLen = (int)(cursor - lineStart);
            if (lineLen > 0 && lineStart[lineLen - 1] == L'\r') {
                lineLen--;
            }

            SIZE lineSize = { 0, 0 };
            if (lineLen > 0) {
                _gl_GetTextExtentPoint32W(dc, lineStart, lineLen, &lineSize);
                if (lineSize.cx > maxWidth) maxWidth = lineSize.cx;
                if (lineHeight <= 0) lineHeight = lineSize.cy;
            }

            if (lineHeight > 0) totalHeight += lineHeight;

            if (*cursor == L'\0') break;
            lineStart = cursor + 1;
        }
        cursor++;
    }

    if (outWidth) *outWidth = maxWidth;
    if (outHeight) *outHeight = totalHeight;
}

void GameLib::DrawTextFont(int x, int y, const char *text, uint32_t color, const char *fontName, int fontSize)
{
    if (!_memDC || !text || fontSize <= 0) return;
    if (COLOR_GET_A(color) == 0) return;

    wchar_t *wideText = _gamelib_utf8_to_wide(text, NULL);
    if (!wideText) return;

    HFONT font = _gamelib_create_font_utf8(fontName, fontSize);

    if (font) {
        HFONT oldFont = (HFONT)_gl_SelectObject(_memDC, font);
        int textWidth = 0;
        int textHeight = 0;
        _gamelib_measure_font_text(_memDC, wideText, &textWidth, &textHeight);

        int screenX0 = x;
        int screenY0 = y;
        int screenX1 = x + textWidth;
        int screenY1 = y + textHeight;
        if (screenX0 < 0) screenX0 = 0;
        if (screenY0 < 0) screenY0 = 0;
        if (screenX1 > _width) screenX1 = _width;
        if (screenY1 > _height) screenY1 = _height;

        uint32_t *savedPixels = NULL;
        int savedWidth = 0;
        int savedHeight = 0;
        int visibleX0 = screenX0;
        int visibleY0 = screenY0;
        int visibleX1 = screenX1;
        int visibleY1 = screenY1;
        bool canDraw = false;

        if (screenX0 < screenX1 && screenY0 < screenY1 &&
            _ClipRectToCurrentClip(&visibleX0, &visibleY0, &visibleX1, &visibleY1)) {
            bool needRestoreOutsideClip = (screenX0 != visibleX0) || (screenY0 != visibleY0) ||
                                          (screenX1 != visibleX1) || (screenY1 != visibleY1);
            bool needSavedPixels = _framebuffer && (COLOR_GET_A(color) < 255 || needRestoreOutsideClip);

            savedWidth = screenX1 - screenX0;
            savedHeight = screenY1 - screenY0;
            if (needSavedPixels && savedWidth > 0 && savedHeight > 0) {
                savedPixels = (uint32_t*)malloc((size_t)savedWidth * savedHeight * sizeof(uint32_t));
                if (savedPixels) {
                    for (int py = 0; py < savedHeight; py++) {
                        memcpy(savedPixels + py * savedWidth,
                               _framebuffer + (screenY0 + py) * _width + screenX0,
                               (size_t)savedWidth * sizeof(uint32_t));
                    }
                    canDraw = true;
                } else if (!needRestoreOutsideClip) {
                    canDraw = true;
                }
            } else {
                canDraw = true;
            }
        }

        if (canDraw) {
            // Set text color (convert ARGB to COLORREF: swap R and B)
            COLORREF cref = RGB(COLOR_GET_R(color), COLOR_GET_G(color), COLOR_GET_B(color));
            _gl_SetTextColor(_memDC, cref);
            _gl_SetBkMode(_memDC, 1);  // TRANSPARENT

            // Draw text line by line so '\n' works the same way as DrawText.
            SIZE sample = { 0, 0 };
            _gl_GetTextExtentPoint32W(_memDC, L"Hg", 2, &sample);
            int lineHeight = sample.cy > 0 ? sample.cy : fontSize;
            int penY = y;
            const wchar_t *lineStart = wideText;
            const wchar_t *cursor = wideText;

            for (;;) {
                if (*cursor == L'\n' || *cursor == L'\0') {
                    int lineLen = (int)(cursor - lineStart);
                    if (lineLen > 0 && lineStart[lineLen - 1] == L'\r') {
                        lineLen--;
                    }
                    if (lineLen > 0) {
                        _gl_TextOutW(_memDC, x, penY, lineStart, lineLen);
                    }
                    penY += lineHeight;
                    if (*cursor == L'\0') break;
                    lineStart = cursor + 1;
                }
                cursor++;
            }

            // Flush GDI to ensure writes are visible in the framebuffer.
            if (_gl_GdiFlush) _gl_GdiFlush();

            // GDI TextOut writes alpha=0. Repair the text bounding box to keep
            // the framebuffer consistent for later alpha-aware drawing.
            if (_framebuffer) {
                for (int py = screenY0; py < screenY1; py++) {
                    uint32_t *row = _framebuffer + py * _width;
                    for (int px = screenX0; px < screenX1; px++) {
                        uint32_t c = row[px];
                        if ((c & 0xFF000000) == 0 && (c & 0x00FFFFFF) != 0) {
                            row[px] = c | 0xFF000000;
                        }
                    }
                }

                if (savedPixels) {
                    uint32_t alphaValue = COLOR_GET_A(color);
                    for (int py = 0; py < savedHeight; py++) {
                        int dstY = screenY0 + py;
                        bool insideVisibleY = (dstY >= visibleY0 && dstY < visibleY1);
                        uint32_t *dstRow = _framebuffer + dstY * _width + screenX0;
                        uint32_t *srcRow = savedPixels + py * savedWidth;
                        for (int px = 0; px < savedWidth; px++) {
                            int dstX = screenX0 + px;
                            uint32_t before = srcRow[px];
                            if (!insideVisibleY || dstX < visibleX0 || dstX >= visibleX1) {
                                dstRow[px] = before;
                                continue;
                            }

                            if (alphaValue < 255) {
                                uint32_t after = dstRow[px];
                                if (after == before) continue;
                                uint32_t blendedSrc = COLOR_ARGB(alphaValue,
                                                                 COLOR_GET_R(after),
                                                                 COLOR_GET_G(after),
                                                                 COLOR_GET_B(after));
                                dstRow[px] = _gamelib_alpha_blend(before, blendedSrc);
                            }
                        }
                    }
                    free(savedPixels);
                }
            }
        }

        // Restore old font
        _gl_SelectObject(_memDC, oldFont);
        _gl_DeleteObject(font);
    }

    free(wideText);
}

void GameLib::DrawTextFont(int x, int y, const char *text, uint32_t color, int fontSize)
{
    DrawTextFont(x, y, text, color, GAMELIB_DEFAULT_FONT_NAME, fontSize);
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
    DrawTextFont(x, y, buf, color, GAMELIB_DEFAULT_FONT_NAME, fontSize);
}

int GameLib::GetTextWidthFont(const char *text, const char *fontName, int fontSize)
{
    if (!_memDC || !text || fontSize <= 0) return 0;

    wchar_t *wideText = _gamelib_utf8_to_wide(text, NULL);
    if (!wideText) return 0;

    HFONT font = _gamelib_create_font_utf8(fontName, fontSize);

    int width = 0;
    if (font) {
        HFONT oldFont = (HFONT)_gl_SelectObject(_memDC, font);
        _gamelib_measure_font_text(_memDC, wideText, &width, NULL);
        _gl_SelectObject(_memDC, oldFont);
        _gl_DeleteObject(font);
    }

    free(wideText);
    return width;
}

int GameLib::GetTextWidthFont(const char *text, int fontSize)
{
    return GetTextWidthFont(text, GAMELIB_DEFAULT_FONT_NAME, fontSize);
}

int GameLib::GetTextHeightFont(const char *text, const char *fontName, int fontSize)
{
    if (!_memDC || !text || fontSize <= 0) return 0;

    wchar_t *wideText = _gamelib_utf8_to_wide(text, NULL);
    if (!wideText) return 0;

    HFONT font = _gamelib_create_font_utf8(fontName, fontSize);

    int height = 0;
    if (font) {
        HFONT oldFont = (HFONT)_gl_SelectObject(_memDC, font);
        _gamelib_measure_font_text(_memDC, wideText, NULL, &height);
        _gl_SelectObject(_memDC, oldFont);
        _gl_DeleteObject(font);
    }

    free(wideText);
    return height;
}

int GameLib::GetTextHeightFont(const char *text, int fontSize)
{
    return GetTextHeightFont(text, GAMELIB_DEFAULT_FONT_NAME, fontSize);
}


//=====================================================================
// Sprite System
//=====================================================================

int GameLib::_AllocSpriteSlot()
{
    for (size_t i = 0; i < _sprites.size(); i++) {
        if (!_sprites[i].used) {
            _sprites[i].colorKey = COLORKEY_DEFAULT;
            return (int)i;
        }
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
    _sprites[id].used = true;
    return id;
}

int GameLib::LoadSpriteBMP(const char *filename)
{
    FILE *fp = _gamelib_fopen_utf8(filename, L"rb");
    if (!fp) return -1;

    unsigned char header[54];
    if (fread(header, 1, 54, fp) != 54) { fclose(fp); return -1; }
    if (header[0] != 'B' || header[1] != 'M') { fclose(fp); return -1; }

    // Read header fields using memcpy to avoid strict aliasing / unaligned access
    int dataOffset; memcpy(&dataOffset, &header[10], 4);
    int dibSize;    memcpy(&dibSize,    &header[14], 4);
    int width;      memcpy(&width,      &header[18], 4);
    int height;     memcpy(&height,     &header[22], 4);
    short bppShort; memcpy(&bppShort,   &header[28], 2);
    int bpp = bppShort;
    int colorsUsed; memcpy(&colorsUsed, &header[46], 4);

    if (bpp != 8 && bpp != 24 && bpp != 32) { fclose(fp); return -1; }

    bool bottomUp = (height > 0);
    if (height < 0) height = -height;

    // Validate dimensions to prevent integer overflow and unreasonable allocations
    if (width <= 0 || height <= 0 || width > 16384 || height > 16384) {
        fclose(fp); return -1;
    }

    // Read palette for 8-bit indexed BMP
    uint32_t palette[256];
    memset(palette, 0, sizeof(palette));

    if (bpp == 8) {
        int paletteCount = colorsUsed > 0 ? colorsUsed : 256;
        if (paletteCount > 256) paletteCount = 256;

        // Palette starts right after the DIB header (at offset 14 + dibSize)
        fseek(fp, 14 + dibSize, SEEK_SET);

        unsigned char palData[256 * 4];
        int palBytes = paletteCount * 4;
        if (fread(palData, 1, palBytes, fp) != (size_t)palBytes) {
            fclose(fp); return -1;
        }
        // BMP palette entries are BGRX (4 bytes each: Blue, Green, Red, Reserved)
        for (int i = 0; i < paletteCount; i++) {
            unsigned char b = palData[i * 4 + 0];
            unsigned char g = palData[i * 4 + 1];
            unsigned char r = palData[i * 4 + 2];
            palette[i] = COLOR_ARGB(0xFF, r, g, b);
        }
    }

    fseek(fp, dataOffset, SEEK_SET);

    int bytesPerPixel = bpp / 8;  // 1 for 8-bit, 3 for 24-bit, 4 for 32-bit
    int rowSize = ((width * bytesPerPixel + 3) / 4) * 4;
    unsigned char *rowData = (unsigned char*)malloc(rowSize);
    if (!rowData) { fclose(fp); return -1; }

    int id = CreateSprite(width, height);
    if (id < 0) { free(rowData); fclose(fp); return -1; }

    bool readOk = true;
    for (int y = 0; y < height; y++) {
        if (fread(rowData, 1, rowSize, fp) != (size_t)rowSize) {
            readOk = false;
            break;
        }
        int destY = bottomUp ? (height - 1 - y) : y;
        uint32_t *destRow = _sprites[id].pixels + destY * width;
        if (bpp == 8) {
            // 8-bit indexed: each byte is a palette index
            for (int x = 0; x < width; x++) {
                destRow[x] = palette[rowData[x]];
            }
        } else {
            for (int x = 0; x < width; x++) {
                unsigned char b = rowData[x * bytesPerPixel + 0];
                unsigned char g = rowData[x * bytesPerPixel + 1];
                unsigned char r = rowData[x * bytesPerPixel + 2];
                unsigned char a = (bpp == 32) ? rowData[x * bytesPerPixel + 3] : 0xFF;
                destRow[x] = COLOR_ARGB(a, r, g, b);
            }
        }
    }

    free(rowData);
    fclose(fp);
    if (!readOk) {
        FreeSprite(id);
        return -1;
    }
    return id;
}

int GameLib::LoadSprite(const char *filename)
{
    // Read file into memory
    FILE *fp = _gamelib_fopen_utf8(filename, L"rb");
    if (!fp) return -1;

    fseek(fp, 0, SEEK_END);
    long fileSize = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    if (fileSize <= 0) { fclose(fp); return -1; }

    unsigned char *fileData = (unsigned char*)malloc(fileSize);
    if (!fileData) { fclose(fp); return -1; }

    if ((long)fread(fileData, 1, fileSize, fp) != fileSize) {
        free(fileData);
        fclose(fp);
        return -1;
    }
    fclose(fp);

    // Decode via GDI+ (supports PNG/JPG/BMP/GIF/TIFF)
    int imgW = 0, imgH = 0;
    uint32_t *pixels = _gamelib_gdiplus_load(fileData, fileSize, &imgW, &imgH);

    if (!pixels || imgW <= 0 || imgH <= 0) {
        if (pixels) free(pixels);
        // GDI+ failed, if BMP file then fallback to LoadSpriteBMP
        bool isBMP = (fileSize >= 2 && fileData[0] == 'B' && fileData[1] == 'M');
        free(fileData);
        return isBMP ? LoadSpriteBMP(filename) : -1;
    }

    free(fileData);

    if (imgW > 16384 || imgH > 16384) {
        free(pixels);
        return -1;
    }

    // Allocate sprite slot, use pixel data returned by GDI+ directly
    int id = _AllocSpriteSlot();
    _sprites[id].width = imgW;
    _sprites[id].height = imgH;
    _sprites[id].pixels = pixels;
    _sprites[id].used = true;

    return id;
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
                if (c != colorKey)
                    dstRow[x + localX] = c;
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

int GameLib::CreateTilemap(int cols, int rows, int tileSize, int tilesetId)
{
    if (cols <= 0 || rows <= 0 || tileSize <= 0) return -1;
    if (tilesetId < 0 || tilesetId >= (int)_sprites.size()) return -1;
    if (!_sprites[tilesetId].used) return -1;

    if (cols > 4096 || rows > 4096) return -1;  // prevent overflow
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

    const GameTilemap &tm = _tilemaps[mapId];
    FILE *fp = _gamelib_fopen_utf8(filename, L"wb");
    if (!fp) return false;

    if (fprintf(fp, "GLM1\n%d %d %d\n", tm.tileSize, tm.rows, tm.cols) < 0) {
        fclose(fp);
        return false;
    }

    for (int row = 0; row < tm.rows; row++) {
        for (int col = 0; col < tm.cols; col++) {
            if (col > 0 && fputc(' ', fp) == EOF) {
                fclose(fp);
                return false;
            }
            if (fprintf(fp, "%d", tm.tiles[row * tm.cols + col]) < 0) {
                fclose(fp);
                return false;
            }
        }
        if (fputc('\n', fp) == EOF) {
            fclose(fp);
            return false;
        }
    }

    return fclose(fp) == 0;
}

int GameLib::LoadTilemap(const char *filename, int tilesetId)
{
    if (!filename) return -1;

    FILE *fp = _gamelib_fopen_utf8(filename, L"rb");
    if (!fp) return -1;

    std::string line;
    if (!_gamelib_read_text_line(fp, line)) {
        fclose(fp);
        return -1;
    }
    _gamelib_strip_utf8_bom(line);
    if (line != "GLM1") {
        fclose(fp);
        return -1;
    }

    if (!_gamelib_read_text_line(fp, line)) {
        fclose(fp);
        return -1;
    }

    int header[3];
    int headerCount = 0;
    if (!_gamelib_parse_int_tokens(line, header, 3, &headerCount) || headerCount < 3) {
        fclose(fp);
        return -1;
    }

    int tileSize = header[0];
    int rows = header[1];
    int cols = header[2];
    int mapId = CreateTilemap(cols, rows, tileSize, tilesetId);
    if (mapId < 0) {
        fclose(fp);
        return -1;
    }

    for (int row = 0; row < rows; row++) {
        if (!_gamelib_read_text_line(fp, line)) break;

        int count = 0;
        int *rowPtr = _tilemaps[mapId].tiles + row * cols;
        if (!_gamelib_parse_int_tokens(line, rowPtr, cols, &count)) {
            FreeTilemap(mapId);
            fclose(fp);
            return -1;
        }
        for (int col = 0; col < count; col++) {
            if (rowPtr[col] < -1) {
                FreeTilemap(mapId);
                fclose(fp);
                return -1;
            }
        }
    }

    fclose(fp);
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

static int _gamelib_floor_div(int value, int divisor)
{
    if (divisor <= 0) return 0;
    int q = value / divisor;
    int r = value % divisor;
    if (r != 0 && ((r > 0) != (divisor > 0))) q--;
    return q;
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

    // Calculate visible tile range on screen, avoid traversing the whole map
    int col0 = (clipX0 - x) / ts;
    int row0 = (clipY0 - y) / ts;
    int col1 = (clipX1 - 1 - x) / ts + 1;
    int row1 = (clipY1 - 1 - y) / ts + 1;
    if (col0 < 0) col0 = 0;
    if (row0 < 0) row0 = 0;
    if (col1 > tm.cols) col1 = tm.cols;
    if (row1 > tm.rows) row1 = tm.rows;

    bool useAlpha    = (flags & SPRITE_ALPHA) != 0;
    bool useColorKey = (flags & SPRITE_COLORKEY) != 0;
    int tileFlags = flags & (SPRITE_ALPHA | SPRITE_COLORKEY);
    bool canMemcpyTiles = !useAlpha && !useColorKey;

    for (int r = row0; r < row1; r++) {
        for (int c = col0; c < col1; c++) {
            int tid = tm.tiles[r * tm.cols + c];
            if (tid < 0 || tid >= tileCount) continue;

            // Pixel start position of this tile in tileset
            int srcCol = tid % tsCols;
            int srcRow = tid / tsCols;
            int srcX0 = srcCol * ts;
            int srcY0 = srcRow * ts;

            // Screen destination position
            int dstX0 = x + c * ts;
            int dstY0 = y + r * ts;

            if (canMemcpyTiles) {
                int ix0 = 0, iy0 = 0, ix1 = ts, iy1 = ts;
                if (dstX0 < clipX0) ix0 = clipX0 - dstX0;
                if (dstY0 < clipY0) iy0 = clipY0 - dstY0;
                if (dstX0 + ix1 > clipX1)  ix1 = clipX1 - dstX0;
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
// Input System
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


//=====================================================================
// Sound
//=====================================================================

void GameLib::PlayBeep(int frequency, int duration)
{
    Beep(frequency, duration);
}

bool GameLib::PlayMusic(const char *filename, bool loop)
{
    if (!filename || !_gl_mciSendStringW) return false;

    if (!_gamelib_mci_path_is_safe(filename)) return false;

    const wchar_t *deviceType = _gamelib_mci_device_type_for_path(filename);
    if (!deviceType) return false;

    bool isMidi = _gamelib_mci_is_midi_path(filename);

    wchar_t *wideFilename = _gamelib_utf8_to_wide(filename, NULL);
    if (!wideFilename) return false;

    // Stop previous music first
    if (_musicPlaying || _musicIsMidi) {
        _musicPlaying = false;
        _musicLoop = false;
        _musicIsMidi = false;
        _gamelib_close_music_alias(_musicAlias.c_str());
    }

    std::wstring openCmd = L"open \"";
    openCmd += wideFilename;
    openCmd += L"\" type ";
    openCmd += deviceType;
    openCmd += L" alias ";
    openCmd += _musicAlias;
    if (_gl_mciSendStringW(openCmd.c_str(), NULL, 0, NULL) != 0) {
        free(wideFilename);
        return false;
    }
    free(wideFilename);

    _musicLoop = loop;
    _musicIsMidi = isMidi;

    if (!_gamelib_mci_play_music_alias(_musicAlias.c_str(), _hwnd, isMidi, loop)) {
        _gamelib_close_music_alias(_musicAlias.c_str());
        _musicLoop = false;
        _musicIsMidi = false;
        return false;
    }

    _musicPlaying = true;
    return true;
}

void GameLib::StopMusic()
{
    bool hadMusic = _musicPlaying || _musicIsMidi;
    _musicPlaying = false;
    _musicLoop = false;
    _musicIsMidi = false;
    if (hadMusic) {
        _gamelib_close_music_alias(_musicAlias.c_str());
    }
}

bool GameLib::IsMusicPlaying() const
{
    return _musicPlaying;
}


//=====================================================================
// Helper Functions
//=====================================================================

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


//=====================================================================
// Grid Helpers
//=====================================================================

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


//=====================================================================
// UI System
//=====================================================================
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

static bool _gamelib_delete_file_utf8(const char *filename)
{
    if (!filename || !filename[0]) return false;

    wchar_t *wideFilename = _gamelib_utf8_to_wide(filename, NULL);
    if (!wideFilename) return false;

    BOOL ok = DeleteFileW(wideFilename);
    free(wideFilename);
    return ok != 0;
}

static int _gamelib_save_read_all(const char *filename,
                                  _gamelib_save_entry *entries, int maxEntries)
{
    FILE *fp = _gamelib_fopen_utf8(filename, L"r");
    if (!fp) return 0;

    char line[_GAMELIB_SAVE_MAX_LINE];
    int count = 0;

    // Read and verify magic header
    if (!fgets(line, sizeof(line), fp)) { fclose(fp); return 0; }
    // Strip trailing newline/CR
    size_t len = strlen(line);
    while (len > 0 && (line[len - 1] == '\n' || line[len - 1] == '\r')) line[--len] = '\0';
    if (strcmp(line, _GAMELIB_SAVE_MAGIC) != 0) { fclose(fp); return 0; }

    while (count < maxEntries && fgets(line, sizeof(line), fp)) {
        len = strlen(line);
        while (len > 0 && (line[len - 1] == '\n' || line[len - 1] == '\r')) line[--len] = '\0';
        if (len == 0) continue;

        // Find first '='
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
    FILE *fp = _gamelib_fopen_utf8(filename, L"w");
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

    // Remove by shifting
    for (int i = idx; i < count - 1; i++) {
        entries[i] = entries[i + 1];
    }
    count--;

    return _gamelib_save_write_all(filename, entries, count);
}

bool GameLib::DeleteSave(const char *filename)
{
    if (!filename) return false;
    return _gamelib_delete_file_utf8(filename);
}


//=====================================================================
// Software Mixer (waveOut backend, dynamically loaded)
//=====================================================================

bool GameLib::_InitAudioBackend()
{
    if (!_gl_waveOutOpen) return false;

    WAVEFORMATEX wfx;
    memset(&wfx, 0, sizeof(wfx));
    wfx.wFormatTag = WAVE_FORMAT_PCM;
    wfx.nSamplesPerSec = 44100;
    wfx.wBitsPerSample = 16;
    wfx.nChannels = 2;
    wfx.nBlockAlign = (wfx.wBitsPerSample / 8) * wfx.nChannels;
    wfx.nAvgBytesPerSec = wfx.nSamplesPerSec * wfx.nBlockAlign;
    wfx.cbSize = 0;

    MMRESULT result = _gl_waveOutOpen(&_hWaveOut, WAVE_MAPPER, &wfx,
                                       (DWORD_PTR)_WaveOutCallback,
                                       (DWORD_PTR)this, CALLBACK_FUNCTION);
    if (result != MMSYSERR_NOERROR) {
        _hWaveOut = NULL;
        return false;
    }

    // Prepare and submit initial buffers (double buffering)
    for (int i = 0; i < 2; i++) {
        _wave_hdr[i] = new WAVEHDR;
        memset(_wave_hdr[i], 0, sizeof(WAVEHDR));
        _wave_hdr[i]->lpData = (LPSTR)new char[_AUDIO_BUFFER_BYTES];
        _wave_hdr[i]->dwBufferLength = _AUDIO_BUFFER_BYTES;
        memset(_wave_hdr[i]->lpData, 0, _AUDIO_BUFFER_BYTES);
        _gl_waveOutPrepareHeader(_hWaveOut, _wave_hdr[i], sizeof(WAVEHDR));
        _gl_waveOutWrite(_hWaveOut, _wave_hdr[i], sizeof(WAVEHDR));
    }
    return true;
}

void GameLib::_ShutdownAudioBackend()
{
    if (_hWaveOut) {
        _audio_closing = true;
        _gl_waveOutReset(_hWaveOut);
        Sleep(50);
        _gl_waveOutClose(_hWaveOut);
        _hWaveOut = NULL;
    }
    for (int i = 0; i < 2; i++) {
        if (_wave_hdr[i]) {
            if (_wave_hdr[i]->lpData) {
                delete[] _wave_hdr[i]->lpData;
                _wave_hdr[i]->lpData = NULL;
            }
            delete _wave_hdr[i];
            _wave_hdr[i] = NULL;
        }
    }
}

void CALLBACK GameLib::_WaveOutCallback(HWAVEOUT hwo, UINT uMsg,
                                         DWORD_PTR dwInstance,
                                         DWORD_PTR dwParam1,
                                         DWORD_PTR dwParam2)
{
    if (uMsg != WOM_DONE) return;

    GameLib *game = (GameLib*)dwInstance;
    if (game->_audio_closing) return;

    WAVEHDR *hdr = (WAVEHDR*)dwParam1;
    if (!hdr || !hdr->lpData) return;

    int16_t output_buffer[_AUDIO_BUFFER_TOTAL];
    game->_MixAudio(output_buffer, _AUDIO_BUFFER_TOTAL);

    if (game->_audio_closing) return;

    memcpy(hdr->lpData, output_buffer, _AUDIO_BUFFER_BYTES);
    hdr->dwBufferLength = _AUDIO_BUFFER_BYTES;
    hdr->dwFlags = 0;
    _gl_waveOutPrepareHeader(hwo, hdr, sizeof(WAVEHDR));
    _gl_waveOutWrite(hwo, hdr, sizeof(WAVEHDR));
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

    EnterCriticalSection(&_audio_lock);

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
            if (ch->repeat == 0) {
                ch->position = 0;
            } else if (ch->repeat > 1) {
                ch->position = 0;
                ch->repeat--;
            } else {
                if (ch->wav) ch->wav->ref_count--;
                delete ch;
                it = _audio_channels.erase(it);
                continue;
            }
        }
        ++it;
    }

    LeaveCriticalSection(&_audio_lock);

    _ClampAndConvert(_mix_buffer, output_buffer, sample_count);
}

GameLib::_WavData *GameLib::_LoadWAVFromFile(const char *filename)
{
    FILE *f = _gamelib_fopen_utf8(filename, L"rb");
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
    if (!filename) return -2;
    if (!_audio_initialized) {
        _audio_initialized = _InitAudioBackend();
        if (!_audio_initialized) return -2;
    }

    _WavData *wav = _LoadOrCacheWAV(filename);
    if (!wav) return -1;

    int ch_id = _AllocateChannel();
    if (ch_id == 0) return -4;

    _Channel *ch = new _Channel();
    ch->id = ch_id;
    ch->wav = wav;
    ch->position = 0;
    ch->repeat = repeat;
    ch->volume = (volume < 0) ? 0 : (volume > 1000 ? 1000 : volume);
    ch->is_playing = true;

    EnterCriticalSection(&_audio_lock);
    _audio_channels[ch_id] = ch;
    LeaveCriticalSection(&_audio_lock);

    return ch_id;
}

int GameLib::StopWAV(int channel)
{
    EnterCriticalSection(&_audio_lock);
    std::unordered_map<int, _Channel*>::iterator it = _audio_channels.find(channel);
    if (it == _audio_channels.end()) {
        LeaveCriticalSection(&_audio_lock);
        return -1;
    }
    _ReleaseChannel(channel);
    LeaveCriticalSection(&_audio_lock);
    return 0;
}

int GameLib::IsPlaying(int channel)
{
    EnterCriticalSection(&_audio_lock);
    std::unordered_map<int, _Channel*>::iterator it = _audio_channels.find(channel);
    int result = -1;
    if (it != _audio_channels.end()) {
        result = it->second->is_playing ? 1 : 0;
    }
    LeaveCriticalSection(&_audio_lock);
    return result;
}

int GameLib::SetVolume(int channel, int volume)
{
    EnterCriticalSection(&_audio_lock);
    std::unordered_map<int, _Channel*>::iterator it = _audio_channels.find(channel);
    int result = -1;
    if (it != _audio_channels.end()) {
        it->second->volume = (volume < 0) ? 0 : (volume > 1000 ? 1000 : volume);
        result = 0;
    }
    LeaveCriticalSection(&_audio_lock);
    return result;
}

void GameLib::StopAll()
{
    EnterCriticalSection(&_audio_lock);
    std::vector<int> channel_ids;
    for (std::unordered_map<int, _Channel*>::iterator it = _audio_channels.begin();
         it != _audio_channels.end(); ++it) {
        channel_ids.push_back(it->first);
    }
    for (size_t i = 0; i < channel_ids.size(); i++) {
        _ReleaseChannel(channel_ids[i]);
    }
    _audio_channels.clear();
    LeaveCriticalSection(&_audio_lock);
}

int GameLib::SetMasterVolume(int volume)
{
    _master_volume = (volume < 0) ? 0 : (volume > 1000 ? 1000 : volume);
    return 0;
}

int GameLib::GetMasterVolume() const
{
    return _master_volume;
}


#endif // GAMELIB_IMPLEMENTATION

#endif // GAMELIB_H
