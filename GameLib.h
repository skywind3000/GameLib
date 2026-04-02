//=====================================================================
//
// GameLib.h - 面向初学者的单头文件游戏库
//
// 基于 Win32 GDI，无需 SDL 或其他第三方库。
// 适用于 Dev C++ (GCC 4.9.2+)，可开发空战、俄罗斯方块、走迷宫等游戏。
//
// 使用方法（单文件项目，最常见情况）：
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
// 多文件项目：在主 .cpp 文件中 #include 之前加一行
//     #define GAMELIB_IMPLEMENTATION
//     #include "GameLib.h"
// 其他 .cpp 文件中加一行
//     #define GAMELIB_NO_IMPLEMENTATION
//     #include "GameLib.h"
//
// 编译命令（MinGW / Dev C++）：
//     g++ -o game.exe main.cpp -mwindows
//
// Last Modified: 2026/04/02
//
//=====================================================================
#ifndef GAMELIB_H
#define GAMELIB_H

// 默认行为：include 即启用实现（方便单文件项目）
#ifndef GAMELIB_NO_IMPLEMENTATION
#ifndef GAMELIB_IMPLEMENTATION
#define GAMELIB_IMPLEMENTATION
#endif
#endif

//---------------------------------------------------------------------
// 系统头文件
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
// 链接库：仅需 -mwindows，gdi32 和 winmm 通过 LoadLibrary 动态加载
//---------------------------------------------------------------------
#ifdef _MSC_VER
#pragma comment(lib, "user32.lib")
#endif


//---------------------------------------------------------------------
// mmsystem 类型和常量声明（不再 #include <mmsystem.h>）
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
// 动态加载的函数指针类型
//---------------------------------------------------------------------

// gdi32.dll
typedef int (WINAPI *PFN_SetDIBitsToDevice)(
    HDC, int, int, DWORD, DWORD, int, int, UINT, UINT,
    const void*, const BITMAPINFO*, UINT);
typedef HGDIOBJ (WINAPI *PFN_GetStockObject)(int);

// winmm.dll
typedef DWORD   (WINAPI *PFN_timeGetTime)(void);
typedef DWORD   (WINAPI *PFN_timeBeginPeriod)(UINT);
typedef DWORD   (WINAPI *PFN_timeEndPeriod)(UINT);
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
// 第一部分：常量定义
//=====================================================================

//---------------------------------------------------------------------
// 颜色常量 (ARGB 格式: 0xAARRGGBB)
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

// 颜色构造宏
#define COLOR_RGB(r, g, b)     ((uint32_t)(0xFF000000 | ((r) << 16) | ((g) << 8) | (b)))
#define COLOR_ARGB(a, r, g, b) ((uint32_t)(((a) << 24) | ((r) << 16) | ((g) << 8) | (b)))

// 颜色分量提取
#define COLOR_GET_A(c)    (((c) >> 24) & 0xFF)
#define COLOR_GET_R(c)    (((c) >> 16) & 0xFF)
#define COLOR_GET_G(c)    (((c) >> 8) & 0xFF)
#define COLOR_GET_B(c)    ((c) & 0xFF)

//---------------------------------------------------------------------
// 键盘常量（使用 Windows Virtual Key Code）
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
// 鼠标按键常量
//---------------------------------------------------------------------
#define MOUSE_LEFT    0
#define MOUSE_RIGHT   1
#define MOUSE_MIDDLE  2

//---------------------------------------------------------------------
// 精灵绘制标志
//---------------------------------------------------------------------
#define SPRITE_FLIP_H     1    // 水平翻转
#define SPRITE_FLIP_V     2    // 垂直翻转
#define SPRITE_COLORKEY   4    // 启用透明色
#define SPRITE_ALPHA      8    // 启用 Alpha 混合

// 默认 Color Key 颜色：品红 (255, 0, 255)，常见 2D 资源透明色
#ifndef COLORKEY_DEFAULT
#define COLORKEY_DEFAULT  0xFFFF00FF
#endif


//=====================================================================
// 第二部分：类声明
//=====================================================================

//---------------------------------------------------------------------
// GameLib 主类
//---------------------------------------------------------------------
class GameLib
{
public:
    GameLib();
    ~GameLib();

    // -------- 窗口与主循环 --------
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

    // -------- 帧缓冲 --------
    void Clear(uint32_t color = COLOR_BLACK);
    void SetPixel(int x, int y, uint32_t color);
    uint32_t GetPixel(int x, int y) const;

    // -------- 图形绘制 --------
    void DrawLine(int x1, int y1, int x2, int y2, uint32_t color);
    void DrawRect(int x, int y, int w, int h, uint32_t color);
    void FillRect(int x, int y, int w, int h, uint32_t color);
    void DrawCircle(int cx, int cy, int r, uint32_t color);
    void FillCircle(int cx, int cy, int r, uint32_t color);
    void DrawTriangle(int x1, int y1, int x2, int y2, int x3, int y3, uint32_t color);
    void FillTriangle(int x1, int y1, int x2, int y2, int x3, int y3, uint32_t color);

    // -------- 文字渲染（内嵌 8x8 点阵字体） --------
    void DrawText(int x, int y, const char *text, uint32_t color);
    void DrawNumber(int x, int y, int number, uint32_t color);
    void DrawTextScale(int x, int y, const char *text, uint32_t color, int scale);
    void DrawPrintf(int x, int y, uint32_t color, const char *fmt, ...);

    // -------- 精灵系统（整数 ID 管理） --------
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

    // -------- 输入系统 --------
    bool IsKeyDown(int key) const;
    bool IsKeyPressed(int key) const;
    int GetMouseX() const;
    int GetMouseY() const;
    bool IsMouseDown(int button) const;

    // -------- 声音 --------
    void PlayBeep(int frequency, int duration);
    void PlayWAV(const char *filename, bool loop = false);
    void StopWAV();
    void PlayMusic(const char *filename, bool loop = true);
    void StopMusic();

    // -------- 工具函数 --------
    static int Random(int minVal, int maxVal);
    static bool RectOverlap(int x1, int y1, int w1, int h1,
                            int x2, int y2, int w2, int h2);
    static bool CircleOverlap(int cx1, int cy1, int r1,
                              int cx2, int cy2, int r2);
    static bool PointInRect(int px, int py, int x, int y, int w, int h);
    static float Distance(int x1, int y1, int x2, int y2);

    // -------- 网格辅助 --------
    void DrawGrid(int x, int y, int rows, int cols, int cellSize, uint32_t color);
    void FillCell(int gridX, int gridY, int row, int col, int cellSize, uint32_t color);

    // -------- Tilemap 系统 --------
    int CreateTilemap(int cols, int rows, int tileSize, int tilesetId);
    void FreeTilemap(int mapId);
    void SetTile(int mapId, int col, int row, int tileId);
    int GetTile(int mapId, int col, int row) const;
    void DrawTilemap(int mapId, int x, int y, int flags = 0);

private:
    // 禁止拷贝
    GameLib(const GameLib &);
    GameLib &operator=(const GameLib &);

    // 内部窗口管理
    static LRESULT CALLBACK _WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
    static int _InitWindowClass();
    void _DispatchMessages();
    void _InitDIBInfo(void *ptr, int width, int height);
    void _UpdateTitleFps();

    // 内部像素绘制（无边界检查，用于已裁剪后的快速绘制）
    void _SetPixelFast(int x, int y, uint32_t color);
    void _DrawHLine(int x1, int x2, int y, uint32_t color);

    // 内部精灵管理
    int _AllocSpriteSlot();

    // 内部 Tilemap 管理
    int _AllocTilemapSlot();

private:
    // 窗口状态
    HWND _hwnd;
    bool _closing;
    bool _active;
    bool _showFps;
    std::string _title;
    int _width;
    int _height;

    // 帧缓冲
    uint32_t *_framebuffer;

    // DIB 信息（用于 SetDIBitsToDevice）
    unsigned char _bmi_data[sizeof(BITMAPINFO) + 16 * sizeof(RGBQUAD)];

    // 输入状态
    int _keys[512];
    int _keys_prev[512];
    int _mouseX;
    int _mouseY;
    int _mouseButtons[3];

    // 时间
    DWORD _timeStart;
    DWORD _timePrev;
    float _deltaTime;
    float _fps;
    float _fpsAccum;
    DWORD _fpsTime;

    // 精灵存储
    struct GameSprite { int width, height; uint32_t *pixels; bool used; };
    std::vector<GameSprite> _sprites;

    // Tilemap 存储
    struct GameTilemap {
        int cols, rows;     // 地图网格大小
        int tileSize;       // 瓦片边长（像素）
        int tilesetId;      // tileset 精灵 ID
        int tilesetCols;    // tileset 每行有多少个瓦片
        int *tiles;         // cols * rows 的瓦片 ID 数组（-1 = 空）
        bool used;          // 槽位是否被使用
    };
    std::vector<GameTilemap> _tilemaps;

    // 音乐状态（MCI）
    bool _musicPlaying;

    // 随机数初始化标记
    static bool _srandDone;
};


//=====================================================================
// 第三部分：8x8 点阵字体数据（ASCII 32-126）
//=====================================================================

// 经典 8x8 点阵字体，每个字符 8 字节，每字节一行，MSB 在左
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
// 第四部分：实现
//=====================================================================
#ifdef GAMELIB_IMPLEMENTATION

//---------------------------------------------------------------------
// 动态加载的函数指针（全局，进程生命周期内有效）
//---------------------------------------------------------------------
static PFN_SetDIBitsToDevice   _gl_SetDIBitsToDevice  = NULL;
static PFN_GetStockObject      _gl_GetStockObject     = NULL;
static PFN_timeGetTime         _gl_timeGetTime        = NULL;
static PFN_timeBeginPeriod     _gl_timeBeginPeriod    = NULL;
static PFN_timeEndPeriod       _gl_timeEndPeriod      = NULL;
static PFN_PlaySoundA          _gl_PlaySoundA         = NULL;
static PFN_mciSendStringA      _gl_mciSendStringA     = NULL;

static int _gamelib_apis_loaded = 0;

static int _gamelib_load_apis()
{
    if (_gamelib_apis_loaded) return 0;

    HMODULE hGdi32 = LoadLibraryA("gdi32.dll");
    HMODULE hWinmm = LoadLibraryA("winmm.dll");
    if (!hGdi32 || !hWinmm) return -1;

    _gl_SetDIBitsToDevice = (PFN_SetDIBitsToDevice)GetProcAddress(hGdi32, "SetDIBitsToDevice");
    _gl_GetStockObject    = (PFN_GetStockObject)GetProcAddress(hGdi32, "GetStockObject");
    _gl_timeGetTime       = (PFN_timeGetTime)GetProcAddress(hWinmm, "timeGetTime");
    _gl_timeBeginPeriod   = (PFN_timeBeginPeriod)GetProcAddress(hWinmm, "timeBeginPeriod");
    _gl_timeEndPeriod     = (PFN_timeEndPeriod)GetProcAddress(hWinmm, "timeEndPeriod");
    _gl_PlaySoundA        = (PFN_PlaySoundA)GetProcAddress(hWinmm, "PlaySoundA");
    _gl_mciSendStringA    = (PFN_mciSendStringA)GetProcAddress(hWinmm, "mciSendStringA");

    if (!_gl_SetDIBitsToDevice || !_gl_GetStockObject ||
        !_gl_timeGetTime ||
        !_gl_timeBeginPeriod  || !_gl_timeEndPeriod ||
        !_gl_PlaySoundA       || !_gl_mciSendStringA) {
        return -1;
    }
    _gamelib_apis_loaded = 1;
    return 0;
}


//---------------------------------------------------------------------
// GDI+ 动态加载（用于 PNG/JPG 图片加载）
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

// 初始化 GDI+：加载 gdiplus.dll / ole32.dll 并调用 GdiplusStartup
static int _gamelib_gdiplus_init()
{
    if (_gamelib_gdiplus_ready) return 0;

    HMODULE hGdiPlus = LoadLibraryA("gdiplus.dll");
    if (!hGdiPlus) return -1;

    HMODULE hOle32 = LoadLibraryA("ole32.dll");
    if (!hOle32) return -2;

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

// 通过 COM vtable 调用 IUnknown::Release（无需 #include <ObjBase.h>）
static void _gamelib_com_release(void *obj)
{
    if (!obj) return;
    // COM 对象布局：第一个指针指向 vtable
    // IUnknown vtable: [0]=QueryInterface, [1]=AddRef, [2]=Release
    typedef unsigned long (WINAPI *PFN_Release)(void*);
    void **vtbl = *(void***)obj;
    PFN_Release release_fn = (PFN_Release)vtbl[2];
    release_fn(obj);
}

// GDI+ PixelFormat32bppARGB
#define _GL_PIXFMT_32bppARGB 2498570

// 通过 GDI+ 从内存加载图片（PNG/JPG/BMP/GIF/TIFF 等），返回 ARGB 像素
static uint32_t* _gamelib_gdiplus_load(
    const void *data, long size, int *out_w, int *out_h)
{
    if (_gamelib_gdiplus_init() != 0) return NULL;

    // 分配全局内存并复制文件数据
    HGLOBAL hg = GlobalAlloc(GMEM_MOVEABLE, size);
    if (!hg) return NULL;

    void *pg = GlobalLock(hg);
    if (!pg) { GlobalFree(hg); return NULL; }
    memcpy(pg, data, size);
    GlobalUnlock(hg);

    // 创建 IStream
    void *pStream = NULL;
    if (_gl_CreateStreamOnHGlobal(hg, FALSE, &pStream) != 0 || !pStream) {
        GlobalFree(hg);
        return NULL;
    }

    // 从 IStream 创建 GDI+ Bitmap
    void *gpBitmap = NULL;
    if (_gl_GdipCreateBitmapFromStream(pStream, &gpBitmap) != 0 || !gpBitmap) {
        _gamelib_com_release(pStream);
        GlobalFree(hg);
        return NULL;
    }

    // 获取图片尺寸
    UINT width = 0, height = 0;
    _gl_GdipGetImageWidth(gpBitmap, &width);
    _gl_GdipGetImageHeight(gpBitmap, &height);

    uint32_t *pixels = NULL;

    if (width == 0 || height == 0) goto cleanup;

    // LockBits 结构体（匹配 GDI+ BitmapData 布局）
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

        // ImageLockModeRead = 1，请求 32bppARGB 格式
        if (_gl_GdipBitmapLockBits(gpBitmap, lockRect, 1,
                _GL_PIXFMT_32bppARGB, &bd) != 0) {
            goto cleanup;
        }

        pixels = (uint32_t*)malloc(width * height * sizeof(uint32_t));
        if (pixels) {
            for (UINT y = 0; y < height; y++) {
                uint32_t *dst = pixels + y * width;
                const char *src = (const char*)bd.bScan0 + y * bd.bStride;
                memcpy(dst, src, width * sizeof(uint32_t));
            }
            // 24 位图片经 GDI+ 转换后 alpha 可能为 0，检测并修正为 255
            {
                UINT total = width * height;
                bool allZeroAlpha = true;
                for (UINT i = 0; i < total; i++) {
                    if (COLOR_GET_A(pixels[i]) != 0) {
                        allZeroAlpha = false;
                        break;
                    }
                }
                if (allZeroAlpha) {
                    for (UINT i = 0; i < total; i++) {
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


// 静态成员初始化
bool GameLib::_srandDone = false;


//---------------------------------------------------------------------
// 构造函数
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
    memset(_keys, 0, sizeof(_keys));
    memset(_keys_prev, 0, sizeof(_keys_prev));
    _mouseX = 0;
    _mouseY = 0;
    memset(_mouseButtons, 0, sizeof(_mouseButtons));
    _timeStart = 0;
    _timePrev = 0;
    _deltaTime = 0.0f;
    _fps = 0.0f;
    _fpsAccum = 0.0f;
    _fpsTime = 0;
    memset(_bmi_data, 0, sizeof(_bmi_data));
    _musicPlaying = false;
    if (!_srandDone) {
        srand((unsigned int)time(NULL));
        _srandDone = true;
    }
}


//---------------------------------------------------------------------
// 析构函数
//---------------------------------------------------------------------
GameLib::~GameLib()
{
    // 停止音乐
    if (_musicPlaying) {
        _gl_mciSendStringA("stop gamelib_music", NULL, 0, NULL);
        _gl_mciSendStringA("close gamelib_music", NULL, 0, NULL);
        _musicPlaying = false;
    }
    // 释放所有精灵
    for (size_t i = 0; i < _sprites.size(); i++) {
        if (_sprites[i].used && _sprites[i].pixels) {
            free(_sprites[i].pixels);
            _sprites[i].pixels = NULL;
            _sprites[i].used = false;
        }
    }
    // 释放所有 Tilemap
    for (size_t i = 0; i < _tilemaps.size(); i++) {
        if (_tilemaps[i].used && _tilemaps[i].tiles) {
            free(_tilemaps[i].tiles);
            _tilemaps[i].tiles = NULL;
            _tilemaps[i].used = false;
        }
    }
    // 释放帧缓冲
    if (_framebuffer) {
        free(_framebuffer);
        _framebuffer = NULL;
    }
    // 销毁窗口
    if (_hwnd) {
        DestroyWindow(_hwnd);
        _hwnd = NULL;
    }
    _gl_timeEndPeriod(1);
}


//---------------------------------------------------------------------
// 注册窗口类（静态，只执行一次）
//---------------------------------------------------------------------
int GameLib::_InitWindowClass()
{
    static int initialized = 0;
    if (initialized) return 0;
    HINSTANCE inst = GetModuleHandle(NULL);
    WNDCLASSA wc;
    memset(&wc, 0, sizeof(wc));
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wc.lpszClassName = "GameLibWindowClass";
    wc.hbrBackground = (HBRUSH)_gl_GetStockObject(BLACK_BRUSH);
    wc.hInstance = inst;
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = _WndProc;
    wc.cbWndExtra = 0;
    wc.cbClsExtra = 0;
    if (RegisterClassA(&wc) == 0) return -1;
    initialized = 1;
    return 0;
}


//---------------------------------------------------------------------
// 窗口过程（静态回调）
//---------------------------------------------------------------------
#define GAMELIB_REPEATED_KEYMASK (1 << 30)

LRESULT CALLBACK GameLib::_WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    GameLib *self = NULL;

    if (msg == WM_CREATE) {
        self = (GameLib*)(((LPCREATESTRUCT)lParam)->lpCreateParams);
        SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)self);
        return DefWindowProc(hWnd, msg, wParam, lParam);
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
        return DefWindowProc(hWnd, msg, wParam, lParam);

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
            if (self && self->_framebuffer && _gl_SetDIBitsToDevice) {
                _gl_SetDIBitsToDevice(ps.hdc, 0, 0,
                    self->_width, self->_height,
                    0, 0, 0, self->_height,
                    self->_framebuffer,
                    (BITMAPINFO*)self->_bmi_data,
                    DIB_RGB_COLORS);
            }
            EndPaint(hWnd, &ps);
        }
        return 0;

    default:
        return DefWindowProc(hWnd, msg, wParam, lParam);
    }
}


//---------------------------------------------------------------------
// 初始化 DIB 信息（32 位 ARGB）
//---------------------------------------------------------------------
void GameLib::_InitDIBInfo(void *ptr, int width, int height)
{
    BITMAPINFO *info = (BITMAPINFO*)ptr;
    memset(info, 0, sizeof(BITMAPINFOHEADER));
    info->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    info->bmiHeader.biWidth = width;
    info->bmiHeader.biHeight = -height;  // 负值表示 top-down
    info->bmiHeader.biPlanes = 1;
    info->bmiHeader.biBitCount = 32;
    info->bmiHeader.biCompression = BI_RGB;
    info->bmiHeader.biSizeImage = width * height * 4;
}


//---------------------------------------------------------------------
// 派发 Windows 消息
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
// Open: 创建窗口并初始化
//---------------------------------------------------------------------
int GameLib::Open(int width, int height, const char *title, bool center)
{
    // 动态加载 gdi32/winmm API
    if (_gamelib_load_apis() != 0) return -5;

    // 先销毁已有资源
    if (_framebuffer) { free(_framebuffer); _framebuffer = NULL; }
    if (_hwnd) { DestroyWindow(_hwnd); _hwnd = NULL; }

    if (_InitWindowClass() != 0) return -1;

    _width = width;
    _height = height;
    _title = title;
    _closing = false;
    _active = true;

    // 分配帧缓冲
    _framebuffer = (uint32_t*)malloc(width * height * sizeof(uint32_t));
    if (!_framebuffer) return -2;
    memset(_framebuffer, 0, width * height * sizeof(uint32_t));

    // 初始化 DIB 信息
    _InitDIBInfo(_bmi_data, width, height);

    // 计算窗口大小（使客户区等于 width x height）
    DWORD style = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;
    RECT rc = { 0, 0, width, height };
    AdjustWindowRect(&rc, style, FALSE);
    int ww = rc.right - rc.left;
    int wh = rc.bottom - rc.top;

    // 窗口位置：默认让系统决定，center 时居中
    int posX = CW_USEDEFAULT;
    int posY = CW_USEDEFAULT;
    if (center) {
        int screenW = GetSystemMetrics(SM_CXSCREEN);
        int screenH = GetSystemMetrics(SM_CYSCREEN);
        posX = (screenW - ww) / 2;
        posY = (screenH - wh) / 2;
    }

    // UTF-8 转宽字符
    int tlen = (int)strlen(title);
    wchar_t *wtitle = (wchar_t*)malloc((tlen * 2 + 10) * sizeof(wchar_t));
    if (!wtitle) { free(_framebuffer); _framebuffer = NULL; return -3; }
    MultiByteToWideChar(CP_UTF8, 0, title, -1, wtitle, tlen * 2 + 2);

    HINSTANCE inst = GetModuleHandle(NULL);
    _hwnd = CreateWindowW(L"GameLibWindowClass", wtitle, style,
        posX, posY, ww, wh, NULL, NULL, inst, this);

    free(wtitle);

    if (!_hwnd) {
        free(_framebuffer);
        _framebuffer = NULL;
        return -4;
    }

    // 创建后二次校正：确保客户区严格等于 width x height
    // AdjustWindowRect 在某些 DPI 环境下可能不精确
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

    // 初始化时间
    _gl_timeBeginPeriod(1);
    _timeStart = _gl_timeGetTime();
    _timePrev = _timeStart;
    _fpsTime = _timeStart;
    _fpsAccum = 0.0f;
    _fps = 0.0f;
    _deltaTime = 0.0f;

    // 初始化输入
    memset(_keys, 0, sizeof(_keys));
    memset(_keys_prev, 0, sizeof(_keys_prev));
    memset(_mouseButtons, 0, sizeof(_mouseButtons));

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
// Update: 将帧缓冲刷新到窗口，然后处理消息、更新输入
//---------------------------------------------------------------------
void GameLib::Update()
{
    if (!_hwnd || !_framebuffer) return;

    // 将帧缓冲区绘制到窗口
    HDC hdc = ::GetDC(_hwnd);
    if (hdc) {
        _gl_SetDIBitsToDevice(hdc, 0, 0, _width, _height,
            0, 0, 0, _height,
            _framebuffer,
            (BITMAPINFO*)_bmi_data,
            DIB_RGB_COLORS);
        ::ReleaseDC(_hwnd, hdc);
    }

    // 保存上一帧的按键状态（用于边沿检测）
    memcpy(_keys_prev, _keys, sizeof(_keys));

    // 派发消息
    _DispatchMessages();

    // 更新时间
    DWORD now = _gl_timeGetTime();
    int32_t delta = (int32_t)(now - _timePrev);
    if (delta < 0) delta = 0;
    _deltaTime = delta / 1000.0f;
    _timePrev = now;

    // 更新 FPS
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
// WaitFrame: 帧率控制
//---------------------------------------------------------------------
void GameLib::WaitFrame(int fps)
{
    if (fps <= 0) fps = 60;
    DWORD frameTime = 1000 / fps;
    DWORD now = _gl_timeGetTime();
    int32_t elapsed = (int32_t)(now - _timePrev);
    if (elapsed < 0) elapsed = 0;
    if ((DWORD)elapsed < frameTime) {
        Sleep(frameTime - (DWORD)elapsed);
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
        int len = (int)strlen(title);
        wchar_t *wt = (wchar_t*)malloc((len * 2 + 10) * sizeof(wchar_t));
        if (wt) {
            MultiByteToWideChar(CP_UTF8, 0, title, -1, wt, len * 2 + 2);
            SetWindowTextW(_hwnd, wt);
            free(wt);
        }
    }
}


//---------------------------------------------------------------------
// ShowFps: 是否在标题栏显示 FPS
//---------------------------------------------------------------------
void GameLib::ShowFps(bool show)
{
    _showFps = show;
    if (!show && _hwnd) {
        // 关闭时恢复原始标题
        SetTitle(_title.c_str());
    }
}


//---------------------------------------------------------------------
// _UpdateTitleFps: 更新标题栏 FPS 显示（内部方法）
//---------------------------------------------------------------------
void GameLib::_UpdateTitleFps()
{
    if (!_showFps || !_hwnd) return;
    char buf[256];
    snprintf(buf, sizeof(buf), "%s (FPS: %.1f)", _title.c_str(), _fps);
    int len = (int)strlen(buf);
    wchar_t *wt = (wchar_t*)malloc((len * 2 + 10) * sizeof(wchar_t));
    if (wt) {
        MultiByteToWideChar(CP_UTF8, 0, buf, -1, wt, len * 2 + 2);
        SetWindowTextW(_hwnd, wt);
        free(wt);
    }
}


//=====================================================================
// 帧缓冲操作
//=====================================================================

void GameLib::Clear(uint32_t color)
{
    if (!_framebuffer) return;
    int count = _width * _height;
    for (int i = 0; i < count; i++) _framebuffer[i] = color;
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
// 图形绘制
//=====================================================================

//---------------------------------------------------------------------
// DrawLine: Bresenham 算法
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
// 水平线（内部用，带裁剪）
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
    DrawLine(x, y, x + w - 1, y, color);
    DrawLine(x, y + h - 1, x + w - 1, y + h - 1, color);
    DrawLine(x, y, x, y + h - 1, color);
    DrawLine(x + w - 1, y, x + w - 1, y + h - 1, color);
}

void GameLib::FillRect(int x, int y, int w, int h, uint32_t color)
{
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
// DrawCircle: 中点圆算法
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
// FillTriangle: 扫描线填充
//---------------------------------------------------------------------
void GameLib::FillTriangle(int x1, int y1, int x2, int y2, int x3, int y3, uint32_t color)
{
    // 按 y 排序: y1 <= y2 <= y3
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
        // y3 != y1 在此处一定成立（退化情况已在上方 return）
        xa = x1 + (x3 - x1) * (y - y1) / (y3 - y1);
        if (y < y2) {
            if (y2 != y1) {
                xb = x1 + (x2 - x1) * (y - y1) / (y2 - y1);
            } else {
                xb = x1;
            }
        } else {
            if (y3 != y2) {
                xb = x2 + (x3 - x2) * (y - y2) / (y3 - y2);
            } else {
                xb = x2;
            }
        }
        _DrawHLine(xa, xb, y, color);
    }
}


//=====================================================================
// 文字渲染
//=====================================================================

void GameLib::DrawText(int x, int y, const char *text, uint32_t color)
{
    if (!text) return;
    int ox = x;
    for (const char *p = text; *p; p++) {
        unsigned char ch = (unsigned char)*p;
        if (ch == '\n') {
            x = ox;
            y += 10;  // 8 像素高 + 2 像素行间距
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
    sprintf(buf, "%d", number);
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


//=====================================================================
// 精灵系统
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
    if (width <= 0 || height <= 0) return -1;
    int id = _AllocSpriteSlot();
    uint32_t *pixels = (uint32_t*)malloc(width * height * sizeof(uint32_t));
    if (!pixels) return -1;
    memset(pixels, 0, width * height * sizeof(uint32_t));
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

    int dataOffset = *(int*)&header[10];
    int width      = *(int*)&header[18];
    int height     = *(int*)&header[22];
    int bpp        = *(short*)&header[28];

    if (bpp != 24 && bpp != 32) { fclose(fp); return -1; }

    bool bottomUp = (height > 0);
    if (height < 0) height = -height;

    fseek(fp, dataOffset, SEEK_SET);

    int bytesPerPixel = bpp / 8;
    int rowSize = ((width * bytesPerPixel + 3) / 4) * 4;
    unsigned char *rowData = (unsigned char*)malloc(rowSize);
    if (!rowData) { fclose(fp); return -1; }

    int id = CreateSprite(width, height);
    if (id < 0) { free(rowData); fclose(fp); return -1; }

    for (int y = 0; y < height; y++) {
        if (fread(rowData, 1, rowSize, fp) != (size_t)rowSize) break;
        int destY = bottomUp ? (height - 1 - y) : y;
        uint32_t *destRow = _sprites[id].pixels + destY * width;
        for (int x = 0; x < width; x++) {
            unsigned char b = rowData[x * bytesPerPixel + 0];
            unsigned char g = rowData[x * bytesPerPixel + 1];
            unsigned char r = rowData[x * bytesPerPixel + 2];
            unsigned char a = (bpp == 32) ? rowData[x * bytesPerPixel + 3] : 0xFF;
            destRow[x] = COLOR_ARGB(a, r, g, b);
        }
    }

    free(rowData);
    fclose(fp);
    return id;
}

int GameLib::LoadSprite(const char *filename)
{
    // 读取文件到内存
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

    // 通过 GDI+ 解码（支持 PNG/JPG/BMP/GIF/TIFF）
    int imgW = 0, imgH = 0;
    uint32_t *pixels = _gamelib_gdiplus_load(fileData, fileSize, &imgW, &imgH);

    if (!pixels || imgW <= 0 || imgH <= 0) {
        if (pixels) free(pixels);
        // GDI+ 失败，若为 BMP 文件则回退到 LoadSpriteBMP
        bool isBMP = (fileSize >= 2 && fileData[0] == 'B' && fileData[1] == 'M');
        free(fileData);
        return isBMP ? LoadSpriteBMP(filename) : -1;
    }

    free(fileData);

    // 分配精灵槽位，直接使用 GDI+ 返回的像素数据
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
    if (id < 0 || id >= (int)_sprites.size()) return;
    if (!_sprites[id].used) return;
    GameSprite &spr = _sprites[id];
    for (int sy = 0; sy < spr.height; sy++) {
        int dy = y + sy;
        if (dy < 0 || dy >= _height) continue;
        for (int sx = 0; sx < spr.width; sx++) {
            int dx = x + sx;
            if (dx < 0 || dx >= _width) continue;
            uint32_t c = spr.pixels[sy * spr.width + sx];
            if (COLOR_GET_A(c) > 0) {
                _framebuffer[dy * _width + dx] = c;
            }
        }
    }
}

void GameLib::DrawSpriteEx(int id, int x, int y, int flags)
{
    if (id < 0 || id >= (int)_sprites.size()) return;
    if (!_sprites[id].used) return;
    GameSprite &spr = _sprites[id];
    bool flipH = (flags & SPRITE_FLIP_H) != 0;
    bool flipV = (flags & SPRITE_FLIP_V) != 0;

    // 预裁剪，避免逐像素边界检查
    int sx0 = 0, sx1 = spr.width;
    int sy0 = 0, sy1 = spr.height;
    if (x < 0) sx0 = -x;
    if (y < 0) sy0 = -y;
    if (x + sx1 > _width)  sx1 = _width - x;
    if (y + sy1 > _height) sy1 = _height - y;
    if (sx0 >= sx1 || sy0 >= sy1) return;

    if (flags & SPRITE_ALPHA) {
        // ---- Alpha 混合路径 ----
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
        // ---- ColorKey 透明色路径 ----
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
        // ---- 不透明路径（跳过 alpha=0） ----
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
// 输入系统
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


//=====================================================================
// 声音
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
    // 先停止之前的音乐
    if (_musicPlaying) {
        _gl_mciSendStringA("stop gamelib_music", NULL, 0, NULL);
        _gl_mciSendStringA("close gamelib_music", NULL, 0, NULL);
        _musicPlaying = false;
    }
    // 打开音频文件（支持 mp3/mid/wav 等 MCI 支持的格式）
    char cmd[1024];
    snprintf(cmd, sizeof(cmd), "open \"%s\" type mpegvideo alias gamelib_music", filename);
    if (_gl_mciSendStringA(cmd, NULL, 0, NULL) != 0) {
        // mpegvideo 失败时尝试自动检测类型
        snprintf(cmd, sizeof(cmd), "open \"%s\" alias gamelib_music", filename);
        if (_gl_mciSendStringA(cmd, NULL, 0, NULL) != 0) {
            return;
        }
    }
    // 播放
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
// 工具函数
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
    int dx = cx1 - cx2;
    int dy = cy1 - cy2;
    int distSq = dx * dx + dy * dy;
    int rSum = r1 + r2;
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
// 网格辅助
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
// Tilemap 系统
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

    int id = _AllocTilemapSlot();
    int *tiles = (int*)malloc(cols * rows * sizeof(int));
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

    // 计算屏幕可见的瓦片范围，避免遍历整张地图
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

            // tileset 中该瓦片的像素起点
            int srcCol = tid % tsCols;
            int srcRow = tid / tsCols;
            int srcX0 = srcCol * ts;
            int srcY0 = srcRow * ts;

            // 屏幕目标位置
            int dstX0 = x + c * ts;
            int dstY0 = y + r * ts;

            // 瓦片内裁剪
            int ix0 = 0, iy0 = 0, ix1 = ts, iy1 = ts;
            if (dstX0 < 0) ix0 = -dstX0;
            if (dstY0 < 0) iy0 = -dstY0;
            if (dstX0 + ix1 > _width)  ix1 = _width - dstX0;
            if (dstY0 + iy1 > _height) iy1 = _height - dstY0;

            if (useAlpha) {
                // ---- Alpha 混合路径 ----
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
                // ---- ColorKey 透明色路径 ----
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
                // ---- 不透明路径（跳过 alpha=0） ----
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
