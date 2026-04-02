# GameLib.h

**Single-header C++ game library for beginners — no SDL, no setup, just include and play.**

一个给孩子们写的 C++ 游戏库 —— 只有一个头文件，无需安装任何东西，包含即可开始做游戏。

---

## 它长什么样？

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
        game.DrawText(10, 10, "WASD to move!", COLOR_WHITE);
        game.Update();
        game.WaitFrame(60);
    }
    return 0;
}
```

编译：

```bash
g++ -o game.exe main.cpp -mwindows
```

没了。不需要配置项目，不需要下载 SDK，不需要链接一堆库。

---

## 为什么做这个？

市面上的游戏库（SDL、SFML、raylib）都很好，但对于刚接触 C++ 的孩子来说：

- SDL 要配置头文件路径、链接十几个 dll
- SFML 要用 CMake
- raylib 相对简单，但仍需要编译或下载预编译包

**GameLib 的目标是零门槛**：把 `GameLib.h` 拷到项目文件夹，写一个 `.cpp` 文件，点编译，游戏就跑起来了。

它专门为 **Dev C++**（很多学校编程课在用的 IDE）设计，兼容其自带的 GCC 4.9.2 编译器。当然，任何支持 C++11 的 Windows 编译器都可以用。

---

## 特性一览

### 零配置

- 单个头文件 `GameLib.h`，拷贝即用
- 不依赖 SDL / SFML / DirectX / OpenGL
- 只用 Windows 自带的 GDI，编译只需 `-mwindows`（gdi32/winmm 自动动态加载）
- 兼容 Dev C++ 自带的 GCC 4.9.2

### 开箱即用的绘图

- 画点、线、矩形、圆、三角形（描边和填充）
- 内嵌 8x8 像素点阵字体，支持所有可打印 ASCII 字符
- `DrawPrintf` 像 `printf` 一样在屏幕上格式化输出
- 所有图形算法自行实现（Bresenham 直线、中点圆、扫描线填充）

### 精灵系统

- 从 BMP 文件加载精灵（24-bit / 32-bit）
- 翻转、Color Key 透明、区域裁剪绘制
- 用整数 ID 管理，不需要理解指针和对象生命周期

### 键盘和鼠标

- `IsKeyDown` — 按住检测
- `IsKeyPressed` — 单次按下检测（按下瞬间触发一次）
- 鼠标位置和三键状态
- 预定义所有常用按键常量：`KEY_A`~`KEY_Z`、方向键、F1~F12

### 声音

- `PlayWAV` — 播放音效（WAV 格式，异步）
- `PlayMusic` / `StopMusic` — 播放背景音乐（MP3/MIDI，基于 MCI）
- 音效和音乐独立通道，互不干扰

### 游戏工具

- `Random(min, max)` — 随机数
- `RectOverlap` / `CircleOverlap` — 碰撞检测
- `Distance` — 两点距离
- `DrawGrid` / `FillCell` — 网格绘制（适合俄罗斯方块、棋盘类游戏）
- `GetDeltaTime` / `GetFPS` — 帧时间和帧率

---

## 快速上手

### 第一步：下载

把 `GameLib.h` 放到你的项目文件夹里。

### 第二步：写代码

创建一个 `main.cpp`：

```cpp
#include "GameLib.h"

int main()
{
    GameLib game;
    game.Open(800, 600, "Hello GameLib", true);

    while (!game.IsClosed()) {
        game.Clear(COLOR_DARK_BLUE);
        game.DrawTextScale(200, 250, "Hello, World!", COLOR_GOLD, 3);
        game.DrawText(280, 320, "Press ESC to exit", COLOR_GRAY);

        if (game.IsKeyPressed(KEY_ESCAPE))
            break;

        game.Update();
        game.WaitFrame(60);
    }
    return 0;
}
```

### 第三步：编译运行

**Dev C++**：新建项目 > 添加 `main.cpp` > 编译运行。

**命令行**：

```bash
g++ -o game.exe main.cpp -mwindows
```

---

## 更多示例

### 弹跳小球

```cpp
#include "GameLib.h"

int main()
{
    GameLib game;
    game.Open(640, 480, "Bouncing Ball", true);

    float x = 320, y = 240;
    float vx = 4, vy = 3;
    int r = 20;

    while (!game.IsClosed()) {
        // 移动
        x += vx;
        y += vy;

        // 碰壁反弹
        if (x - r < 0 || x + r > game.GetWidth())  vx = -vx;
        if (y - r < 0 || y + r > game.GetHeight()) vy = -vy;

        // 绘制
        game.Clear(COLOR_BLACK);
        game.FillCircle((int)x, (int)y, r, COLOR_RED);
        game.DrawPrintf(10, 10, COLOR_WHITE, "FPS: %.0f", game.GetFPS());
        game.Update();
        game.WaitFrame(60);
    }
    return 0;
}
```

### 键盘控制 + 鼠标跟踪

```cpp
#include "GameLib.h"

int main()
{
    GameLib game;
    game.Open(640, 480, "Input Demo", true);

    int px = 320, py = 240;

    while (!game.IsClosed()) {
        // 键盘控制方块
        if (game.IsKeyDown(KEY_A)) px -= 3;
        if (game.IsKeyDown(KEY_D)) px += 3;
        if (game.IsKeyDown(KEY_W)) py -= 3;
        if (game.IsKeyDown(KEY_S)) py += 3;

        // 鼠标位置
        int mx = game.GetMouseX();
        int my = game.GetMouseY();

        game.Clear(COLOR_DARK_GRAY);

        // 方块
        game.FillRect(px - 15, py - 15, 30, 30, COLOR_GREEN);

        // 鼠标十字线
        game.DrawLine(mx - 20, my, mx + 20, my, COLOR_YELLOW);
        game.DrawLine(mx, my - 20, mx, my + 20, COLOR_YELLOW);

        // 鼠标点击画圆
        if (game.IsMouseDown(MOUSE_LEFT)) {
            game.FillCircle(mx, my, 10, COLOR_RED);
        }

        game.DrawPrintf(10, 10, COLOR_WHITE, "Player: %d, %d", px, py);
        game.DrawPrintf(10, 22, COLOR_WHITE, "Mouse:  %d, %d", mx, my);
        game.Update();
        game.WaitFrame(60);
    }
    return 0;
}
```

### 随机星空

```cpp
#include "GameLib.h"

int main()
{
    GameLib game;
    game.Open(800, 600, "Starfield", true);

    // 生成 200 颗星星
    int sx[200], sy[200], speed[200];
    uint32_t colors[] = {COLOR_WHITE, COLOR_LIGHT_GRAY, COLOR_YELLOW, COLOR_CYAN};
    for (int i = 0; i < 200; i++) {
        sx[i] = GameLib::Random(0, 799);
        sy[i] = GameLib::Random(0, 599);
        speed[i] = GameLib::Random(1, 5);
    }

    while (!game.IsClosed()) {
        game.Clear(COLOR_BLACK);

        for (int i = 0; i < 200; i++) {
            sx[i] -= speed[i];
            if (sx[i] < 0) {
                sx[i] = 800;
                sy[i] = GameLib::Random(0, 599);
            }
            game.SetPixel(sx[i], sy[i], colors[speed[i] % 4]);
        }

        game.DrawText(250, 290, "Press ESC to exit", COLOR_GRAY);
        if (game.IsKeyPressed(KEY_ESCAPE)) break;

        game.Update();
        game.WaitFrame(60);
    }
    return 0;
}
```

更多示例见 `examples/` 目录。

---

## 示例程序

`examples/` 目录包含 13 个由浅入深的示例，逐步展示 GameLib 的各项功能。前 7 个纯代码绘图，复制即可编译运行；后 6 个涉及精灵、音效等外部资源（存放在 `assets/` 目录）。

编译任意示例：

```bash
g++ -o demo.exe examples/01_hello.cpp -mwindows
```

### 入门基础

| 示例 | 说明 | 学到什么 |
|------|------|----------|
| `01_hello.cpp` | Hello World | 游戏循环、窗口创建、文字绘制 |
| `02_moving_box.cpp` | 键盘控制方块移动 | 键盘输入、实时更新 |
| `03_shapes.cpp` | 所有图形绘制展示 | 线、矩形、圆、三角形的描边与填充 |
| `04_bouncing_ball.cpp` | 弹跳小球 | 浮点运动、碰壁反弹、DrawPrintf |

### 交互进阶

| 示例 | 说明 | 学到什么 |
|------|------|----------|
| `05_paint.cpp` | 简易画板 | 鼠标输入、SetPixel、按键切换颜色 |
| `06_catch_fruit.cpp` | 接水果小游戏 | Random、碰撞检测、计分 |
| `07_shooting_stars.cpp` | 简易射击 | 数组管理多对象、子弹发射、碰撞销毁 |

### 精灵与声音

| 示例 | 说明 | 学到什么 |
|------|------|----------|
| `08_sprite_demo.cpp` | 精灵加载与绘制 | LoadSpriteBMP、翻转、Region 裁剪 |
| `09_sprite_animation.cpp` | 精灵帧动画 | DrawSpriteRegion 实现 sprite sheet 动画 |
| `10_sound_demo.cpp` | 声音播放演示 | PlayWAV 音效、PlayMusic 背景音乐 |

### 完整小游戏

| 示例 | 说明 | 学到什么 |
|------|------|----------|
| `11_snake.cpp` | 贪吃蛇 | DrawGrid/FillCell、游戏状态机 |
| `12_breakout.cpp` | 打砖块 | 碰撞检测深度运用、多对象管理 |
| `13_space_shooter.cpp` | 太空射击 | 综合：精灵 + 音效 + 碰撞 + 滚动背景 + 计分 |

---

## API 速查

### 窗口

| 函数 | 说明 |
|------|------|
| `Open(w, h, title, center)` | 创建窗口，center=true 时居中 |
| `IsClosed()` | 窗口是否关闭 |
| `Update()` | 刷新画面、处理输入 |
| `WaitFrame(fps)` | 帧率控制 |
| `GetWidth()` / `GetHeight()` | 窗口尺寸 |
| `GetDeltaTime()` | 帧间隔（秒） |
| `GetFPS()` | 当前帧率 |
| `GetTime()` | 运行总时间（秒） |
| `SetTitle(title)` | 修改窗口标题 |

### 绘图

| 函数 | 说明 |
|------|------|
| `Clear(color)` | 清屏 |
| `SetPixel(x, y, color)` | 画点 |
| `GetPixel(x, y)` | 读点 |
| `DrawLine(x1, y1, x2, y2, color)` | 画线 |
| `DrawRect(x, y, w, h, color)` | 矩形边框 |
| `FillRect(x, y, w, h, color)` | 填充矩形 |
| `DrawCircle(cx, cy, r, color)` | 圆形边框 |
| `FillCircle(cx, cy, r, color)` | 填充圆 |
| `DrawTriangle(...)` | 三角形边框 |
| `FillTriangle(...)` | 填充三角形 |

### 文字

| 函数 | 说明 |
|------|------|
| `DrawText(x, y, text, color)` | 绘制文字 |
| `DrawNumber(x, y, number, color)` | 绘制整数 |
| `DrawTextScale(x, y, text, color, scale)` | 放大文字 |
| `DrawPrintf(x, y, color, fmt, ...)` | 格式化输出 |

### 精灵

| 函数 | 说明 |
|------|------|
| `CreateSprite(w, h)` | 创建空白精灵，返回 ID |
| `LoadSpriteBMP(filename)` | 从 BMP 加载精灵 |
| `FreeSprite(id)` | 释放精灵 |
| `DrawSprite(id, x, y)` | 绘制精灵 |
| `DrawSpriteEx(id, x, y, flags)` | 带翻转/透明绘制 |
| `DrawSpriteRegion(id, x, y, sx, sy, sw, sh)` | 绘制精灵子区域 |
| `SetSpritePixel(id, x, y, color)` | 修改精灵像素 |
| `GetSpritePixel(id, x, y)` | 读取精灵像素 |

精灵标志：`SPRITE_FLIP_H`（水平翻转）、`SPRITE_FLIP_V`（垂直翻转）、`SPRITE_COLORKEY`（品红色透明）

### 输入

| 函数 | 说明 |
|------|------|
| `IsKeyDown(key)` | 按键是否按住 |
| `IsKeyPressed(key)` | 按键是否刚按下（单次触发） |
| `GetMouseX()` / `GetMouseY()` | 鼠标位置 |
| `IsMouseDown(button)` | 鼠标按键是否按下 |

### 声音

| 函数 | 说明 |
|------|------|
| `PlayWAV(filename, loop)` | 播放音效 |
| `StopWAV()` | 停止音效 |
| `PlayMusic(filename, loop)` | 播放背景音乐（MP3/MIDI） |
| `StopMusic()` | 停止背景音乐 |
| `PlayBeep(freq, duration)` | 蜂鸣器 |

### 工具

| 函数 | 说明 |
|------|------|
| `Random(min, max)` | 随机数 |
| `RectOverlap(...)` | 矩形碰撞检测 |
| `CircleOverlap(...)` | 圆形碰撞检测 |
| `PointInRect(px, py, x, y, w, h)` | 点在矩形内 |
| `Distance(x1, y1, x2, y2)` | 两点距离 |
| `DrawGrid(x, y, rows, cols, size, color)` | 画网格 |
| `FillCell(gx, gy, row, col, size, color)` | 填充网格单元 |

### 颜色常量

```
COLOR_BLACK    COLOR_WHITE     COLOR_RED       COLOR_GREEN     COLOR_BLUE
COLOR_YELLOW   COLOR_CYAN      COLOR_MAGENTA   COLOR_ORANGE    COLOR_PINK
COLOR_PURPLE   COLOR_GRAY      COLOR_DARK_GRAY COLOR_LIGHT_GRAY
COLOR_DARK_RED COLOR_DARK_GREEN COLOR_DARK_BLUE COLOR_SKY_BLUE
COLOR_BROWN    COLOR_GOLD      COLOR_TRANSPARENT
```

自定义颜色：`COLOR_RGB(r, g, b)` 或 `COLOR_ARGB(a, r, g, b)`

---

## 适合做什么？

- 太空射击 (Space Shooter)
- 俄罗斯方块 (Tetris)
- 贪吃蛇 (Snake)
- 打砖块 (Breakout)
- 走迷宫 (Maze)
- 接水果 (Fruit Catcher)
- 弹幕游戏 (Bullet Hell)
- 画板程序 (Paint)
- 任何你能想到的 2D 小游戏

---

## 技术细节

- **双缓冲**：所有绘制写入内存帧缓冲（`uint32_t*` ARGB），`Update()` 时通过 `SetDIBitsToDevice` 一次性刷新到窗口，无闪烁
- **图形自实现**：不调用任何 GDI 绘图函数（`LineTo`、`Ellipse` 等），所有算法从零实现。唯一使用的 GDI 调用是 `SetDIBitsToDevice`
- **精确窗口尺寸**：`Open()` 保证客户区严格等于请求的宽高（含高 DPI 二次校正）
- **stb 风格单头文件**：默认 include 即启用实现；多文件项目可用 `GAMELIB_NO_IMPLEMENTATION` 控制

---

## 环境要求

- Windows 7 及以上
- 任何支持 C++11 的编译器（GCC 4.9+、MSVC 2015+、Clang）
- 推荐 IDE：Dev C++、Visual Studio、Code::Blocks

---

## 协议

MIT License. 随便用。
