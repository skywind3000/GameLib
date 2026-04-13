# AGENTS.md

本项目用来帮助 C++ 初学者编写游戏，提供简单的 API，迅速体验到乐趣。

## Structure

```
- assets/    # images and audios
- docs/      # documentations
- examples/  # examples (01~16 渐进式示例，可作为参考和回归测试)
- tests/     # tests
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

## Assets

```
- assets/sprites.md # 精灵图资源索引，包含所有图片的尺寸、用途和瓦片集详情
- assets/sound.md   # 音效资源索引，包含所有 WAV 音效的参数和用途说明
```

## Build

编译只需要一条命令，不需要 Makefile 或其他构建工具：

```bash
g++ -o output.exe source.cpp -mwindows
```

- `-mwindows` 使程序以 Windows 窗口模式运行（不弹出控制台）。
- 不需要 `-lwinmm -lgdi32` 等链接参数，GameLib.h 通过动态加载解决所有依赖。
- 源文件通过 `#include "GameLib.h"` 引入即可，确保路径正确。

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

### 用 GameLib.h 做游戏

- 先完整阅读 docs/Manual.md（接口说明）和 docs/GameLib.md（设计思路），确保理解 GameLib.h 的用法。
- 阅读 assets/sprites.md 和 assets/sound.md，了解可用的图片和音效资源，优先使用现有素材。
- 参考 examples/ 目录下的示例代码（从 01_hello.cpp 到 16_playsound.cpp），它们按功能渐进排列，涵盖窗口、图形、精灵、动画、声音、Tilemap、字体等主题。
- 游戏文件放在 examples/ 或 tests/ 目录下，通过 `#include "../GameLib.h"` 引入。

### 迭代 GameLib.h

- 先完整阅读 GameLib.h 源码，理解内部架构（动态加载层、GDI 双缓冲、精灵管理、Tilemap 等模块）。
- 阅读 docs/GameLib.md，了解当前设计思路、技术约束和已合并的接口演进。
- 修改后必须遵守 Code Constraints 中的所有约束。
- 修改后用 examples/ 下的示例进行回归验证，确保编译通过且功能正常。

### 迭代 GameLib.SDL.h

- `GameLib.SDL.h` 是独立 SDL 产品线，不替代 `GameLib.h` 的 Win32 零依赖主线；修改时不要把两者的约束混为一谈。
- 先阅读 `docs/GameLib.SDL.md`，确认 SDL 版的目标平台、可选依赖、兼容边界和当前实现决策。
- 修改 `GameLib.SDL.h` 时优先保持 `GameLib` 风格 API 与软件 framebuffer 语义不变。
- 修改后优先用 `tests/sdldemo1.cpp` ~ `tests/sdldemo4.cpp` 做回归验证，分别覆盖基础绘制、字体与音频路径。
