# SDL2PORT.md

`GameLib.SDL.h` 是 `GameLib.h` 的独立 SDL 产品线，用来把 GameLib 风格 API 移植到 Windows / Linux / macOS。它不是对 Win32 主线的直接替换，也不是把 SDL 后端塞回 `GameLib.h` 的混合版本。

这份文档只保留 SDL 版最常用的三类信息：

- 怎么编译
- 需要哪些依赖
- 当前有哪些明确限制

更完整的设计和内部行为说明仍以 `docs/GameLib.SDL.md` 为准。

## 1. 基本使用方式

单文件项目：

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

多文件项目：

```cpp
// 一个主实现文件
#define GAMELIB_SDL_IMPLEMENTATION
#include "GameLib.SDL.h"

// 其他 .cpp 文件
#define GAMELIB_SDL_NO_IMPLEMENTATION
#include "GameLib.SDL.h"
```

注意：`GameLib.SDL.h` 和 `GameLib.h` 不能出现在同一个翻译单元里，头文件内部会直接 `#error`。

## 2. 依赖分层

- `SDL2`：必需。窗口、事件、纹理提交、基础音频都靠它。
- `SDL2_image`：可选。启用 `LoadSprite()` 的 PNG/JPG/GIF 等格式解码。
- `SDL2_ttf`：可选。启用 `DrawTextFont()` 和字体测量。
- `SDL2_mixer`：可选。启用 `PlayWAV()` / `PlayMusic()` 的高层路径。

如果编译器能找到这些扩展头文件，`GameLib.SDL.h` 会自动启用对应功能；如果你已经安装了头文件但暂时不想链接某个扩展库，请在 `#include` 前显式关闭：

```cpp
#define GAMELIB_SDL_DISABLE_IMAGE 1
#define GAMELIB_SDL_DISABLE_TTF 1
#define GAMELIB_SDL_DISABLE_MIXER 1
#include "GameLib.SDL.h"
```

## 3. 编译命令

### 3.1 Windows / MinGW 最小命令

只启用 SDL2 核心能力：

```bash
g++ -std=c++11 -O2 -mwindows -o game.exe main.cpp -lSDL2
```

说明：

- 不需要 `-lSDL2main`，头文件内部已经定义 `SDL_MAIN_HANDLED` 并会调用 `SDL_SetMainReady()`。
- 如果你不介意控制台窗口，也可以去掉 `-mwindows`。
- 这种写法适合只测试基础窗口、输入、图元、内置 8x8 文字、软件精灵和 Tilemap。

### 3.2 Windows / MinGW + vcpkg 全功能示例

下面这条命令适合测试图片、字体、音频、Tilemap、裁剪等完整路径，例如编译 `examples/13_clip_rect.cpp`：

```bash
g++ -O2 -Wall -std=c++11 -fstrict-aliasing examples/13_clip_rect.cpp -o clip_rect.exe \
    -Ie:/local/vcpkg/installed/x86-mingw-dynamic/include \
    -Le:/local/vcpkg/installed/x86-mingw-dynamic/lib \
    -lSDL2 -lSDL2_image -lSDL2_ttf -lSDL2_mixer -mwindows
```

如果你只想编译核心路径，请把扩展库和对应 include 宏一起关掉。

### 3.3 Linux

```bash
g++ -std=c++11 -O2 -o game main.cpp $(sdl2-config --cflags --libs) -lSDL2_image -lSDL2_ttf -lSDL2_mixer
```

### 3.4 macOS

```bash
clang++ -std=c++11 -O2 -o game main.cpp $(sdl2-config --cflags --libs) -lSDL2_image -lSDL2_ttf -lSDL2_mixer
```

### 3.5 只测 SDL2 核心能力的写法

```cpp
#define GAMELIB_SDL_DISABLE_IMAGE 1
#define GAMELIB_SDL_DISABLE_TTF 1
#define GAMELIB_SDL_DISABLE_MIXER 1
#include "GameLib.SDL.h"
```

对应命令：

```bash
g++ -std=c++11 -O2 -mwindows -o game.exe main.cpp -lSDL2
```

## 4. 运行时要求

Windows 下如果使用动态库版本，需要让运行时能找到这些 DLL：

- `SDL2.dll`
- `SDL2_image.dll`
- `SDL2_ttf.dll`
- `SDL2_mixer.dll`

最简单的方法是把 DLL 放到可执行文件同目录，或者把 DLL 所在目录加入 `PATH`。

## 5. 当前限制

### 5.1 产品线边界

- `GameLib.SDL.h` 是独立产品线，不承诺和 `GameLib.h` 后端逐像素完全一致。
- 公开 API 风格尽量一致，但构建模型、依赖和平台行为分开维护。

### 5.2 编译与平台限制

- 代码基线是 C++11。
- 不要求继续兼容 Dev-C++ 5 自带那套极旧 MinGW。
- 不支持把 `GameLib.h` 和 `GameLib.SDL.h` 同时包含进同一个翻译单元。

### 5.3 可选能力限制

- 没有 `SDL2_image` 时，`LoadSprite()` 最终只剩 BMP 后路；PNG/JPG/GIF 等格式不会可用。
- 没有 `SDL2_ttf` 时，`DrawTextFont()` / `DrawPrintfFont()` 不可用，字体测量函数返回 0。
- 没有 `SDL2_mixer` 时，`PlayWAV()` / `PlayMusic()` 的高层路径不可用，但 `PlayBeep()` 仍可走 plain SDL audio 兜底。

### 5.4 行为限制

- `PlayMusic()` 当前会明确拒绝 `.mid` / `.midi`。
- `fontName` 在 SDL 版里是 best effort：推荐直接传字体文件路径，或显式设置 `GAMELIB_SDL_DEFAULT_FONT`。
- `LoadSpriteBMP()` 当前依赖 SDL 的 BMP 解码结果，不追求完全复刻 Win32 主线的自有 BMP 解析细节。
- Clip Rectangle 约束的是软件 `_framebuffer` 写入区域，不是 SDL renderer 的 viewport。

### 5.5 迁移现状

- 当前 SDL 回归入口是 `examples/01_hello.cpp` ~ `examples/15_ui_controls.cpp`，这 15 个统一示例通过预处理器自动选择 Win32 或 SDL 后端，同时作为两条产品线的回归验证。
- 覆盖路径包括：基础窗口、图元、精灵、动画、字体、音频、Tilemap、裁剪矩形、缩放、旋转与 UI 控件。