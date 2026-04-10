# AGENTS.md

本项目用来帮助 C++ 初学者编写游戏，提供简单的 API，迅速体验到乐趣。

## Structure

```
- assets/    # images and audios
- docs/      # documentations
- examples/  # examples
- tests/     # tests
- GameLib.h  # main source
- AGENTS.md  # this file
- README.md  # project home page
```

## Documentation

```
- docs/GameLib.md   # GameLib.h 的技术规格，如果要用它做游戏请先阅读
- docs/Manual.md    # GameLib.h 的接口说明，如果要用它做游戏请先阅读
```

## Requirements

- 单个头文件实现所有内容，没有任何依赖，通过 include 就能使用。
- 使用 Dev-Cpp 5 里自带的 gcc 4.9.2 也能编译。
- 动态加载方式避免编译引入额外的 -lwinmm -lgdi32 等参数。
- 包括：窗口管理，二级缓存，输入输出，图形绘制，精灵，声音，文字等功能。

## Guidelines

- 如果要用 GameLib.h 实现游戏，请先完整阅读 GameLib.h 这个代码，以及 docs 目录下面的  Manual.md （GameLib.h 的接口介绍）和 GameLib.md （里面包含 GameLib.h 的设计思路）两个文件，确保完全理解 GameLib.h 的设计逻辑和用法；
