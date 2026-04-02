# GameLib.h 规格说明文档

## 概述

`GameLib.h` 是一个面向初学者的 **单头文件游戏库**，基于 Win32 GDI，无需 SDL 或其他第三方库。目标用户是小朋友，用于在 Dev C++ (GCC 4.9.2) 环境下开发简单游戏（空战、俄罗斯方块、走迷宫等）。

**文件位置**: `render/device/GameLib.h`
**当前行数**: ~1606 行
**最后修改**: 2026/04/02

---

## 1. 设计原则

### 1.1 单头文件模式 (stb-style)

采用 `GAMELIB_IMPLEMENTATION` 宏守卫模式：

- **单文件项目**（最常见场景）：直接 `#include "GameLib.h"` 即可，默认自动启用实现
- **多文件项目**：
  - 主 .cpp 文件：`#define GAMELIB_IMPLEMENTATION` 后 `#include "GameLib.h"`
  - 其他 .cpp 文件：`#define GAMELIB_NO_IMPLEMENTATION` 后 `#include "GameLib.h"`

实现逻辑：如果没定义 `GAMELIB_NO_IMPLEMENTATION`，则自动定义 `GAMELIB_IMPLEMENTATION`。

### 1.2 编译器兼容性

- **目标编译器**: GCC 4.9.2 (Dev C++ 自带)
- **不使用 C++17 特性**，不使用 `inline` 变量
- **C-style 强制类型转换** 可以接受（为了简洁）
- **编译命令**: `g++ -o game.exe main.cpp -mwindows`（gdi32/winmm 通过 LoadLibrary 动态加载）

### 1.3 双缓冲架构

所有绘制操作都写入内存帧缓冲 (`uint32_t*` ARGB 数组)，只有调用 `Update()` 时才通过 `SetDIBitsToDevice` 刷新到窗口。

### 1.4 图形自实现

**不使用任何 GDI 绘图函数**（如 `LineTo`, `Ellipse` 等），所有图形算法自行实现：
- Bresenham 直线算法
- 中点圆算法
- 扫描线三角形填充
- 内嵌 8x8 ASCII 位图字体

唯一使用的 GDI 函数是 `SetDIBitsToDevice`（用于最终显示）。

### 1.5 精灵系统

使用 **整数 ID** 管理精灵（非对象），GameLib 内部管理精灵内存，析构时自动释放。

---

## 2. 文件结构

```
GameLib.h
├── 文件头注释 & 使用示例
├── #ifndef GAMELIB_H / #define GAMELIB_H
├── GAMELIB_IMPLEMENTATION 自动定义逻辑
├── 第一部分：常量定义
│   ├── 颜色常量 (COLOR_xxx)
│   ├── 颜色构造/提取宏
│   ├── 键盘常量 (KEY_xxx)
│   ├── 鼠标按键常量 (MOUSE_xxx)
│   ├── 精灵标志 (SPRITE_xxx)
│   └── Color Key 默认色 (COLORKEY_DEFAULT)
├── 第二部分：类声明
│   ├── GameSprite 结构体（GameLib 类内部嵌套）
│   └── GameLib 类 (公开方法 + 私有成员)
├── 第三部分：8x8 点阵字体数据 (ASCII 32-126)
├── #ifdef GAMELIB_IMPLEMENTATION
│   └── 第四部分：完整实现
│       ├── 构造/析构
│       ├── 窗口管理 (Open, IsClosed, Update, WaitFrame)
│       ├── 帧缓冲操作 (Clear, SetPixel, GetPixel)
│       ├── 图形绘制 (Line, Rect, Circle, Triangle)
│       ├── 文字渲染 (DrawText, DrawNumber, DrawTextScale, DrawPrintf)
│       ├── 精灵系统 (Create, Load, Free, Draw)
│       ├── 输入系统 (Key, Mouse)
│       ├── 声音 (Beep, WAV, Music/MCI)
│       ├── 工具函数 (Random, 碰撞检测, 距离)
│       └── 网格辅助 (DrawGrid, FillCell)
├── #endif // GAMELIB_IMPLEMENTATION
└── #endif // GAMELIB_H
```

---

## 3. 依赖

### 3.1 系统头文件

```c
#include <windows.h>
#include <mmsystem.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <stdarg.h>
#include <vector>
#include <string>
```

### 3.2 链接库

- `gdi32` — SetDIBitsToDevice（通过 LoadLibrary 动态加载）
- `winmm` — timeGetTime, timeBeginPeriod, PlaySound, mciSendString（通过 LoadLibrary 动态加载）
- `user32` — 窗口管理（MSVC 自动链接，GCC 需要 `-mwindows`）

> 编译时只需 `-mwindows`，不再需要 `-lgdi32 -lwinmm`。

---

## 4. 常量定义

### 4.1 颜色常量（ARGB 格式: `0xAARRGGBB`）

| 常量名 | 值 | 说明 |
|--------|-----|------|
| `COLOR_BLACK` | `0xFF000000` | 黑色 |
| `COLOR_WHITE` | `0xFFFFFFFF` | 白色 |
| `COLOR_RED` | `0xFFFF0000` | 红色 |
| `COLOR_GREEN` | `0xFF00FF00` | 绿色 |
| `COLOR_BLUE` | `0xFF0000FF` | 蓝色 |
| `COLOR_YELLOW` | `0xFFFFFF00` | 黄色 |
| `COLOR_CYAN` | `0xFF00FFFF` | 青色 |
| `COLOR_MAGENTA` | `0xFFFF00FF` | 品红 |
| `COLOR_ORANGE` | `0xFFFF8800` | 橙色 |
| `COLOR_PINK` | `0xFFFF88CC` | 粉色 |
| `COLOR_PURPLE` | `0xFF8800FF` | 紫色 |
| `COLOR_GRAY` | `0xFF888888` | 灰色 |
| `COLOR_DARK_GRAY` | `0xFF444444` | 深灰 |
| `COLOR_LIGHT_GRAY` | `0xFFCCCCCC` | 浅灰 |
| `COLOR_DARK_RED` | `0xFF880000` | 深红 |
| `COLOR_DARK_GREEN` | `0xFF008800` | 深绿 |
| `COLOR_DARK_BLUE` | `0xFF000088` | 深蓝 |
| `COLOR_SKY_BLUE` | `0xFF87CEEB` | 天蓝 |
| `COLOR_BROWN` | `0xFF8B4513` | 棕色 |
| `COLOR_GOLD` | `0xFFFFD700` | 金色 |
| `COLOR_TRANSPARENT` | `0x00000000` | 透明 |

### 4.2 颜色工具宏

```c
COLOR_RGB(r, g, b)       // 从 R/G/B 构造不透明颜色
COLOR_ARGB(a, r, g, b)   // 从 A/R/G/B 构造颜色
COLOR_GET_A(c)            // 提取 Alpha 分量
COLOR_GET_R(c)            // 提取 Red 分量
COLOR_GET_G(c)            // 提取 Green 分量
COLOR_GET_B(c)            // 提取 Blue 分量
```

### 4.3 键盘常量

- **方向键**: `KEY_LEFT`, `KEY_RIGHT`, `KEY_UP`, `KEY_DOWN`
- **功能键**: `KEY_SPACE`, `KEY_ENTER`, `KEY_ESCAPE`, `KEY_TAB`, `KEY_SHIFT`, `KEY_CONTROL`, `KEY_BACK`
- **字母键**: `KEY_A` ~ `KEY_Z` (0x41~0x5A)
- **数字键**: `KEY_0` ~ `KEY_9` (0x30~0x39)
- **F键**: `KEY_F1` ~ `KEY_F12`

所有键盘常量直接使用 Windows Virtual Key Code。

### 4.4 鼠标按键常量

| 常量名 | 值 | 说明 |
|--------|-----|------|
| `MOUSE_LEFT` | 0 | 左键 |
| `MOUSE_RIGHT` | 1 | 右键 |
| `MOUSE_MIDDLE` | 2 | 中键 |

### 4.5 精灵标志

| 常量名 | 值 | 说明 |
|--------|-----|------|
| `SPRITE_FLIP_H` | 1 | 水平翻转 |
| `SPRITE_FLIP_V` | 2 | 垂直翻转 |
| `SPRITE_COLORKEY` | 4 | 透明色模式 |
| `COLORKEY_DEFAULT` | `0xFFFF00FF` | 默认透明色：品红 (255,0,255)，可用 `#ifndef` 覆盖 |

---

## 5. 数据结构

### 5.1 GameSprite

```cpp
struct GameSprite {
    int width;          // 精灵宽度（像素）
    int height;         // 精灵高度（像素）
    uint32_t *pixels;   // ARGB 像素数组 (malloc 分配)
    bool used;          // 槽位是否被使用
};
```

### 5.2 GameLib 私有成员

```cpp
// 窗口状态
HWND _hwnd;
bool _closing;          // 窗口是否关闭
bool _active;           // 窗口是否激活
bool _showFps;          // 是否在标题栏显示 FPS
std::string _title;
int _width, _height;    // 客户区尺寸

// 帧缓冲
uint32_t *_framebuffer; // width * height 的 ARGB 数组，malloc 分配

// DIB 信息（用于 SetDIBitsToDevice）
unsigned char _bmi_data[sizeof(BITMAPINFO) + 16 * sizeof(RGBQUAD)];

// 输入状态
int _keys[512];         // 当前帧按键状态
int _keys_prev[512];    // 上一帧按键状态（用于边沿检测）
int _mouseX, _mouseY;   // 鼠标坐标
int _mouseButtons[3];   // 鼠标三键状态

// 时间
DWORD _timeStart;       // Open() 时的时间戳
DWORD _timePrev;        // 上一帧时间戳
float _deltaTime;       // 帧间隔（秒）
float _fps;             // 当前 FPS
float _fpsAccum;        // FPS 计数累加器
DWORD _fpsTime;         // FPS 统计时间窗口起点

// 精灵存储
std::vector<GameSprite> _sprites;

// 音乐状态（MCI）
bool _musicPlaying;     // 是否正在播放 MCI 音乐

// 随机数
static bool _srandDone; // srand 是否已初始化
```

---

## 6. 公开 API

### 6.1 窗口与主循环

#### `int Open(int width, int height, const char *title, bool center = false)`
- 创建窗口并初始化帧缓冲、输入、时间系统
- **保证客户区严格等于 width × height**：先用 `AdjustWindowRect` 计算，创建后用 `GetClientRect` 二次校正
- `center=true` 时窗口居中显示；否则使用 `CW_USEDEFAULT`
- 窗口样式: `WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX`（不可缩放）
- 窗口标题支持 UTF-8（内部转 WideChar）
- 返回值: 0=成功, 负数=错误
- 使用 `GWLP_USERDATA` 存储 this 指针
- 调用 `timeBeginPeriod(1)` 提高时钟精度

#### `bool IsClosed() const`
- 窗口是否已关闭（WM_CLOSE 或 WM_DESTROY 触发）

#### `void Update()`
1. 通过 `SetDIBitsToDevice` 将帧缓冲刷新到窗口
2. 保存上一帧按键状态到 `_keys_prev`
3. 派发 Windows 消息（PeekMessage 循环）
4. 更新 deltaTime 和 FPS（每秒统计一次）
5. FPS 更新时调用 `_UpdateTitleFps()` 更新标题栏显示

#### `void WaitFrame(int fps)`
- 帧率控制，计算距离上次 Update 的耗时，不足则 `Sleep`
- fps <= 0 时默认按 60 处理

#### `float GetDeltaTime() const`
- 上一帧到当前帧的时间间隔（秒）

#### `float GetFPS() const`
- 当前帧率（每秒更新一次）

#### `float GetTime() const`
- 从 `Open()` 开始到现在的总时间（秒）

#### `int GetWidth() const` / `int GetHeight() const`
- 客户区宽度/高度

#### `void SetTitle(const char *title)`
- 修改窗口标题（支持 UTF-8）

#### `void ShowFps(bool show)`
- 设置是否在窗口标题栏显示实时 FPS
- `show=true` 时，标题栏显示为 `"原标题 (FPS: 58.8)"` 格式，每秒更新一次（与 FPS 统计周期同步）
- `show=false` 时，恢复为原始标题（调用 `SetTitle` 恢复）
- 适用于开发调试，方便观察帧率

### 6.2 帧缓冲操作

#### `void Clear(uint32_t color = COLOR_BLACK)`
- 用指定颜色填充整个帧缓冲

#### `void SetPixel(int x, int y, uint32_t color)`
- 设置指定像素颜色（带边界检查）

#### `uint32_t GetPixel(int x, int y) const`
- 获取指定像素颜色（越界返回 0）

### 6.3 图形绘制

#### `void DrawLine(int x1, int y1, int x2, int y2, uint32_t color)`
- **Bresenham 直线算法**，通过 SetPixel 逐点绘制（带边界检查）

#### `void DrawRect(int x, int y, int w, int h, uint32_t color)`
- 绘制矩形边框（由 4 条 DrawLine 组成）

#### `void FillRect(int x, int y, int w, int h, uint32_t color)`
- 填充矩形（带裁剪，直接写帧缓冲）

#### `void DrawCircle(int cx, int cy, int r, uint32_t color)`
- **中点圆算法**，绘制 8 个对称点

#### `void FillCircle(int cx, int cy, int r, uint32_t color)`
- **中点圆算法 + 水平线填充**（4 条水平线/每步）

#### `void DrawTriangle(int x1, int y1, int x2, int y2, int x3, int y3, uint32_t color)`
- 绘制三角形边框（3 条 DrawLine）

#### `void FillTriangle(int x1, int y1, int x2, int y2, int x3, int y3, uint32_t color)`
- **扫描线填充算法**
- 先按 y 排序三个顶点，然后逐行扫描
- 处理了退化情况（三点共线/水平线）

### 6.4 文字渲染

#### `void DrawText(int x, int y, const char *text, uint32_t color)`
- 使用内嵌 8x8 位图字体（ASCII 32~126, 共 95 个字符）
- 支持 `\n` 换行（行间距 = 8 + 2 = 10 像素）
- 每个字符宽度 8 像素

#### `void DrawNumber(int x, int y, int number, uint32_t color)`
- 将整数转为字符串后调用 DrawText

#### `void DrawTextScale(int x, int y, const char *text, uint32_t color, int scale)`
- 放大版文字绘制，每个字体像素变为 scale × scale 的矩形
- 支持 `\n` 换行

#### `void DrawPrintf(int x, int y, uint32_t color, const char *fmt, ...)`
- 格式化输出，类似 `printf`，支持 `%d`, `%s`, `%f` 等格式符
- 内部使用 `vsnprintf`（1024 字节缓冲），格式化后调用 `DrawText` 绘制
- 方便在屏幕上显示变量值、分数、调试信息等

### 6.5 精灵系统

#### `int CreateSprite(int width, int height)`
- 创建空白精灵，返回整数 ID（失败返回 -1）
- 像素初始化为全 0（透明黑）

#### `int LoadSpriteBMP(const char *filename)`
- 从 BMP 文件加载精灵，支持 **24-bit 和 32-bit** BMP
- 处理 bottom-up / top-down 行序
- 每行按 4 字节对齐读取
- 24-bit BMP alpha 默认为 0xFF
- 返回精灵 ID（失败返回 -1）

#### `void FreeSprite(int id)`
- 释放精灵内存，标记槽位为未使用

#### `void DrawSprite(int id, int x, int y)`
- 绘制精灵到帧缓冲（带裁剪）
- **Alpha > 0 的像素才绘制**（简单透明判定，非混合）

#### `void DrawSpriteEx(int id, int x, int y, int flags)`
- 带标志的精灵绘制
- `SPRITE_FLIP_H`: 水平翻转
- `SPRITE_FLIP_V`: 垂直翻转
- `SPRITE_COLORKEY`: 与 `COLORKEY_DEFAULT`（品红 0xFFFF00FF）完全匹配的像素跳过

#### `void DrawSpriteRegion(int id, int x, int y, int sx, int sy, int sw, int sh)`
- 绘制精灵的子区域（sprite sheet 切图）

#### `void SetSpritePixel(int id, int x, int y, uint32_t color)`
- 修改精灵指定像素

#### `uint32_t GetSpritePixel(int id, int x, int y) const`
- 读取精灵指定像素

#### `int GetSpriteWidth(int id) const` / `int GetSpriteHeight(int id) const`
- 获取精灵尺寸

### 6.6 输入系统

#### `bool IsKeyDown(int key) const`
- 按键是否正在按下（持续按住也返回 true）

#### `bool IsKeyPressed(int key) const`
- 按键是否刚刚按下（**边沿检测**：当前帧按下且上一帧未按下）

#### `int GetMouseX() const` / `int GetMouseY() const`
- 鼠标相对于客户区的坐标

#### `bool IsMouseDown(int button) const`
- 鼠标按键是否按下

### 6.7 声音

#### `void PlayBeep(int frequency, int duration)`
- Windows Beep（阻塞式，简单蜂鸣）

#### `void PlayWAV(const char *filename, bool loop = false)`
- 使用 `PlaySoundA` 播放 WAV 文件（异步）
- `loop=true` 时循环播放

#### `void StopWAV()`
- 停止当前 WAV 播放

#### `void PlayMusic(const char *filename, bool loop = true)`
- 使用 **MCI (Media Control Interface)** 播放背景音乐
- 支持 MP3、MIDI、WAV 等 MCI 支持的格式
- 默认循环播放（`loop=true`）
- 先尝试以 `mpegvideo` 类型打开，失败则自动检测类型
- 使用固定别名 `gamelib_music`，同时只能播放一首背景音乐
- 调用时会自动停止之前的音乐
- **与 PlayWAV 独立**，可同时播放背景音乐和音效

#### `void StopMusic()`
- 停止当前 MCI 背景音乐并释放资源

### 6.8 工具函数（static）

#### `static int Random(int minVal, int maxVal)`
- 返回 [minVal, maxVal] 范围内的随机整数
- 内部使用 `rand()`，构造函数中自动 `srand(time(NULL))`

#### `static bool RectOverlap(int x1, int y1, int w1, int h1, int x2, int y2, int w2, int h2)`
- AABB 矩形碰撞检测

#### `static bool CircleOverlap(int cx1, int cy1, int r1, int cx2, int cy2, int r2)`
- 圆形碰撞检测（距离平方比较，无浮点开方）

#### `static bool PointInRect(int px, int py, int x, int y, int w, int h)`
- 点是否在矩形内

#### `static float Distance(int x1, int y1, int x2, int y2)`
- 两点距离（浮点，使用 sqrtf）

### 6.9 网格辅助

#### `void DrawGrid(int x, int y, int rows, int cols, int cellSize, uint32_t color)`
- 绘制网格线

#### `void FillCell(int gridX, int gridY, int row, int col, int cellSize, uint32_t color)`
- 填充网格中的一个单元格（留 1 像素内边距避免覆盖网格线）

---

## 7. 窗口过程 (WndProc) 处理的消息

| 消息 | 处理 |
|------|------|
| `WM_CREATE` | 通过 `LPCREATESTRUCT` 保存 this 指针到 `GWLP_USERDATA` |
| `WM_DESTROY` | 设置 `_closing = true`，调用 `PostQuitMessage(0)` |
| `WM_CLOSE` | 设置 `_closing = true`（不调用 DestroyWindow，留给析构） |
| `WM_ACTIVATE` | 更新 `_active` 状态 |
| `WM_KEYDOWN` | 过滤重复按键（bit 30），设置 `_keys[vk] = 1` |
| `WM_KEYUP` | 设置 `_keys[vk] = 0` |
| `WM_MOUSEMOVE` | 更新 `_mouseX`, `_mouseY` |
| `WM_LBUTTONDOWN/UP` | 更新 `_mouseButtons[0]` |
| `WM_RBUTTONDOWN/UP` | 更新 `_mouseButtons[1]` |
| `WM_MBUTTONDOWN/UP` | 更新 `_mouseButtons[2]` |
| `WM_PAINT` | 用 `SetDIBitsToDevice` 重绘帧缓冲 |

---

## 8. 内部辅助方法

| 方法 | 说明 |
|------|------|
| `static int _InitWindowClass()` | 注册窗口类（只执行一次，使用静态局部变量） |
| `static LRESULT CALLBACK _WndProc(...)` | 窗口过程回调 |
| `void _DispatchMessages()` | PeekMessage 循环派发消息 |
| `void _InitDIBInfo(void *ptr, int w, int h)` | 初始化 BITMAPINFO（32-bit, top-down） |
| `void _SetPixelFast(int x, int y, uint32_t c)` | 无边界检查的像素写入 |
| `void _DrawHLine(int x1, int x2, int y, uint32_t c)` | 带裁剪的水平线（FillCircle/FillTriangle 内部用） |
| `void _UpdateTitleFps()` | 当 `_showFps=true` 时更新标题栏 FPS 显示（在 FPS 统计更新时调用） |
| `int _AllocSpriteSlot()` | 在 `_sprites` 向量中找空闲槽位或追加新槽位 |

---

## 9. 字体数据

- 8x8 位图字体，覆盖 ASCII 32~126（95 个字符）
- 存储为 `static const unsigned char _gamelib_font8x8[95][8]`
- 每个字符 8 字节，每字节表示一行，MSB 在左
- 包含所有可打印 ASCII 字符：空格、标点、数字、大小写字母

---

## 10. 使用示例

```cpp
#include "GameLib.h"

int main() {
    GameLib game;
    game.Open(640, 480, "My Game", true);

    int x = 320, y = 240;

    while (!game.IsClosed()) {
        if (game.IsKeyDown(KEY_UP))    y -= 3;
        if (game.IsKeyDown(KEY_DOWN))  y += 3;
        if (game.IsKeyDown(KEY_LEFT))  x -= 3;
        if (game.IsKeyDown(KEY_RIGHT)) x += 3;

        game.Clear(COLOR_BLACK);
        game.FillRect(x - 10, y - 10, 20, 20, COLOR_RED);
        game.DrawText(10, 10, "Move the box!", COLOR_WHITE);
        game.Update();

        game.WaitFrame(60);
    }
    return 0;
}
```

---

## 11. 已知限制与未来改进方向

### 已知限制

1. **无 Alpha 混合** — DrawSprite 仅判断 alpha > 0 来决定是否绘制，没有真正的 alpha blending
2. **未经编译测试** — 当前系统无 C++ 编译器，仅通过 cppcheck 静态分析验证（0 errors）
3. **C-style 强制转换** — cppcheck 会报 style 警告，但对目标用户群体来说可以接受
4. **PlayBeep 是阻塞的** — 会暂停游戏循环
5. **单窗口** — 窗口类名固定为 "GameLibWindowClass"，同时只能有一个 GameLib 实例正常工作
6. **仅支持 BMP 加载** — 不支持 PNG/JPG 等格式
7. **WaitFrame 精度** — 依赖 `Sleep`，在低 fps 目标下足够，高精度场景可能不够
8. **MCI 音乐单通道** — 同时只能播放一首背景音乐（固定别名 `gamelib_music`）

### 未来改进方向

1. **Alpha 混合** — 为 DrawSprite 系列方法添加真正的 per-pixel alpha blending
2. **精灵缩放/旋转** — 添加 `DrawSpriteScaled` / `DrawSpriteRotated`
3. **更多图元** — 椭圆、圆角矩形、贝塞尔曲线等
4. **简单动画系统** — 帧动画支持（sprite sheet 自动切帧）
5. **音频增强** — 多通道音效、音量控制
6. **文字增强** — 支持更多字体大小、中文显示
7. **示例游戏 Demo** — 编写打砖块、太空射击等示例

---

## 12. 技术决策备忘

| 决策 | 原因 |
|------|------|
| 使用 `GAMELIB_IMPLEMENTATION` 而非 C++17 inline | GCC 4.9.2 不支持 C++17 inline 变量 |
| 默认启用实现（除非定义 `GAMELIB_NO_IMPLEMENTATION`） | 单文件项目不需要额外宏，降低上手门槛 |
| 创建后二次校正窗口尺寸 | `AdjustWindowRect` 在高 DPI 下不准确 |
| 使用 `GWLP_USERDATA`（非 `GWL_USERDATA`） | 兼容 64 位编译 |
| 精灵用整数 ID 管理 | 比对象指针更简单，适合初学者 |
| 使用 `malloc/free` 管理像素内存 | 与 C 风格一致，简单直接 |
| 窗口类注册使用 `RegisterClassA` + `CreateWindowW` | 类名用 ANSI 简单，标题用 WideChar 支持 UTF-8 |
| 键盘重复过滤（bit 30） | 避免按住一个键产生大量 KeyDown 事件 |
| FPS 每秒统计一次 | 平滑显示，避免帧间波动 |
| Color Key 用品红 (0xFFFF00FF) 而非黑色 | 黑色难以制作和判断，品红是 2D 资源常用透明色 |
| `COLORKEY_DEFAULT` 用 `#ifndef` 保护 | 允许用户在 include 前自定义覆盖 |
| PlayMusic 用 MCI 而非 PlaySound | MCI 支持 MP3，且与 PlaySound 独立通道，可同时播放音乐和音效 |
| MCI 先尝试 mpegvideo 再自动检测 | mpegvideo 是 MP3 最可靠的类型，回退保证兼容其他格式 |
