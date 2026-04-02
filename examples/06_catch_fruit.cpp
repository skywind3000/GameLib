// 06_catch_fruit.cpp - Catch the Fruit
//
// 水果从天上掉下来，左右移动篮子接住它们。
// 学习：Random, RectOverlap, 计分, 游戏状态
//
// 编译: g++ -o 06_catch_fruit.exe 06_catch_fruit.cpp -mwindows

#include "../GameLib.h"

#define MAX_FRUITS 20

struct Fruit {
    int x, y;
    int speed;
    uint32_t color;
    bool active;
};

int main()
{
    GameLib game;
    game.Open(640, 480, "06 - Catch Fruit", true);

    // 篮子
    int basketX = 280;
    int basketW = 80, basketH = 20;
    int basketY = 450;

    // 水果
    Fruit fruits[MAX_FRUITS];
    for (int i = 0; i < MAX_FRUITS; i++)
        fruits[i].active = false;

    uint32_t fruitColors[] = {COLOR_RED, COLOR_GREEN, COLOR_YELLOW, COLOR_ORANGE, COLOR_PINK};

    int score = 0;
    int missed = 0;
    int spawnTimer = 0;
    bool gameOver = false;

    while (!game.IsClosed()) {
        if (game.IsKeyPressed(KEY_ESCAPE)) break;

        if (!gameOver) {
            // 篮子移动
            if (game.IsKeyDown(KEY_LEFT))  basketX -= 6;
            if (game.IsKeyDown(KEY_RIGHT)) basketX += 6;
            if (basketX < 0) basketX = 0;
            if (basketX + basketW > game.GetWidth()) basketX = game.GetWidth() - basketW;

            // 生成水果
            spawnTimer++;
            int spawnRate = 30 - score / 5;  // 分数越高掉得越快
            if (spawnRate < 8) spawnRate = 8;

            if (spawnTimer >= spawnRate) {
                spawnTimer = 0;
                for (int i = 0; i < MAX_FRUITS; i++) {
                    if (!fruits[i].active) {
                        fruits[i].active = true;
                        fruits[i].x = GameLib::Random(10, game.GetWidth() - 20);
                        fruits[i].y = -10;
                        fruits[i].speed = GameLib::Random(2, 4 + score / 10);
                        fruits[i].color = fruitColors[GameLib::Random(0, 4)];
                        break;
                    }
                }
            }

            // 更新水果
            for (int i = 0; i < MAX_FRUITS; i++) {
                if (!fruits[i].active) continue;

                fruits[i].y += fruits[i].speed;

                // 接住了？
                if (GameLib::RectOverlap(
                        fruits[i].x - 5, fruits[i].y - 5, 10, 10,
                        basketX, basketY, basketW, basketH)) {
                    fruits[i].active = false;
                    score++;
                }
                // 掉到底了？
                else if (fruits[i].y > game.GetHeight()) {
                    fruits[i].active = false;
                    missed++;
                    if (missed >= 10)
                        gameOver = true;
                }
            }
        } else {
            // Game Over 时按 R 重新开始
            if (game.IsKeyPressed(KEY_R)) {
                score = 0;
                missed = 0;
                gameOver = false;
                for (int i = 0; i < MAX_FRUITS; i++)
                    fruits[i].active = false;
            }
        }

        // --- 绘制 ---
        game.Clear(COLOR_DARK_BLUE);

        // 水果 (小圆)
        for (int i = 0; i < MAX_FRUITS; i++) {
            if (!fruits[i].active) continue;
            game.FillCircle(fruits[i].x, fruits[i].y, 8, fruits[i].color);
        }

        // 篮子
        game.FillRect(basketX, basketY, basketW, basketH, COLOR_BROWN);
        game.DrawRect(basketX, basketY, basketW, basketH, COLOR_GOLD);

        // HUD
        game.DrawPrintf(10, 10, COLOR_WHITE, "Score: %d", score);
        game.DrawPrintf(10, 25, COLOR_RED,   "Missed: %d / 10", missed);
        game.DrawText(game.GetWidth() - 200, 10, "Left/Right to move", COLOR_GRAY);

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
