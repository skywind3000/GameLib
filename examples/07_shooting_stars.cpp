// 07_shooting_stars.cpp - Simple Shooter
//
// 控制飞船左右移动，按空格发射子弹打掉从上方下落的目标。
// 学习：数组管理多对象、子弹发射、碰撞销毁、IsKeyPressed
//
// 编译: g++ -o 07_shooting_stars.exe 07_shooting_stars.cpp -mwindows

#include "../GameLib.h"

#define MAX_BULLETS 30
#define MAX_ENEMIES 15

struct Bullet {
    int x, y;
    bool active;
};

struct Enemy {
    int x, y;
    int speed;
    bool active;
};

int main()
{
    GameLib game;
    game.Open(640, 480, "07 - Shooting Stars", true);

    // 玩家飞船
    int shipX = 300, shipY = 440;
    int shipW = 30, shipH = 20;

    // 子弹
    Bullet bullets[MAX_BULLETS];
    for (int i = 0; i < MAX_BULLETS; i++)
        bullets[i].active = false;

    // 敌人
    Enemy enemies[MAX_ENEMIES];
    for (int i = 0; i < MAX_ENEMIES; i++)
        enemies[i].active = false;

    int score = 0;
    int spawnTimer = 0;
    int lives = 5;
    bool gameOver = false;

    while (!game.IsClosed()) {
        if (game.IsKeyPressed(KEY_ESCAPE)) break;

        if (!gameOver) {
            // 飞船移动
            if (game.IsKeyDown(KEY_LEFT))  shipX -= 5;
            if (game.IsKeyDown(KEY_RIGHT)) shipX += 5;
            if (shipX < 0) shipX = 0;
            if (shipX + shipW > game.GetWidth()) shipX = game.GetWidth() - shipW;

            // 发射子弹
            if (game.IsKeyPressed(KEY_SPACE)) {
                for (int i = 0; i < MAX_BULLETS; i++) {
                    if (!bullets[i].active) {
                        bullets[i].active = true;
                        bullets[i].x = shipX + shipW / 2;
                        bullets[i].y = shipY - 5;
                        break;
                    }
                }
            }

            // 更新子弹
            for (int i = 0; i < MAX_BULLETS; i++) {
                if (!bullets[i].active) continue;
                bullets[i].y -= 8;
                if (bullets[i].y < 0)
                    bullets[i].active = false;
            }

            // 生成敌人
            spawnTimer++;
            int rate = 40 - score / 3;
            if (rate < 12) rate = 12;
            if (spawnTimer >= rate) {
                spawnTimer = 0;
                for (int i = 0; i < MAX_ENEMIES; i++) {
                    if (!enemies[i].active) {
                        enemies[i].active = true;
                        enemies[i].x = GameLib::Random(10, game.GetWidth() - 30);
                        enemies[i].y = -20;
                        enemies[i].speed = GameLib::Random(1, 3 + score / 15);
                        break;
                    }
                }
            }

            // 更新敌人
            for (int i = 0; i < MAX_ENEMIES; i++) {
                if (!enemies[i].active) continue;
                enemies[i].y += enemies[i].speed;

                // 跑出屏幕 -> 扣命
                if (enemies[i].y > game.GetHeight()) {
                    enemies[i].active = false;
                    lives--;
                    if (lives <= 0) gameOver = true;
                }
            }

            // 碰撞检测: 子弹 vs 敌人
            for (int i = 0; i < MAX_BULLETS; i++) {
                if (!bullets[i].active) continue;
                for (int j = 0; j < MAX_ENEMIES; j++) {
                    if (!enemies[j].active) continue;
                    if (GameLib::RectOverlap(
                            bullets[i].x - 2, bullets[i].y - 4, 4, 8,
                            enemies[j].x, enemies[j].y, 20, 20)) {
                        bullets[i].active = false;
                        enemies[j].active = false;
                        score++;
                        break;
                    }
                }
            }

            // 碰撞检测: 敌人 vs 飞船
            for (int i = 0; i < MAX_ENEMIES; i++) {
                if (!enemies[i].active) continue;
                if (GameLib::RectOverlap(
                        enemies[i].x, enemies[i].y, 20, 20,
                        shipX, shipY, shipW, shipH)) {
                    enemies[i].active = false;
                    lives--;
                    if (lives <= 0) gameOver = true;
                }
            }
        } else {
            if (game.IsKeyPressed(KEY_R)) {
                score = 0;
                lives = 5;
                gameOver = false;
                for (int i = 0; i < MAX_BULLETS; i++) bullets[i].active = false;
                for (int i = 0; i < MAX_ENEMIES; i++) enemies[i].active = false;
            }
        }

        // --- 绘制 ---
        game.Clear(COLOR_BLACK);

        // 星空背景
        for (int i = 0; i < 60; i++) {
            // 用固定种子让星星位置每帧相同 (视觉伪随机)
            int sx = (i * 137 + 59) % game.GetWidth();
            int sy = (i * 251 + 31) % game.GetHeight();
            game.SetPixel(sx, sy, COLOR_WHITE);
        }

        // 子弹
        for (int i = 0; i < MAX_BULLETS; i++) {
            if (!bullets[i].active) continue;
            game.FillRect(bullets[i].x - 1, bullets[i].y - 4, 3, 8, COLOR_YELLOW);
        }

        // 敌人 (红色方块)
        for (int i = 0; i < MAX_ENEMIES; i++) {
            if (!enemies[i].active) continue;
            game.FillRect(enemies[i].x, enemies[i].y, 20, 20, COLOR_RED);
            game.DrawRect(enemies[i].x, enemies[i].y, 20, 20, COLOR_ORANGE);
        }

        // 飞船 (三角形)
        game.FillTriangle(
            shipX + shipW / 2, shipY - 5,
            shipX, shipY + shipH,
            shipX + shipW, shipY + shipH,
            COLOR_CYAN);

        // HUD
        game.DrawPrintf(10, 10, COLOR_WHITE, "Score: %d", score);
        game.DrawPrintf(10, 25, COLOR_GREEN, "Lives: %d", lives);
        game.DrawText(game.GetWidth() - 230, 10, "Left/Right + Space", COLOR_GRAY);

        if (gameOver) {
            game.FillRect(170, 180, 300, 100, COLOR_DARK_GRAY);
            game.DrawRect(170, 180, 300, 100, COLOR_WHITE);
            game.DrawTextScale(210, 200, "GAME OVER", COLOR_RED, 2);
            game.DrawPrintf(240, 240, COLOR_WHITE, "Final Score: %d", score);
            game.DrawText(220, 260, "Press R to restart", COLOR_YELLOW);
        }

        game.Update();
        game.WaitFrame(60);
    }
    return 0;
}
