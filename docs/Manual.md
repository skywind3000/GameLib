## API 速查

### 窗口

| 函数                         | 说明                         |
| ---------------------------- | ---------------------------- |
| `Open(w, h, title, center)`  | 创建窗口，center=true 时居中 |
| `IsClosed()`                 | 窗口是否关闭                 |
| `Update()`                   | 刷新画面、处理输入           |
| `WaitFrame(fps)`             | 帧率控制                     |
| `GetWidth()` / `GetHeight()` | 窗口尺寸                     |
| `GetDeltaTime()`             | 帧间隔（秒，返回 `double`）  |
| `GetFPS()`                   | 当前帧率（返回 `double`）    |
| `GetTime()`                  | 运行总时间（秒，返回 `double`） |
| `SetTitle(title)`            | 修改窗口标题                 |
| `ShowFps(show)`              | 是否在标题栏显示实时 FPS     |
| `ShowMouse(show)`            | 显示或隐藏窗口内鼠标光标     |
| `ShowMessage(text, title, buttons)` | 弹出消息框，支持 `OK` 或 `YES/NO` |

### 绘图

| 函数                              | 说明       |
| --------------------------------- | ---------- |
| `Clear(color)`                    | 清屏（受当前裁剪矩形约束） |
| `SetPixel(x, y, color)`           | 画点（支持 Alpha 混合） |
| `GetPixel(x, y)`                  | 读点       |
| `SetClip(x, y, w, h)`             | 设置裁剪矩形 |
| `ClearClip()`                     | 清除裁剪，恢复整屏可见 |
| `GetClip(int*, int*, int*, int*)` | 读取当前有效裁剪矩形 |
| `GetClipX()` / `GetClipY()` / `GetClipW()` / `GetClipH()` | 读取当前有效裁剪矩形分量 |
| `DrawLine(x1, y1, x2, y2, color)` | 画线（支持 Alpha 混合） |
| `DrawRect(x, y, w, h, color)`     | 矩形边框（支持 Alpha 混合） |
| `FillRect(x, y, w, h, color)`     | 填充矩形（支持 Alpha 混合） |
| `DrawCircle(cx, cy, r, color)`    | 圆形边框（支持 Alpha 混合） |
| `FillCircle(cx, cy, r, color)`    | 填充圆（支持 Alpha 混合） |
| `DrawEllipse(cx, cy, rx, ry, color)` | 椭圆边框（支持 Alpha 混合） |
| `FillEllipse(cx, cy, rx, ry, color)` | 填充椭圆（支持 Alpha 混合） |
| `DrawTriangle(x1, y1, x2, y2, x3, y3, color)` | 三角形边框（支持 Alpha 混合） |
| `FillTriangle(x1, y1, x2, y2, x3, y3, color)` | 填充三角形（支持 Alpha 混合） |

`SetClip` 会把传入矩形与当前屏幕范围自动求交。`w <= 0 || h <= 0`，或求交后为空时，表示“无可见区域”，此后 `Clear`、图元、内置字体、`DrawTextFont`、精灵、Tilemap 等所有写入屏幕帧缓冲的绘制函数都不生效。`GetClip*` 返回的是求交后的有效裁剪矩形。

### 文字

| 函数                                      | 说明       |
| ----------------------------------------- | ---------- |
| `DrawText(x, y, text, color)`             | 绘制文字   |
| `DrawNumber(x, y, number, color)`         | 绘制整数   |
| `DrawTextScale(x, y, text, color, scale)` | 放大文字   |
| `DrawPrintf(x, y, color, fmt, ...)`       | 格式化输出 |
| `DrawTextFont(x, y, text, color, fontSize)`   | 用默认字体绘制文字 |
| `DrawTextFont(x, y, text, color, fontName, fontSize)` | 用指定字体绘制文字 |
| `DrawPrintfFont(x, y, color, fontSize, fmt, ...)` | 用默认字体格式化输出 |
| `DrawPrintfFont(x, y, color, fontName, fontSize, fmt, ...)` | 用指定字体格式化输出 |
| `GetTextWidthFont(text, fontSize)` / `GetTextHeightFont(text, fontSize)` | 用默认字体测量文字尺寸 |
| `GetTextWidthFont(text, fontName, fontSize)` / `GetTextHeightFont(text, fontName, fontSize)` | 用指定字体测量文字尺寸 |

### 精灵

| 函数                                         | 说明                            |
| -------------------------------------------- | ------------------------------- |
| `CreateSprite(w, h)`                         | 创建空白精灵，返回 ID           |
| `LoadSprite(filename)`                       | 加载图片精灵（PNG/JPG/BMP/GIF/TIFF，路径按 UTF-8） |
| `LoadSpriteBMP(filename)`                    | 从 BMP 加载精灵（8/24/32-bit，路径按 UTF-8）  |
| `FreeSprite(id)`                             | 释放精灵                        |
| `DrawSprite(id, x, y)`                       | 绘制精灵                        |
| `DrawSpriteEx(id, x, y, flags)`              | 带翻转/透明/Alpha混合绘制       |
| `DrawSpriteRegion(id, x, y, sx, sy, sw, sh)` | 绘制精灵子区域                  |
| `DrawSpriteRegionEx(id, x, y, sx, sy, sw, sh, flags)` | 带 flags 绘制精灵子区域 |
| `DrawSpriteScaled(id, x, y, w, h, flags = 0)` | 按目标尺寸缩放绘制精灵        |
| `DrawSpriteFrame(id, x, y, frameW, frameH, frameIndex, flags = 0)` | 按帧号绘制 sprite sheet 中的帧 |
| `DrawSpriteFrameScaled(id, x, y, frameW, frameH, frameIndex, w, h, flags = 0)` | 按帧号缩放绘制 sprite sheet 中的帧 |
| `SetSpritePixel(id, x, y, color)`            | 修改精灵像素                    |
| `GetSpritePixel(id, x, y)`                   | 读取精灵像素                    |
| `GetSpriteWidth(id)` / `GetSpriteHeight(id)` | 读取精灵尺寸                    |
| `SetSpriteColorKey(id, color)`               | 设置该精灵的 Color Key          |
| `GetSpriteColorKey(id)`                      | 读取该精灵的 Color Key          |

精灵标志：`SPRITE_FLIP_H`（水平翻转）、`SPRITE_FLIP_V`（垂直翻转）、`SPRITE_COLORKEY`（按该精灵当前 Color Key 透明，默认品红色）、`SPRITE_ALPHA`（Alpha 混合）

默认的 `DrawSprite(id, x, y)` 走不透明快路径；如果素材依赖透明孔洞，请显式传 `SPRITE_COLORKEY` 或 `SPRITE_ALPHA`。

### 输入

| 函数                          | 说明                       |
| ----------------------------- | -------------------------- |
| `IsKeyDown(key)`              | 按键是否按住               |
| `IsKeyPressed(key)`           | 按键是否刚按下（单次触发） |
| `IsKeyReleased(key)`          | 按键是否刚松开（单次触发） |
| `GetMouseX()` / `GetMouseY()` | 鼠标位置                   |
| `IsMouseDown(button)`         | 鼠标按键是否按下           |
| `IsMousePressed(button)`      | 鼠标按键是否刚按下（单次触发） |
| `IsMouseReleased(button)`     | 鼠标按键是否刚松开（单次触发） |
| `GetMouseWheelDelta()`        | 自上次 `Update()` 以来累计的滚轮增量 |
| `IsActive()`                  | 窗口当前是否处于激活状态   |

### 声音

| 函数                        | 说明                     |
| --------------------------- | ------------------------ |
| `PlayWAV(filename, loop)`   | 播放音效，成功返回 `true` |
| `StopWAV()`                 | 停止音效                 |
| `PlayMusic(filename, loop)` | 播放背景音乐（MP3/MIDI/WAV；按扩展名选择 MCI 类型，MIDI 循环走 notify 重播），成功返回 `true` |
| `StopMusic()`               | 停止背景音乐             |
| `IsMusicPlaying()`          | 当前音乐是否处于播放状态 |
| `PlayBeep(freq, duration)`  | 蜂鸣器                   |

### 工具

| 函数                                      | 说明         |
| ----------------------------------------- | ------------ |
| `Random(min, max)`                        | 随机数       |
| `RectOverlap(...)`                        | 矩形碰撞检测 |
| `CircleOverlap(...)`                      | 圆形碰撞检测 |
| `PointInRect(px, py, x, y, w, h)`         | 点在矩形内   |
| `Distance(x1, y1, x2, y2)`                | 两点距离（返回 `float`） |
| `DrawGrid(x, y, rows, cols, size, color)` | 画网格       |
| `FillCell(gx, gy, row, col, size, color)` | 填充网格单元 |

### Tilemap

| 函数                                             | 说明                            |
| ------------------------------------------------ | ------------------------------- |
| `CreateTilemap(cols, rows, tileSize, tilesetId)` | 创建瓦片地图，返回 ID           |
| `SaveTilemap(filename, mapId)`                   | 保存地图到纯文本 `.glm` 文件    |
| `LoadTilemap(filename, tilesetId)`               | 从纯文本 `.glm` 文件载入地图    |
| `FreeTilemap(mapId)`                             | 释放地图                        |
| `SetTile(mapId, col, row, tileId)`               | 设置瓦片（-1=空）               |
| `GetTile(mapId, col, row)`                       | 读取瓦片                        |
| `GetTilemapCols(mapId)` / `GetTilemapRows(mapId)` | 读取地图列数和行数             |
| `GetTileSize(mapId)`                             | 读取地图瓦片尺寸                |
| `WorldToTileCol(mapId, x)` / `WorldToTileRow(mapId, y)` | 像素坐标转瓦片坐标      |
| `GetTileAtPixel(mapId, x, y)`                    | 按像素位置读取瓦片              |
| `FillTileRect(mapId, col, row, cols, rows, tileId)` | 批量填充矩形区域           |
| `ClearTilemap(mapId, tileId = -1)`               | 用指定瓦片填满整张地图（默认 `-1` 为清空） |
| `DrawTilemap(mapId, x, y, flags)`                | 绘制地图（支持 ColorKey/Alpha） |

tileset 是一张普通精灵（`LoadSprite` / `CreateSprite`），按 `tileSize` 自动切分。`WorldToTileCol/Row` 对负坐标也按向下取整换算。`DrawTilemap` 默认走不透明快路径，只绘制当前裁剪矩形内可见的瓦片；传 `(-cameraX, -cameraY)` 即可实现卷轴，如需透明孔洞请显式传 `SPRITE_COLORKEY` 或 `SPRITE_ALPHA`。Tilemap 数据允许保存超出当前 tileset 范围的非负 `tileId`；`SetTile` / `FillTileRect` / `ClearTilemap` / `LoadTilemap` 不会因为它大于当前 tileset 而拒绝，`DrawTilemap` 会在绘制时按当前 tileset 实际尺寸自动跳过这些格子。`-1` 仍是唯一合法的空瓦片值，其他负值仍视为无效。`.glm` 是纯文本格式：第一行固定 `GLM1`，第二行是 `tileSize rows cols`，后续每行一行瓦片数据，使用空格或 Tab 分隔。载入时每行超过 `cols` 的数据会忽略，不足的部分补 `-1`；超过 `rows` 的行忽略，不足的行补 `-1`。文件里不记录 tileset 路径，载入时由调用者提供 `tilesetId`。

消息框按钮类型：`MESSAGEBOX_OK`、`MESSAGEBOX_YESNO`。返回值：`MESSAGEBOX_RESULT_OK`、`MESSAGEBOX_RESULT_YES`、`MESSAGEBOX_RESULT_NO`。

### 颜色常量

```
COLOR_BLACK    COLOR_WHITE     COLOR_RED       COLOR_GREEN     COLOR_BLUE
COLOR_YELLOW   COLOR_CYAN      COLOR_MAGENTA   COLOR_ORANGE    COLOR_PINK
COLOR_PURPLE   COLOR_GRAY      COLOR_DARK_GRAY COLOR_LIGHT_GRAY
COLOR_DARK_RED COLOR_DARK_GREEN COLOR_DARK_BLUE COLOR_SKY_BLUE
COLOR_BROWN    COLOR_GOLD      COLOR_TRANSPARENT
```

自定义颜色：`COLOR_RGB(r, g, b)` 或 `COLOR_ARGB(a, r, g, b)`。当 `a < 255` 时，`SetPixel` 和图元绘制会按 Alpha 做混合。

颜色拆分宏：`COLOR_GET_A(color)`、`COLOR_GET_R(color)`、`COLOR_GET_G(color)`、`COLOR_GET_B(color)`。

默认透明色常量：`COLORKEY_DEFAULT`（默认值是品红 `0xFFFF00FF`）。