# GameLib.SDL.h 规格说明文档

## 概述

`GameLib.SDL.h` 是 `GameLib.h` 的 **独立 SDL 版产品线**，目标是在 **Windows / macOS / Linux** 上提供尽量一致的教学型 2D 游戏开发体验。

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

本规格文档的目标是先把 `GameLib.SDL.h` 的定位、依赖、内部架构、兼容边界和分阶段实现路线固定下来，确认后再进入实现。

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
- 线段、圆、三角形、精灵绘制、Tilemap 绘制、Alpha 混合等逻辑继续由库自行实现。
- SDL 主要负责：窗口、事件、纹理提交、图片解码、TTF 字体、音频输出。

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

### 3.1 必需依赖

`GameLib.SDL.h` 首版规格选择如下依赖：

- `SDL2`：窗口、事件、定时、纹理提交、基础音频设备初始化。
- `SDL2_image`：PNG / JPG / BMP / GIF 等图片解码。
- `SDL2_ttf`：可缩放字体渲染。
- `SDL_mixer`：音效与背景音乐播放。

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

Windows / MinGW 示例：

```bash
g++ -std=c++11 -o game.exe main.cpp -lSDL2main -lSDL2 -lSDL2_image -lSDL2_ttf -lSDL2_mixer
```

Linux 示例：

```bash
g++ -std=c++11 -o game main.cpp $(sdl2-config --cflags --libs) -lSDL2_image -lSDL2_ttf -lSDL2_mixer
```

macOS（Homebrew 安装 SDL 后）示例：

```bash
clang++ -std=c++11 -o game main.cpp $(sdl2-config --cflags --libs) -lSDL2_image -lSDL2_ttf -lSDL2_mixer
```

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

这样可以保证：

- 图元、精灵、字体都共享同一像素语义。
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
- `Clear` / `SetPixel` / `GetPixel`
- `DrawLine` / `DrawRect` / `FillRect` / `DrawCircle` / `FillCircle` / `DrawTriangle` / `FillTriangle`
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
- `CreateTilemap` / `FreeTilemap` / `SetTile` / `GetTile` / `GetTilemapCols` / `GetTilemapRows`
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

#### `PlayBeep`

- Windows 版调用系统 `Beep`。
- SDL 版改为 **库内合成短音频波形** 并播放。

#### `WaitFrame`

- Windows 版依赖多媒体定时器。
- SDL 版改为基于高精度性能计数器和 `SDL_Delay` 的帧率等待。

#### `SetTitle` / `ShowFps`

- SDL 版通过 `SDL_SetWindowTitle` 更新标题栏。
- 在不同平台窗口管理器中，标题刷新时机与显示样式可能略有差异。

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

- 目标帧长 = `1.0 / fps`
- 距离目标时间还很远时调用 `SDL_Delay(1)`
- 距离目标时间很近时使用短忙等或 `SDL_Delay(0)` 收尾

原则：

- 精度优先于极端省电，但要避免整帧纯 busy wait。
- 若 `fps <= 0`，按 60 处理。

### 7.5 输入系统

SDL 版输入系统要求：

- 继续使用 `_keys[512]` / `_keys_prev[512]` 进行边沿检测。
- 鼠标维持 `MOUSE_LEFT` / `MOUSE_RIGHT` / `MOUSE_MIDDLE` 三键模型。
- 滚轮增量累加到 `_mouseWheelDelta`，在每次 `Update()` 开始时清零。
- `_active` 由窗口焦点、最小化状态等 SDL window event 共同决定。

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
- 再逐像素 Alpha 混合写入 `_framebuffer`

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
- 后续若需要，也可再补一套“常见系统字体候选路径”搜索逻辑。

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

建议直接复用当前 `GameLib.h` 的 BMP 解析策略：

- 支持 8-bit 调色板 BMP
- 支持 24-bit BMP
- 支持 32-bit BMP
- 处理 top-down / bottom-up 行序
- 行按 4 字节对齐

### 9.4 LoadSprite

`LoadSprite()` 首版通过 `SDL2_image` 实现，要求：

- 文件路径按 UTF-8 解释。
- 使用 `IMG_Load()` 或 `IMG_Load_RW()` 解码。
- 统一转换为 `SDL_PIXELFORMAT_ARGB8888`。
- 再拷贝到 GameLib 自己分配的 `uint32_t*` 精灵缓冲中。

若 `SDL2_image` 解码失败：

- 若文件头是 BMP，可回退到 `LoadSpriteBMP()`。
- 否则返回 `-1`。

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

### 10.4 PlayBeep

SDL 版 `PlayBeep(int frequency, int duration)` 改为：

- 运行时生成一小段 PCM 波形（建议正弦波或方波）
- 再通过 SDL_mixer 或 SDL 音频设备播放

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

- `GetTime()` 返回从 `Open()` 开始累计的秒数
- `GetDeltaTime()` 返回上一帧间隔秒数
- `GetFPS()` 采用与现有 `GameLib.h` 接近的“一秒一更新”统计方式

内部建议状态：

- `_timeStartCounter`
- `_timePrevCounter`
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
std::string _title;
int _width;
int _height;

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
uint64_t _perfFrequency;
float _deltaTime;
float _fps;
float _fpsAccum;

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

## 14. 首版实现优先级

建议按以下顺序推进：

### 阶段 1：最小运行闭环

- `Open`
- `IsClosed`
- `Update`
- `WaitFrame`
- `GetDeltaTime` / `GetFPS` / `GetTime`
- 键盘鼠标输入
- `Clear` / `SetPixel` / `GetPixel`
- `DrawLine` / `DrawRect` / `FillRect` / `DrawCircle` / `FillCircle`
- `DrawText` / `DrawNumber` / `DrawTextScale` / `DrawPrintf`

目标：

- 让最简单的 hello / moving box / basic drawing 示例先跑起来。

### 阶段 2：精灵与 Tilemap

- `CreateSprite`
- `LoadSpriteBMP`
- `LoadSprite`
- `DrawSprite*`
- `CreateTilemap` / `DrawTilemap`

目标：

- 让 sprite demo、animation、tilemap 类示例可迁移。

### 阶段 3：字体与音频

- `DrawTextFont`
- `GetTextWidthFont` / `GetTextHeightFont`
- `PlayWAV`
- `PlayMusic`
- `PlayBeep`

目标：

- 补齐跨平台版本最容易有差异的部分。

### 阶段 4：文档与示例迁移

- 为 SDL 版补单独 README 段落
- 增加 SDL 版构建命令
- 迁移 2~4 个代表性示例验证 API 兼容度

目标：

- 让 SDL 版具备可验证的构建文档和最小回归样例。

---

## 15. 需要在实现前确认的决策

本规格当前的默认决策如下，若无异议则按此推进：

1. **产品线分离**：保留现有 `GameLib.h`，新建独立 `GameLib.SDL.h`。
2. **公开类名不变**：SDL 版仍使用 `GameLib` 类名，不与 Win32 版混合包含。
3. **音频库选择 `SDL_mixer`**：不以 `SDL_sound` 作为首版默认方案。
4. **字体参数优先按路径解释**：家族名解析仅 best effort。
5. **继续坚持软件帧缓冲**：不把用户绘图直接切到 SDL renderer API。
6. **窗口默认不可缩放**：先做与现有 `GameLib.h` 接近的行为。
7. **Windows 默认 DPI unaware**：让 `800x600` 这类教学窗口跟随系统缩放一起放大，视觉效果与 `GameLib.h` 保持一致。

---

## 16. 技术决策备忘

- 选择独立 `GameLib.SDL.h`，是为了避免把现有 `GameLib.h` 变成一个充满平台分支和依赖分支的巨型头文件。
- 选择继续保留软件帧缓冲，是为了最大化复用现有图元、精灵、Tilemap、像素混合逻辑，也让 SDL 版与 Win32 版更容易保持语义一致。
- 选择 `SDL_mixer` 而不是纯 `SDL_Audio`，是为了让 `PlayWAV` / `PlayMusic` 这类高层 API 更快落地。
- 字体部分不把“系统字体家族名跨平台精确解析”列为首版硬要求，是为了控制复杂度，避免一开始就把 Win32 / macOS / Linux 的字体发现机制都卷进来。
- Windows DPI 默认选择 `unaware`，不是因为它技术上更先进，而是因为它更符合教学场景：同样一个 `800x600` 示例换到高分屏机器上不会显得过小。
