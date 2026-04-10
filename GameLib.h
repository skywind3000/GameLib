//=====================================================================
//
// GameLib.h - A single-header game library for beginners
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
// Last Modified: 2026/04/03
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
#define GAMELIB_VERSION_MINOR     0
#define GAMELIB_VERSION_PATCH     2


//---------------------------------------------------------------------
// System header files
//---------------------------------------------------------------------
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <stdarg.h>

#include <vector>
#include <string>


//---------------------------------------------------------------------
// Link library: only needs -mwindows, gdi32 and winmm are loaded 
// via LoadLibrary
//---------------------------------------------------------------------
#ifdef _MSC_VER
#pragma comment(lib, "user32.lib")
#endif


//---------------------------------------------------------------------
// mmsystem types and constants (no need to #include <mmsystem.h>)
//---------------------------------------------------------------------
#ifndef SND_ASYNC
#define SND_ASYNC       0x0001
#define SND_LOOP        0x0008
#define SND_FILENAME    0x00020000
#endif

#ifndef MCIERR_BASE
typedef DWORD MCIERROR;
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
typedef HFONT (WINAPI *PFN_CreateFontW)(int, int, int, int, int, DWORD, DWORD, DWORD, DWORD, DWORD, DWORD, DWORD, DWORD, const WCHAR*);
typedef BOOL (WINAPI *PFN_TextOutW)(HDC, int, int, LPCWSTR, int);
typedef COLORREF (WINAPI *PFN_SetTextColor)(HDC, COLORREF);
typedef int (WINAPI *PFN_SetBkMode)(HDC, int);
typedef BOOL (WINAPI *PFN_GetTextExtentPoint32W)(HDC, LPCWSTR, int, SIZE*);
typedef BOOL (WINAPI *PFN_GdiFlush)(void);

// winmm.dll
typedef DWORD   (WINAPI *PFN_timeGetTime)(void);
typedef DWORD   (WINAPI *PFN_timeBeginPeriod)(UINT);
typedef DWORD   (WINAPI *PFN_timeEndPeriod)(UINT);
typedef UINT (WINAPI *PFN_timeSetEvent)(UINT, UINT, DWORD_PTR, DWORD_PTR, UINT);
typedef UINT (WINAPI *PFN_timeKillEvent)(UINT);
typedef BOOL    (WINAPI *PFN_PlaySoundA)(LPCSTR, HMODULE, DWORD);
typedef MCIERROR (WINAPI *PFN_mciSendStringA)(LPCSTR, LPSTR, UINT, HWND);

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

//---------------------------------------------------------------------
// Mouse button constants
//---------------------------------------------------------------------
#define MOUSE_LEFT    0
#define MOUSE_RIGHT   1
#define MOUSE_MIDDLE  2

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

    // -------- Frame Buffer --------
    void Clear(uint32_t color = COLOR_BLACK);
    void SetPixel(int x, int y, uint32_t color);
    uint32_t GetPixel(int x, int y) const;

    // -------- Drawing --------
    void DrawLine(int x1, int y1, int x2, int y2, uint32_t color);
    void DrawRect(int x, int y, int w, int h, uint32_t color);
    void FillRect(int x, int y, int w, int h, uint32_t color);
    void DrawCircle(int cx, int cy, int r, uint32_t color);
    void FillCircle(int cx, int cy, int r, uint32_t color);
    void DrawTriangle(int x1, int y1, int x2, int y2, int x3, int y3, uint32_t color);
    void FillTriangle(int x1, int y1, int x2, int y2, int x3, int y3, uint32_t color);

    // -------- Text Rendering (built-in 8x8 font) --------
    void DrawText(int x, int y, const char *text, uint32_t color);
    void DrawNumber(int x, int y, int number, uint32_t color);
    void DrawTextScale(int x, int y, const char *text, uint32_t color, int scale);
    void DrawPrintf(int x, int y, uint32_t color, const char *fmt, ...);
    void DrawPrintfScale(int x, int y, uint32_t color, int scale, const char *fmt, ...);

    // -------- GDI Text Rendering (system fonts, Chinese support) --------
    void DrawTextGDI(int x, int y, const char *text, uint32_t color, const char *fontName, int fontSize);
    void DrawTextGDI(int x, int y, const char *text, uint32_t color, int fontSize);
    int GetTextWidthGDI(const char *text, const char *fontName, int fontSize);
    int GetTextHeightGDI(int fontSize);

    // -------- Sprite System (managed by integer ID) --------
    int CreateSprite(int width, int height);
    int LoadSpriteBMP(const char *filename);
    int LoadSprite(const char *filename);
    void FreeSprite(int id);
    void DrawSprite(int id, int x, int y);
    void DrawSpriteEx(int id, int x, int y, int flags);
    void DrawSpriteRegion(int id, int x, int y, int sx, int sy, int sw, int sh);
    void SetSpritePixel(int id, int x, int y, uint32_t color);
    uint32_t GetSpritePixel(int id, int x, int y) const;
    int GetSpriteWidth(int id) const;
    int GetSpriteHeight(int id) const;

    // -------- Input --------
    bool IsKeyDown(int key) const;
    bool IsKeyPressed(int key) const;
    int GetMouseX() const;
    int GetMouseY() const;
    bool IsMouseDown(int button) const;
    bool IsMousePressed(int button) const;

    // -------- Sound --------
    void PlayBeep(int frequency, int duration);
    void PlayWAV(const char *filename, bool loop = false);
    void StopWAV();
    void PlayMusic(const char *filename, bool loop = true);
    void StopMusic();

    // -------- Helper Functions --------
    static int Random(int minVal, int maxVal);
    static bool RectOverlap(int x1, int y1, int w1, int h1,
                            int x2, int y2, int w2, int h2);
    static bool CircleOverlap(int cx1, int cy1, int r1,
                              int cx2, int cy2, int r2);
    static bool PointInRect(int px, int py, int x, int y, int w, int h);
    static float Distance(int x1, int y1, int x2, int y2);

    // -------- Grid Helpers --------
    void DrawGrid(int x, int y, int rows, int cols, int cellSize, uint32_t color);
    void FillCell(int gridX, int gridY, int row, int col, int cellSize, uint32_t color);

    // -------- Tilemap System --------
    int CreateTilemap(int cols, int rows, int tileSize, int tilesetId);
    void FreeTilemap(int mapId);
    void SetTile(int mapId, int col, int row, int tileId);
    int GetTile(int mapId, int col, int row) const;
    void DrawTilemap(int mapId, int x, int y, int flags = 0);

private:
    // disable copy
    GameLib(const GameLib &);
    GameLib &operator=(const GameLib &);

    // internal window management
    static LRESULT CALLBACK _WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
    static int _InitWindowClass();
    void _DispatchMessages();
    void _InitDIBInfo(void *ptr, int width, int height);
    void _UpdateTitleFps();

    // internal pixel drawing (no bounds check, for fast drawing after clipping)
    void _SetPixelFast(int x, int y, uint32_t color);
    void _DrawHLine(int x1, int x2, int y, uint32_t color);

    // internal sprite management
    int _AllocSpriteSlot();

    // internal tilemap management
    int _AllocTilemapSlot();

private:
    // window state
    HWND _hwnd;
    bool _closing;
    bool _active;
    bool _showFps;
    std::string _title;
    int _width;
    int _height;

    // frame buffer
    uint32_t *_framebuffer;

    // DIB Section (for GDI text rendering)
    HDC _memDC;
    HBITMAP _dibSection;
    HBITMAP _oldBmp;

    // DIB info (for SetDIBitsToDevice, kept for compatibility)
    unsigned char _bmi_data[sizeof(BITMAPINFO) + 16 * sizeof(RGBQUAD)];

    // input state
    int _keys[512];
    int _keys_prev[512];
    int _mouseX;
    int _mouseY;
    int _mouseButtons[3];
    int _mouseButtons_prev[3];

    // timing
    DWORD _timeStart;
    DWORD _timePrev;
    float _deltaTime;
    float _fps;
    float _fpsAccum;
    DWORD _fpsTime;
    HANDLE _timerEvent;     // event signaled by multimedia timer
    UINT   _timerId;        // multimedia timer ID (from timeSetEvent)

    // sprite storage
    struct GameSprite { int width, height; uint32_t *pixels; bool used; };
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
static PFN_CreateFontW         _gl_CreateFontW        = NULL;
static PFN_TextOutW            _gl_TextOutW           = NULL;
static PFN_SetTextColor        _gl_SetTextColor       = NULL;
static PFN_SetBkMode           _gl_SetBkMode          = NULL;
static PFN_GetTextExtentPoint32W _gl_GetTextExtentPoint32W = NULL;
static PFN_GdiFlush              _gl_GdiFlush              = NULL;
static PFN_timeGetTime         _gl_timeGetTime        = NULL;
static PFN_timeBeginPeriod     _gl_timeBeginPeriod    = NULL;
static PFN_timeEndPeriod       _gl_timeEndPeriod      = NULL;
static PFN_timeSetEvent        _gl_timeSetEvent       = NULL;
static PFN_timeKillEvent       _gl_timeKillEvent      = NULL;
static PFN_PlaySoundA          _gl_PlaySoundA         = NULL;
static PFN_mciSendStringA      _gl_mciSendStringA     = NULL;

static int _gamelib_apis_loaded = 0;

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

    _gl_SetDIBitsToDevice = (PFN_SetDIBitsToDevice)GetProcAddress(hGdi32, "SetDIBitsToDevice");
    _gl_GetStockObject    = (PFN_GetStockObject)GetProcAddress(hGdi32, "GetStockObject");
    _gl_CreateCompatibleDC = (PFN_CreateCompatibleDC)GetProcAddress(hGdi32, "CreateCompatibleDC");
    _gl_DeleteDC          = (PFN_DeleteDC)GetProcAddress(hGdi32, "DeleteDC");
    _gl_CreateDIBSection  = (PFN_CreateDIBSection)GetProcAddress(hGdi32, "CreateDIBSection");
    _gl_SelectObject      = (PFN_SelectObject)GetProcAddress(hGdi32, "SelectObject");
    _gl_DeleteObject      = (PFN_DeleteObject)GetProcAddress(hGdi32, "DeleteObject");
    _gl_BitBlt            = (PFN_BitBlt)GetProcAddress(hGdi32, "BitBlt");
    _gl_CreateFontW       = (PFN_CreateFontW)GetProcAddress(hGdi32, "CreateFontW");
    _gl_TextOutW          = (PFN_TextOutW)GetProcAddress(hGdi32, "TextOutW");
    _gl_SetTextColor      = (PFN_SetTextColor)GetProcAddress(hGdi32, "SetTextColor");
    _gl_SetBkMode         = (PFN_SetBkMode)GetProcAddress(hGdi32, "SetBkMode");
    _gl_GetTextExtentPoint32W = (PFN_GetTextExtentPoint32W)GetProcAddress(hGdi32, "GetTextExtentPoint32W");
    _gl_GdiFlush              = (PFN_GdiFlush)GetProcAddress(hGdi32, "GdiFlush");

    _gl_timeGetTime       = (PFN_timeGetTime)GetProcAddress(hWinmm, "timeGetTime");
    _gl_timeBeginPeriod   = (PFN_timeBeginPeriod)GetProcAddress(hWinmm, "timeBeginPeriod");
    _gl_timeEndPeriod     = (PFN_timeEndPeriod)GetProcAddress(hWinmm, "timeEndPeriod");
    _gl_timeSetEvent      = (PFN_timeSetEvent)GetProcAddress(hWinmm, "timeSetEvent");
    _gl_timeKillEvent     = (PFN_timeKillEvent)GetProcAddress(hWinmm, "timeKillEvent");
    _gl_PlaySoundA        = (PFN_PlaySoundA)GetProcAddress(hWinmm, "PlaySoundA");
    _gl_mciSendStringA    = (PFN_mciSendStringA)GetProcAddress(hWinmm, "mciSendStringA");

    if (!_gl_SetDIBitsToDevice || !_gl_GetStockObject ||
        !_gl_CreateCompatibleDC || !_gl_DeleteDC ||
        !_gl_CreateDIBSection || !_gl_SelectObject || !_gl_DeleteObject ||
        !_gl_BitBlt || !_gl_CreateFontW || !_gl_TextOutW ||
        !_gl_SetTextColor || !_gl_SetBkMode || !_gl_GetTextExtentPoint32W ||
        !_gl_timeGetTime ||
        !_gl_timeBeginPeriod  || !_gl_timeEndPeriod ||
        !_gl_PlaySoundA       || !_gl_mciSendStringA) {
        // NULL out all pointers to prevent dangling state
        _gl_SetDIBitsToDevice = NULL; _gl_GetStockObject = NULL;
        _gl_CreateCompatibleDC = NULL; _gl_DeleteDC = NULL;
        _gl_CreateDIBSection = NULL; _gl_SelectObject = NULL;
        _gl_DeleteObject = NULL; _gl_BitBlt = NULL;
        _gl_CreateFontW = NULL; _gl_TextOutW = NULL;
        _gl_SetTextColor = NULL; _gl_SetBkMode = NULL;
        _gl_GetTextExtentPoint32W = NULL; _gl_GdiFlush = NULL;
        _gl_timeGetTime = NULL; _gl_timeBeginPeriod = NULL;
        _gl_timeEndPeriod = NULL; _gl_timeSetEvent = NULL;
        _gl_timeKillEvent = NULL; _gl_PlaySoundA = NULL;
        _gl_mciSendStringA = NULL;
        FreeLibrary(hGdi32);
        FreeLibrary(hWinmm);
        return -1;
    }
    _gamelib_apis_loaded = 1;
    return 0;
}


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

    _gl_GdiplusStartup = (PFN_GdiplusStartup)
        GetProcAddress(hGdiPlus, "GdiplusStartup");
    _gl_GdipCreateBitmapFromStream = (PFN_GdipCreateBitmapFromStream)
        GetProcAddress(hGdiPlus, "GdipCreateBitmapFromStream");
    _gl_GdipGetImageWidth = (PFN_GdipGetImageWidth)
        GetProcAddress(hGdiPlus, "GdipGetImageWidth");
    _gl_GdipGetImageHeight = (PFN_GdipGetImageHeight)
        GetProcAddress(hGdiPlus, "GdipGetImageHeight");
    _gl_GdipBitmapLockBits = (PFN_GdipBitmapLockBits)
        GetProcAddress(hGdiPlus, "GdipBitmapLockBits");
    _gl_GdipBitmapUnlockBits = (PFN_GdipBitmapUnlockBits)
        GetProcAddress(hGdiPlus, "GdipBitmapUnlockBits");
    _gl_GdipDisposeImage = (PFN_GdipDisposeImage)
        GetProcAddress(hGdiPlus, "GdipDisposeImage");
    _gl_CreateStreamOnHGlobal = (PFN_CreateStreamOnHGlobal)
        GetProcAddress(hOle32, "CreateStreamOnHGlobal");

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

    if (width == 0 || height == 0) goto cleanup;

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
    _width = 0;
    _height = 0;
    _framebuffer = NULL;
    _memDC = NULL;
    _dibSection = NULL;
    _oldBmp = NULL;
    memset(_keys, 0, sizeof(_keys));
    memset(_keys_prev, 0, sizeof(_keys_prev));
    _mouseX = 0;
    _mouseY = 0;
    memset(_mouseButtons, 0, sizeof(_mouseButtons));
    memset(_mouseButtons_prev, 0, sizeof(_mouseButtons_prev));
    _timeStart = 0;
    _timePrev = 0;
    _deltaTime = 0.0f;
    _fps = 0.0f;
    _fpsAccum = 0.0f;
    _fpsTime = 0;
    _timerEvent = NULL;
    _timerId = 0;
    memset(_bmi_data, 0, sizeof(_bmi_data));
    _musicPlaying = false;
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


//---------------------------------------------------------------------
// Destructor
//---------------------------------------------------------------------
GameLib::~GameLib()
{
    // Stop music
    if (_musicPlaying && _gl_mciSendStringA) {
        _gl_mciSendStringA("stop gamelib_music", NULL, 0, NULL);
        _gl_mciSendStringA("close gamelib_music", NULL, 0, NULL);
        _musicPlaying = false;
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
    // Free DIB Section resources
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
    // Frame buffer is managed by DIB Section, no need to free separately
    _framebuffer = NULL;
    // Destroy window
    if (_hwnd) {
        DestroyWindow(_hwnd);
        _hwnd = NULL;
    }
    // Clean up multimedia timer
    if (_timerId && _gl_timeKillEvent) {
        _gl_timeKillEvent(_timerId);
        _timerId = 0;
    }
    if (_timerEvent) {
        CloseHandle(_timerEvent);
        _timerEvent = NULL;
    }
    if (_gl_timeEndPeriod) {
        _gl_timeEndPeriod(1);
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

    case WM_KEYDOWN:
        if (lParam & GAMELIB_REPEATED_KEYMASK) return 0;
        if (self) self->_keys[wParam & 511] = 1;
        return 0;

    case WM_KEYUP:
        if (self) self->_keys[wParam & 511] = 0;
        return 0;

    case WM_MOUSEMOVE:
        if (self) {
            self->_mouseX = (int)(short)LOWORD(lParam);
            self->_mouseY = (int)(short)HIWORD(lParam);
        }
        return 0;

    case WM_LBUTTONDOWN:
        if (self) self->_mouseButtons[MOUSE_LEFT] = 1;
        return 0;
    case WM_LBUTTONUP:
        if (self) self->_mouseButtons[MOUSE_LEFT] = 0;
        return 0;
    case WM_RBUTTONDOWN:
        if (self) self->_mouseButtons[MOUSE_RIGHT] = 1;
        return 0;
    case WM_RBUTTONUP:
        if (self) self->_mouseButtons[MOUSE_RIGHT] = 0;
        return 0;
    case WM_MBUTTONDOWN:
        if (self) self->_mouseButtons[MOUSE_MIDDLE] = 1;
        return 0;
    case WM_MBUTTONUP:
        if (self) self->_mouseButtons[MOUSE_MIDDLE] = 0;
        return 0;

    case WM_PAINT: {
            PAINTSTRUCT ps;
            BeginPaint(hWnd, &ps);
            if (self && self->_memDC && _gl_BitBlt) {
                _gl_BitBlt(ps.hdc, 0, 0, self->_width, self->_height,
                           self->_memDC, 0, 0, 0x00CC0020 /* SRCCOPY */);
            }
            EndPaint(hWnd, &ps);
        }
        return 0;

    default:
        return DefWindowProcW(hWnd, msg, wParam, lParam);
    }
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
int GameLib::Open(int width, int height, const char *title, bool center)
{
    // Validate dimensions
    if (width <= 0 || height <= 0 || width > 16384 || height > 16384) return -7;

    // Destroy existing resources first
    if (_memDC) {
        if (_oldBmp) {
            _gl_SelectObject(_memDC, _oldBmp);
            _oldBmp = NULL;
        }
        if (_dibSection) {
            _gl_DeleteObject(_dibSection);
            _dibSection = NULL;
        }
        _gl_DeleteDC(_memDC);
        _memDC = NULL;
    }
    _framebuffer = NULL;
    if (_hwnd) { DestroyWindow(_hwnd); _hwnd = NULL; }

    if (_InitWindowClass() != 0) return -1;

    _width = width;
    _height = height;
    _title = title;
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
    int wtitleLen = MultiByteToWideChar(CP_UTF8, 0, title, -1, NULL, 0);
    if (wtitleLen <= 0) return -5;
    wchar_t *wtitle = (wchar_t*)malloc(wtitleLen * sizeof(wchar_t));
    if (!wtitle) return -5;
    MultiByteToWideChar(CP_UTF8, 0, title, -1, wtitle, wtitleLen);

    HINSTANCE inst = GetModuleHandle(NULL);
    _hwnd = CreateWindowW(L"GameLibWindowClass", wtitle, style,
        posX, posY, ww, wh, NULL, NULL, inst, this);

    free(wtitle);

    if (!_hwnd) return -6;

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

    ShowWindow(_hwnd, SW_SHOW);
    UpdateWindow(_hwnd);

    // Initialize time
    _gl_timeBeginPeriod(1);
    _timeStart = _gl_timeGetTime();
    _timePrev = _timeStart;
    _fpsTime = _timeStart;
    _fpsAccum = 0.0f;
    _fps = 0.0f;
    _deltaTime = 0.0f;

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

    // Draw frame buffer to window using BitBlt (faster than SetDIBitsToDevice)
    HDC hdc = ::GetDC(_hwnd);
    if (hdc) {
        _gl_BitBlt(hdc, 0, 0, _width, _height, _memDC, 0, 0, 0x00CC0020 /* SRCCOPY */);
        ::ReleaseDC(_hwnd, hdc);
    }

    // Save previous frame key/mouse state (for edge detection)
    memcpy(_keys_prev, _keys, sizeof(_keys));
    memcpy(_mouseButtons_prev, _mouseButtons, sizeof(_mouseButtons));

    // Dispatch messages
    _DispatchMessages();

    // Update time
    DWORD now = _gl_timeGetTime();
    int32_t delta = (int32_t)(now - _timePrev);
    if (delta < 0) delta = 0;
    _deltaTime = delta / 1000.0f;
    _timePrev = now;

    // Update FPS
    _fpsAccum += 1.0f;
    int32_t fpsDelta = (int32_t)(now - _fpsTime);
    if (fpsDelta >= 1000) {
        _fps = _fpsAccum * 1000.0f / fpsDelta;
        _fpsAccum = 0.0f;
        _fpsTime = now;
        _UpdateTitleFps();
    }
}


//---------------------------------------------------------------------
// WaitFrame: frame rate control
//---------------------------------------------------------------------
void GameLib::WaitFrame(int fps)
{
    if (fps <= 0) fps = 60;
    DWORD frameTime = 1000 / fps;
    for (;;) {
        DWORD now = _gl_timeGetTime();
        int32_t elapsed = (int32_t)(now - _timePrev);
        if (elapsed < 0) elapsed = 0;
        if ((DWORD)elapsed >= frameTime) break;
        if (_timerEvent && _timerId) {
            WaitForSingleObject(_timerEvent, 2);
        } else {
            Sleep(1);
        }
    }
}


//---------------------------------------------------------------------
// GetDeltaTime / GetFPS / GetTime / GetWidth / GetHeight
//---------------------------------------------------------------------
float GameLib::GetDeltaTime() const { return _deltaTime; }
float GameLib::GetFPS() const { return _fps; }
float GameLib::GetTime() const
{
    return (_gl_timeGetTime() - _timeStart) / 1000.0f;
}
int GameLib::GetWidth() const { return _width; }
int GameLib::GetHeight() const { return _height; }


//---------------------------------------------------------------------
// SetTitle
//---------------------------------------------------------------------
void GameLib::SetTitle(const char *title)
{
    _title = title;
    if (_hwnd) {
        int wlen = MultiByteToWideChar(CP_UTF8, 0, title, -1, NULL, 0);
        if (wlen > 0) {
            wchar_t *wt = (wchar_t*)malloc(wlen * sizeof(wchar_t));
            if (wt) {
                MultiByteToWideChar(CP_UTF8, 0, title, -1, wt, wlen);
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

void GameLib::Clear(uint32_t color)
{
    if (!_framebuffer) return;
    size_t count = (size_t)_width * _height;
    for (size_t i = 0; i < count; i++) _framebuffer[i] = color;
}

void GameLib::SetPixel(int x, int y, uint32_t color)
{
    if (x >= 0 && x < _width && y >= 0 && y < _height) {
        _framebuffer[y * _width + x] = color;
    }
}

uint32_t GameLib::GetPixel(int x, int y) const
{
    if (x >= 0 && x < _width && y >= 0 && y < _height) {
        return _framebuffer[y * _width + x];
    }
    return 0;
}

void GameLib::_SetPixelFast(int x, int y, uint32_t color)
{
    _framebuffer[y * _width + x] = color;
}


//=====================================================================
// Drawing Functions
//=====================================================================

//---------------------------------------------------------------------
// DrawLine: Bresenham's algorithm
//---------------------------------------------------------------------
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


//---------------------------------------------------------------------
// Horizontal line (internal use, with clipping)
//---------------------------------------------------------------------
void GameLib::_DrawHLine(int x1, int x2, int y, uint32_t color)
{
    if (y < 0 || y >= _height) return;
    if (x1 > x2) { int t = x1; x1 = x2; x2 = t; }
    if (x1 < 0) x1 = 0;
    if (x2 >= _width) x2 = _width - 1;
    for (int x = x1; x <= x2; x++) {
        _SetPixelFast(x, y, color);
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
    if (!_framebuffer) return;
    int x1 = x, y1 = y, x2 = x + w, y2 = y + h;
    if (x1 < 0) x1 = 0;
    if (y1 < 0) y1 = 0;
    if (x2 > _width) x2 = _width;
    if (y2 > _height) y2 = _height;
    for (int j = y1; j < y2; j++) {
        uint32_t *row = _framebuffer + j * _width;
        for (int i = x1; i < x2; i++) {
            row[i] = color;
        }
    }
}


//---------------------------------------------------------------------
// DrawCircle: midpoint circle algorithm
//---------------------------------------------------------------------
void GameLib::DrawCircle(int cx, int cy, int r, uint32_t color)
{
    int x = 0, y = r;
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


//---------------------------------------------------------------------
// FillCircle
//---------------------------------------------------------------------
void GameLib::FillCircle(int cx, int cy, int r, uint32_t color)
{
    int x = 0, y = r;
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
// GDI Text Rendering (system fonts, Chinese support)
//=====================================================================

void GameLib::DrawTextGDI(int x, int y, const char *text, uint32_t color, const char *fontName, int fontSize)
{
    if (!_memDC || !text) return;

    // Convert UTF-8 to wide string (query required size first)
    int wtextLen = MultiByteToWideChar(CP_UTF8, 0, text, -1, NULL, 0);
    if (wtextLen <= 0) return;
    wchar_t *wtext = (wchar_t*)malloc(wtextLen * sizeof(wchar_t));
    if (!wtext) return;
    MultiByteToWideChar(CP_UTF8, 0, text, -1, wtext, wtextLen);

    // Convert font name to wide string
    int wfontLen = MultiByteToWideChar(CP_UTF8, 0, fontName, -1, NULL, 0);
    if (wfontLen <= 0) { free(wtext); return; }
    wchar_t *wfont = (wchar_t*)malloc(wfontLen * sizeof(wchar_t));
    if (!wfont) { free(wtext); return; }
    MultiByteToWideChar(CP_UTF8, 0, fontName, -1, wfont, wfontLen);

    // Create font
    HFONT font = _gl_CreateFontW(
        fontSize, 0, 0, 0, FW_NORMAL,
        FALSE, FALSE, FALSE,
        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
        CLIP_DEFAULT_PRECIS,
        ANTIALIASED_QUALITY,
        DEFAULT_PITCH,
        wfont);

    if (font) {
        HFONT oldFont = (HFONT)_gl_SelectObject(_memDC, font);

        // Set text color (convert ARGB to COLORREF: swap R and B)
        COLORREF cref = RGB(COLOR_GET_R(color), COLOR_GET_G(color), COLOR_GET_B(color));
        _gl_SetTextColor(_memDC, cref);
        _gl_SetBkMode(_memDC, 1);  // TRANSPARENT

        // Draw text
        _gl_TextOutW(_memDC, x, y, wtext, wtextLen - 1);

        // Flush GDI to ensure writes are visible in the framebuffer
        if (_gl_GdiFlush) _gl_GdiFlush();

        // Fix alpha channel: GDI TextOut writes alpha=0, which makes text
        // invisible to alpha-aware drawing. Scan the text bounding box and
        // set alpha=0xFF for any pixel that GDI modified.
        if (_framebuffer) {
            SIZE sz = { 0, 0 };
            _gl_GetTextExtentPoint32W(_memDC, wtext, wtextLen - 1, &sz);
            int x0 = x, y0 = y;
            int x1 = x + sz.cx, y1 = y + sz.cy;
            if (x0 < 0) x0 = 0;
            if (y0 < 0) y0 = 0;
            if (x1 > _width) x1 = _width;
            if (y1 > _height) y1 = _height;
            for (int py = y0; py < y1; py++) {
                uint32_t *row = _framebuffer + py * _width;
                for (int px = x0; px < x1; px++) {
                    uint32_t c = row[px];
                    if ((c & 0xFF000000) == 0 && (c & 0x00FFFFFF) != 0) {
                        row[px] = c | 0xFF000000;
                    }
                }
            }
        }

        // Restore old font
        _gl_SelectObject(_memDC, oldFont);
        _gl_DeleteObject(font);
    }

    free(wtext);
    free(wfont);
}

void GameLib::DrawTextGDI(int x, int y, const char *text, uint32_t color, int fontSize)
{
    // Default font: Microsoft YaHei for Chinese support, fallback to system default
    DrawTextGDI(x, y, text, color, "Microsoft YaHei", fontSize);
}

int GameLib::GetTextWidthGDI(const char *text, const char *fontName, int fontSize)
{
    if (!_memDC || !text) return 0;

    // Convert UTF-8 to wide string (query required size first)
    int wtextLen = MultiByteToWideChar(CP_UTF8, 0, text, -1, NULL, 0);
    if (wtextLen <= 0) return 0;
    wchar_t *wtext = (wchar_t*)malloc(wtextLen * sizeof(wchar_t));
    if (!wtext) return 0;
    MultiByteToWideChar(CP_UTF8, 0, text, -1, wtext, wtextLen);

    // Convert font name to wide string
    int wfontLen = MultiByteToWideChar(CP_UTF8, 0, fontName, -1, NULL, 0);
    if (wfontLen <= 0) { free(wtext); return 0; }
    wchar_t *wfont = (wchar_t*)malloc(wfontLen * sizeof(wchar_t));
    if (!wfont) { free(wtext); return 0; }
    MultiByteToWideChar(CP_UTF8, 0, fontName, -1, wfont, wfontLen);

    // Create font
    HFONT font = _gl_CreateFontW(
        fontSize, 0, 0, 0, FW_NORMAL,
        FALSE, FALSE, FALSE,
        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
        CLIP_DEFAULT_PRECIS,
        ANTIALIASED_QUALITY,
        DEFAULT_PITCH,
        wfont);

    SIZE size = {0, 0};
    if (font) {
        HFONT oldFont = (HFONT)_gl_SelectObject(_memDC, font);
        _gl_GetTextExtentPoint32W(_memDC, wtext, wtextLen - 1, &size);
        _gl_SelectObject(_memDC, oldFont);
        _gl_DeleteObject(font);
    }

    free(wtext);
    free(wfont);
    return (int)size.cx;
}

int GameLib::GetTextHeightGDI(int fontSize)
{
    return fontSize;  // Approximate height
}


//=====================================================================
// Sprite System
//=====================================================================

int GameLib::_AllocSpriteSlot()
{
    for (size_t i = 0; i < _sprites.size(); i++) {
        if (!_sprites[i].used) return (int)i;
    }
    GameSprite spr;
    spr.width = 0;
    spr.height = 0;
    spr.pixels = NULL;
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
    FILE *fp = fopen(filename, "rb");
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

    for (int y = 0; y < height; y++) {
        if (fread(rowData, 1, rowSize, fp) != (size_t)rowSize) break;
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
    return id;
}

int GameLib::LoadSprite(const char *filename)
{
    // Read file into memory
    FILE *fp = fopen(filename, "rb");
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
    _sprites[id].used = false;
}

void GameLib::DrawSprite(int id, int x, int y)
{
    DrawSpriteEx(id, x, y, 0);
}

void GameLib::DrawSpriteEx(int id, int x, int y, int flags)
{
    if (id < 0 || id >= (int)_sprites.size()) return;
    if (!_sprites[id].used) return;
    GameSprite &spr = _sprites[id];
    bool flipH = (flags & SPRITE_FLIP_H) != 0;
    bool flipV = (flags & SPRITE_FLIP_V) != 0;

    // Pre-clip to avoid per-pixel boundary check
    int sx0 = 0, sx1 = spr.width;
    int sy0 = 0, sy1 = spr.height;
    if (x < 0) sx0 = -x;
    if (y < 0) sy0 = -y;
    if (x + sx1 > _width)  sx1 = _width - x;
    if (y + sy1 > _height) sy1 = _height - y;
    if (sx0 >= sx1 || sy0 >= sy1) return;

    if (flags & SPRITE_ALPHA) {
        // ---- Alpha blending path ----
        bool colorKey = (flags & SPRITE_COLORKEY) != 0;
        for (int sy = sy0; sy < sy1; sy++) {
            int srcY = flipV ? (spr.height - 1 - sy) : sy;
            const uint32_t *srcRow = spr.pixels + srcY * spr.width;
            uint32_t *dstRow = _framebuffer + (y + sy) * _width + x;
            for (int sx = sx0; sx < sx1; sx++) {
                int srcX = flipH ? (spr.width - 1 - sx) : sx;
                uint32_t c = srcRow[srcX];
                if (colorKey && c == COLORKEY_DEFAULT) continue;
                uint32_t sa = COLOR_GET_A(c);
                if (sa == 0) continue;
                if (sa == 255) {
                    dstRow[sx] = c;
                } else {
                    uint32_t dc = dstRow[sx];
                    uint32_t ia = 255 - sa;
                    uint32_t or_ = (sa * COLOR_GET_R(c) + ia * COLOR_GET_R(dc)) / 255;
                    uint32_t og = (sa * COLOR_GET_G(c) + ia * COLOR_GET_G(dc)) / 255;
                    uint32_t ob = (sa * COLOR_GET_B(c) + ia * COLOR_GET_B(dc)) / 255;
                    dstRow[sx] = COLOR_ARGB(255, or_, og, ob);
                }
            }
        }
    } else if (flags & SPRITE_COLORKEY) {
        // ---- ColorKey transparent path ----
        for (int sy = sy0; sy < sy1; sy++) {
            int srcY = flipV ? (spr.height - 1 - sy) : sy;
            const uint32_t *srcRow = spr.pixels + srcY * spr.width;
            uint32_t *dstRow = _framebuffer + (y + sy) * _width + x;
            for (int sx = sx0; sx < sx1; sx++) {
                int srcX = flipH ? (spr.width - 1 - sx) : sx;
                uint32_t c = srcRow[srcX];
                if (c != COLORKEY_DEFAULT)
                    dstRow[sx] = c;
            }
        }
    } else {
        // ---- Opaque path (skip alpha=0) ----
        for (int sy = sy0; sy < sy1; sy++) {
            int srcY = flipV ? (spr.height - 1 - sy) : sy;
            const uint32_t *srcRow = spr.pixels + srcY * spr.width;
            uint32_t *dstRow = _framebuffer + (y + sy) * _width + x;
            for (int sx = sx0; sx < sx1; sx++) {
                int srcX = flipH ? (spr.width - 1 - sx) : sx;
                uint32_t c = srcRow[srcX];
                if (COLOR_GET_A(c) > 0)
                    dstRow[sx] = c;
            }
        }
    }
}

void GameLib::DrawSpriteRegion(int id, int x, int y, int sx, int sy, int sw, int sh)
{
    if (id < 0 || id >= (int)_sprites.size()) return;
    if (!_sprites[id].used) return;
    GameSprite &spr = _sprites[id];
    for (int j = 0; j < sh; j++) {
        int srcY = sy + j;
        int dy = y + j;
        if (srcY < 0 || srcY >= spr.height) continue;
        if (dy < 0 || dy >= _height) continue;
        for (int i = 0; i < sw; i++) {
            int srcX = sx + i;
            int dx = x + i;
            if (srcX < 0 || srcX >= spr.width) continue;
            if (dx < 0 || dx >= _width) continue;
            uint32_t c = spr.pixels[srcY * spr.width + srcX];
            if (COLOR_GET_A(c) > 0) {
                _framebuffer[dy * _width + dx] = c;
            }
        }
    }
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


//=====================================================================
// Sound
//=====================================================================

void GameLib::PlayBeep(int frequency, int duration)
{
    Beep(frequency, duration);
}

void GameLib::PlayWAV(const char *filename, bool loop)
{
    DWORD flags = SND_FILENAME | SND_ASYNC;
    if (loop) flags |= SND_LOOP;
    _gl_PlaySoundA(filename, NULL, flags);
}

void GameLib::StopWAV()
{
    _gl_PlaySoundA(NULL, NULL, 0);
}

void GameLib::PlayMusic(const char *filename, bool loop)
{
    if (!filename) return;

    // Reject filenames containing quotes (MCI command injection prevention)
    for (const char *p = filename; *p; p++) {
        if (*p == '"') return;
    }

    // Stop previous music first
    if (_musicPlaying) {
        _gl_mciSendStringA("stop gamelib_music", NULL, 0, NULL);
        _gl_mciSendStringA("close gamelib_music", NULL, 0, NULL);
        _musicPlaying = false;
    }
    // Open audio file (supports mp3/mid/wav etc., formats supported by MCI)
    char cmd[1024];
    int n = snprintf(cmd, sizeof(cmd), "open \"%s\" type mpegvideo alias gamelib_music", filename);
    if (n < 0 || n >= (int)sizeof(cmd)) return;  // truncation guard
    if (_gl_mciSendStringA(cmd, NULL, 0, NULL) != 0) {
        // If mpegvideo fails, try auto-detect type
        n = snprintf(cmd, sizeof(cmd), "open \"%s\" alias gamelib_music", filename);
        if (n < 0 || n >= (int)sizeof(cmd)) return;  // truncation guard
        if (_gl_mciSendStringA(cmd, NULL, 0, NULL) != 0) {
            return;
        }
    }
    // Play
    if (loop) {
        _gl_mciSendStringA("play gamelib_music repeat", NULL, 0, NULL);
    } else {
        _gl_mciSendStringA("play gamelib_music", NULL, 0, NULL);
    }
    _musicPlaying = true;
}

void GameLib::StopMusic()
{
    if (_musicPlaying) {
        _gl_mciSendStringA("stop gamelib_music", NULL, 0, NULL);
        _gl_mciSendStringA("close gamelib_music", NULL, 0, NULL);
        _musicPlaying = false;
    }
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

    // Calculate visible tile range on screen, avoid traversing the whole map
    int col0 = (-x) / ts;
    int row0 = (-y) / ts;
    int col1 = (-x + _width - 1) / ts + 1;
    int row1 = (-y + _height - 1) / ts + 1;
    if (col0 < 0) col0 = 0;
    if (row0 < 0) row0 = 0;
    if (col1 > tm.cols) col1 = tm.cols;
    if (row1 > tm.rows) row1 = tm.rows;

    bool useAlpha    = (flags & SPRITE_ALPHA) != 0;
    bool useColorKey = (flags & SPRITE_COLORKEY) != 0;

    for (int r = row0; r < row1; r++) {
        for (int c = col0; c < col1; c++) {
            int tid = tm.tiles[r * tm.cols + c];
            if (tid < 0) continue;

            // Pixel start position of this tile in tileset
            int srcCol = tid % tsCols;
            int srcRow = tid / tsCols;
            int srcX0 = srcCol * ts;
            int srcY0 = srcRow * ts;

            // Screen destination position
            int dstX0 = x + c * ts;
            int dstY0 = y + r * ts;

            // Clip within tile
            int ix0 = 0, iy0 = 0, ix1 = ts, iy1 = ts;
            if (dstX0 < 0) ix0 = -dstX0;
            if (dstY0 < 0) iy0 = -dstY0;
            if (dstX0 + ix1 > _width)  ix1 = _width - dstX0;
            if (dstY0 + iy1 > _height) iy1 = _height - dstY0;

            if (useAlpha) {
                // ---- Alpha blending path ----
                for (int iy = iy0; iy < iy1; iy++) {
                    int sy = srcY0 + iy;
                    if (sy < 0 || sy >= tset.height) continue;
                    const uint32_t *srcRow_ = tset.pixels + sy * tset.width;
                    uint32_t *dstRow_ = _framebuffer + (dstY0 + iy) * _width;
                    for (int ix = ix0; ix < ix1; ix++) {
                        int sx = srcX0 + ix;
                        if (sx < 0 || sx >= tset.width) continue;
                        uint32_t sc = srcRow_[sx];
                        if (useColorKey && sc == COLORKEY_DEFAULT) continue;
                        uint32_t sa = COLOR_GET_A(sc);
                        if (sa == 0) continue;
                        int dx = dstX0 + ix;
                        if (sa == 255) {
                            dstRow_[dx] = sc;
                        } else {
                            uint32_t dc = dstRow_[dx];
                            uint32_t ia = 255 - sa;
                            uint32_t or_ = (sa * COLOR_GET_R(sc) + ia * COLOR_GET_R(dc)) / 255;
                            uint32_t og = (sa * COLOR_GET_G(sc) + ia * COLOR_GET_G(dc)) / 255;
                            uint32_t ob = (sa * COLOR_GET_B(sc) + ia * COLOR_GET_B(dc)) / 255;
                            dstRow_[dx] = COLOR_ARGB(255, or_, og, ob);
                        }
                    }
                }
            } else if (useColorKey) {
                // ---- ColorKey transparent path ----
                for (int iy = iy0; iy < iy1; iy++) {
                    int sy = srcY0 + iy;
                    if (sy < 0 || sy >= tset.height) continue;
                    const uint32_t *srcRow_ = tset.pixels + sy * tset.width;
                    uint32_t *dstRow_ = _framebuffer + (dstY0 + iy) * _width;
                    for (int ix = ix0; ix < ix1; ix++) {
                        int sx = srcX0 + ix;
                        if (sx < 0 || sx >= tset.width) continue;
                        uint32_t sc = srcRow_[sx];
                        if (sc != COLORKEY_DEFAULT) {
                            dstRow_[dstX0 + ix] = sc;
                        }
                    }
                }
            } else {
                // ---- Opaque path (skip alpha=0) ----
                for (int iy = iy0; iy < iy1; iy++) {
                    int sy = srcY0 + iy;
                    if (sy < 0 || sy >= tset.height) continue;
                    const uint32_t *srcRow_ = tset.pixels + sy * tset.width;
                    uint32_t *dstRow_ = _framebuffer + (dstY0 + iy) * _width;
                    for (int ix = ix0; ix < ix1; ix++) {
                        int sx = srcX0 + ix;
                        if (sx < 0 || sx >= tset.width) continue;
                        uint32_t sc = srcRow_[sx];
                        if (COLOR_GET_A(sc) > 0) {
                            dstRow_[dstX0 + ix] = sc;
                        }
                    }
                }
            }
        }
    }
}


#endif // GAMELIB_IMPLEMENTATION

#endif // GAMELIB_H
