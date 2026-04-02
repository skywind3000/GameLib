// 08_sprite_demo.cpp - Sprite Demo
//
// 展示精灵系统：用代码创建精灵、绘制、翻转、区域裁剪。
// 如果 assets/ 目录下有 BMP 文件，也可以用 LoadSpriteBMP 加载。
// 学习：CreateSprite, SetSpritePixel, DrawSprite, DrawSpriteEx, DrawSpriteRegion
//
// 编译: g++ -o 08_sprite_demo.exe 08_sprite_demo.cpp -mwindows

#include "../GameLib.h"

// 用代码画一个 16x16 的小飞机精灵
int CreateShipSprite(GameLib &game)
{
    int id = game.CreateSprite(16, 16);
    if (id < 0) return -1;

    // 先全部透明
    for (int y = 0; y < 16; y++)
        for (int x = 0; x < 16; x++)
            game.SetSpritePixel(id, x, y, 0x00000000);

    // 机身 (青色)
    for (int y = 4; y < 14; y++)
        for (int x = 6; x < 10; x++)
            game.SetSpritePixel(id, x, y, COLOR_CYAN);

    // 机头 (白色)
    game.SetSpritePixel(id, 7, 2, COLOR_WHITE);
    game.SetSpritePixel(id, 8, 2, COLOR_WHITE);
    game.SetSpritePixel(id, 7, 3, COLOR_WHITE);
    game.SetSpritePixel(id, 8, 3, COLOR_WHITE);

    // 左翼
    for (int x = 1; x < 6; x++) {
        game.SetSpritePixel(id, x, 9, COLOR_GRAY);
        game.SetSpritePixel(id, x, 10, COLOR_GRAY);
    }

    // 右翼
    for (int x = 10; x < 15; x++) {
        game.SetSpritePixel(id, x, 9, COLOR_GRAY);
        game.SetSpritePixel(id, x, 10, COLOR_GRAY);
    }

    // 尾翼
    game.SetSpritePixel(id, 5, 13, COLOR_DARK_GRAY);
    game.SetSpritePixel(id, 10, 13, COLOR_DARK_GRAY);

    // 引擎火焰 (橙色)
    game.SetSpritePixel(id, 7, 14, COLOR_ORANGE);
    game.SetSpritePixel(id, 8, 14, COLOR_ORANGE);
    game.SetSpritePixel(id, 7, 15, COLOR_YELLOW);
    game.SetSpritePixel(id, 8, 15, COLOR_YELLOW);

    return id;
}

// 用代码画一个 32x8 的 sprite sheet (4 帧，每帧 8x8)
int CreateAnimSheet(GameLib &game)
{
    int id = game.CreateSprite(32, 8);
    if (id < 0) return -1;

    // 全部透明
    for (int y = 0; y < 8; y++)
        for (int x = 0; x < 32; x++)
            game.SetSpritePixel(id, x, y, 0x00000000);

    // 4 帧，每帧画一个不同大小的圆点 (模拟动画)
    uint32_t colors[] = {COLOR_RED, COLOR_ORANGE, COLOR_YELLOW, COLOR_WHITE};
    int sizes[] = {1, 2, 3, 2};
    for (int f = 0; f < 4; f++) {
        int cx = f * 8 + 4;
        int cy = 4;
        int s = sizes[f];
        for (int dy = -s; dy <= s; dy++)
            for (int dx = -s; dx <= s; dx++)
                if (dx*dx + dy*dy <= s*s)
                    game.SetSpritePixel(id, cx + dx, cy + dy, colors[f]);
    }

    return id;
}

int main()
{
    GameLib game;
    game.Open(640, 480, "08 - Sprite Demo", true);

    int ship = CreateShipSprite(game);
    int sheet = CreateAnimSheet(game);

    int shipX = 300, shipY = 350;
    int frame = 0;
    int frameTimer = 0;

    while (!game.IsClosed()) {
        if (game.IsKeyPressed(KEY_ESCAPE)) break;

        // 移动飞船
        if (game.IsKeyDown(KEY_LEFT))  shipX -= 3;
        if (game.IsKeyDown(KEY_RIGHT)) shipX += 3;
        if (game.IsKeyDown(KEY_UP))    shipY -= 3;
        if (game.IsKeyDown(KEY_DOWN))  shipY += 3;

        // 动画帧
        frameTimer++;
        if (frameTimer >= 10) {
            frameTimer = 0;
            frame = (frame + 1) % 4;
        }

        game.Clear(COLOR_BLACK);

        // --- DrawSprite 基础 ---
        game.DrawText(20, 20, "DrawSprite (normal):", COLOR_WHITE);
        game.DrawSprite(ship, 20, 40);

        // --- DrawSpriteEx 翻转 ---
        game.DrawText(20, 70, "DrawSpriteEx (flipped):", COLOR_WHITE);
        game.DrawSpriteEx(ship, 20, 90, SPRITE_FLIP_H);                // 水平翻转
        game.DrawText(50, 95, "H", COLOR_GRAY);
        game.DrawSpriteEx(ship, 80, 90, SPRITE_FLIP_V);                // 垂直翻转
        game.DrawText(110, 95, "V", COLOR_GRAY);
        game.DrawSpriteEx(ship, 140, 90, SPRITE_FLIP_H | SPRITE_FLIP_V); // 双翻转
        game.DrawText(170, 95, "H+V", COLOR_GRAY);

        // --- DrawSpriteRegion 裁剪 ---
        game.DrawText(20, 130, "DrawSpriteRegion (sprite sheet):", COLOR_WHITE);
        // 显示整个 sheet
        game.DrawSprite(sheet, 20, 150);
        game.DrawRect(20, 150, 32, 8, COLOR_GRAY);
        // 显示当前帧 (放大 4 倍来看清)
        game.DrawText(70, 148, "<-- full sheet", COLOR_GRAY);
        game.DrawText(20, 168, "Current frame:", COLOR_GRAY);
        game.DrawSpriteRegion(sheet, 130, 163, frame * 8, 0, 8, 8);

        // --- 可移动的飞船 ---
        game.DrawText(20, 200, "Move with arrow keys:", COLOR_WHITE);
        game.DrawSprite(ship, shipX, shipY);

        // 放大显示飞船精灵的像素
        game.DrawText(400, 20, "Zoomed view (4x):", COLOR_WHITE);
        if (ship >= 0) {
            int zoomX = 400, zoomY = 40;
            int scale = 4;
            int sw = game.GetSpriteWidth(ship);
            int sh = game.GetSpriteHeight(ship);
            for (int py = 0; py < sh; py++) {
                for (int px = 0; px < sw; px++) {
                    uint32_t c = game.GetSpritePixel(ship, px, py);
                    if ((c >> 24) > 0)
                        game.FillRect(zoomX + px * scale, zoomY + py * scale, scale, scale, c);
                }
            }
            game.DrawRect(zoomX, zoomY, sw * scale, sh * scale, COLOR_GRAY);
        }

        game.DrawText(10, 460, "Sprites created with code (CreateSprite + SetSpritePixel)", COLOR_DARK_GRAY);

        game.Update();
        game.WaitFrame(60);
    }

    game.FreeSprite(ship);
    game.FreeSprite(sheet);
    return 0;
}
