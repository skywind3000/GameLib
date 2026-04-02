// 09_sprite_animation.cpp - Sprite Animation
//
// 用 DrawSpriteRegion 实现 sprite sheet 帧动画：
// 一个小人在屏幕上走动，根据方向切换动画帧。
// 精灵用代码生成 (无需外部 BMP 文件)。
// 学习：DrawSpriteRegion, GetDeltaTime, 帧动画原理
//
// 编译: g++ -o 09_sprite_animation.exe 09_sprite_animation.cpp -mwindows

#include "../GameLib.h"

// 创建一个简易角色 sprite sheet: 4 方向 x 3 帧 = 12 帧
// 每帧 12x16, 整个 sheet 36x64
// 行: 下(0) 左(1) 右(2) 上(3)
// 列: 3 帧步行动画
int CreateCharSheet(GameLib &game)
{
    int fw = 12, fh = 16;
    int cols = 3, rows = 4;
    int id = game.CreateSprite(fw * cols, fh * rows);
    if (id < 0) return -1;

    // 全部透明
    for (int y = 0; y < fh * rows; y++)
        for (int x = 0; x < fw * cols; x++)
            game.SetSpritePixel(id, x, y, 0x00000000);

    uint32_t skin = COLOR_RGB(255, 200, 150);
    uint32_t hair = COLOR_BROWN;
    uint32_t shirt = COLOR_BLUE;
    uint32_t pants = COLOR_DARK_BLUE;
    uint32_t shoe = COLOR_DARK_GRAY;

    // 给每个方向的每一帧画一个简笔画小人
    for (int dir = 0; dir < 4; dir++) {
        for (int f = 0; f < 3; f++) {
            int ox = f * fw;
            int oy = dir * fh;

            // 头 (4x4)
            for (int dy = 0; dy < 4; dy++)
                for (int dx = 0; dx < 4; dx++)
                    game.SetSpritePixel(id, ox + 4 + dx, oy + dy, skin);

            // 头发 (顶部一行)
            for (int dx = 0; dx < 4; dx++)
                game.SetSpritePixel(id, ox + 4 + dx, oy, hair);

            // 身体 (6x5)
            for (int dy = 4; dy < 9; dy++)
                for (int dx = 0; dx < 6; dx++)
                    game.SetSpritePixel(id, ox + 3 + dx, oy + dy, shirt);

            // 裤子 (6x3)
            for (int dy = 9; dy < 12; dy++)
                for (int dx = 0; dx < 6; dx++)
                    game.SetSpritePixel(id, ox + 3 + dx, oy + dy, pants);

            // 脚 - 根据帧做走路动画
            int leftFootX, rightFootX;
            if (f == 0) { leftFootX = 3; rightFootX = 7; }       // 站立
            else if (f == 1) { leftFootX = 2; rightFootX = 8; }  // 左脚前
            else { leftFootX = 4; rightFootX = 6; }              // 右脚前

            for (int dy = 12; dy < 14; dy++) {
                game.SetSpritePixel(id, ox + leftFootX, oy + dy, shoe);
                game.SetSpritePixel(id, ox + leftFootX + 1, oy + dy, shoe);
                game.SetSpritePixel(id, ox + rightFootX, oy + dy, shoe);
                game.SetSpritePixel(id, ox + rightFootX + 1, oy + dy, shoe);
            }

            // 眼睛 (方向不同，眼睛位置不同)
            if (dir == 0) { // 下
                game.SetSpritePixel(id, ox + 5, oy + 2, COLOR_BLACK);
                game.SetSpritePixel(id, ox + 7, oy + 2, COLOR_BLACK);
            } else if (dir == 1) { // 左
                game.SetSpritePixel(id, ox + 4, oy + 2, COLOR_BLACK);
            } else if (dir == 2) { // 右
                game.SetSpritePixel(id, ox + 7, oy + 2, COLOR_BLACK);
            } else { // 上 - 不画眼睛，画后脑勺发色
                for (int dx = 0; dx < 4; dx++)
                    game.SetSpritePixel(id, ox + 4 + dx, oy + 1, hair);
            }
        }
    }

    return id;
}

int main()
{
    GameLib game;
    game.Open(640, 480, "09 - Sprite Animation", true);

    int charSheet = CreateCharSheet(game);
    int fw = 12, fh = 16;  // 每帧尺寸
    int scale = 3;          // 显示放大倍数

    float px = 300, py = 220;
    float speed = 100.0f;   // 像素/秒
    int dir = 0;            // 0=下 1=左 2=右 3=上
    int frame = 0;
    float animTimer = 0;
    bool moving = false;

    while (!game.IsClosed()) {
        if (game.IsKeyPressed(KEY_ESCAPE)) break;

        float dt = game.GetDeltaTime();
        moving = false;

        // 移动
        if (game.IsKeyDown(KEY_DOWN)  || game.IsKeyDown(KEY_S)) { py += speed * dt; dir = 0; moving = true; }
        if (game.IsKeyDown(KEY_LEFT)  || game.IsKeyDown(KEY_A)) { px -= speed * dt; dir = 1; moving = true; }
        if (game.IsKeyDown(KEY_RIGHT) || game.IsKeyDown(KEY_D)) { px += speed * dt; dir = 2; moving = true; }
        if (game.IsKeyDown(KEY_UP)    || game.IsKeyDown(KEY_W)) { py -= speed * dt; dir = 3; moving = true; }

        // 边界限制
        if (px < 0) px = 0;
        if (py < 0) py = 0;
        if (px > game.GetWidth() - fw * scale) px = (float)(game.GetWidth() - fw * scale);
        if (py > game.GetHeight() - fh * scale) py = (float)(game.GetHeight() - fh * scale);

        // 动画帧更新
        if (moving) {
            animTimer += dt;
            if (animTimer >= 0.15f) {
                animTimer = 0;
                frame = (frame + 1) % 3;
            }
        } else {
            frame = 0;  // 站立帧
            animTimer = 0;
        }

        // --- 绘制 ---
        game.Clear(COLOR_DARK_GREEN);

        // 地面装饰
        for (int i = 0; i < 30; i++) {
            int gx = (i * 97 + 13) % game.GetWidth();
            int gy = (i * 173 + 47) % game.GetHeight();
            game.FillRect(gx, gy, 3, 3, COLOR_GREEN);
        }

        // 角色 (放大绘制)
        int sx = frame * fw;
        int sy = dir * fh;
        // 手动放大: 读 sprite sheet 的像素，画大矩形
        for (int fy = 0; fy < fh; fy++) {
            for (int fx = 0; fx < fw; fx++) {
                uint32_t c = game.GetSpritePixel(charSheet, sx + fx, sy + fy);
                if ((c >> 24) > 0) {
                    game.FillRect((int)px + fx * scale, (int)py + fy * scale, scale, scale, c);
                }
            }
        }

        // Sprite Sheet 预览 (右上角)
        game.DrawText(470, 10, "Sprite Sheet:", COLOR_WHITE);
        int previewScale = 2;
        int sheetW = fw * 3, sheetH = fh * 4;
        int pvX = 470, pvY = 25;
        for (int sy2 = 0; sy2 < sheetH; sy2++) {
            for (int sx2 = 0; sx2 < sheetW; sx2++) {
                uint32_t c = game.GetSpritePixel(charSheet, sx2, sy2);
                if ((c >> 24) > 0)
                    game.FillRect(pvX + sx2 * previewScale, pvY + sy2 * previewScale,
                                  previewScale, previewScale, c);
            }
        }
        game.DrawRect(pvX, pvY, sheetW * previewScale, sheetH * previewScale, COLOR_GRAY);
        // 高亮当前帧
        game.DrawRect(pvX + frame * fw * previewScale, pvY + dir * fh * previewScale,
                      fw * previewScale, fh * previewScale, COLOR_YELLOW);

        // 信息
        const char *dirNames[] = {"Down", "Left", "Right", "Up"};
        game.DrawText(10, 10, "WASD / Arrow keys to move", COLOR_WHITE);
        game.DrawPrintf(10, 25, COLOR_GRAY, "Dir: %s  Frame: %d", dirNames[dir], frame);

        game.Update();
        game.WaitFrame(60);
    }

    game.FreeSprite(charSheet);
    return 0;
}
