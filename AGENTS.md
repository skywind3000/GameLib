# AGENTS.md

本项目用来帮助 C++ 初学者编写游戏，提供简单的 API，迅速体验到乐趣。

## Structure

```
- assets/    # images and audios
- docs/      # documentations
- examples/  # examples (01~19 渐进式示例，可作为参考和回归测试)
- tests/     # tests（Win32 demo*.cpp 和 SDL sdldemo*.cpp；目录里可能已有编译产物 .exe/.o）
- GameLib.h  # main source (Win32 主线，单头文件，所有实现都在这一个文件里)
- GameLib.SDL.h # independent SDL product line (跨平台 SDL 版单头文件)
- AGENTS.md  # this file
- README.md  # project home page
```

## Documentation

```
- docs/Manual.md              # GameLib.h 的接口说明，做游戏和改库都要先读
- docs/GameLib.md             # GameLib.h 的技术规格、设计思路和已合并的 1.1 演进说明
- docs/GameLib.SDL.md         # GameLib.SDL.h 的规格说明、依赖选择、兼容边界和实现决策
```

- 以上三个文档是当前稳定行为的权威来源；如果实现发生稳定变化，应同步更新对应 spec，而不是把关键信息只留在 repo memory 或聊天记录里。

## Assets

```
- assets/sprites.md # 精灵图资源索引，包含所有图片的尺寸、用途和瓦片集详情
- assets/sound.md   # 音效资源索引，包含所有 WAV 音效的参数和用途说明
```

## Build

### Win32 主线（GameLib.h）

编译只需要一条命令，不需要 Makefile 或其他构建工具：

```bash
g++ -o output.exe source.cpp -mwindows
```

- `-mwindows` 使程序以 Windows 窗口模式运行（不弹出控制台）。
- 不需要 `-lwinmm -lgdi32` 等链接参数，GameLib.h 通过动态加载解决所有依赖。
- 源文件通过 `#include "GameLib.h"` 引入即可，确保路径正确。

### SDL 产品线（GameLib.SDL.h）

- SDL 版不是“只加一个 include 就能沿用同一条编译命令”的变体；构建命令以 `docs/GameLib.SDL.md` 的 3.5 节为准。
- 只启用 SDL2 核心能力时，可参考最小命令：`g++ -std=c++11 -O2 -o game.exe main.cpp -lSDL2`
- 若启用了 `SDL2_image` / `SDL2_ttf` / `SDL2_mixer`，需要按 `docs/GameLib.SDL.md` 同时链接对应库。

## Requirements

- 单个头文件实现所有内容，没有任何依赖，通过 include 就能使用。
- 使用 Dev-Cpp 5 里自带的 gcc 4.9.2 也能编译。
- 动态加载方式避免编译引入额外的 -lwinmm -lgdi32 等参数。
- 包括：窗口管理，二级缓存，输入输出，图形绘制，精灵，声音，文字等功能。

## Code Constraints

修改 GameLib.h 时必须遵守以下约束：

- 只能使用 C++11 语法，不能用 C++14/17/20 特性（如 `auto` 返回类型推导、`std::make_unique`、结构化绑定等）。
- 必须兼容 GCC 4.9.2，避免该编译器不支持的特性。
- 所有 Windows API 通过 `LoadLibrary`/`GetProcAddress` 动态加载，禁止直接链接。
- 禁止引入外部依赖，所有功能在单个头文件内实现。
- 保持 `GameLib game; game.Open(...); while (...) { ... }` 的上手模型不变。
- 精灵和 Tilemap 使用整数 ID 管理，不引入对象生命周期负担。

## Guidelines

### 通用 agent 工作方式

- 先看 spec 再下手：公开 API 先看 `docs/Manual.md`，Win32 内部决策看 `docs/GameLib.md`，SDL 内部决策看 `docs/GameLib.SDL.md`。
- 搜索或 review 时优先忽略 `examples/` 和 `tests/` 下已有的 `.exe` / `.o` 编译产物，除非用户明确要分析构建输出。
- 若修改的是稳定行为、接口语义、回归入口或产品线边界，同一个改动里同步更新对应文档。

### 用 GameLib.h 做游戏

- 先完整阅读 docs/Manual.md（接口说明）和 docs/GameLib.md（设计思路），确保理解 GameLib.h 的用法。
- 阅读 assets/sprites.md 和 assets/sound.md，了解可用的图片和音效资源，优先使用现有素材。
- 参考 examples/ 目录下的示例代码（从 01_hello.cpp 到 19_clip_tilemap.cpp），它们按功能渐进排列，涵盖窗口、图形、精灵、动画、声音、Tilemap、裁剪矩形、字体、缩放与 tilemap 文件等主题。
- 游戏文件放在 examples/ 或 tests/ 目录下，通过 `#include "../GameLib.h"` 引入。

### 迭代 GameLib.h

- 先阅读 docs/GameLib.md，确认当前设计思路、技术约束、回归策略和最近已固化的实现决策；涉及核心子系统时再深入对应源码块。
- 若改动涉及公开 API 或稳定行为，同步检查 docs/Manual.md 和 docs/GameLib.md 是否需要更新。
- 修改后必须遵守 Code Constraints 中的所有约束。
- 修改后优先用相关 examples/ 或 tests/demo*.cpp 做回归验证，确保编译通过且功能正常。

### 迭代 GameLib.SDL.h

- `GameLib.SDL.h` 是独立 SDL 产品线，不替代 `GameLib.h` 的 Win32 零依赖主线；修改时不要把两者的约束混为一谈。
- 先阅读 `docs/GameLib.SDL.md`，确认 SDL 版的目标平台、可选依赖、兼容边界、当前实现状态和回归顺序。
- 修改 `GameLib.SDL.h` 时优先保持 `GameLib` 风格 API 与软件 framebuffer 语义不变。
- 若改动涉及稳定行为、构建方式或兼容边界，同步更新 `docs/GameLib.SDL.md`。
- 修改后优先用 `tests/sdldemo1.cpp` ~ `tests/sdldemo14.cpp` 做回归验证，分别覆盖基础绘制、基础图元与 primitive alpha、字体、音频、真实资产 Tilemap、完整游戏循环、经典碰撞/关卡清除、网格离散移动、接取判定，以及精灵基础展示、精灵帧动画与声音演示路径。
