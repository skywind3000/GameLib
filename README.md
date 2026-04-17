# GameLib.h

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE) [![C++ Standard](https://img.shields.io/badge/C%2B%2B-11-blue.svg)](https://en.cppreference.com/w/cpp/11) [![Platform](https://img.shields.io/badge/Platform-Windows%20%7C%20Linux%20%7C%20macOS-0078d4.svg)](https://github.com/skywind3000/GameLib) [![Header Only](https://img.shields.io/badge/Header--Only-single%20file-green.svg)](GameLib.h) [![No Dependencies](https://img.shields.io/badge/Dependencies-zero-brightgreen.svg)](https://github.com/skywind3000/GameLib) [![GCC 4.9+](https://img.shields.io/badge/GCC-4.9%2B-orange.svg?logo=gnu)](https://gcc.gnu.org/)

给 C++ 初学者的游戏开发库 —— 单个头文件，零配置，拷贝即用。

学 C++ 最痛苦的是什么？几周下来只能在黑窗口里 `cout` 一行字。想用 SDL、SFML？光配环境就劝退一半人。GameLib 就是为了干掉这段痛苦：把 `GameLib.h` 丢进项目文件夹，写句 `#include "GameLib.h"`，十行代码就能画出能用键盘操控的彩色圆。窗口、绘图、键盘、鼠标、精灵、声音、碰撞检测全都有，不需要下载 SDK，不需要配置路径，不需要链接任何库。

![](assets/demo.png)


## 十行代码做个游戏

```cpp
#include "GameLib.h"

int main()
{
    GameLib game;
    game.Open(640, 480, "My Game", true);

    int x = 320, y = 240;

    while (!game.IsClosed()) {
        if (game.IsKeyDown(KEY_LEFT))  x -= 3;
        if (game.IsKeyDown(KEY_RIGHT)) x += 3;
        if (game.IsKeyDown(KEY_UP))    y -= 3;
        if (game.IsKeyDown(KEY_DOWN))  y += 3;

        game.Clear(COLOR_BLACK);
        game.FillCircle(x, y, 15, COLOR_CYAN);
        game.DrawText(10, 10, "Up/Down/Left/Right to move!", COLOR_WHITE);
        game.Update();
        game.WaitFrame(60);
    }
    return 0;
}
```

![](https://skywind3000.github.io/images/p/gamelib/demo1.png)


## 快速上手

**第一步** — 下载 `GameLib.h`，放到项目文件夹。

**第二步** — 创建 `main.cpp`，写上方的代码。

**第三步** — 编译运行：

```bash
g++ main.cpp -o game.exe -mwindows
```

Dev C++ 用户：新建项目 > 添加 `main.cpp` > 编译运行，完事。

不需要 `-lgdi32 -lwinmm` 等任何链接参数，所有依赖由库内动态加载。


## 特性亮点

- **零配置** — 单头文件拷贝即用，无需安装/链接任何库，兼容 GCC 4.9.2（Dev C++）
- **开箱绘图** — 点、线、矩形、圆、椭圆、三角（描边+填充），内置 8×8 像素字体，支持可缩放字体和 UTF-8
- **精灵系统** — 加载 PNG/JPG/BMP/GIF，帧动画、翻转、缩放、旋转、Alpha 混合，整数 ID 管理
- **键盘鼠标** — 按住/按下/松开检测，鼠标三键+滚轮，窗口失焦暂停
- **声音** — WAV 音效 + MP3/MIDI 背景音乐，独立通道互不干扰
- **碰撞检测** — 矩形重叠、圆碰撞、两点距离
- **Tilemap** — 双层瓦片地图、视差滚动、只渲染可见区域，保存/载入 `.glm` 数据
- **窗口缩放** — resizable 窗口自动拉伸，鼠标坐标自动映射回逻辑坐标
- **场景管理** — 整数标识场景，`switch` 即可实现菜单→游戏→结算切换
- **存档读写** — `SaveInt/LoadInt` 等一行代码存取，纯文本 `key=value` 格式
- **UI 控件** — Button、Checkbox、RadioBox、ToggleButton，即时模式零依赖
- **跨平台** — SDL2 版 `GameLib.SDL.h` 支持 Linux/macOS（详见 [SDL2PORT.md](SDL2PORT.md))


## 适合做什么？

- 太空射击 / 横版卷轴 / 俄罗斯方块 / 贪吃蛇 / 打砖块
- 走迷宫 / 接水果 / 弹幕游戏
- 回合制 RPG / 视觉小说 / 地图编辑器 / 画板程序
- 课程作业演示（零配置交付）
- 任何 2D 小游戏或互动程序


## 示例程序

`examples/` 目录包含 15 个由浅入深的示例，每个兼容 Win32 和 SDL 两条产品线：

### 入门基础

| 示例 | 说明 | 学到什么 |
|-|-|-|
| `01_hello.cpp` | Hello World | 游戏循环、窗口创建、文字绘制 |
| `02_movement.cpp` | 键盘移动 + 弹跳小球 | 键盘输入、GetDeltaTime、碰壁反弹 |
| `03_shapes.cpp` | 所有图形绘制展示 | 线、矩形、圆、椭圆、三角的描边与填充 |
| `04_paint.cpp` | 简易画板 | 鼠标输入、滚轮调笔刷、失焦暂停 |

### 精灵与声音

| 示例 | 说明 | 学到什么 |
|-|-|-|
| `05_sprites.cpp` | 精灵基础 + 帧动画 | CreateSprite、方向动画 |
| `06_sound.cpp` | 声音播放演示 | PlayBeep、PlayWAV、PlayMusic |
| `07_shooting.cpp` | 简易射击 | 子弹发射、碰撞销毁 |

### 完整小游戏

| 示例 | 说明 | 学到什么 |
|-|-|-|
| `08_breakout.cpp` | 打砖块 | 碰撞检测、多对象管理 |
| `09_snake.cpp` | 贪吃蛇 | DrawGrid/FillCell、状态机 |

### Tilemap 与文字

| 示例 | 说明 | 学到什么 |
|-|-|-|
| `10_tilemap.cpp` | 双层视差卷轴 | Tilemap、视差滚动 |
| `11_font_text.cpp` | 可缩放字体与 UI | DrawTextFont、ShowMessage |

### 高级特性

| 示例 | 说明 | 学到什么 |
|-|-|-|
| `12_sprite_transform.cpp` | 精灵缩放 + 旋转 | DrawSpriteScaled/Rotated |
| `13_clip_rect.cpp` | 裁剪矩形 | SetClip/ClearClip |
| `14_space_shooter.cpp` | 太空射击 | 综合实战 |
| `15_ui_controls.cpp` | UI 控件演示 | Button、Checkbox、RadioBox |

编译任意示例：

```bash
g++ -o 03_shapes.exe examples/03_shapes.cpp -mwindows
```


## 环境要求

**Win32 主线**：Windows 7+，任何 C++11 编译器（GCC 4.9+ / MSVC 2015+ / Clang），推荐 Dev C++、Visual Studio。

**SDL 版**：Linux/macOS/Windows，需要 SDL2 及可选扩展（详见 [SDL2PORT.md](SDL2PORT.md)）。


## 文档入口

- [docs/quickref.md](docs/quickref.md) - API 快速检索；
- [docs/manual.md](docs/manual.md) - 完整的接口说明；


## 资源推荐

想找游戏素材？试试这些：

- 老游戏精灵：https://www.spriters-resource.com
- 免费像素宝库：https://opengameart.org/
- 像素风格资源：http://www.charas-project.net/resources.php?lang=en
- 免费图片/音频/字体：https://craftpix.net/tags/pixel-art-game-assets-download
- PyGame 资源页：https://www.pygame.org/wiki/resources


## 协议

MIT License. 随便用。