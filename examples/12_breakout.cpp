// 12_breakout.cpp - Breakout
//
// 经典打砖块：挡板反弹球，打掉所有砖块即获胜。
// 学习：碰撞检测深度运用、多对象管理、游戏状态
//
// 编译: g++ -o 12_breakout.exe 12_breakout.cpp -mwindows

#include "../GameLib.h"
#include <math.h>

#define BRICK_ROWS 6
#define BRICK_COLS 10
#define BRICK_W 58
#define BRICK_H 18
#define BRICK_GAP 4
#define BRICK_OFFSET_X 12
#define BRICK_OFFSET_Y 50

int main()
{
    GameLib game;
    game.Open(640, 480, "12 - Breakout", true);

    // 砖块
    bool bricks[BRICK_ROWS][BRICK_COLS];
    uint32_t brickColors[] = {COLOR_RED, COLOR_ORANGE, COLOR_YELLOW, COLOR_GREEN, COLOR_CYAN, COLOR_PURPLE};

    // 挡板
    int padW = 80, padH = 12;
    int padX = 280, padY = 450;

    // 球
    float ballX = 320, ballY = 430;
    float ballVX = 3.0f, ballVY = -4.0f;
    int ballR = 5;

    int score = 0;
    int lives = 3;
    int totalBricks = 0;
    bool started = false;  // 球是否发射了
    bool gameOver = false;
    bool gameWin = false;

    // 初始化砖块
    for (int r = 0; r < BRICK_ROWS; r++)
        for (int c = 0; c < BRICK_COLS; c++)
            bricks[r][c] = true;
    totalBricks = BRICK_ROWS * BRICK_COLS;

    while (!game.IsClosed()) {
        if (game.IsKeyPressed(KEY_ESCAPE)) break;

        if (!gameOver && !gameWin) {
            // 挡板移动
            if (game.IsKeyDown(KEY_LEFT))  padX -= 6;
            if (game.IsKeyDown(KEY_RIGHT)) padX += 6;
            if (padX < 0) padX = 0;
            if (padX + padW > game.GetWidth()) padX = game.GetWidth() - padW;

            // 发射
            if (!started) {
                ballX = (float)(padX + padW / 2);
                ballY = (float)(padY - ballR - 1);
                if (game.IsKeyPressed(KEY_SPACE)) {
                    started = true;
                    ballVX = 3.0f;
                    ballVY = -4.0f;
                }
            } else {
                // 移动球
                ballX += ballVX;
                ballY += ballVY;

                // 左右墙壁
                if (ballX - ballR < 0) { ballX = (float)ballR; ballVX = -ballVX; }
                if (ballX + ballR > game.GetWidth()) { ballX = (float)(game.GetWidth() - ballR); ballVX = -ballVX; }

                // 顶部
                if (ballY - ballR < 0) { ballY = (float)ballR; ballVY = -ballVY; }

                // 底部 (丢球)
                if (ballY + ballR > game.GetHeight()) {
                    lives--;
                    if (lives <= 0) {
                        gameOver = true;
                    } else {
                        started = false;
                    }
                }

                // 挡板碰撞
                if (ballVY > 0 &&
                    ballX + ballR > padX && ballX - ballR < padX + padW &&
                    ballY + ballR >= padY && ballY + ballR <= padY + padH + 4) {
                    ballVY = -ballVY;
                    ballY = (float)(padY - ballR);
                    // 根据击中挡板的位置调整水平速度
                    float hitPos = (ballX - padX) / padW;  // 0.0 ~ 1.0
                    ballVX = (hitPos - 0.5f) * 8.0f;
                }

                // 砖块碰撞
                for (int r = 0; r < BRICK_ROWS; r++) {
                    for (int c = 0; c < BRICK_COLS; c++) {
                        if (!bricks[r][c]) continue;

                        int bx = BRICK_OFFSET_X + c * (BRICK_W + BRICK_GAP);
                        int by = BRICK_OFFSET_Y + r * (BRICK_H + BRICK_GAP);

                        // 球圆心 vs 砖块矩形
                        if (ballX + ballR > bx && ballX - ballR < bx + BRICK_W &&
                            ballY + ballR > by && ballY - ballR < by + BRICK_H) {
                            bricks[r][c] = false;
                            totalBricks--;
                            score += 10 * (BRICK_ROWS - r);  // 上排分更高

                            // 简易反弹方向判断
                            float overlapLeft   = (ballX + ballR) - bx;
                            float overlapRight  = (bx + BRICK_W) - (ballX - ballR);
                            float overlapTop    = (ballY + ballR) - by;
                            float overlapBottom = (by + BRICK_H) - (ballY - ballR);

                            float minOverlapX = (overlapLeft < overlapRight) ? overlapLeft : overlapRight;
                            float minOverlapY = (overlapTop < overlapBottom) ? overlapTop : overlapBottom;

                            if (minOverlapX < minOverlapY)
                                ballVX = -ballVX;
                            else
                                ballVY = -ballVY;

                            if (totalBricks <= 0) gameWin = true;
                            goto done_collision;
                        }
                    }
                }
                done_collision:;
            }
        } else {
            // 重新开始
            if (game.IsKeyPressed(KEY_R)) {
                for (int r = 0; r < BRICK_ROWS; r++)
                    for (int c = 0; c < BRICK_COLS; c++)
                        bricks[r][c] = true;
                totalBricks = BRICK_ROWS * BRICK_COLS;
                score = 0;
                lives = 3;
                padX = 280;
                started = false;
                gameOver = false;
                gameWin = false;
            }
        }

        // --- 绘制 ---
        game.Clear(COLOR_BLACK);

        // 顶部信息
        game.DrawPrintf(10, 10, COLOR_WHITE, "Score: %d", score);
        game.DrawPrintf(10, 25, COLOR_GREEN, "Lives: %d", lives);
        game.DrawPrintf(game.GetWidth() - 130, 10, COLOR_GRAY, "Bricks: %d", totalBricks);

        // 砖块
        for (int r = 0; r < BRICK_ROWS; r++) {
            for (int c = 0; c < BRICK_COLS; c++) {
                if (!bricks[r][c]) continue;
                int bx = BRICK_OFFSET_X + c * (BRICK_W + BRICK_GAP);
                int by = BRICK_OFFSET_Y + r * (BRICK_H + BRICK_GAP);
                game.FillRect(bx, by, BRICK_W, BRICK_H, brickColors[r]);
                game.DrawRect(bx, by, BRICK_W, BRICK_H, COLOR_WHITE);
            }
        }

        // 挡板
        game.FillRect(padX, padY, padW, padH, COLOR_WHITE);

        // 球
        game.FillCircle((int)ballX, (int)ballY, ballR, COLOR_WHITE);

        // 提示
        if (!started && !gameOver && !gameWin)
            game.DrawText(240, 420, "SPACE to launch", COLOR_YELLOW);

        if (gameOver) {
            game.FillRect(200, 200, 240, 80, COLOR_DARK_GRAY);
            game.DrawRect(200, 200, 240, 80, COLOR_WHITE);
            game.DrawTextScale(230, 210, "GAME OVER", COLOR_RED, 2);
            game.DrawPrintf(260, 245, COLOR_WHITE, "Score: %d", score);
            game.DrawText(245, 262, "R to restart", COLOR_YELLOW);
        }

        if (gameWin) {
            game.FillRect(200, 200, 240, 80, COLOR_DARK_GRAY);
            game.DrawRect(200, 200, 240, 80, COLOR_WHITE);
            game.DrawTextScale(240, 210, "YOU WIN!", COLOR_GREEN, 2);
            game.DrawPrintf(260, 245, COLOR_WHITE, "Score: %d", score);
            game.DrawText(245, 262, "R to restart", COLOR_YELLOW);
        }

        game.Update();
        game.WaitFrame(60);
    }
    return 0;
}
