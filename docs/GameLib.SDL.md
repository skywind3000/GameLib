# GameLib.SDL.h 规格说明文档

## 概述

`GameLib.SDL.h` 是 `GameLib.h` 的 **独立 SDL 版产品线**，目标是在 **Windows / macOS / Linux** 上提供尽量一致的教学型 2D 游戏开发体验。

**当前版本**: `0.4.3`

它不是对现有 `GameLib.h` 的直接替换，也不是在原头文件中塞入大量 `#ifdef SDL` 的混合版本，而是一份 **单独维护的跨平台单头文件**。其公开使用方式尽量保持与 `GameLib.h` 一致：

```cpp
#include "GameLib.SDL.h"

int main() {
    GameLib game;
    game.Open(640, 480, "My Game", true);

    while (!game.IsClosed()) {
        game.Clear(COLOR_BLACK);
        game.DrawText(10, 10, "Hello SDL", COLOR_WHITE);
        game.Update();
        game.WaitFrame(60);
    }
    return 0;
}
```

本文档既用于固定 `GameLib.SDL.h` 的定位、依赖、内部架构与兼容边界，也用于同步当前实现状态、已知差异与后续演进边界。

当前 `0.4.3` 已加入与 Win32 主线对齐的 Clip Rectangle 裁剪接口；所有最终写入 `_framebuffer` 的绘制路径都会受当前裁剪矩形约束，空裁剪区域时统一不绘制；`DrawLine()` 会在 Bresenham 前先裁剪线段，`LoadSprite()` 也会拒绝超出 `16384` 限制的图片。

若你只想快速查看 SDL 版的编译命令、依赖开关和当前限制，可直接看仓库根目录的 `SDL2PORT.md`。

---

## 1. 设计目标

### 1.1 独立产品线，不污染现有 GameLib.h

- `GameLib.h` 继续保持当前定位：Win32、零额外依赖、面向 Dev-C++/GCC 4.9.2、编译命令极简。
- `GameLib.SDL.h` 作为跨平台版本，允许依赖 SDL 生态库，不要求继续满足“零额外依赖”约束。
- 两者 **API 风格尽量一致**，但实现和构建模型分离。

### 1.2 优先保持“源码级教学体验”兼容

- 优先保留 `GameLib game; game.Open(...); while (...) { ... }` 这一上手模型。
- 优先保留现有颜色常量、精灵标志、Tilemap、整数 ID 精灵管理、内置 8x8 字体等核心概念。
- 优先让现有示例可以通过“改 include + 改编译命令”迁移到 SDL 版。

### 1.3 保持软件渲染核心

- 库内部仍以 `uint32_t*` ARGB 帧缓冲作为统一绘制目标。
- 线段、圆、椭圆、三角形、精灵绘制、Tilemap 绘制、Alpha 混合等逻辑继续由库自行实现。
- SDL 主要负责：窗口、事件、纹理提交、图片解码、TTF 字体、音频输出。
- Clip Rectangle 也属于软件渲染核心的一部分：它限制的是 `_framebuffer` 上允许写入的像素区域，而不是 SDL renderer 的 viewport。

### 1.4 面向初学者，而不是暴露 SDL 细节

- 对使用者不暴露 `SDL_Window*`、`SDL_Renderer*`、事件泵、纹理锁定等 SDL 概念。
- 不要求用户自己管理事件循环、音频设备、渲染器生命周期。
- 公开 API 保持和 `GameLib.h` 同一风格，尽量不引入对象生命周期负担。

### 1.5 非目标

以下内容 **不属于 `GameLib.SDL.h` 首版目标**：

- 不追求与 `GameLib.h` 在每个后端细节上完全逐像素一致。
- 不追求同时包含 Win32 后端和 SDL 后端于同一个 `GameLib.h` 内。
- 不追求暴露 GPU 渲染管线、着色器、OpenGL/Vulkan 等高级接口。
- 不追求移动端、Web 端、手柄、触摸输入等首版支持。

---

## 2. 目标平台与支持范围

### 2.1 首版目标平台

| 平台 | 状态 | 说明 |
|------|------|------|
| Windows | 目标支持 | MinGW / MSVC 均可，依赖 SDL2 运行时 |
| macOS | 目标支持 | Clang + SDL2 系列库 |
| Linux | 目标支持 | 依赖 SDL2 视频后端，覆盖 X11 / Wayland |

### 2.2 Linux 桌面环境说明

`GameLib.SDL.h` 不直接按 XFCE / GNOME / KDE Plasma 分别适配，而是依赖 SDL2 统一覆盖 Linux 图形栈：

- 在 X11 环境下，通过 SDL2 的 X11 backend 运行。
- 在 Wayland 环境下，通过 SDL2 的 Wayland backend 运行。
- XFCE、GNOME、KDE Plasma 的兼容性由 SDL2 的 backend 决定，而不是由 `GameLib.SDL.h` 自己写桌面环境分支。

因此，“支持 XFCE / GNOME / KDE Plasma”在技术上等价于：

- SDL2 在这些环境中能正常创建窗口、接收输入、提交纹理和输出音频。

### 2.3 编译器基线

- 实现代码尽量维持 **C++11**。
- 不依赖 C++14/17/20 特性。
- 不要求兼容 Dev-C++ 5 自带的极旧 MinGW 发行版，但代码风格仍以简单、稳定、可读为优先。

### 2.4 Windows DPI 策略

`GameLib.SDL.h` 在 Windows 上的默认 DPI 策略应当 **刻意与现有 `GameLib.h` 保持一致**：

- 默认 **不声明 DPI aware**，等价于让进程处于 **DPI unaware** 模式。
- 这样逻辑尺寸为 `800x600` 的窗口，在系统缩放为 150% / 200% 的高分屏机器上，会由 Windows 整体放大。
- 目标不是追求像素最锐利，而是优先保证初学者在高分屏上看到的窗口 **不会因为物理尺寸太小而难以辨认**。

SDL 版实现上，要求在 `SDL_Init()` 之前设置 Windows DPI awareness 提示为 `unaware`。

建议通过编译期宏暴露这一行为：

```cpp
#ifndef GAMELIB_SDL_WINDOWS_DPI_AWARENESS
#define GAMELIB_SDL_WINDOWS_DPI_AWARENESS "unaware"
#endif
```

说明：

- 默认值 `"unaware"` 用于匹配 `GameLib.h` 的教学场景。
- 若未来需要高 DPI aware 行为，可由高级用户在包含头文件前自行覆盖该宏。
- 若可执行文件被外部 manifest 或其他更早的 DPI API 调用改变了 DPI 策略，则最终行为以进程实际 DPI 上下文为准。

---

## 3. 依赖与构建模型

### 3.1 依赖分层

`GameLib.SDL.h` 的依赖分为一层基础依赖和三层可选扩展：

- `SDL2`：必需。负责窗口、事件、定时、纹理提交、基础音频设备初始化。
- `SDL2_image`：可选。负责 PNG / JPG / BMP / GIF 等图片解码。
- `SDL2_ttf`：可选。负责可缩放字体渲染。
- `SDL_mixer`：可选。负责音效与背景音乐播放。

当前实现默认策略是：

- 若编译器能在 include path 里找到 `SDL_image.h` / `SDL_ttf.h` / `SDL_mixer.h`，则自动启用对应功能。
- 若找不到对应头文件，则自动关闭该功能，对应 API 进入 no-op / 失败返回的降级路径。
- 若你已经安装了这些头文件，但暂时不想链接某个扩展库，可以在 `#include "GameLib.SDL.h"` 之前显式关闭：

```cpp
#define GAMELIB_SDL_DISABLE_IMAGE 1
#define GAMELIB_SDL_DISABLE_TTF 1
#define GAMELIB_SDL_DISABLE_MIXER 1
#include "GameLib.SDL.h"
```

说明：

- `GAMELIB_SDL_DISABLE_IMAGE=1` 时，`LoadSprite()` 的非 BMP 扩展格式支持会被关闭。
- `GAMELIB_SDL_DISABLE_TTF=1` 时，`DrawTextFont()` 和字体测量函数会退化为不可用状态。
- `GAMELIB_SDL_DISABLE_MIXER=1` 时，`PlayWAV()` / `PlayMusic()` 的高层 SDL_mixer 路径会被关闭，但 `PlayBeep()` 仍可走 plain SDL 音频兜底。
- 若走“无扩展头但仍保留类型名”的前向声明路径，`TTF_Font`、`Mix_Chunk`、`Mix_Music` 的 typedef tag 必须与 SDL 官方头完全一致（如 `typedef struct TTF_Font TTF_Font;`），否则 MinGW 可能报 conflicting declaration。

### 3.2 关于音频库的决策

说明：SDL 官方并不存在名为 `SDL2_audio` 的高层扩展库；SDL2 本体只提供底层音频设备 API。

本规格 **首版选择 `SDL_mixer`**，而不是 `SDL_sound`，原因如下：

- 现有 `GameLib.h` 的音频 API 是高层接口：`PlayWAV`、`StopWAV`、`PlayMusic`、`StopMusic`、`IsMusicPlaying`。
- `SDL_mixer` 天然对应“音效 chunk + 背景音乐 music”模型，最接近现有 API 形状。
- `SDL_sound` 更像解码辅助层；若后续需要更细粒度编解码控制，再单独评估。

因此，本规格中“SDL 音频后端”默认指：

- `SDL2 + SDL_mixer`

### 3.3 单头文件使用模式

采用与现有 `GameLib.h` 类似的单头文件模式：

```cpp
// 单文件项目（默认）
#include "GameLib.SDL.h"

// 多文件项目中的主实现文件
#define GAMELIB_SDL_IMPLEMENTATION
#include "GameLib.SDL.h"

// 其他 .cpp 文件
#define GAMELIB_SDL_NO_IMPLEMENTATION
#include "GameLib.SDL.h"
```

自动定义逻辑建议为：

- 若未定义 `GAMELIB_SDL_NO_IMPLEMENTATION`，则自动定义 `GAMELIB_SDL_IMPLEMENTATION`。

### 3.4 与 GameLib.h 的共存规则

- `GameLib.SDL.h` 与 `GameLib.h` **不应在同一个翻译单元中同时包含**。
- 若检测到两者同时包含，应直接 `#error` 报错，避免宏、类名、常量名冲突。
- 两者都可继续对外暴露类名 `GameLib`，以保持示例源码迁移成本最低。

### 3.5 推荐编译命令

Windows / MinGW 最小命令（只启用 SDL2 核心能力）示例：

```bash
g++ -std=c++11 -O2 -o game.exe main.cpp -lSDL2
```

说明：

- 当前实现已在头文件内部定义 `SDL_MAIN_HANDLED` 并调用 `SDL_SetMainReady()`，因此 **不需要 `-lSDL2main`**。
- 若要隐藏控制台，可自行追加 `-mwindows`。
- 若 include path 中存在 `SDL2_image` / `SDL2_ttf` / `SDL2_mixer` 头文件，则还应同时链接对应库；否则请用 `GAMELIB_SDL_DISABLE_*` 宏显式关闭对应后端。

Windows / MinGW + vcpkg（已验证可编译 `tests/sdldemo2.cpp`）示例：

```bash
g++ -O2 -Wall -std=c++11 -fstrict-aliasing main.cpp -o game.exe \
    -Ie:/local/vcpkg/installed/x86-mingw-dynamic/include \
    -Le:/local/vcpkg/installed/x86-mingw-dynamic/lib \
    -lSDL2 -lSDL2_image -lSDL2_ttf -lSDL2_mixer
```

若你只想测试核心 SDL2 功能，可写成：

```cpp
#define GAMELIB_SDL_DISABLE_IMAGE 1
#define GAMELIB_SDL_DISABLE_TTF 1
#define GAMELIB_SDL_DISABLE_MIXER 1
#include "GameLib.SDL.h"
```

对应编译命令只需：

```bash
g++ -O2 -Wall -std=c++11 main.cpp -o game.exe -lSDL2
```

Linux 示例：

```bash
g++ -std=c++11 -o game main.cpp $(sdl2-config --cflags --libs) -lSDL2_image -lSDL2_ttf -lSDL2_mixer
```

macOS（Homebrew 安装 SDL 后）示例：

```bash
clang++ -std=c++11 -o game main.cpp $(sdl2-config --cflags --libs) -lSDL2_image -lSDL2_ttf -lSDL2_mixer
```

运行时说明：

- Windows 下若使用动态库版本，还需要确保 `SDL2.dll`、`SDL2_image.dll`、`SDL2_ttf.dll`、`SDL2_mixer.dll` 等运行时 DLL 能被找到。
- 最简单的方式是把它们放到可执行文件同目录，或保证其所在目录已加入 `PATH`。

---

## 4. 总体架构

`GameLib.SDL.h` 的总体结构分为四层：

```text
公开 API 层
    GameLib 类、颜色常量、键值常量、精灵标志、Tilemap API

软件渲染核心层
    framebuffer、图元算法、8x8 字体、sprite/tilemap 绘制、Alpha 混合

SDL 资源适配层
    SDL_image 解码、SDL_ttf 文本光栅化、SDL_mixer 音频播放

SDL 平台层
    SDL_Init / SDL_Quit、窗口、renderer、texture、事件、时间、标题栏
```

关键原则：

- **所有公开绘制 API 最终都写入 `_framebuffer`**。
- SDL renderer 只用于把 `_framebuffer` 显示出来，不直接承载用户绘图。
- 即使是 `DrawTextFont()`，也应先把字体渲染结果合成进 `_framebuffer`，而不是直接画到 SDL renderer 上。
- `SetClip()` / `ClearClip()` 改变的是 `_framebuffer` 上的有效写入区域；图元、内置字体、TTF 字体、sprite 和 Tilemap 都必须共享同一套裁剪语义。
- `DrawLine()` 这类逐像素图元也应先与当前 clip rect 求交，再进入 Bresenham，避免把大量无效像素检查留给 `SetPixel()`。

这样可以保证：

- 图元、精灵、字体都共享同一像素语义。
- `SetPixel` 与线段/矩形/圆/椭圆/三角形都使用同一套 source-over Alpha 混合规则。
- 后续 `GetPixel()`、Alpha 混合、截图、碰撞辅助不会因为“部分内容是直接渲染到 GPU”而失真。

---

## 5. 文件结构与内部组织

建议 `GameLib.SDL.h` 保持如下结构：

```text
GameLib.SDL.h
├── 文件头注释与使用示例
├── #ifndef GAMELIB_SDL_H / #define GAMELIB_SDL_H
├── IMPLEMENTATION 自动定义逻辑
├── SDL 头文件与基础标准库头文件
├── 常量定义
│   ├── 颜色常量
│   ├── GameLib 自有键值常量
│   ├── 鼠标常量
│   ├── 精灵标志
│   └── 默认字体/DPI/编译开关宏
├── GameLib 类声明
├── 8x8 位图字体数据
├── #ifdef GAMELIB_SDL_IMPLEMENTATION
│   ├── SDL 初始化与销毁辅助函数
│   ├── 文件路径/UTF-8 辅助函数
│   ├── 构造/析构
│   ├── Open / Update / WaitFrame
│   ├── 帧缓冲与图形绘制
│   ├── 字体渲染（SDL_ttf -> framebuffer）
│   ├── 精灵与图片加载（SDL_image）
│   ├── 输入系统
│   ├── 音频系统（SDL_mixer）
│   ├── Tilemap
│   └── 工具函数
├── #endif
└── #endif
```

说明：

- 这是一份 **独立头文件**，不要求直接复用 `GameLib.h` 的源代码块。
- 但在实现时，应有意识地把“平台无关算法”写成便于未来共享的形态，减少长期分叉成本。

---

## 6. 公开 API 兼容策略

### 6.1 保持不变的 API 形状

以下公开接口目标是 **名称、参数、基本语义保持一致**：

- `Open` / `IsClosed` / `Update` / `WaitFrame`
- `Clear` / `SetPixel` / `GetPixel` / `SetClip` / `ClearClip` / `GetClip`
- `GetClipX` / `GetClipY` / `GetClipW` / `GetClipH`
- `DrawLine` / `DrawRect` / `FillRect` / `DrawCircle` / `FillCircle` / `DrawEllipse` / `FillEllipse` / `DrawTriangle` / `FillTriangle`
- `DrawText` / `DrawNumber` / `DrawTextScale` / `DrawPrintf`
- `CreateSprite` / `LoadSpriteBMP` / `LoadSprite` / `FreeSprite`
- `DrawSprite` / `DrawSpriteEx` / `DrawSpriteRegion` / `DrawSpriteRegionEx`
- `DrawSpriteScaled` / `DrawSpriteFrame` / `DrawSpriteFrameScaled`
- `SetSpritePixel` / `GetSpritePixel` / `GetSpriteWidth` / `GetSpriteHeight`
- `SetSpriteColorKey` / `GetSpriteColorKey`
- `IsKeyDown` / `IsKeyPressed` / `IsKeyReleased`
- `GetMouseX` / `GetMouseY` / `IsMouseDown` / `IsMousePressed` / `IsMouseReleased`
- `GetMouseWheelDelta` / `IsActive`
- `PlayBeep` / `PlayWAV` / `StopWAV` / `PlayMusic` / `StopMusic` / `IsMusicPlaying`
- `Random` / `RectOverlap` / `CircleOverlap` / `PointInRect` / `Distance`
- `DrawGrid` / `FillCell`
- `CreateTilemap` / `SaveTilemap` / `LoadTilemap` / `FreeTilemap` / `SetTile` / `GetTile` / `GetTilemapCols` / `GetTilemapRows`
- `GetTileSize` / `WorldToTileCol` / `WorldToTileRow` / `GetTileAtPixel`
- `FillTileRect` / `ClearTilemap` / `DrawTilemap`

### 6.2 保持名称，但允许实现语义调整的 API

以下接口名称保留，但底层行为允许与 Windows 版不同：

#### `DrawTextFont` / `GetTextWidthFont` / `GetTextHeightFont`

- Windows 版按“字体名”工作；SDL 版首版不保证所有平台都能稳定按系统字体家族名解析。
- `fontName` 参数在 SDL 版中定义为：
  - **优先支持字体文件路径**（如 `assets/fonts/NotoSansSC-Regular.ttf`）
  - 对纯家族名（如 `Arial`、`Microsoft YaHei`）采用 **best effort** 解析，不保证所有平台一致
- 不额外引入平台特定字体发现 API 作为首版硬依赖。

#### `DrawPrintfFont`

- 保持与 Windows 版一致的 API 形状。
- SDL 版内部先用 `vsnprintf` 生成 UTF-8 文本，再复用 `DrawTextFont()` 的渲染路径。

#### `PlayBeep`

- Windows 版调用系统 `Beep`。
- SDL 版改为 **库内合成短音频波形** 并播放。

#### `WaitFrame`

- Windows 版依赖多媒体定时器。
- SDL 版改为基于高精度性能计数器和 `SDL_Delay` 的帧率等待。

#### `SetTitle` / `ShowFps`

- SDL 版通过 `SDL_SetWindowTitle` 更新标题栏。
- 在不同平台窗口管理器中，标题刷新时机与显示样式可能略有差异。

#### `ShowMouse`

- SDL 版通过 `SDL_ShowCursor()` 控制鼠标光标显示状态。
- 库内部缓存 `_mouseVisible`，因此在 `Open()` 前调用也能在窗口创建后自动生效。
- SDL 的鼠标光标显示是进程级语义，因此析构时应恢复为可见，避免把宿主环境留在隐藏光标状态。

#### `ShowMessage`

- SDL 版使用 `SDL_ShowSimpleMessageBox()` 或 `SDL_ShowMessageBox()` 实现，按钮布局与 Windows 版保持 `MESSAGEBOX_OK` / `MESSAGEBOX_YESNO` 两档。
- 若当前尚未初始化视频子系统，允许临时初始化 `SDL_INIT_VIDEO` 仅用于显示消息框。
- 若当前窗口处于隐藏鼠标状态，显示对话框期间应临时恢复鼠标可见，避免 `YES/NO` 对话框在无光标时难以操作。

### 6.3 键盘常量策略

现有 `GameLib.h` 的键值常量直接使用 Windows VK 值；SDL 版不应继续依赖这些平台值。

因此，本规格要求：

- `GameLib.SDL.h` 定义 **GameLib 自有键值常量**。
- 常量名保持不变，例如：`KEY_LEFT`、`KEY_RIGHT`、`KEY_A`、`KEY_0`、`KEY_F1`。
- 常量的数值只需保证在 `GameLib.SDL.h` 内部稳定，不要求等于 Windows VK。
- SDL 事件层负责把 `SDL_Scancode` / `SDL_Keycode` 映射到这些 GameLib 键值。

这也是未来若要让两条产品线进一步收敛时，最值得先统一的一层抽象。

---

## 7. SDL 平台后端设计

### 7.1 窗口与显示提交流程

首版建议使用：

- `SDL_Window* _window`
- `SDL_Renderer* _renderer`
- `SDL_Texture* _frameTexture`

其中：

- `_framebuffer` 仍是 `uint32_t*` 的 CPU 内存帧缓冲。
- `_frameTexture` 使用 `SDL_PIXELFORMAT_ARGB8888`。
- `Update()` 将 `_framebuffer` 上传到 `_frameTexture`，再复制到 renderer 并 `SDL_RenderPresent()`。

建议流程：

1. `Open()` 中创建固定大小窗口。
2. 创建 renderer。
3. 创建与窗口尺寸一致的 streaming texture。
4. 分配 `_framebuffer` 并清零。
5. `Update()` 中调用 `SDL_UpdateTexture()` 或 `SDL_LockTexture() + memcpy()`。
6. 调用 `SDL_RenderClear()`、`SDL_RenderCopy()`、`SDL_RenderPresent()`。

约束：

- 首版窗口默认 **不可缩放**，与现有 `GameLib.h` 行为对齐。
- 默认关闭插值缩放，优先像素风清晰显示。
- 不默认依赖 vsync，由 `WaitFrame()` 控制帧率。
- Windows 上默认采用 **DPI unaware** 策略，由系统缩放整体放大窗口，以匹配 `GameLib.h` 的视觉效果。

### 7.2 Open 的语义

`Open(int width, int height, const char *title, bool center)` 的首版语义：

- 验证尺寸范围仍为 `1~16384`。
- 必须成功初始化 SDL 视频和音频子系统。
- `center=true` 时使用 `SDL_WINDOWPOS_CENTERED`。
- 标题字符串按 UTF-8 处理。
- Windows 下在首次 `SDL_Init()` 前设置 DPI awareness 提示，默认值为 `unaware`。
- 返回值仍为整数错误码，保持与 `GameLib.h` 一样的“显式失败”风格。

建议错误码分层：

- `-1`: SDL 初始化失败
- `-2`: 创建窗口失败
- `-3`: 创建 renderer 失败
- `-4`: 创建 frame texture 失败
- `-5`: 分配 framebuffer 失败
- `-6`: SDL_image 初始化失败
- `-7`: SDL_ttf 初始化失败
- `-8`: SDL_mixer 初始化失败
- `-9`: 尺寸非法

### 7.3 Update 的职责

`Update()` 在 SDL 版中应承担以下职责：

1. 保存上一帧键鼠状态到 `_keys_prev` / `_mouseButtons_prev`
2. 将 `_mouseWheelDelta` 清零
3. 派发 SDL 事件队列
4. 更新 `_active`、键盘、鼠标、关闭状态
5. 更新 `_deltaTime`、`_fps`
6. 提交 `_framebuffer` 到窗口
7. 需要时更新标题栏 FPS

说明：

- SDL 版中“事件处理”与“画面提交”都集中在 `Update()`，继续保持用户只需每帧调用一次。

### 7.4 WaitFrame 的策略

首版建议使用 `SDL_GetPerformanceCounter()` / `SDL_GetPerformanceFrequency()` 维护高精度时间。

推荐算法：

- 维护一个绝对帧边界基准（如 `_frameStartCounter`），目标时间 = 上一帧目标时间 + `1.0 / fps`
- 距离目标时间还很远时调用 `SDL_Delay(1)`
- 距离目标时间很近时使用 `SDL_Delay(0)` 或短忙等收尾

原则：

- 精度优先于极端省电，但要避免整帧纯 busy wait。
- 若 `fps <= 0`，按 60 处理。
- 不要直接用 `_timePrevCounter` 作为等待起点，否则会把本帧工作耗时重复算进 pacing。

### 7.5 输入系统

SDL 版输入系统要求：

- 继续使用 `_keys[512]` / `_keys_prev[512]` 进行边沿检测。
- 鼠标维持 `MOUSE_LEFT` / `MOUSE_RIGHT` / `MOUSE_MIDDLE` 三键模型。
- 滚轮增量累加到 `_mouseWheelDelta`，在每次 `Update()` 开始时清零。
- `_active` 由窗口焦点、最小化状态等 SDL window event 共同决定。
- `ShowMouse()` 的目标状态缓存在 `_mouseVisible` 中，并在窗口存在时同步到 `SDL_ShowCursor()`。

需要处理的 SDL 事件至少包括：

- `SDL_QUIT`
- `SDL_KEYDOWN`
- `SDL_KEYUP`
- `SDL_MOUSEMOTION`
- `SDL_MOUSEBUTTONDOWN`
- `SDL_MOUSEBUTTONUP`
- `SDL_MOUSEWHEEL`
- `SDL_WINDOWEVENT_FOCUS_GAINED`
- `SDL_WINDOWEVENT_FOCUS_LOST`
- `SDL_WINDOWEVENT_MINIMIZED`
- `SDL_WINDOWEVENT_RESTORED`
- `SDL_WINDOWEVENT_CLOSE`

说明：

- 首版不暴露文本输入 IME 接口。
- 首版不处理游戏手柄。

### 7.6 ShowMouse / ShowMessage 设计约束

- `ShowMouse(bool)` 只负责“显示/隐藏鼠标”，不引入相对鼠标模式，也不锁定鼠标位置。
- `ShowMessage(...)` 只暴露两种按钮布局，保持与 Win32 主线一致的初学者 API 复杂度。
- `ShowMessage(...)` 的返回值统一映射到 `MESSAGEBOX_RESULT_OK`、`MESSAGEBOX_RESULT_YES`、`MESSAGEBOX_RESULT_NO`，避免把 SDL 按钮 ID 泄露到公开 API。

---

## 8. 文字渲染设计

### 8.1 内置 8x8 字体

- `DrawText` / `DrawNumber` / `DrawTextScale` / `DrawPrintf` 的内置位图字体逻辑应与 `GameLib.h` 保持一致。
- 继续使用 ASCII 32~126 的 8x8 点阵数据。
- 这是 SDL 版始终可用的零外部字体方案。

### 8.2 DrawTextFont 的实现原则

`DrawTextFont()` 的首版实现采用：

- `SDL_ttf` 把 UTF-8 文本渲染为临时 `SDL_Surface`
- 将 surface 转换为 `SDL_PIXELFORMAT_ARGB8888`
- 再逐像素 Alpha 混合写入 `_framebuffer`，并先与当前裁剪矩形求交

`DrawPrintfFont()` 复用同一路径：

- 先格式化为临时 UTF-8 字符串
- 再调用 `DrawTextFont()` 完成字体绘制

不能采用的方式：

- 直接把 TTF 文字画到 renderer，然后跳过 `_framebuffer`

原因：

- 否则 `GetPixel()`、截图、后续 Alpha 绘制、文字与图元叠加顺序都会出现语义分裂。

### 8.3 多行文本规则

SDL 版的 `DrawTextFont()` 与测量函数应继续支持：

- `\n` 换行
- 多行高度累加
- 忽略 Windows 风格换行中的 `\r`

### 8.4 字体参数语义

#### `fontName` 参数

在 SDL 版中，`fontName` 定义为：

- **推荐**：字体文件路径（如 `.ttf` / `.otf` / `.ttc`）
- **可选**：平台字体家族名（best effort）

首版不保证以下行为：

- 在三个平台上都能把同一个家族名稳定解析到同一款字体

#### 默认字体

建议增加编译期宏：

```cpp
#ifndef GAMELIB_SDL_DEFAULT_FONT
#define GAMELIB_SDL_DEFAULT_FONT ""
#endif
```

语义：

- 若 `GAMELIB_SDL_DEFAULT_FONT` 非空，则作为 `DrawTextFont(..., fontSize)` 重载的默认字体参数。
- 若为空，则默认字体重载在无字体可用时直接不绘制，测量函数返回 0。

说明：

- 这比在库内硬编码某个平台字体路径更可控。
- 当前实现已经补上了“常见系统字体候选路径”搜索逻辑：会优先尝试显式字体文件路径，再尝试常见家族名映射，最后回退到平台默认候选字体。
- 但这仍然是 best effort 行为；若你需要稳定、可复现的字形结果，仍建议显式设置 `GAMELIB_SDL_DEFAULT_FONT` 或直接传入字体文件路径。

### 8.5 字体缓存

首版建议加入轻量字体缓存：

- key: `fontName + fontSize`
- value: `TTF_Font*`

原因：

- 避免每帧反复打开同一字体文件。
- 这对 `DrawTextFont()` 性能影响明显。

缓存规则：

- 析构时统一关闭所有 `TTF_Font*`
- 不对文字 surface 做长期缓存，首版保持实现简单

---

## 9. 精灵与图片加载设计

### 9.1 精灵存储模型

- 与现有 `GameLib.h` 一致，精灵继续由整数 ID 管理。
- 每个精灵内部保存：
  - `width`
  - `height`
  - `uint32_t *pixels`
  - `colorKey`
  - `used`

### 9.2 CreateSprite

- 创建空白 ARGB 精灵。
- 像素初始化为 0。
- 尺寸检查继续使用 `1~16384`。

### 9.3 LoadSpriteBMP

SDL 版仍保留 `LoadSpriteBMP()`，目的有二：

- 与现有 API 兼容。
- 即使 `SDL2_image` 在某些极简构建中不可用，也仍有纯 BMP 加载后路。

当前实现采用 `SDL_LoadBMP()` + `SDL_ConvertSurfaceFormat(..., SDL_PIXELFORMAT_ARGB8888, 0)` 的路径，再拷贝到库自己的精灵缓冲中。

这意味着：

- BMP 解码细节交由 SDL 负责。
- `GameLib.SDL.h` 仍然保持“最终像素数据由自己持有和绘制”的语义。
- 若未来发现需要与 `GameLib.h` 的某些 BMP 边角行为严格对齐，再单独评估是否引入自有 BMP 解析路径。

### 9.4 LoadSprite

`LoadSprite()` 首版通过 `SDL2_image` 实现，要求：

- 文件路径按 UTF-8 解释。
- 使用 `IMG_Load()` 或 `IMG_Load_RW()` 解码。
- 统一转换为 `SDL_PIXELFORMAT_ARGB8888`。
- 尺寸限制仍为 `1~16384`；超出限制的图片会直接拒绝载入。
- 再拷贝到 GameLib 自己分配的 `uint32_t*` 精灵缓冲中。

若 `SDL2_image` 解码失败：

- 当前实现最终都会继续尝试 `LoadSpriteBMP()` 作为 BMP 后路；因此在没有 `SDL2_image`、初始化失败或 `IMG_Load()` 失败时，BMP 仍可加载，非 BMP 最终仍返回 `-1`。

### 9.5 DrawSprite 系列

`DrawSprite`、`DrawSpriteEx`、`DrawSpriteRegionEx`、`DrawSpriteScaled`、`DrawSpriteFrame`、`DrawSpriteFrameScaled` 的像素语义应与 `GameLib.h` 保持一致：

- `SPRITE_FLIP_H`
- `SPRITE_FLIP_V`
- `SPRITE_COLORKEY`
- `SPRITE_ALPHA`

要求：

- 继续保留无缩放快路径与缩放路径分离的设计。
- 不依赖 SDL 的 `SDL_RenderCopyEx` 做精灵翻转或缩放。
- 所有像素混合仍由库自己对 `_framebuffer` 完成。
- 未启用 `SPRITE_ALPHA` / `SPRITE_COLORKEY` 时，非缩放路径直接覆盖目标像素；无翻转时优先逐行 `memcpy`，带翻转时仍逐像素直写，不检查源像素 alpha 是否为 0。
- 缩放路径也保持同样语义：未启用 `SPRITE_ALPHA` 时不因为源像素 alpha 为 0 而跳过，只有显式传入 `SPRITE_ALPHA` 时才按源 alpha 混合。
- 只有显式传入 `SPRITE_ALPHA` 或 `SPRITE_COLORKEY` 时，源像素 alpha / Color Key 才参与透明语义。
- 所有 `DrawSprite*` 与 `DrawTilemap()` 路径都必须先与当前裁剪矩形求交；空裁剪区域时直接 no-op。

---

## 10. 音频系统设计

### 10.1 总体原则

SDL 版音频继续保持“简单高层 API”风格：

- 一个背景音乐流
- 一个由库管理的音效通道
- 一个 `PlayBeep()` 兼容接口

不向用户暴露：

- mixer 通道管理
- 音频回调
- 解码线程
- 设备枚举

### 10.2 PlayWAV / StopWAV

首版语义建议与 Windows 版对齐：

- `PlayWAV(filename, loop)` 由库自己保留“当前音效”状态
- 再次调用 `PlayWAV()` 时，新音效会替换旧音效
- `StopWAV()` 停止当前由库启动的音效

实现建议：

- 用 `Mix_Chunk*` 表示音效
- 使用一个保留 channel 播放普通音效
- `loop=true` 时使用无限循环或很大循环次数

### 10.3 PlayMusic / StopMusic / IsMusicPlaying

首版背景音乐由 `SDL_mixer` 的 `Mix_Music*` 实现。

语义：

- `PlayMusic()` 会先停止旧音乐，再播放新音乐
- `loop=true` 时循环播放
- `StopMusic()` 停止当前音乐
- `IsMusicPlaying()` 返回库管理的音乐播放状态

说明：

- 实际支持格式取决于 `SDL_mixer` 编译时启用的解码器
- 规格层默认面向：WAV / OGG / MP3 / FLAC
- `.mid` / `.midi` 会被 `PlayMusic()` 明确拒绝并返回 `false`

### 10.4 PlayBeep

SDL 版 `PlayBeep(int frequency, int duration)` 当前实现为 **阻塞式合成提示音**：

- 运行时生成一小段 PCM 波形
- 优先在 `SDL_mixer` 可用时通过保留 channel 播放
- 若 mixer 不可用，则临时打开 plain SDL audio device，走 `SDL_QueueAudio`
- 在声音播完或达到保护超时之前不返回，尽量保持与 Win32 `Beep()` 接近的教学语义

目标：

- API 保留
- 教学效果保留
- 不要求与 Windows 系统蜂鸣器音色一致

### 10.5 音频资源释放

- 析构时必须停止音乐和音效
- 释放 `Mix_Chunk*` / `Mix_Music*`
- 关闭 mixer 和 SDL 音频子系统

---

## 11. 时间与 FPS 统计

SDL 版时间系统要求：

- `GetTime()` 返回从 `Open()` 开始累计的秒数，公开类型为 `double`
- `GetDeltaTime()` 返回上一帧间隔秒数，公开类型为 `double`
- `GetFPS()` 采用与现有 `GameLib.h` 接近的“一秒一更新”统计方式，公开类型为 `double`
- `_deltaTime`、`_fps`、`_fpsAccum` 的内部存储与计算也统一使用 `double`

内部建议状态：

- `_timeStartCounter`
- `_timePrevCounter`
- `_frameStartCounter`
- `_perfFrequency`
- `_deltaTime`
- `_fps`
- `_fpsAccum`
- `_fpsTimeCounter`

说明：

- SDL 版不再依赖平台特定高精度计时器 API。
- 一律通过 SDL 的性能计数器封装实现。

---

## 12. 数据结构建议

SDL 版 `GameLib` 私有成员建议至少包含以下内容：

```cpp
// 窗口与显示
SDL_Window *_window;
SDL_Renderer *_renderer;
SDL_Texture *_frameTexture;
bool _closing;
bool _active;
bool _showFps;
bool _mouseVisible;
std::string _title;
int _width;
int _height;
int _clipX;
int _clipY;
int _clipW;
int _clipH;

// 帧缓冲
uint32_t *_framebuffer;

// 输入
int _keys[512];
int _keys_prev[512];
int _mouseX;
int _mouseY;
int _mouseButtons[3];
int _mouseButtons_prev[3];
int _mouseWheelDelta;

// 时间
uint64_t _timeStartCounter;
uint64_t _timePrevCounter;
uint64_t _fpsTimeCounter;
uint64_t _frameStartCounter;
uint64_t _perfFrequency;
double _deltaTime;
double _fps;
double _fpsAccum;

// 精灵
std::vector<GameSprite> _sprites;

// Tilemap
std::vector<GameTilemap> _tilemaps;

// 字体缓存
std::vector<GameFontCacheEntry> _fontCache;

// 音频
Mix_Chunk *_currentWav;
int _wavChannel;
Mix_Music *_currentMusic;
bool _musicPlaying;

// 随机数初始化标志
static bool _srandDone;
```

说明：

- 这里的 `GameFontCacheEntry` 可定义为 `{ std::string key; TTF_Font *font; }`
- 当前实现中的 `GameTilemap` 至少保存 `cols`、`rows`、`tileSize`、`tilesetId`、`tilesetCols`、`tiles`、`used`。
- SDL 版不再缓存 `tilesetTileCount`；`SetTile` / `FillTileRect` / `ClearTilemap` / `LoadTilemap` 只拒绝小于 `-1` 的值，超出当前 tileset 范围的非负 `tileId` 会保留到地图里，由 `DrawTilemap()` 按 live tileset 尺寸跳过不可绘制格子。
- 是否用 `std::vector` 还是更轻量结构体数组，可在实现阶段再调整，但首版不建议过度优化

---

## 13. 已知兼容差异

以下差异在首版是可接受的：

### 13.1 字体家族名不可完全跨平台一致

- Windows 下的 `Microsoft YaHei`
- macOS 下的 `PingFang SC`
- Linux 下的 `Noto Sans CJK SC` / `DejaVu Sans`

它们的存在与路径都不一致，因此 SDL 版首版不承诺“同一个字体名到处都能用”。

### 13.2 音乐格式支持受构建环境影响

- `SDL_mixer` 实际支持的编码格式与其编译选项和系统动态库有关。
- 规范层可承诺 API，不承诺每台机器都原生支持同样的音乐格式组合。

### 13.3 窗口管理器行为会略有差别

- 标题栏刷新
- 焦点切换
- 最小化恢复
- DPI 缩放

这些行为在 Win32 / macOS / X11 / Wayland 下不可能做到完全一样。

补充说明：

- Windows 默认目标是 **故意匹配 `GameLib.h`**，即使用 `DPI unaware` 模式，让系统缩放整体放大窗口。
- macOS 和 Linux 下的实际缩放观感则取决于 SDL backend、窗口系统和桌面环境，不承诺与 Windows 完全一致。

### 13.4 PlayBeep 的声音不再是系统蜂鸣器

- 只保留“能发出提示音”这个教学语义
- 不保留 Windows `Beep()` 的系统声音特征

---

## 14. 当前实现状态与已知差异

当前状态已经不再是纯规划阶段。下面保留最初的分阶段思路，但同步标注当前落地情况。

### 阶段 1：最小运行闭环（已完成）

- `Open`
- `IsClosed`
- `Update`
- `WaitFrame`
- `GetDeltaTime` / `GetFPS` / `GetTime`
- 键盘鼠标输入
- `Clear` / `SetPixel` / `GetPixel`
- `DrawLine` / `DrawRect` / `FillRect` / `DrawCircle` / `FillCircle` / `DrawEllipse` / `FillEllipse`
- `DrawText` / `DrawNumber` / `DrawTextScale` / `DrawPrintf`
- `ShowMouse`

目标：

- 让最简单的 hello / moving box / basic drawing 示例先跑起来。
- 当前状态：已完成，基础窗口、输入、时间、图元与内置文本路径已经由 `tests/sdldemo1.cpp` 覆盖。

### 阶段 2：精灵与 Tilemap（已完成）

- `CreateSprite`
- `LoadSpriteBMP`
- `LoadSprite`
- `DrawSprite*`
- `CreateTilemap` / `SaveTilemap` / `LoadTilemap` / `DrawTilemap`

目标：

- 让 sprite demo、animation、tilemap 类示例可迁移。
- 当前状态：已完成，`CreateSprite`、`LoadSpriteBMP`、`LoadSprite`、`DrawSprite*`、`CreateTilemap` / `SaveTilemap` / `LoadTilemap` / `DrawTilemap` 已落地；其中 `DrawTilemap()` 路径已由 `tests/sdldemo1.cpp` 和 `tests/sdldemo5.cpp` 覆盖基础回归，`SaveTilemap` / `LoadTilemap` 目前还缺少一份对齐 `examples/18_tilemap_file.cpp` 的 SDL 独立示例。
- Tilemap 侧现在允许保留超出当前 tileset 范围的非负 `tileId`；`DrawTilemap()` 在绘制前按 live tileset sprite 尺寸即时计算可用瓦片数，并跳过当前不可绘制的格子，避免 sprite 槽位释放重建后访问越界像素。

### 阶段 3：字体与音频（已完成）

- `DrawTextFont`
- `DrawPrintfFont`
- `GetTextWidthFont` / `GetTextHeightFont`
- `PlayWAV`
- `PlayMusic`
- `PlayBeep`
- `ShowMessage`

目标：

- 补齐跨平台版本最容易有差异的部分。
- 当前状态：已完成。`DrawTextFont` / `DrawPrintfFont`、字体测量、best-effort 字体解析、`PlayWAV`、`PlayMusic`、`PlayBeep`、`ShowMessage` 都已落地。
- 回归入口：`tests/sdldemo2.cpp`、`tests/sdldemo3.cpp`、`tests/sdldemo4.cpp`。

### 阶段 4：文档与示例迁移（当前阶段目标已完成）

- 补一份独立的 `SDL2PORT.md` 移植/编译说明
- 增加 SDL 版构建命令
- 迁移一组代表性示例验证 API 兼容度

目标：

- 让 SDL 版具备可验证的构建文档和最小回归样例。

当前状态：

- `docs/GameLib.SDL.md` 已同步到当前实现状态，仓库根目录也已新增 `SDL2PORT.md` 作为简版移植说明。
- `AGENTS.md` 已补充 `GameLib.SDL.h` / `docs/GameLib.SDL.md` 的索引与用途。
- README 仍只保留一句 SDL 产品线提示，主叙事继续突出 Win32 零依赖主线；具体 SDL 编译命令与限制统一放到 `SDL2PORT.md`。
- `tests/sdldemo1.cpp` ~ `tests/sdldemo15.cpp` 已形成最小 SDL 回归集，其中 `tests/sdldemo5.cpp` 是从 `examples/14_tilemap.cpp` 迁移来的代表性资产示例，`tests/sdldemo6.cpp` 是从 `examples/13_space_shooter.cpp` 迁移来的完整游戏循环示例，`tests/sdldemo7.cpp` 是从 `examples/12_breakout.cpp` 迁移来的经典碰撞/清版示例，`tests/sdldemo8.cpp` 是从 `examples/11_snake.cpp` 迁移来的网格离散移动示例，`tests/sdldemo9.cpp` 是从 `examples/06_catch_fruit.cpp` 迁移来的接取/漏接判定示例，`tests/sdldemo10.cpp` 是从 `examples/09_sprite_animation.cpp` 迁移来的精灵帧动画示例，`tests/sdldemo11.cpp` 是从 `examples/10_sound_demo.cpp` 迁移来的声音演示示例，`tests/sdldemo12.cpp` 是从 `examples/08_sprite_demo.cpp` 迁移来的精灵基础展示示例，`tests/sdldemo13.cpp` 是从 `examples/16_playsound.cpp` 迁移来的最小音效触发示例，`tests/sdldemo14.cpp` 是从 `examples/03_shapes.cpp` 迁移来的基础图元与 primitive alpha 展示示例，`tests/sdldemo15.cpp` 是从 `examples/19_clip_tilemap.cpp` 迁移来的裁剪矩形、Tilemap、字体与图元组合示例。
- 其中 `tests/sdldemo5.cpp` ~ `tests/sdldemo13.cpp` 已全部完成用户实机运行验证；说明 SDL 版当前不仅能编译，也已经覆盖了真实素材路径、完整小游戏循环、经典碰撞/清版、网格离散移动、接取判定、精灵基础绘制、精灵帧动画以及声音控制等实际使用场景。
- 本阶段收口结论：`GameLib.SDL.h` 已具备独立产品线的最小可维护状态，后续工作可以从“补齐代表性回归入口”切换到“按具体差异或新需求增量演进”。

### 14.5 当前已知差异与可继续完善项

以下项目仍属于“已知差异或可继续完善项”，但不阻塞 SDL 版当前作为独立产品线使用：

- `examples/01`、`02`、`04`、`05`、`07`、`15`、`17`、`18` 还没有逐个迁成 SDL 版独立测试，但它们覆盖的能力大多已经被现有 `sdldemo1.cpp` ~ `sdldemo15.cpp` 交叉覆盖；其中 `18_tilemap_file.cpp` 对应的 `SaveTilemap` / `LoadTilemap` 往返流程仍然缺一份 SDL 独立示例。
- 还没有把 `examples/` 中的更多 Win32 示例系统迁成 SDL 版；目前以 `tests/sdldemo1.cpp` ~ `tests/sdldemo15.cpp` 为代表性回归入口，其中已经覆盖了一个资产驱动示例、一个完整游戏循环示例、一个经典碰撞/清版示例、网格离散移动和接取判定两类轻量玩法示例，以及精灵基础展示、精灵动画、声音演示、一份专门覆盖基础图元和 primitive alpha 的 shapes 示例，以及一份专门覆盖 Clip Rectangle、tilemap 窗口、字体与图元裁剪的综合示例。
- 字体家族名解析虽然已经有 best-effort 候选链，但不同平台上的字形、回退顺序与最终命中字库仍不保证完全一致。
- `PlayMusic()` 的成功率仍受 `SDL_mixer` 在目标机器上的解码器支持影响；当前实现会明确拒绝 `.mid/.midi`，仓库内回归样例主要验证了 WAV 路径。
- `LoadSpriteBMP()` 当前依赖 SDL 的 BMP 解码结果，而不是复刻 `GameLib.h` 的自有 BMP 解析实现；若后续发现具体兼容差异，再按案例补齐。

### 14.6 建议回归顺序

- 图形与精灵相关改动：优先回归 `tests/sdldemo1.cpp`、`tests/sdldemo5.cpp`、`tests/sdldemo10.cpp`、`tests/sdldemo12.cpp`、`tests/sdldemo14.cpp`、`tests/sdldemo15.cpp`。
- 字体相关改动：优先回归 `tests/sdldemo2.cpp`、`tests/sdldemo3.cpp`、`tests/sdldemo15.cpp`。
- 音频相关改动：优先回归 `tests/sdldemo4.cpp`、`tests/sdldemo11.cpp`、`tests/sdldemo13.cpp`。
- 游戏循环、碰撞和输入相关改动：优先回归 `tests/sdldemo6.cpp`、`tests/sdldemo7.cpp`、`tests/sdldemo8.cpp`、`tests/sdldemo9.cpp`。

---

## 15. 已确认的关键决策

当前实现已经按以下决策落地，这些也是后续继续维护 SDL 产品线时应保持稳定的前提：

1. **产品线分离**：保留现有 `GameLib.h`，新建独立 `GameLib.SDL.h`。
2. **公开类名不变**：SDL 版仍使用 `GameLib` 类名，不与 Win32 版混合包含。
3. **音频库选择 `SDL_mixer`**：不以 `SDL_sound` 作为首版默认方案。
4. **字体参数优先按路径解释**：家族名解析仅 best effort。
5. **继续坚持软件帧缓冲**：不把用户绘图直接切到 SDL renderer API。
6. **窗口默认不可缩放**：先做与现有 `GameLib.h` 接近的行为。
7. **Windows 默认 DPI unaware**：让 `800x600` 这类教学窗口跟随系统缩放一起放大，视觉效果与 `GameLib.h` 保持一致。
8. **Tilemap 校验与绘制分离**：写入路径可以依赖缓存做快速合法性判断，但真正绘制前必须按当前 tileset sprite 尺寸刷新缓存。

---

## 16. 技术决策备忘

- 选择独立 `GameLib.SDL.h`，是为了避免把现有 `GameLib.h` 变成一个充满平台分支和依赖分支的巨型头文件。
- 选择继续保留软件帧缓冲，是为了最大化复用现有图元、精灵、Tilemap、像素混合逻辑，也让 SDL 版与 Win32 版更容易保持语义一致。
- 无 Alpha 且无 ColorKey 的 sprite/tilemap 快路径继续保持“直接覆盖目标像素”的规则，只有显式传入 `SPRITE_ALPHA` / `SPRITE_COLORKEY` 时才启用透明语义；这样 SDL 版与 Win32 版的默认 sprite 行为一致。
- 选择 `SDL_mixer` 而不是纯 `SDL_Audio`，是为了让 `PlayWAV` / `PlayMusic` 这类高层 API 更快落地。
- `PlayBeep` 优先复用 `SDL_mixer`，失败时再回退到 plain SDL queued audio，但整体仍保持阻塞式调用；这样更接近 Win32 版“提示音期间短暂停一下”的教学预期。
- 字体部分不把“系统字体家族名跨平台精确解析”列为首版硬要求，是为了控制复杂度，避免一开始就把 Win32 / macOS / Linux 的字体发现机制都卷进来。
- 可选 SDL 扩展类型的前向声明必须复用 SDL 官方 typedef tag；这是为了兼容 MinGW，对外看起来只是声明细节，但改错会直接导致编译冲突。
- Tilemap 不再缓存 `tilesetTileCount`，而是由 `DrawTilemap()` 在 memcpy 风险点前按 live sprite 尺寸即时计算 `tileCount`；这样既减少内部状态，又保持 sprite 槽位复用后的内存安全。
- Windows DPI 默认选择 `unaware`，不是因为它技术上更先进，而是因为它更符合教学场景：同样一个 `800x600` 示例换到高分屏机器上不会显得过小。
