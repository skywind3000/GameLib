# AGENTS.md

本项目用来帮助 C++ 初学者编写游戏，提供简单的 API，迅速体验到乐趣。

## Structure

```
- assets/        # 图片和音效资源
- docs/          # 文档
- examples/      # 01~15 统一示例，兼容 Win32 和 SDL 双后端
- GameLib.h      # Win32 主线，单头文件，零外部依赖
- GameLib.SDL.h  # SDL 产品线，跨平台单头文件，依赖 SDL2
- SDL2PORT.md    # SDL 版快速编译指南（依赖、命令、限制）
```

## Documentation

| 文档 | 用途 | 何时需要读 |
|------|------|-----------|
| `docs/manual.md` | GameLib.h 公开 API 接口说明 | 做游戏、改公开 API 时 |
| `docs/GameLib.md` | GameLib.h 内部设计思路与技术规格 | 改 GameLib.h 内部实现时 |
| `docs/GameLib.SDL.md` | GameLib.SDL.h 的规格、兼容边界与实现决策 | 改 GameLib.SDL.h 时 |
| `docs/quickref.md` | GameLib.h 的快速 API 列表 | 做游戏，改公开 API 时 |
| `docs/cheatsheet.html` | A4 横版打印速查表（数据来源：`docs/quickref.md`） | quickref 变动时需同步更新 |
| `SDL2PORT.md` | SDL 版编译命令与当前限制（简版） | 编译 SDL 版时 |
| `assets/sprites.md` | 精灵图资源索引（尺寸、用途、瓦片集详情） | 做游戏需要图片素材时 |
| `assets/sound.md` | 音效资源索引（WAV 参数和用途） | 做游戏需要音效素材时 |

以上文档是当前稳定行为的权威来源；如果实现发生稳定变化，应同步更新对应文档。

## Examples

`examples/` 下的 15 个示例通过预处理器自动选择 Win32 或 SDL 后端，按功能渐进排列：

| 示例 | 主题 |
|------|------|
| `01_hello.cpp` | 窗口创建、文字显示 |
| `02_movement.cpp` | 键盘移动、物理反弹、碰撞 |
| `03_shapes.cpp` | 图元绘制（椭圆、三角形、Alpha 混合） |
| `04_paint.cpp` | 鼠标输入、简易画板 |
| `05_sprites.cpp` | 精灵基础、帧动画、方向移动 |
| `06_sound.cpp` | 音效、背景音乐、PlayBeep |
| `07_shooting.cpp` | 射击、子弹、碰撞检测 |
| `08_breakout.cpp` | 打砖块（碰撞、清版） |
| `09_snake.cpp` | 贪吃蛇（网格离散移动） |
| `10_tilemap.cpp` | 双层 Tilemap、视差滚动、摄像机跟随 |
| `11_font_text.cpp` | TTF 字体渲染、文本测量、消息框 |
| `12_sprite_transform.cpp` | 精灵缩放、旋转 |
| `13_clip_rect.cpp` | 裁剪矩形 |
| `14_space_shooter.cpp` | 完整小游戏（敌人、编队、计分） |
| `15_ui_controls.cpp` | Button、Checkbox、RadioBox、ToggleButton |

## Build

### Win32 主线（GameLib.h）

```bash
g++ -o output.exe source.cpp -mwindows
```

- 不需要 `-lwinmm -lgdi32` 等参数，GameLib.h 通过动态加载解决所有依赖。
- 源文件通过 `#include "GameLib.h"` 引入即可。

### SDL 产品线（GameLib.SDL.h）

最小命令（只启用 SDL2 核心）：

```bash
g++ -std=c++11 -O2 -o game.exe main.cpp -lSDL2
```

启用 `SDL2_image` / `SDL2_ttf` / `SDL2_mixer` 时需要同时链接对应库。完整编译命令见 `SDL2PORT.md`。

## Code Constraints

### GameLib.h（Win32 主线）

- 只能使用 C++11 语法，不能用 C++14/17/20 特性（如 `auto` 返回类型推导、`std::make_unique`、结构化绑定等）。
- 必须兼容 GCC 4.9.2（Dev-C++ 5 自带）。
- 所有 Windows API 通过 `LoadLibrary`/`GetProcAddress` 动态加载，禁止直接链接。
- 禁止引入外部依赖，所有功能在单个头文件内实现。
- 保持 `GameLib game; game.Open(...); while (...) { ... }` 的上手模型不变。
- 精灵和 Tilemap 使用整数 ID 管理，不引入对象生命周期负担。

### GameLib.SDL.h（SDL 产品线）

- C++11 基线，不要求兼容 GCC 4.9.2。
- 允许依赖 SDL2 及其可选扩展（SDL2_image / SDL2_ttf / SDL2_mixer），但禁止引入 SDL 生态外的依赖。
- 保持软件 framebuffer 语义：所有绘制写入 `uint32_t* _framebuffer`，SDL renderer 只负责最终提交。
- 公开 API 风格与 GameLib.h 尽量一致，两者不能在同一翻译单元中共存。
- 两条产品线约束独立，修改时不要混为一谈。

## Guidelines

### 通用 agent 工作方式

- 先看上面 Documentation 表格中对应场景的文档再下手，不需要每次都读所有文档。
- 搜索或 review 时忽略 `examples/` 下的 `.exe` / `.o` 编译产物。
- 若修改涉及稳定行为、接口语义或产品线边界，同一个改动里同步更新对应文档。

### 任务分类与行动路线

收到用户请求后，先判断属于哪类任务，再执行对应流程：

| 任务类型 | 判断标准 | 行动路线 |
|----------|---------|---------|
| 用 GameLib.h 写新游戏 | 用户描述想做一个游戏/功能 | → 见"用 GameLib.h 做游戏"流程 |
| 迭代 GameLib.h 内部实现 | 改动 GameLib.h 文件本身 | → 见"迭代 GameLib.h"流程 |
| 迭代 GameLib.SDL.h | 改动 GameLib.SDL.h 文件本身 | → 见"迭代 GameLib.SDL.h"流程 |
| 新增/修改公开 API | 涉及 `GameLib` 类的公开方法或成员 | 两条线都要检查，双线同步 |
| 改文档/示例/资源 | 只改 `.md`、`examples/`、`assets/` | 读对应文档，改完更新同一文档 |

不确定时，先问用户再动手，不要猜。

### 用 GameLib.h 做游戏

1. **先理解需求**：问用户想要什么类型的游戏或功能，确认核心玩法。
2. **读 API 文档**：阅读 `docs/manual.md` 或 `docs/quickref.md` 了解公开 API。
3. **选示例起点**：参考上方 Examples 表格，找到与目标功能最接近的示例复制或修改。
4. **优先用现有素材**：查 `assets/sprites.md` 和 `assets/sound.md`，优先使用已有图片和音效。
5. **先跑最小原型**：不要一开始就写完整游戏，先让基本功能跑起来（显示窗口、能操作），再逐步加功能。
6. **文件位置**：游戏文件放在 `examples/` 目录下，通过 `#include "../GameLib.h"` 引入。

### 迭代 GameLib.h

1. 先阅读 `docs/GameLib.md` 中与改动相关的章节（不需要每次通读全文）。
2. 若改动涉及公开 API，同步检查 `docs/manual.md` 和 `docs/GameLib.md`。
3. 修改后必须遵守上方 Code Constraints（Win32 主线）。
4. 用 `examples/` 下的相关示例做回归验证（参考 Examples 表格选择对应主题）。

### 迭代 GameLib.SDL.h

1. 先阅读 `docs/GameLib.SDL.md` 中与改动相关的章节。
2. 若改动涉及稳定行为或构建方式，同步更新 `docs/GameLib.SDL.md`。
3. 修改后必须遵守上方 Code Constraints（SDL 产品线）。
4. 用 `examples/` 下的统一示例做回归验证（参考 Examples 表格选择对应主题）。

### 每次改动最小清单

1. **读**：读相关文档（见 Documentation 表和任务分类表）
2. **写**：写/改代码（遵守 Code Constraints）
3. **验**：编译对应示例验证（见回归验证速查）
4. **更**：更新相关文档（如果语义/行为/接口变了）

### 禁止事项

- **禁止使用 C++14+ 特性**：`auto` 返回类型推导、`std::make_unique`、结构化绑定、`if constexpr` 等。
- **禁止混用两条产品线**：不要在同一个 `.cpp` 文件中同时 include `GameLib.h` 和 `GameLib.SDL.h`。
- **禁止直接链接 Windows API**：必须通过 `LoadLibrary`/`GetProcAddress` 动态加载。
- **禁止引入外部依赖**：GameLib.h 零外部依赖，所有功能在单个头文件内实现。
- **不要用 `git add -A` 或 `git add .`**：可能误提交编译产物或敏感文件，按文件名逐个 `git add`。
- **不要提交编译产物**：忽略 `examples/` 下的 `.exe`、`.o` 等文件。
- **不要假设 SDL 版和 Win32 版行为完全一致**：修改一条线时，另一线需独立验证。
- **不要在没有读过对应文档的情况下直接改代码**：先看 Documentation 表格找到该读的文档。

### 回归验证速查

改动涉及哪个领域，优先编译并运行对应示例：

- **图形与图元**：`01`、`03`、`13`
- **精灵与动画**：`05`、`12`
- **字体与文本**：`11`
- **音频**：`06`
- **Tilemap**：`10`
- **输入与碰撞**：`02`、`04`、`07`
- **游戏循环（综合）**：`08`、`09`、`14`
- **UI 控件**：`15`
