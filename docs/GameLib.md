# GameLib.h 规格说明文档

## 概述

`GameLib.h` 是一个面向初学者的 **单头文件游戏库**，基于 Win32 GDI，无需 SDL 或其他第三方库。目标用户是小朋友，用于在 Dev C++ (GCC 4.9.2) 环境下开发简单游戏（空战、俄罗斯方块、走迷宫等）。

**当前版本**: `1.9.8`
**最后修改**: 2026/05/01


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

所有绘制操作都写入内存帧缓冲 (`uint32_t*` ARGB 数组)。`Open()` 时确定 framebuffer 固定尺寸；之后调用 `Update()` 时，会在库内部先把 framebuffer 以最近邻方式缩放到一个 `present buffer`，再用 `SetDIBitsToDevice` 1:1 提交到当前窗口客户区。

### 1.4 图形自实现

**不使用 `LineTo`、`Ellipse` 等 GDI 图元函数**，线段、圆、三角形等基础图形算法全部自行实现：
- Bresenham 直线算法
- 中点圆算法
- 扫描线三角形填充
- 内嵌 8x8 ASCII 位图字体

GDI 只用于两类后端能力：
- `BitBlt` / `SetDIBitsToDevice` 等函数负责把 DIB Section 帧缓冲和缩放后的 `present buffer` 提交到窗口
- `TextOutW`、`CreateFontW`、`GetTextExtentPoint32W` 等函数负责当前 Windows 后端的可缩放字体绘制与测量

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
│   ├── Color Key 默认色 (COLORKEY_DEFAULT)
│   └── 动态加载函数指针类型 (gdi32/winmm/gdiplus/ole32)
├── 第二部分：类声明
│   ├── GameSprite 结构体（GameLib 类内部嵌套）
│   ├── GameTilemap 结构体（GameLib 类内部嵌套）
│   └── GameLib 类 (公开方法 + 私有成员)
├── 第三部分：8x8 点阵字体数据 (ASCII 32-126)
├── #ifdef GAMELIB_IMPLEMENTATION
│   └── 第四部分：完整实现
│       ├── 动态加载 API 初始化 (gdi32/winmm)
│       ├── GDI+ 懒初始化与图片解码 (_gamelib_gdiplus_init, _gamelib_gdiplus_load)
│       ├── 构造/析构
│       ├── 窗口管理 (Open, IsClosed, Update, WaitFrame)
│       ├── 帧缓冲操作 (Clear, SetPixel, GetPixel, Clip Rectangle)
│       ├── 图形绘制 (Line, Rect, Circle, Triangle)
│       ├── 文字渲染 (DrawText, DrawNumber, DrawTextScale, DrawPrintf, DrawPrintfScale)
│       ├── 字体文字渲染 (DrawTextFont, DrawPrintfFont, GetTextWidthFont, GetTextHeightFont)
│       ├── 精灵系统 (Create, Load, Free, Draw, Region, Scale, Frame)
│       ├── 输入系统 (Key, Mouse, Released, Wheel, Active)
│       ├── 声音 (Beep, WAV, Music/MCI)
│       ├── 工具函数 (Random, 碰撞检测, 距离)
│       ├── 场景管理 (SetScene, GetScene, IsSceneChanged, GetPreviousScene)
│       ├── 网格辅助 (DrawGrid, FillCell)
│       ├── Tilemap 系统 (Create, Free, Set/Get, Convert, Fill/Clear, Draw)
│       └── 存档读写 (SaveInt/Float/String, LoadInt/Float/String, HasSaveKey, DeleteSaveKey, DeleteSave)
├── #endif // GAMELIB_IMPLEMENTATION
└── #endif // GAMELIB_H
```

---

## 3. 依赖

### 3.1 系统头文件

```c
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
```

> 注：不再 `#include <mmsystem.h>`，所需的 mmsystem 类型和常量（`SND_ASYNC`、`MCIERROR` 等）在头文件内自行声明。

### 3.2 链接库

- `gdi32` — BitBlt, StretchBlt, SetStretchBltMode, CreateDIBSection, CreateCompatibleDC, CreateFontW, TextOutW, SelectObject, DeleteObject, DeleteDC, GetStockObject, SetDIBitsToDevice, SetTextColor, SetBkMode, GetTextExtentPoint32W, GdiFlush（通过 LoadLibrary 动态加载）
- `winmm` — timeBeginPeriod, timeEndPeriod, timeSetEvent, timeKillEvent, waveOutOpen, waveOutClose, waveOutPrepareHeader, waveOutUnprepareHeader, waveOutWrite, waveOutReset, mciSendStringW（通过 LoadLibrary 动态加载）
- `gdiplus` — GdiplusStartup, GdipCreateBitmapFromStream, GdipBitmapLockBits 等（通过 LoadLibrary 动态加载，首次调用 `LoadSprite` 时懒加载）
- `ole32` — CreateStreamOnHGlobal（通过 LoadLibrary 动态加载，随 gdiplus 一起加载）
- `kernel32` — QueryPerformanceCounter, QueryPerformanceFrequency, Sleep, CreateEventA, WaitForSingleObject, CloseHandle（系统默认提供）
- `user32` — 窗口管理（MSVC 自动链接，GCC 需要 `-mwindows`）

> 编译时只需 `-mwindows`，不再需要 `-lgdi32 -lwinmm -lgdiplus -lole32`。

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
COLOR_RGB(r, g, b)       // 从 R/G/B 构造不透明颜色（每分量 & 0xFF 防溢出）
COLOR_ARGB(a, r, g, b)   // 从 A/R/G/B 构造颜色（每分量 & 0xFF 防溢出）
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
| `SPRITE_ALPHA` | 8 | Alpha 混合模式 |
| `COLORKEY_DEFAULT` | `0xFFFF00FF` | 默认透明色：品红 (255,0,255)，可用 `#ifndef` 覆盖 |

---

## 5. 数据结构

### 5.1 GameSprite

```cpp
struct GameSprite {
    int width;          // 精灵宽度（像素）
    int height;         // 精灵高度（像素）
    uint32_t *pixels;   // ARGB 像素数组 (malloc 分配)
    uint32_t colorKey; // 该精灵自己的 Color Key（默认 COLORKEY_DEFAULT）
    bool used;          // 槽位是否被使用
};
```

### 5.2 GameTilemap

```cpp
struct GameTilemap {
    int cols, rows;     // 地图网格大小（列数 × 行数）
    int tileSize;       // 瓦片边长（像素）
    int tilesetId;      // tileset 精灵 ID
    int tilesetCols;    // tileset 每行有多少个瓦片（自动计算）
    int *tiles;         // cols * rows 的瓦片 ID 数组（-1 = 空，malloc 分配）
    bool used;          // 槽位是否被使用
};
```

### 5.3 GameLib 私有成员

```cpp
// 窗口状态
HWND _hwnd;
bool _closing;          // 窗口是否关闭
bool _active;           // 窗口是否激活
bool _showFps;          // 是否在标题栏显示 FPS
bool _mouseVisible;     // 是否显示窗口客户区内的鼠标光标
std::string _title;
int _width, _height;    // 固定 framebuffer 逻辑尺寸
int _windowWidth, _windowHeight; // 当前窗口客户区尺寸
int _clipX, _clipY;     // 当前有效裁剪矩形左上角（已与屏幕求交）
int _clipW, _clipH;     // 当前有效裁剪矩形尺寸；<=0 表示无可见区域
bool _resizable;        // Open() 是否允许用户拖拽缩放 / 最大化

// Aspect Lock
bool _aspectLock;       // 是否锁定长宽比
uint32_t _aspectColor;  // 黑边填充颜色
int _aspectOffsetX;     // 缩放内容在 present buffer 中的水平偏移
int _aspectOffsetY;     // 缩放内容在 present buffer 中的垂直偏移
int _aspectContentW;    // 缩放内容的实际宽度
int _aspectContentH;    // 缩放内容的实际高度

// 帧缓冲
uint32_t *_framebuffer; // width * height 的 ARGB 数组，由 DIB Section 管理
uint32_t *_presentBuffer; // 当前窗口尺寸的缩放后临时显示缓冲
int *_presentMapX, *_presentMapY; // 预计算的横/纵缩放映射

// DIB Section（用于 GDI 文字渲染 + BitBlt 刷新）
HDC _memDC;             // 常备内存 DC
HBITMAP _dibSection;    // DIB Section 位图
HBITMAP _oldBmp;        // 旧位图（用于恢复）

// DIB 信息（用于 CreateDIBSection，兼容 SetDIBitsToDevice 数据布局）
unsigned char _bmi_data[sizeof(BITMAPINFO) + 16 * sizeof(RGBQUAD)];

// 输入状态
int _keys[512];         // 当前帧按键状态
int _keys_prev[512];    // 上一帧按键状态（用于边沿检测）
int _mouseX, _mouseY;   // 鼠标坐标
int _mouseButtons[3];   // 鼠标三键状态
int _mouseButtons_prev[3]; // 上一帧鼠标状态（用于边沿检测）
int _mouseWheelDelta;   // 自上次 Update() 以来累计的滚轮增量
uint32_t _uiActiveId;   // 当前由鼠标左键按住的 UI 控件 ID（Button / Checkbox / RadioBox / ToggleButton）

// 时间
uint64_t _timeStartCounter; // Open() 时的高精度计数器值
uint64_t _timePrevCounter;  // 上一帧的高精度计数器值
uint64_t _fpsTimeCounter;   // FPS 统计时间窗口起点（高精度计数器）
uint64_t _frameStartCounter; // WaitFrame 使用的绝对帧起点/节拍基准
uint64_t _perfFrequency;    // QueryPerformanceFrequency() 返回的计数频率
double _deltaTime;      // 帧间隔（秒）
double _fps;            // 当前 FPS
double _fpsAccum;       // FPS 计数累加器
HANDLE _timerEvent;     // 多媒体定时器触发的事件对象
UINT _timerId;          // 多媒体定时器 ID（来自 timeSetEvent）
bool _timerPeriodActive; // 是否已成功调用 timeBeginPeriod(1)

// 精灵存储
std::vector<GameSprite> _sprites;

// Tilemap 存储
std::vector<GameTilemap> _tilemaps;

// 音乐状态（MCI）
bool _musicPlaying;     // 是否正在播放 MCI 音乐
bool _musicLoop;        // 当前音乐是否应循环
bool _musicIsMidi;      // 当前音乐是否走 MIDI notify 重播路径
std::wstring _musicAlias; // 当前实例使用的 MCI alias

// 音频混音器状态
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
    int id;             // 通道 ID（由 _AllocateChannel 分配）
    _WavData *wav;
    int volume;
    int repeat;         // 剩余播放次数；<=0 = 无限循环，1 = 最后一次，>1 = 递减
    uint32_t position;
    bool is_playing;
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
static const int _AUDIO_BUFFER_BYTES = _AUDIO_BUFFER_TOTAL * sizeof(int32_t);
int32_t _mix_buffer[_AUDIO_BUFFER_TOTAL];

// 场景状态
int _scene;              // 当前场景编号
int _pendingScene;       // 待切换场景编号
bool _hasPendingScene;   // 是否有待处理的场景切换
bool _sceneChanged;      // 本帧是否刚切换了场景
int _previousScene;      // 切换前的场景编号

// 随机数
static bool _srandDone; // srand 是否已初始化
```

---

## 6. 公开 API

### 6.1 窗口与主循环

#### `GameLib()` (构造函数)
- 初始化所有成员为零/默认值
- 调用 `srand(time(NULL))` 初始化随机数（只执行一次，静态标志控制）
- **加载核心 API**：调用 `_gamelib_load_apis()` 动态加载 `gdi32.dll` 和 `winmm.dll` 的所有函数指针
- 若加载失败，弹出 `MessageBoxA` 错误对话框并调用 `exit(1)` 终止程序
- 这意味着 API 加载在构造时即完成，后续所有方法无需检查函数指针是否为 NULL

#### `int Open(int width, int height, const char *title, bool center = false, bool resizable = false)`
- 创建窗口并初始化帧缓冲（通过 DIB Section）、输入、时间系统
- 若当前对象之前已经 `Open()` 过，会先清理旧窗口、DIB、timer/event 等状态，并丢弃消息队列里残留的 `WM_QUIT`，因此支持 restart-safe 重开
- **尺寸验证**：`width/height` 必须在 `1~16384` 范围内，否则返回 -7
- `width/height` 表示固定 framebuffer 逻辑尺寸；打开后不会因窗口缩放而改变
- **保证初始客户区严格等于 width × height**：先用 `AdjustWindowRect` 计算，创建后用 `GetClientRect` 二次校正
- `center=true` 时窗口居中显示；否则使用 `CW_USEDEFAULT`
- `resizable=false` 时窗口样式为 `WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX`
- `resizable=true` 时额外启用 `WS_THICKFRAME | WS_MAXIMIZEBOX`，允许用户拖拽缩放和最大化
- 窗口标题支持 UTF-8（内部转 WideChar）
- 返回值: 0=成功, -1=窗口类注册失败, -2=创建 DC 失败, -3=创建 DIB Section 失败, -4=SelectObject 失败, -5=UTF-8 转换失败, -6=创建窗口失败, -7=尺寸超限
- 使用 `GWLP_USERDATA` 存储 this 指针
- 使用 `QueryPerformanceFrequency()` / `QueryPerformanceCounter()` 建立高精度时间基准
- 调用 `timeBeginPeriod(1)` 改善等待粒度；若成功，会在关闭/析构时匹配调用 `timeEndPeriod(1)`
- 尝试创建 1ms 周期的多媒体定时器事件（`CreateEventA + timeSetEvent`），供 `WaitFrame()` 更稳定地等待下一帧；若失败则自动回退到 `Sleep(1)`
- **线程模型约束**：当前 Win32 主线按线程维度假设“一个 GameLib 窗口 + 一个 GameLib 主循环”。同一线程里不要同时创建多个 `GameLib` 窗口，也不要把 `GameLib::Update()` 的消息循环和宿主自己的独立消息循环并行驱动；当前实现会直接消费该线程的消息队列，并在 `WM_DESTROY` 时投递 `WM_QUIT`

#### `void WinResize(int width, int height)`
- 设置窗口客户区尺寸，`width/height` 始终表示客户区，不改变 framebuffer 尺寸
- 对不可缩放窗口同样有效；若当前是最大化的可缩放窗口，会先还原再设置尺寸

#### `void SetMaximized(bool maximized)`
- 仅在 `Open(..., ..., ..., ..., true)` 创建的可缩放窗口上有效
- `true` 时最大化，`false` 时还原普通窗口

#### `bool IsClosed() const`
- 窗口是否已关闭（WM_CLOSE 或 WM_DESTROY 触发）

#### `void Update()`
1. 保存上一帧按键状态到 `_keys_prev`，鼠标状态到 `_mouseButtons_prev`，并将 `_mouseWheelDelta` 清零
2. 派发 Windows 消息（PeekMessage 循环），同步当前客户区尺寸和输入状态
3. 若客户区尺寸与 framebuffer 相同，则直接 `BitBlt`；否则按预计算映射做最近邻软件缩放，把结果写入 `present buffer` 后再 `SetDIBitsToDevice` 刷新到窗口客户区。当 `AspectLock` 启用时，缩放保持 framebuffer 长宽比不变，居中显示内容，黑边区域（上下或左右）用 `_aspectColor` 填充
4. 使用 `QueryPerformanceCounter()` 更新 deltaTime 和 FPS（内部使用 `double` 计算，FPS 每秒统计一次）
5. FPS 更新时调用 `_UpdateTitleFps()` 更新标题栏显示

#### `void WaitFrame(int fps)`
- 帧率控制，基于绝对帧边界做节拍：用 `QueryPerformanceCounter()` 维护 `_frameStartCounter`，避免把本帧工作耗时重复算进等待时间
- 等待阶段先用多媒体定时器事件或 `Sleep(1)` 做粗等待，再在最后很短的一段时间里用 `Sleep(0)` / 短轮询收尾，减少 oversleep
- fps <= 0 时默认按 60 处理

#### `double GetDeltaTime() const`
- 上一帧到当前帧的时间间隔（秒）

#### `double GetFPS() const`
- 当前帧率（每秒更新一次）

#### `double GetTime() const`
- 从 `Open()` 开始到现在的总时间（秒）
- 在 `Open()` 前调用时安全返回 `0.0`

#### `int GetWidth() const` / `int GetHeight() const`
- 固定 framebuffer 逻辑宽度/高度（即 `Open()` 时传入的 `width`/`height`）
- 在 `resizable=true` 的可缩放窗口中，此值不随窗口拖拽而改变

#### `uint32_t *GetFramebuffer()`
- 返回 framebuffer 指针，可直接读写像素
- 像素寻址：`fb[y * GetWidth() + x]`
- 在 `Open()` 前或窗口关闭后返回 `NULL`

#### `void SetTitle(const char *title)`
- 修改窗口标题（支持 UTF-8）

#### `void ShowFps(bool show)`
- 设置是否在窗口标题栏显示实时 FPS
- `show=true` 时，标题栏显示为 `"原标题 (FPS: 58.8)"` 格式，每秒更新一次（与 FPS 统计周期同步）
- `show=false` 时，恢复为原始标题（调用 `SetTitle` 恢复）
- 适用于开发调试，方便观察帧率

#### `void ShowMouse(bool show)`
- 控制窗口客户区内的鼠标光标显示/隐藏
- Win32 主线通过 `WM_SETCURSOR` + `_mouseVisible` 保持客户区内光标状态，不依赖全局 `ShowCursor` 引用计数
- 在窗口创建前调用也有效：状态会在下一次 `Open()` 时自动应用

#### `void AspectLock(bool lock, uint32_t color = COLOR_BLACK)`
- 启用或禁用长宽比锁定模式。默认不锁定
- 锁定后，当窗口客户区尺寸与 framebuffer 不同时，缩放会保持 `_width:_height` 的长宽比不变，居中显示内容
- 多余区域（上下或左右的黑边）用 `color` 填充
- `PresentMapX/Y` 中黑边区域的映射值设为 `-1`；内容区域的映射值基于缩放后的内容尺寸计算
- 渲染优化：y 方向黑边行通过 `PresentMapY[y] < 0` 快速跳过（memcpy 模板行）；x 方向每行分三段处理（左黑边、缩放内容、右黑边）
- 鼠标坐标 `_mouseX/_mouseY` 会映射到内容区域，黑边区域的坐标 clamp 到 framebuffer 边缘
- 调用 `AspectLock` 时会强制 present buffer 重建（设置 `_presentWidth = 0`）

#### `int ShowMessage(const char *text, const char *title = NULL, int buttons = MESSAGEBOX_OK)`
- 弹出消息框，当前支持 `MESSAGEBOX_OK` 和 `MESSAGEBOX_YESNO` 两种按钮布局
- `title == NULL` 或空字符串时，优先使用当前窗口标题；若标题为空则回退到 `"GameLib"`
- 返回 `MESSAGEBOX_RESULT_OK`、`MESSAGEBOX_RESULT_YES` 或 `MESSAGEBOX_RESULT_NO`
- 文本和标题按 UTF-8 解释，内部转换为宽字符后调用 `MessageBoxW`

### 6.2 帧缓冲操作

#### `void Clear(uint32_t color = COLOR_BLACK)`
- 用指定颜色填充当前裁剪矩形覆盖到的帧缓冲区域
- 不做 Alpha 混合；即使 `color` 含透明通道，也会直接写入目标像素

#### `void SetPixel(int x, int y, uint32_t color)`
- 设置指定像素颜色（带裁剪和边界检查）
- 当 `color` 的 Alpha 小于 255 时，按 source-over 规则与当前帧缓冲做混合

#### `uint32_t GetPixel(int x, int y) const`
- 获取指定像素颜色（越界返回 0）

#### `void SetClip(int x, int y, int w, int h)`
- 设置当前裁剪矩形
- 传入矩形会自动与当前屏幕 `[0, width) × [0, height)` 求交
- `w <= 0 || h <= 0`，或求交后为空时，当前实例进入“无可见区域”状态，后续所有写入屏幕帧缓冲的绘制函数直接 no-op
- `Open()` 成功后默认裁剪矩形是整屏；再次调用 `Open()` 会重置为新的整屏裁剪

#### `void ClearClip()`
- 清除当前裁剪，恢复整屏可见

#### `void GetClip(int *x, int *y, int *w, int *h) const`
- 读取当前有效裁剪矩形
- 返回值已经是与屏幕求交后的结果，而不是用户原始传入值

#### `int GetClipX() const` / `int GetClipY() const` / `int GetClipW() const` / `int GetClipH() const`
- 读取当前有效裁剪矩形的各个分量

#### `void Screenshot(const char *filename)`
- 将当前 framebuffer 内容保存为 24-bit BMP 文件
- `filename` 按 UTF-8 传递，通过 `_gamelib_fopen_utf8` 写入
- 像素格式：从 ARGB32 framebuffer 中提取 R/G/B 三通道，丢弃 Alpha，按 BGR 顺序写入 BMP（BMP 标准 24-bit 像素布局）
- 行顺序：framebuffer 是 top-down（DIB biHeight 为负值），BMP 标准是 bottom-up，因此逐行从最后一行开始写入
- 每行按 4 字节对齐（row padding），填充字节置零
- `_framebuffer` 为 NULL 或窗口未初始化时直接返回，不创建文件

### 6.3 图形绘制

说明：本节所有图元 API 都接受 `COLOR_ARGB(a, r, g, b)` 颜色；当 `a < 255` 时，会与现有帧缓冲内容做 Alpha 混合。所有图元、文字、精灵与 Tilemap 绘制都会受当前 Clip Rectangle 约束。

#### `void DrawLine(int x1, int y1, int x2, int y2, uint32_t color)`
- 先与当前裁剪矩形做线段裁剪，再进入 **Bresenham 直线算法** 逐点绘制
- 避免整条线大部分落在裁剪区域外时仍逐像素做无效 `SetPixel` 检查

#### `void DrawRect(int x, int y, int w, int h, uint32_t color)`
- 绘制矩形边框（顶部和底部用 `_DrawHLine`，左右垂直边逐像素 `SetPixel`）
- `w <= 0 || h <= 0` 时直接返回

#### `void FillRect(int x, int y, int w, int h, uint32_t color)`
- 填充矩形（带裁剪，直接写帧缓冲；`_framebuffer` 为 NULL 时直接返回）
- 不透明路径（alpha==255）改为首行逐像素填充 + `memcpy` 复制后续行；半透明路径仍逐像素混合

#### `void DrawCircle(int cx, int cy, int r, uint32_t color)`
- **中点圆算法**，按唯一对称点输出轮廓
- 对 `x == 0`、`x == y` 等退化对称点会去重，避免半透明颜色在边界点被重复混合

#### `void FillCircle(int cx, int cy, int r, uint32_t color)`
- 复用椭圆扫描线填充路径（`rx == ry`），每个 y 只写一条水平线
- 这样半透明颜色不会因为重复覆盖同一扫描线而出现额外加深

#### `void DrawEllipse(int cx, int cy, int rx, int ry, uint32_t color)`
- 绘制椭圆边框，参数分别为中心点和横/纵半径
- 使用 midpoint ellipse 风格的分区迭代，并按唯一对称点输出轮廓
- 避免旧的双向采样路径在半透明颜色下对部分边界点重复落笔
- 退化情况（半径为 0）会自动回退为点或直线

#### `void FillEllipse(int cx, int cy, int rx, int ry, uint32_t color)`
- 按扫描线方式填充椭圆，每行通过 `_DrawHLine` 写入
- 同样支持退化为点或直线的情况

#### `void DrawTriangle(int x1, int y1, int x2, int y2, int x3, int y3, uint32_t color)`
- 绘制三角形边框（3 条 DrawLine）

#### `void FillTriangle(int x1, int y1, int x2, int y2, int x3, int y3, uint32_t color)`
- **扫描线填充算法**
- 先按 y 排序三个顶点，然后逐行扫描
- 处理了退化情况（三点共线/水平线）
- 边插值使用 `int64_t` 强制转换防止乘法溢出

### 6.4 文字渲染

#### `void DrawText(int x, int y, const char *text, uint32_t color)`
- 使用内嵌 8x8 位图字体（ASCII 32~126, 共 95 个字符）
- 支持 `\n` 换行（行间距 = 8 + 2 = 10 像素）
- 每个字符宽度 8 像素
- 空行（bits==0）整体跳过，不逐像素检查

#### `void DrawNumber(int x, int y, int number, uint32_t color)`
- 将整数转为字符串后调用 DrawText（内部使用 `snprintf` 防溢出）

#### `void DrawTextScale(int x, int y, const char *text, uint32_t color, int w, int h)`
- 缩放版文字绘制，每个字符按指定宽高渲染，内置 8×8 位图字体通过定点采样映射到 w × h 区域
- w 和 h 可以不同，实现非等比缩放；旧版 `scale` 等价于 `w = 8 × scale, h = 8 × scale`
- w、h 最大值 1024，超出直接返回
- `w==8 && h==8` 时直接走 `DrawText` 快路径，不经过查找表
- 内部使用预计算查找表（`_textSrcRowMap`/`_textSrcColMap`）替代逐像素除法；alpha==255 时直写 framebuffer，alpha<255 时走 `_gamelib_blend_pixel`；空行（bits==0）整体跳过
- 支持 `\n` 换行

#### `void DrawPrintf(int x, int y, uint32_t color, const char *fmt, ...)`
- 格式化输出，类似 `printf`，支持 `%d`, `%s`, `%f` 等格式符
- 内部使用 `vsnprintf`（1024 字节缓冲），格式化后调用 `DrawText` 绘制
- 方便在屏幕上显示变量值、分数、调试信息等

#### `void DrawPrintfScale(int x, int y, uint32_t color, int w, int h, const char *fmt, ...)`
- 缩放版格式化输出，格式化后调用 `DrawTextScale` 绘制
- 每个字符按 w × h 渲染，适合缩放显示分数、标题等

### 6.5 字体文字渲染（当前 Windows 后端用 GDI 实现）

#### `void DrawTextFont(int x, int y, const char *text, uint32_t color, const char *fontName, int fontSize)`
- 使用可缩放字体渲染文字；当前 Windows 版本内部用 GDI 实现
- `fontName`: 字体名称，如 "Microsoft YaHei"、"SimHei"、"Arial" 等
- `fontSize`: 字体大小（像素）
- 文字背景透明，支持 UTF-8 编码
- 支持 `\n` 多行输出
- 当 `color` 的 alpha 为 0 时直接不绘制；当 `0 < alpha < 255` 时，先由 GDI 生成字形，再按调用方 alpha 回混到 `_framebuffer`，保持与其他 ARGB 绘制 API 一致的透明语义
- 即使内部使用 GDI 参与光栅化，最终仍会恢复裁剪矩形外的像素，保证与其他绘制 API 一致的裁剪语义

#### `void DrawTextFont(int x, int y, const char *text, uint32_t color, int fontSize)`
- 简化版本，使用默认字体 `GAMELIB_DEFAULT_FONT_NAME`
- 适合快速输出中文文字

#### `void DrawPrintfFont(int x, int y, uint32_t color, const char *fontName, int fontSize, const char *fmt, ...)`
- 字体版格式化输出，内部用 `vsnprintf` 组装文本后调用 `DrawTextFont`
- 适合分数、FPS、调试变量等需要格式化的字体文字输出

#### `void DrawPrintfFont(int x, int y, uint32_t color, int fontSize, const char *fmt, ...)`
- 使用默认字体 `GAMELIB_DEFAULT_FONT_NAME` 的简化格式化输出版本

#### `int GetTextWidthFont(const char *text, const char *fontName, int fontSize)`
- 获取文字在指定字体下的宽度（像素）
- 用于计算文字对齐、布局等

#### `int GetTextHeightFont(const char *text, const char *fontName, int fontSize)`
- 获取文字在指定字体下的高度（像素）
- 支持多行文本高度计算

#### `int GetTextWidthFont(const char *text, int fontSize)` / `int GetTextHeightFont(const char *text, int fontSize)`
- 使用默认字体的简化测量版本

**注意**: 字体文字渲染当前在 Windows 后端使用 GDI 抗锯齿，效果比内嵌 8x8 字体更平滑，但性能略低。对于大量文字或高性能需求场景，建议使用内嵌字体。

### 6.6 精灵系统

#### `int CreateSprite(int width, int height)`
- 创建空白精灵，返回整数 ID（失败返回 -1）
- 像素初始化为全 0（透明黑）
- 尺寸限制：`1~16384`，使用 `(size_t)` 强转防止 malloc 溢出

#### `int LoadSpriteBMP(const char *filename)`
- 从 BMP 文件加载精灵，支持 **8-bit（调色板）、24-bit 和 32-bit** BMP
- `filename` 按 **UTF-8** 解释，内部转为宽字符路径打开文件
- **8-bit 调色板支持**：自动读取 BMP 调色板（最多 256 色），每个像素字节作为调色板索引，转换为 32-bit ARGB（alpha 默认 0xFF）
- 处理 bottom-up / top-down 行序
- 每行按 4 字节对齐读取
- 尺寸限制仍为 `1~16384`；超出限制的图片会直接拒绝载入
- 24-bit BMP alpha 默认为 0xFF
- **安全性**：使用 `memcpy` 读取 BMP 头字段（避免严格别名/对齐问题），尺寸限制 `1~16384`
- **失败原子性**：如果文件截断、短读或格式非法，已经申请的 sprite 槽位会回滚，不会留下半初始化精灵
- 返回精灵 ID（失败返回 -1）

#### `int LoadSprite(const char *filename)`
- **通用图片加载**，通过动态加载 GDI+ 支持 **PNG、JPG、BMP、GIF、TIFF** 等格式
- 首次调用时懒加载 `gdiplus.dll` 和 `ole32.dll`，调用 `GdiplusStartup` 初始化
- `filename` 按 **UTF-8** 解释，内部转为宽字符路径读取文件
- 将文件读入内存，通过 `CreateStreamOnHGlobal` 创建 COM IStream，再由 `GdipCreateBitmapFromStream` 解码
- 始终请求 **PixelFormat32bppARGB** 格式输出，保证像素格式统一
- **24 位图片 Alpha 修正**：若 GDI+ 解码后所有像素的 alpha 均为 0（24 位无 alpha 通道的图片），自动将 alpha 设为 255（不透明）
- **BMP 回退机制**：若 GDI+ 初始化失败（如系统无 gdiplus.dll），检测文件头魔数（`"BM"`），若为 BMP 文件则自动回退到 `LoadSpriteBMP`
- 返回精灵 ID（失败返回 -1）

#### `void FreeSprite(int id)`
- 释放精灵内存，标记槽位为未使用

#### `void DrawSprite(int id, int x, int y)`
- 绘制精灵到帧缓冲（委托给 `DrawSpriteEx(id, x, y, 0)`）
- 默认调用 `DrawSpriteEx(id, x, y, 0)`；在最常见的无翻转路径下会直接按行拷贝像素，不检查 Color Key，也不做逐像素 Alpha 混合
- 如果素材依赖透明孔洞，请改用 `DrawSpriteEx(..., SPRITE_ALPHA)` 或 `DrawSpriteEx(..., SPRITE_COLORKEY)`

#### `void DrawSpriteEx(int id, int x, int y, int flags)`
- 带标志的精灵绘制
- `SPRITE_FLIP_H`: 水平翻转
- `SPRITE_FLIP_V`: 垂直翻转
- `SPRITE_COLORKEY`: 与该精灵当前 Color Key 完全匹配的像素跳过（默认值为 `COLORKEY_DEFAULT`，即品红 `0xFFFF00FF`）
- `SPRITE_ALPHA`: 启用逐像素 Alpha 混合（alpha=0 跳过，alpha=255 直接覆盖，0<alpha<255 按比例混合）
- 标志可组合使用，如 `SPRITE_FLIP_H | SPRITE_ALPHA`
- 未启用 `SPRITE_ALPHA` / `SPRITE_COLORKEY` 时，会按 flags 允许的最快路径处理；无翻转时优先逐行 `memcpy`，带翻转时仍走逐像素路径，但会直接覆盖目标像素，不检查源像素 alpha 是否为 0
- 整张绘制和区域绘制走非缩放快路径；缩放绘制走独立最近邻采样路径。若未启用 `SPRITE_ALPHA`，缩放路径同样不会因为源像素 alpha 为 0 而跳过，而是直接覆盖目标像素；只有显式传 `SPRITE_ALPHA` 时才按源 alpha 混合
- 所有 `DrawSprite*` 路径都会先与当前裁剪矩形求交；即使是逐行 `memcpy` 的快路径也不会写出裁剪区域

#### `void DrawSpriteRegion(int id, int x, int y, int sx, int sy, int sw, int sh)`
- 绘制精灵的子区域（sprite sheet 切图）
- 等价于 `DrawSpriteRegionEx(..., 0)`

#### `void DrawSpriteRegionEx(int id, int x, int y, int sx, int sy, int sw, int sh, int flags = 0)`
- 带 flags 绘制精灵子区域
- 翻转、ColorKey、Alpha 语义与 `DrawSpriteEx` 完全一致

#### `void DrawSpriteScaled(int id, int x, int y, int w, int h, int flags = 0)`
- 将整张精灵按目标尺寸缩放绘制
- 当前实现使用最近邻采样，适合像素风和教学场景

#### `void DrawSpriteRotated(int id, int cx, int cy, double angleDeg, int flags = 0)`
- 将整张精灵绕自身中心旋转后绘制，中心点落在 `(cx, cy)`
- `angleDeg > 0` 表示按屏幕坐标系顺时针旋转
- 当前实现使用独立的最近邻旋转路径，不复用缩放 helper，因此不会影响既有缩放行为
- `SPRITE_FLIP_H` / `SPRITE_FLIP_V` / `SPRITE_COLORKEY` / `SPRITE_ALPHA` 语义与其他 `DrawSprite*` 接口保持一致

#### `void DrawSpriteFrame(int id, int x, int y, int frameW, int frameH, int frameIndex, int flags = 0)`
- 按从左到右、从上到下的顺序绘制 sprite sheet 中的某一帧
- 每行帧数由 `spriteWidth / frameW` 自动推导

#### `void DrawSpriteFrameScaled(int id, int x, int y, int frameW, int frameH, int frameIndex, int w, int h, int flags = 0)`
- 先按帧号选取 sprite sheet 子区域，再按目标尺寸缩放绘制
- 适合角色动画、头像预览、物品图标放大等常见场景

#### `void DrawSpriteFrameRotated(int id, int cx, int cy, int frameW, int frameH, int frameIndex, double angleDeg, int flags = 0)`
- 先按帧号选取 sprite sheet 子区域，再绕该帧中心旋转绘制
- 帧号顺序、越界判断和 flags 语义与 `DrawSpriteFrame` 保持一致

#### `void SetSpritePixel(int id, int x, int y, uint32_t color)`
- 修改精灵指定像素

#### `uint32_t GetSpritePixel(int id, int x, int y) const`
- 读取精灵指定像素

#### `int GetSpriteWidth(int id) const` / `int GetSpriteHeight(int id) const`
- 获取精灵尺寸

#### `void SetSpriteColorKey(int id, uint32_t color)` / `uint32_t GetSpriteColorKey(int id) const`
- 设置或读取该精灵自己的 Color Key
- 配合 `SPRITE_COLORKEY` 使用时，不再强依赖全局 `COLORKEY_DEFAULT`

### 6.7 输入系统

#### `bool IsKeyDown(int key) const`
- 按键是否正在按下（持续按住也返回 true）

#### `bool IsKeyPressed(int key) const`
- 按键是否刚刚按下（**边沿检测**：当前帧按下且上一帧未按下）

#### `bool IsKeyReleased(int key) const`
- 按键是否刚刚松开（**边沿检测**：当前帧未按下且上一帧按下）

#### `int GetMouseX() const` / `int GetMouseY() const`
- 返回按当前窗口横纵缩放比例反算后的 framebuffer 逻辑坐标
- 当窗口客户区与 framebuffer 同尺寸时，等价于普通客户区像素坐标
- 启用 `AspectLock` 后，坐标映射基于内容区域而非整个窗口；黑边区域的鼠标坐标 clamp 到 framebuffer 边缘

#### `bool IsMouseDown(int button) const`
- 鼠标按键是否按下

#### `bool IsMousePressed(int button) const`
- 鼠标按键是否刚刚按下（**边沿检测**：当前帧按下且上一帧未按下）

#### `bool IsMouseReleased(int button) const`
- 鼠标按键是否刚刚松开（**边沿检测**：当前帧未按下且上一帧按下）

#### `int GetMouseWheelDelta() const`
- 返回自上次 `Update()` 以来累计的滚轮增量
- Windows 标准滚轮一格通常为 `120` 或 `-120`
- 读取本函数不会清零；在下一次 `Update()` 开始时刷新为 0

#### `bool IsActive() const`
- 返回窗口当前是否处于激活状态
- 适合在游戏失焦时暂停输入、停止绘制交互或显示暂停提示

### 6.8 声音

#### `int PlayBeep(int frequency, int duration, int repeat = 1, int volume = 1000)`
- 异步蜂鸣，内部生成正弦波 PCM 通过 `PlayPCM` 播放
- `frequency`: 频率（Hz），`duration`: 持续时间（毫秒）
- `repeat`: 播放次数，≤0 表示无限循环，默认 1（单次播放），>1 时每次播放完递减
- `volume`: 通道音量，范围 0~1000，默认 1000（满音量）
- 成功返回通道 ID（正整数），失败返回 -1（参数错误）或 -2（音频设备初始化失败）
- 音频设备惰性初始化：首次调用时才创建 waveOut 设备

#### `int PlayWAV(const char *filename, int repeat = 1, int volume = 1000)`
- 使用 waveOut 软件混音器播放 WAV 音效（异步，多通道）
- `filename` 按 **UTF-8** 解释，内部转为宽字符路径
- `repeat` 为播放次数，≤0 表示无限循环，默认 1（单次播放），>1 时每次播放完递减
- `volume` 为通道音量，范围 0~1000，默认 1000（满音量）
- 每次调用分配独立通道，同一 WAV 文件可重叠播放
- 成功返回通道 ID（正整数），失败返回 -1（文件错误）或 -2（音频设备初始化失败）
- 音频设备惰性初始化：首次调用时才创建 waveOut 设备

#### `int StopWAV(int channel)`
- 停止指定通道的 WAV 播放
- `channel` 为 `PlayWAV` 返回的通道 ID
- 成功返回 0，无效通道返回 -1

#### `int IsPlaying(int channel)`
- 查询指定通道是否仍在播放
- 正在播放返回 1，已停止或无效通道返回 0

#### `int SetVolume(int channel, int volume)`
- 设置指定通道音量
- `volume` 范围 0~1000
- 成功返回 0，无效通道返回 -1

#### `void StopAll()`
- 停止所有通道的音效播放并清除缓存

#### `int SetMasterVolume(int volume)`
- 设置主音量
- `volume` 范围 0~1000，自动钳制到有效范围
- 始终返回 0

#### `int GetMasterVolume() const`
- 返回当前主音量值（0~1000）

#### `bool PlayMusic(const char *filename, bool loop = true)`
- 使用 **MCI (Media Control Interface)** 播放背景音乐
- 支持 `.mp3`、`.mid/.midi`、`.wav`
- 默认循环播放（`loop=true`）
- 按扩展名显式选择 MCI 设备类型：`.mp3` -> `mpegvideo`，`.mid/.midi` -> `sequencer`，`.wav` -> `waveaudio`
- MIDI 不使用 `repeat` 命令；`sequencer` 设备改为 `notify` 完成回调后从头重播，避免 `play ... repeat` 在部分系统上失败
- 不做自动检测 fallback；其他扩展名直接返回 `false`
- 每个 `GameLib` 对象使用自己的 MCI alias；同一个对象同一时刻只能播放一首背景音乐
- 调用时会自动停止之前的音乐
- `filename` 按 **UTF-8** 解释，内部转为宽字符路径
- **安全性**：`filename` 为 NULL 时直接返回 `false`；拒绝包含引号和换行的文件名（防止 MCI 命令注入）
- **与音效通道独立**，可同时播放背景音乐和多通道音效
- 成功启动返回 `true`，失败返回 `false`

#### `void StopMusic()`
- 停止当前 MCI 背景音乐并释放资源

#### `bool IsMusicPlaying() const`
- 返回库当前记录的背景音乐播放状态
- 适合 UI 状态同步和避免示例代码手工维护布尔变量

### 6.9 工具函数（static）

#### `static int Random(int minVal, int maxVal)`
- 返回 [minVal, maxVal] 范围内的随机整数
- 内部使用 `rand()`，构造函数中自动 `srand(time(NULL))`

#### `static bool RectOverlap(int x1, int y1, int w1, int h1, int x2, int y2, int w2, int h2)`
- AABB 矩形碰撞检测

#### `static bool CircleOverlap(int cx1, int cy1, int r1, int cx2, int cy2, int r2)`
- 圆形碰撞检测（距离平方比较，无浮点开方；使用 `int64_t` 防溢出）

#### `static bool PointInRect(int px, int py, int x, int y, int w, int h)`
- 点是否在矩形内

#### `static float Distance(int x1, int y1, int x2, int y2)`
- 两点距离（浮点，使用 sqrtf）

### 6.10 网格辅助

#### `void DrawGrid(int x, int y, int rows, int cols, int cellSize, uint32_t color)`
- 绘制网格线

#### `void FillCell(int gridX, int gridY, int row, int col, int cellSize, uint32_t color)`
- 填充网格中的一个单元格（留 1 像素内边距避免覆盖网格线）

### 6.11 Tilemap 系统

#### `int CreateTilemap(int cols, int rows, int tileSize, int tilesetId)`
- 创建瓦片地图，返回整数 ID（失败返回 -1）
- `cols × rows`：地图网格大小（均不超过 4096，使用 `(size_t)` 强转防止 malloc 溢出）
- `tileSize`：瓦片边长（像素），tileset 精灵按此尺寸切分为格子
- `tilesetId`：通过 `LoadSprite` 或 `CreateSprite` 加载的精灵 ID，作为瓦片图集
- tileset 中瓦片编号从 0 开始，从左到右、从上到下排列
- 自动计算并缓存 `tilesetCols = spriteWidth / tileSize`，并确认当前 tileset 至少能切出 1 块完整瓦片
- 若 tileset 在当前 `tileSize` 下切不出任何完整瓦片，则创建失败
- 所有格子初始化为 -1（空，不绘制）

#### `bool SaveTilemap(const char *filename, int mapId) const`
- 将地图保存为纯文本 `.glm` 文件，成功返回 `true`
- 第一行固定写 `GLM1`
- 第二行写 `tileSize rows cols`
- 后续每行写一整行瓦片 ID，列之间用单个空格分隔
- 只保存地图尺寸和瓦片数据，不保存 tileset 路径或其他资源元数据

#### `int LoadTilemap(const char *filename, int tilesetId)`
- 从纯文本 `.glm` 文件创建地图，成功返回新地图 ID，失败返回 `-1`
- 文件第一行必须是 `GLM1`（允许 UTF-8 BOM）
- 第二行按 `tileSize rows cols` 解析
- 数据区按行读取，空格或 Tab 都可作为分隔符
- 某一行超过 `cols` 的数据会忽略，不足 `cols` 的剩余格子保留为 `-1`
- 文件超过 `rows` 的多余数据行会忽略；若不足 `rows` 行，缺失行保留为 `-1`
- 不从文件中读取 tileset 信息，调用者必须显式提供 `tilesetId`
- 只接受 `-1` 或非负 `tileId`；读到小于 `-1` 的值时返回 `-1`，不会保留半成品地图
- 超出当前 tileset 可用范围的非负 `tileId` 会原样保留，后续绘制时自动跳过
- 在读到非整数 token 等非法内容时返回 `-1`，不会保留半成品地图

#### `void FreeTilemap(int mapId)`
- 释放地图的 tiles 数组内存，标记槽位为未使用
- 同时清空 `tilesetId` / `tilesetCols` 等缓存元数据
- 不释放 tileset 精灵（由用户通过 `FreeSprite` 管理）

#### `void SetTile(int mapId, int col, int row, int tileId)`
- 设置 (col, row) 处的瓦片编号
- `tileId >= 0`：对应 tileset 中的第几块瓦片
- `tileId = -1`：空格，不绘制
- 若 `tileId` 超出当前 tileset 可用范围，仍会写入；后续 `DrawTilemap()` 会按当前 tileset 尺寸自动跳过不可绘制格子
- 若 `tileId < -1`，则忽略本次写入

#### `int GetTile(int mapId, int col, int row) const`
- 获取 (col, row) 处的瓦片编号
- 越界返回 -1

#### `int GetTilemapCols(int mapId) const` / `int GetTilemapRows(int mapId) const`
- 返回地图的列数和行数
- `mapId` 无效时返回 0

#### `int GetTileSize(int mapId) const`
- 返回地图瓦片边长（像素）
- `mapId` 无效时返回 0

#### `int WorldToTileCol(int mapId, int x) const` / `int WorldToTileRow(int mapId, int y) const`
- 按地图的 `tileSize` 将像素坐标换算为瓦片坐标
- 使用向下取整，因此负坐标也能得到符合直觉的结果（如 `x=-1` 且 `tileSize=16` 时返回 `-1`）

#### `int GetTileAtPixel(int mapId, int x, int y) const`
- 先做像素到瓦片坐标换算，再返回对应瓦片编号
- 越界或空格返回 -1，适合做脚下地面检测、像素级地图碰撞查询

#### `void FillTileRect(int mapId, int col, int row, int cols, int rows, int tileId)`
- 用同一个 `tileId` 批量填充一块矩形瓦片区域
- 会自动裁剪到地图边界，适合快速生成地面、平台、房间块
- 若 `tileId` 超出当前 tileset 可用范围，仍会照常写入；绘制时自动跳过不可绘制格子
- 若 `tileId < -1`，则整次填充直接忽略

#### `void ClearTilemap(int mapId, int tileId = -1)`
- 将整张地图填充为同一个瓦片编号
- 默认值 `-1` 表示清空整张地图
- 若传入的 `tileId` 超出当前 tileset 可用范围，仍会照常写入；绘制时自动跳过不可绘制格子
- 若传入的 `tileId < -1`，则忽略本次清空请求

#### `void DrawTilemap(int mapId, int x, int y, int flags = 0)`
- 绘制瓦片地图到帧缓冲
- `(x, y)`：地图左上角在屏幕上的位置（卷轴时传 `-cameraX, -cameraY`）
- `flags`：绘制模式，与 `DrawSpriteEx` 一致：
  - `0`（默认）— 不透明快路径，不检查 tileset 像素 alpha，适合不透明瓦片图集
  - `SPRITE_COLORKEY` — 跳过 tileset 当前 Color Key 对应的像素
  - `SPRITE_ALPHA` — 逐像素 Alpha 混合
  - `SPRITE_ALPHA | SPRITE_COLORKEY` — 可组合使用
- 如果瓦片图集依赖透明孔洞，应传入 `SPRITE_COLORKEY` 或 `SPRITE_ALPHA`
- **性能优化**：只绘制当前裁剪矩形可见范围内的瓦片（自动计算可见列/行范围），不遍历整张地图
- 绘制前会按当前 tileset sprite 的实际尺寸即时计算可用瓦片总数；即使同一个 sprite 槽位被释放后重新装入更小资源，也不会访问越界像素
- `tileId < 0` 或超出当前可用瓦片总数的格子会直接跳过，不参与绘制
- 每个瓦片做像素级边缘裁剪，处理当前裁剪矩形边界上的半瓦片
- 无 `SPRITE_ALPHA` / `SPRITE_COLORKEY` 时逐行 `memcpy`；其他情况复用 `_DrawSpriteAreaFast`

#### `bool Button(int x, int y, int w, int h, const char *text, uint32_t color)`
- 立即模式按钮；默认文字始终使用内嵌 8x8 位图字体（ASCII 32~126），不走 `DrawTextFont`
- 只有“按钮内按下鼠标左键，再在按钮内松开鼠标左键”时才返回 `true`
- 视觉状态分为 `normal`、`hover`、`pressed` 三种；`pressed` 表示当前正由左键按住，事件本身仍在松开时触发
- `color` 作为按钮基色；悬停与按下的明暗变化由库内部对 RGB 分量自动提亮 / 压暗得到，并叠加经典立体像素边框
- 按下后拖出按钮区域不会触发；拖回区域内并松开仍可触发

#### `bool Checkbox(int x, int y, const char *text, bool *checked)`
- 立即模式复选框；默认文字同样使用内嵌 8x8 位图字体（ASCII 32~126）
- `checked` 必须为非空指针；函数在本帧发生勾选状态变化时返回 `true`
- 在控件区域内按下左键并在控件区域内松开时，会翻转 `*checked`
- 点击区域覆盖 16x16 方框和右侧文字标签，便于菜单和设置页直接使用
- 稳定状态分为 `checked`、`checked-hover`、`unchecked`、`unchecked-hover` 四种；按下时叠加轻微内凹反馈
- 选中态用中心实心块表示，而不是勾号

#### `bool RadioBox(int x, int y, const char *text, int *value, int index)`
- 立即模式单选框；默认文字同样使用内嵌 8x8 位图字体（ASCII 32~126）
- `value` 必须为非空指针；同一组 RadioBox 共享同一个 `value` 指针，通过 `index` 区分各自编号（0, 1, 2...）
- 当 `*value == index` 时，该项为选中态；在控件区域内松开左键时设置 `*value = index`，返回 `true` 表示发生了变更
- 点击区域覆盖 16x16 圆形区域和右侧文字标签
- 视觉外观为纯圆形（用 `FillCircle` 填面色 + 双层 `DrawCircle` 模拟 bevel 3D 效果），选中时在圆心绘制实心小圆点，而非 Checkbox 的方框+实心块
- 悬停/按下时面色提亮/压暗，bevel 双层圆环自动反转；按下时整体偏移 +1,+1 模拟内凹
- 渲染使用 `willSelect` 预判机制：在 release 帧提前计算选中状态用于绘制，避免状态翻转前的一帧视觉延迟

#### `bool ToggleButton(int x, int y, int w, int h, const char *text, bool *toggled, uint32_t color)`
- 立即模式开关按钮；默认文字始终使用内嵌 8x8 位图字体（ASCII 32~126），不走 `DrawTextFont`
- `toggled` 必须为非空指针；函数在本帧发生开关状态变化时返回 `true`
- 在按钮区域内松开左键时翻转 `*toggled`
- `*toggled == true` 时按钮持续显示凹陷外观（立体边框反转 + 面色压暗 + 文字偏移 +1,+1），与普通 Button 的按下态视觉一致，但凹陷是稳定状态而非瞬时
- `*toggled == false` 时按钮显示正常凸起外观
- 视觉状态分为 `normal`、`hover`、`toggled`、`toggled-hover` 四种；toggled+hover 时面色比 toggled 略亮
- 渲染使用 `willToggle` 预判机制：在 release 帧提前计算翻转后的状态用于绘制，避免状态翻转前的一帧 normal 闪烁

### 6.12 场景管理

#### `void SetScene(int scene)`
- 设置下一帧要切换到的场景编号
- 不会立即生效，而是在下一次 `Update()` 时处理——这样避免了同一帧内输入穿透到新场景
- 可以通过 `SetScene(GetScene())` 重启当前场景（会再次触发 `IsSceneChanged() == true`）

#### `int GetScene() const`
- 返回当前场景编号

#### `bool IsSceneChanged() const`
- 返回本帧是否刚切换到了新场景
- 新场景的第一帧返回 `true`，之后返回 `false`
- 初始场景为 `0`，程序启动后第一帧的 `IsSceneChanged()` 也是 `true`（方便在场景 0 的首帧做初始化）

#### `int GetPreviousScene() const`
- 返回切换前的场景编号
- 在首帧或未发生过切换时返回 `0`

#### 场景切换时序

场景切换使用延迟生效机制：

1. 用户调用 `SetScene(newScene)` → 记录到 `_pendingScene`
2. 下一次 `Update()` 执行时 → `_previousScene = _scene; _scene = _pendingScene; _sceneChanged = true`
3. 该帧 `IsSceneChanged()` 返回 `true`
4. 再下一次 `Update()` → `_sceneChanged = false`

场景用整数标识，推荐用 `enum` 定义（如 `enum { SCENE_MENU, SCENE_PLAY, SCENE_GAMEOVER }`）。

### 6.13 存档读写（static）

所有存档函数都是 `static` 的，可以通过 `GameLib::SaveInt(...)` 直接调用，不需要 GameLib 实例。

- `filename` 按 UTF-8 路径解释；Win32 版统一转宽字符路径打开和删除文件
- `key` 不能为空，且不能包含 `=`、`\r` 或 `\n`
- 每行按第一个 `=` 切分 key/value，因此 value 可以包含 `=`

#### `static bool SaveInt(const char *filename, const char *key, int value)`
- 将整数写入存档文件的指定 key
- 若文件不存在则创建；若 key 已存在则覆盖
- 成功返回 `true`

#### `static bool SaveFloat(const char *filename, const char *key, float value)`
- 将浮点数写入存档文件的指定 key（以 `%g` 格式存储）
- 若文件不存在则创建；若 key 已存在则覆盖
- 成功返回 `true`

#### `static bool SaveString(const char *filename, const char *key, const char *value)`
- 将字符串写入存档文件的指定 key
- 字符串值中的 `\` 转义为 `\\`，`\n` 转义为 `\n`，读取时自动还原
- 成功返回 `true`

#### `static int LoadInt(const char *filename, const char *key, int defaultValue = 0)`
- 从存档文件读取整数值
- 文件不存在、key 不存在或解析失败时返回 `defaultValue`

#### `static float LoadFloat(const char *filename, const char *key, float defaultValue = 0.0f)`
- 从存档文件读取浮点数值
- 文件不存在、key 不存在或解析失败时返回 `defaultValue`

#### `static const char *LoadString(const char *filename, const char *key, const char *defaultValue = "")`
- 从存档文件读取字符串值
- 返回指向内部 `static char[1024]` 缓冲区的指针，在下次调用 `LoadString` 前有效
- 文件不存在或 key 不存在时返回 `defaultValue`

#### `static bool HasSaveKey(const char *filename, const char *key)`
- 判断存档文件中是否存在指定 key

#### `static bool DeleteSaveKey(const char *filename, const char *key)`
- 从存档文件中删除指定 key
- 若删除后文件只剩头部，则删除整个文件
- 成功返回 `true`

#### `static bool DeleteSave(const char *filename)`
- 删除整个存档文件
- 成功返回 `true`

#### 存档文件格式

存档文件为纯文本格式，可用记事本打开查看：

```
GAMELIB_SAVE
score=100
name=Player\nOne
level=3
```

- 第一行固定为 `GAMELIB_SAVE` 魔数头
- 后续每行为 `key=value` 格式，解析时按第一个 `=` 切分
- key 不能为空，且不能包含 `=`、回车或换行
- 字符串值中的 `\` 和换行符会转义存储
- Win32 版使用 `_gamelib_fopen_utf8()` 打开文件，`DeleteSave()` 也按 UTF-8 路径删除文件

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
| `WM_SIZE` | 更新 `_windowWidth`, `_windowHeight` |
| `WM_MOUSEMOVE` | 读取客户区坐标并按当前缩放比例换算到 `_mouseX`, `_mouseY` |
| `WM_SETCURSOR` | 在客户区内根据 `_mouseVisible` 显示箭头或隐藏鼠标 |
| `WM_MOUSEWHEEL` | 更新缩放后的鼠标位置并累计 `_mouseWheelDelta` |
| `WM_LBUTTONDOWN/UP` | 更新缩放后的鼠标位置，并更新 `_mouseButtons[0]` |
| `WM_RBUTTONDOWN/UP` | 更新缩放后的鼠标位置，并更新 `_mouseButtons[1]` |
| `WM_MBUTTONDOWN/UP` | 更新缩放后的鼠标位置，并更新 `_mouseButtons[2]` |
| `MM_MCINOTIFY` | 当当前音乐是 MIDI 且 `_musicLoop=true` 时，执行 `seek ... to start` 后重新播放；否则清理该实例的 MCI 资源状态 |
| `WM_PAINT` | 客户区与 framebuffer 同尺寸时直接 `BitBlt`；否则按预计算映射生成 `present buffer` 后用 `SetDIBitsToDevice` 重绘到客户区 |

---

## 8. 内部辅助方法

| 方法 | 说明 |
|------|------|
| `static int _InitWindowClass()` | 注册窗口类（使用 `RegisterClassW`，只执行一次，使用静态局部变量） |
| `static LRESULT CALLBACK _WndProc(...)` | 窗口过程回调 |
| `void _DestroyGraphicsResources()` | 释放 DIB Section、常备 DC 和相关图形资源，供 `Open()` 重开和析构共用 |
| `void _DestroyTimingResources()` | 停止多媒体定时器、关闭事件对象，并在需要时匹配 `timeEndPeriod(1)` |
| `void _DispatchMessages()` | PeekMessage 循环派发消息 |
| `void _InitDIBInfo(void *ptr, int w, int h)` | 初始化 BITMAPINFO（32-bit, top-down，`biSizeImage` 使用 `(size_t)` 防溢出） |
| `void _SetPixelFast(int x, int y, uint32_t c)` | 无边界检查的像素写入；Alpha<255 时转为混合写入 |
| `void _DrawHLine(int x1, int x2, int y, uint32_t c)` | 带裁剪的水平线（DrawRect/FillCircle/FillEllipse/FillTriangle 内部用） |
| `void _UpdateTitleFps()` | 当 `_showFps=true` 时更新标题栏 FPS 显示（在 FPS 统计更新时调用） |
| `int _AllocSpriteSlot()` | 在 `_sprites` 向量中找空闲槽位或追加新槽位 |
| `void _DrawSpriteAreaFast(...)` | 非缩放精灵/区域绘制快路径；无 Alpha 且无 ColorKey 时直接写入源像素（无翻转时优先逐行 `memcpy`），其他情况按 flags 做 Color Key 或 Alpha 处理 |
| `void _DrawSpriteAreaScaled(...)` | 缩放绘制路径，使用最近邻采样并保持翻转、Color Key、Alpha 语义 |
| `void _DrawSpriteAreaRotated(...)` | 旋转绘制路径，按包围盒逐像素做逆向旋转采样，并保持翻转、Color Key、Alpha 语义 |
| `int _AllocTilemapSlot()` | 在 `_tilemaps` 向量中找空闲槽位或追加新槽位 |
| `int _GetTilesetTileCount(int tilesetId, int tileSize) const` | 根据当前 tileset sprite 尺寸计算可用瓦片总数 |
| `static int _gamelib_floor_div(int value, int divisor)` | 向下取整整数除法，供负坐标的 Tilemap 像素到瓦片坐标换算使用 |
| `static int _gamelib_round_to_int(double value)` | 将浮点值按四舍五入转为整数，供椭圆采样使用 |
| `static uint32_t _gamelib_alpha_blend(uint32_t dst, uint32_t src)` | 统一的 ARGB source-over 混色 helper |
| `static void _gamelib_blend_pixel(uint32_t *dst, uint32_t src)` | 对单个目标像素执行 Alpha 混合写入 |
| `static wchar_t* _gamelib_utf8_to_wide(...)` | 将 UTF-8 字符串转换为宽字符，供窗口标题、字体和资源路径共用 |
| `static FILE* _gamelib_fopen_utf8(...)` | 用宽字符路径打开文件，统一图片资源的 UTF-8 文件名语义 |
| `static bool _gamelib_read_text_line(FILE*, std::string&)` | 逐行读取 `.glm` 文本数据，兼容最后一行没有换行符的情况 |
| `static void _gamelib_strip_utf8_bom(std::string&)` | 移除 `.glm` 首行可能存在的 UTF-8 BOM |
| `static bool _gamelib_parse_int_tokens(...)` | 按空格/Tab 解析整数 token，供 `.glm` 头部和瓦片行共用 |
| `static bool _gamelib_mci_path_is_safe(...)` | 检查音乐路径是否包含引号或换行，避免 MCI 命令拼接注入 |
| `static bool _gamelib_mci_play_music_alias(...)` | 按实例 alias 启动 MCI 播放；MIDI 走 `notify` 路径，其他格式可直接 `repeat` |
| `static void _gamelib_close_music_alias(...)` | 停止并关闭指定 alias 对应的 MCI 资源 |
| `static HFONT _gamelib_create_font_utf8(...)` | 用 UTF-8 字体名创建 GDI 字体对象 |
| `static void _gamelib_measure_font_text(...)` | 基于 `GetTextExtentPoint32W` 计算多行字体文字的像素宽高 |
| `static int _gamelib_gdiplus_init()` | 懒加载 `gdiplus.dll` 和 `ole32.dll`，解析函数地址并调用 `GdiplusStartup`（仅首次执行） |
| `static void _gamelib_com_release(void *obj)` | 通过 COM vtable 手动调用 `IUnknown::Release`（无需 ObjBase.h） |
| `static uint32_t* _gamelib_gdiplus_load(...)` | 从内存数据通过 GDI+ 解码图片，返回 32bppARGB 像素数组 |
| `static int _gamelib_load_apis()` | 动态加载 `gdi32.dll` 和 `winmm.dll` 的全部函数指针（构造函数调用，仅首次执行） |
| `static std::string _gamelib_save_escape(const char*)` | 将字符串值中的 `\` 和 `\n` 转义为 `\\` 和 `\n` |
| `static std::string _gamelib_save_unescape(const char*, size_t)` | 将转义后的字符串还原 |
| `static bool _gamelib_save_find_key(...)` | 在存档内容中查找指定 key 所在行 |
| `static std::string _gamelib_save_read_all(const char*)` | 读取整个存档文件内容（Win32 版用 `_gamelib_fopen_utf8`） |
| `static bool _gamelib_save_write_all(const char*, const std::string&)` | 写回整个存档文件（Win32 版用 `_gamelib_fopen_utf8`） |
| `static bool _gamelib_save_write_key(...)` | 写入或更新存档中的指定 key-value |
| `static bool _gamelib_save_read_key(...)` | 从存档中读取指定 key 的 value |

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

1. **C-style 强制转换** — cppcheck 会报 style 警告，但对目标用户群体来说可以接受
2. ~~PlayBeep 是阻塞的~~ — 已改为异步，通过 PlayPCM 播放，返回通道 ID
3. **单窗口** — 窗口类名固定为 "GameLibWindowClass"，同时只能有一个 GameLib 实例正常工作
4. **WaitFrame 精度** — 已改为基于绝对帧边界的 QPC 节拍，并在最后一小段时间里做更细的收尾等待；但底层仍受 Windows 调度粒度影响，不是硬实时计时器
5. **背景音乐单通道** — 每个 `GameLib` 对象同一时刻仍只能播放一首 MCI 背景音乐；但音效已支持多通道同时播放

### 未来改进方向

1. **旋转 + 缩放组合绘制** — 在现有旋转与缩放接口之上补统一仿射接口
2. **更多图元** — 圆角矩形、贝塞尔曲线等
3. **简单动画系统** — 在 `DrawSpriteFrame` 之上补更高层的动画播放辅助
4. ~~**音频增强**~~ — 多通道音效、音量控制（已在 waveOut 软件混音器中实现）
5. **示例游戏 Demo** — 编写打砖块、太空射击等示例

1.1 提案中的接口项已经全部并入当前 `GameLib.h` 与本文档，后续演进直接在这里继续维护。

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
| 窗口类注册使用 `RegisterClassW` + `CreateWindowW` | 全 Unicode 链一致（`RegisterClassW` → `CreateWindowW` → `DefWindowProcW`），窗口标题支持 UTF-8 |
| 资源文件路径统一按 UTF-8 解释 | `LoadSprite*` / `PlayWAV` / `PlayMusic` 内部转宽字符，避免中文目录和文件名失效 |
| 键盘重复过滤（bit 30） | 避免按住一个键产生大量 KeyDown 事件 |
| FPS 每秒统计一次 | 平滑显示，避免帧间波动 |
| 时间统计使用 `QueryPerformanceCounter` | 比 `timeGetTime` 有更高的计时分辨率 |
| `WaitFrame` 使用绝对帧边界 + 粗等 + 短尾收尾 | 避免把本帧工作耗时重复算进等待时间，并减少最后 1ms 左右的 oversleep |
| `Open()` 支持 restart-safe 重开 | 先清旧窗口 / DIB / timer 资源并清掉残留 `WM_QUIT`，避免同一对象重开时继承上一次状态 |
| 关闭流程先释放 timer 资源再销毁窗口 | 避免窗口销毁后极短时间里定时器事件还可能被 signal |
| Color Key 用品红 (0xFFFF00FF) 而非黑色 | 黑色难以制作和判断，品红是 2D 资源常用透明色 |
| `COLORKEY_DEFAULT` 用 `#ifndef` 保护 | 允许用户在 include 前自定义覆盖 |
| 每个精灵独立保存 Color Key | 不同素材可使用不同透明色，同时保留 `COLORKEY_DEFAULT` 作为初始值 |
| PlayMusic 用 MCI 而非 PlaySound | MCI 支持 MP3，且与 PlaySound 独立通道，可同时播放音乐和音效 |
| PlayMusic 按扩展名选择 MCI 类型 | `.mp3` 用 `mpegvideo`，`.mid/.midi` 用 `sequencer`，`.wav` 用 `waveaudio`，不依赖自动检测 |
| MCI 音乐使用实例级 alias | 避免固定全局 alias 在多对象或重开路径下发生冲突 |
| GDI+ 通过 LoadLibrary 动态加载 | 避免编译时链接 `-lgdiplus -lole32`，保持只需 `-mwindows` |
| GDI+ 懒初始化（首次 `LoadSprite` 时） | 不使用图片加载功能时零开销，不影响启动速度 |
| COM Release 通过 vtable 手动调用 | 避免 `#include <ObjBase.h>`，减少头文件依赖 |
| GDI+ 所有类型用 `void*` 不透明指针 | 无需引入 GDI+ 头文件，仅通过 Flat API 函数指针操作 |
| LockBits 始终请求 32bppARGB | 统一像素格式，无论源图片是 24-bit/32-bit/调色板格式 |
| `LoadSprite` GDI+ 失败时 BMP 回退 | 兼容无 GDI+ 的极端环境，BMP 仍可通过 `LoadSpriteBMP` 加载 |
| GDI+ 加载后检测全零 alpha 并修正为 255 | 24 位图片（BMP/JPG 等无 alpha 通道）经 GDI+ 转 32bppARGB 后 alpha 可能为 0，导致绘制时被当作透明跳过 |
| Alpha 混合用 `SPRITE_ALPHA` 标志显式启用 | 默认不混合（性能优先），需要混合时通过 `DrawSpriteEx` 传入标志，避免不必要的每像素计算 |
| 精灵绘制拆分为非缩放快路径和缩放路径 | 常见 `DrawSprite` / `DrawSpriteRegion` 走整数步进快路径；真正缩放时才做最近邻采样 |
| 无 Alpha 且无 ColorKey 的 sprite/tilemap 快路径不检查源像素透明洞 | 默认路径直接覆盖目标像素；无翻转时可进一步退化为逐行 `memcpy`。需要透明语义时显式传 `SPRITE_ALPHA` / `SPRITE_COLORKEY` |
| 使用 `#include <mmsystem.h>` | 需要 `HWAVEOUT`/`WAVEHDR`/`WAVEFORMATEX` 等结构体定义；头文件本身不引入链接依赖，动态加载仍通过 `LoadLibrary`/`GetProcAddress` |
| waveOut 软件混音器替代 PlaySoundW | PlaySoundW 只支持单通道全局播放；waveOut CALLBACK_FUNCTION 双缓冲模式支持 32 通道独立音量混音 |
| `unsigned int` 替代 `UINT32` | GCC 4.9.2 的 MinGW 头文件可能未定义 `UINT32` |
| Tilemap tiles 用 `int*`（malloc 分配）管理 | 与精灵像素内存管理风格一致，析构时自动释放 |
| Tilemap 瓦片 ID 用 -1 表示空 | 0 是有效的 tileset 第一块瓦片，-1 不会与任何瓦片冲突 |
| Tilemap 不再缓存 `tilesetTileCount`，`DrawTilemap()` 统一按当前 sprite 尺寸即时计算 `tileCount` | 避免维护多份上限状态，同时继续以 live sprite 尺寸保证绘制路径内存安全 |
| Tilemap 载入/写入允许保留超出当前 tileset 范围的非负 `tileId` | 地图数据可以先于 tileset 扩容存在；真正不可绘制的格子统一在 `DrawTilemap()` 阶段跳过 |
| `DrawTilemap` 预计算可见瓦片范围 | 大地图（如 200×50）时只遍历当前裁剪矩形内的瓦片，保证绘制性能 |
| `DrawTilemap` 复用非缩放精灵快路径 | 无 `SPRITE_ALPHA` / `SPRITE_COLORKEY` 时逐行 memcpy；其他情况转到 `_DrawSpriteAreaFast`，避免保留重复实现 |
| Tilemap 不管理 tileset 精灵的生命周期 | `FreeTilemap` 只释放 tiles 数组，tileset 精灵由用户通过 `FreeSprite` 控制 |
| DIB Section + present buffer | 创建 `CreateDIBSection` 并选入常备 `_memDC` 作为帧缓冲源；缩放显示时使用预计算映射生成 `present buffer` |
| `DrawTextFont` 动态创建字体 | 每次调用创建/销毁字体，适合少量文字；若需大量文字可后续添加字体缓存 |
| 图元颜色默认接受 ARGB | `SetPixel` 与线段/矩形/圆/椭圆/三角形在 Alpha<255 时统一做 source-over 混合，语义与精灵 Alpha 一致 |
| `ShowMouse` 不使用全局 `ShowCursor` 计数 | 避免不同窗口/库同时操作时把系统全局光标引用计数弄乱 |
| `ShowMessage` 只暴露两种按钮布局 | 对初学者 API 保持足够简单，同时覆盖确认/询问两类常见对话框 |
| GDI 文字函数动态加载 | 所有 GDI 函数通过 `LoadLibrary` + `GetProcAddress` 加载，保持只需 `-mwindows` 编译 |
| `DrawTextFont` 结果按调用方 alpha 再混回帧缓冲 | GDI 栅格本身不遵守 GameLib 的 ARGB 语义，需要先修正字形 alpha，再按调用方 alpha 做 post-blend |
| 构造函数加载核心 API | `gdi32.dll`/`winmm.dll` 是 Windows 系统 DLL，实际不会加载失败；构造时加载消除了"API 未加载"状态，后续方法无需 NULL 检查 |
| 构造失败 `MessageBoxA` + `exit(1)` | 面向初学者的库，构造失败（系统 DLL 无法加载）是灾难性的，直接终止比返回错误码更友好 |
| 精灵/帧缓冲尺寸限制 16384 | 防止 `width * height * 4` 整数溢出（16384 × 16384 × 4 = 1 GB，在 `size_t` 范围内） |
| Tilemap 尺寸限制 4096 | 防止 `cols * rows` 整数溢出，4096 × 4096 = 16M tiles 已足够大 |
| `CircleOverlap` / `FillTriangle` 使用 `int64_t` | 两个 `int` 坐标相减再相乘可溢出 32 位；`int64_t` 保证中间结果不溢出 |
| `COLOR_RGB` / `COLOR_ARGB` 每分量 `& 0xFF` | 防止调用者传入超过 255 的值导致位移溢出污染相邻通道 |
| `LoadSpriteBMP` 支持 8-bit 调色板 BMP | 许多经典游戏素材和工具导出 256 色 BMP；读取 DIB header 后的调色板（BGRX 格式，最多 256 条目），每像素字节查表转 32-bit ARGB |
| `LoadSpriteBMP` 用 `memcpy` 读 BMP 头 | 避免通过指针强制转换（aliasing cast）读取未对齐的多字节字段，符合严格别名规则 |
| `LoadSpriteBMP` 失败时回滚精灵槽 | 截断文件或短读不能留下半初始化 sprite，避免后续误用野数据 |
| `_gamelib_load_apis` 失败时清理所有指针 | GetProcAddress 部分失败时 NULL out 所有指针 + FreeLibrary 两个 DLL，防止悬空指针 |
| `_gamelib_gdiplus_init` 失败时分级清理 + 防重入 | 所有失败路径都设置 `_gamelib_gdiplus_failed = 1`，后续调用直接返回 -5 不再重试；-1/-2/-3 路径 FreeLibrary 已加载的 DLL；-4 路径（`GdiplusStartup` 失败）不释放 DLL 因为函数指针已指向 DLL 内部地址 |
| `PlayMusic` 使用 `mciSendStringW` + 路径过滤 | 路径支持 UTF-8；拒绝引号和换行，避免 MCI 字符串命令注入 |
| 场景切换延迟到下一帧 `Update()` 生效 | 避免同一帧内输入穿透到新场景；pending 机制保证 `SetScene()` 后的当前帧逻辑完整执行 |
| 初始帧 `_sceneChanged = true` | 方便用户在场景 0 的首帧做一次性初始化，不需要额外的 `firstFrame` 变量 |
| Win32 主线按线程维度假设一个窗口和一个主循环 | `PeekMessage` / `PostQuitMessage` 直接作用于线程消息队列，实现简单并符合教学用法；代价是同一线程不支持多个 `GameLib` 窗口并行 |
| 存档函数全部 `static` | 不依赖 GameLib 实例，可在任意时机（包括 `Open()` 前）调用；与 `Random()` 保持一致的调用风格 |
| 存档文件纯文本 `key=value` 格式 | 用记事本可直接查看和编辑，便于调试；`GAMELIB_SAVE` 魔数头防止误解析其他文本文件 |
| 字符串值仅转义 `\` 和 `\n` | 最小转义方案，其他特殊字符（tab、空格等）原样保存；保证 `key=value` 行格式不被换行符破坏 |
| `LoadString` 返回 `static char[1024]` 指针 | 避免引入 `std::string` 返回值或手动 free，对初学者最简单；代价是下次调用覆盖，但教学场景下够用 |
| 存档文件读写/删除统一使用 UTF-8 路径 | Win32 版与精灵/音乐路径一致，支持中文文件名；`DeleteSave()` 不再退回窄字符 `remove()` |
| 存档实现放在文件末尾 `#endif` 前 | 避免在绘制/精灵/Tilemap 等核心渲染代码中间插入大量无关代码，保持中段可读性 |
| 音频设备惰性初始化 | waveOut 设备在首次 `PlayWAV` 调用时才创建，不使用音频的程序不会创建音频设备 |
| `_audio_lock` 在构造函数中初始化 | `CRITICAL_SECTION` 必须在使用前初始化；放在构造函数保证所有音频方法可安全进入临界区 |
| waveOut 使用 CALLBACK_FUNCTION 双缓冲模式 | 回调驱动混音，无需线程池或定时器；双缓冲交替提交保证连续播放不卡顿 |
| WAV 重采样使用线性插值 + 边界钳制 | `step = src_rate / target_rate`，浮点源索引取整后 clamp 到 `samples_per_channel - 1`，防止越界读 |
| WAV 文件按路径缓存 | 同一文件只读一次，重复播放从缓存取 `_WavData`，避免磁盘重复 IO |
| PlayPCM 临时数据 `temporary` 标记 | `temporary=true` 的 `_WavData` 在 ref_count 递减后立即释放；复用 `_ConvertToTargetFormat`，栈上借用外部指针零拷贝 |
| 通道 ID 用自增 `int64_t` 分配 | 简单无冲突，关闭通道后 ID 不回收；`unordered_map<int, _Channel*>` 管理活跃通道 |
| 混音缓冲使用 `int32_t` 累加 | 防止多通道叠加溢出 16-bit；最终按 `master_volume * channel_volume / 1000000` 钳制后截断为 `int16_t` |
| `DrawTextScale` 预计算查找表缩放 | 替代逐像素除法，按缩放因子预生成 x/y 索引表；alpha==255 时直写 framebuffer 略过混合；最大缩放尺寸 1024 |
| `FillRect` 不透明路径首行填充 + memcpy | alpha==255 时首行逐像素写入，后续行用 memcpy 从第一行复制，宽矩形性能显著提升 |
| `DrawText` 空行整体跳过 | 内嵌 8x8 字体逐字符逐行渲染时，bits==0 的行直接 continue，空格等字符几乎零开销 |
| `DrawTextScale` 1:1 快路径 | w==8 && h==8 时直接走 `DrawText`，避免查找表开销 |
| Aspect lock 模式下鼠标坐标实时计算 | `_SetMouseFromWindowCoords` 实时计算布局，不依赖 `_PresentFrame` 缓存的存储值，修复窗口最大化再还原后鼠标坐标偏移 |
| 窗口缩放时返回 framebuffer 逻辑坐标鼠标位置 | 无论窗口实际尺寸如何，`GetMouseX/Y` 始终返回 framebuffer 坐标系内的值，游戏逻辑无需感知窗口缩放 |
| `GetFramebuffer()` 直接暴露 framebuffer 指针 | 允许外部代码直接操作像素数组（如批量填充、memcpy 拷贝），无需逐像素 `SetPixel` |
