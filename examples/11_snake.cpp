// 11_snake.cpp - Snake Game
//
// 经典贪吃蛇：方向键控制蛇移动，吃食物变长，撞墙或撞自己就结束。
// 学习：DrawGrid, FillCell, IsKeyPressed, 游戏状态机, 定时移动
//
// 编译: g++ -o 11_snake.exe 11_snake.cpp -mwindows

#include "../GameLib.h"

#define GRID_ROWS 20
#define GRID_COLS 20
#define CELL_SIZE 22
#define MAX_SNAKE 400

int main()
{
    GameLib game;

    int gridW = GRID_COLS * CELL_SIZE;
    int gridH = GRID_ROWS * CELL_SIZE;
    int winW = gridW + 160;  // 右侧留信息栏
    int winH = gridH + 40;   // 顶部留标题
    game.Open(winW, winH, "11 - Snake", true);

    int gridX = 10, gridY = 30;

    // 蛇身 (用数组存每节的行列)
    int snakeR[MAX_SNAKE], snakeC[MAX_SNAKE];
    int snakeLen = 3;

    // 初始位置
    snakeR[0] = 10; snakeC[0] = 10;
    snakeR[1] = 10; snakeC[1] = 9;
    snakeR[2] = 10; snakeC[2] = 8;

    // 方向: 0=上 1=下 2=左 3=右
    int dir = 3;
    int nextDir = 3;

    // 食物
    int foodR = 5, foodC = 15;

    int score = 0;
    bool gameOver = false;
    bool paused = false;

    // 移动定时器
    float moveTimer = 0;
    float moveInterval = 0.15f;

    while (!game.IsClosed()) {
        if (game.IsKeyPressed(KEY_ESCAPE)) break;

        float dt = game.GetDeltaTime();

        if (!gameOver) {
            // 暂停
            if (game.IsKeyPressed(KEY_P))
                paused = !paused;

            if (!paused) {
                // 方向输入 (不能直接反向)
                if (game.IsKeyPressed(KEY_UP)    && dir != 1) nextDir = 0;
                if (game.IsKeyPressed(KEY_DOWN)  && dir != 0) nextDir = 1;
                if (game.IsKeyPressed(KEY_LEFT)  && dir != 3) nextDir = 2;
                if (game.IsKeyPressed(KEY_RIGHT) && dir != 2) nextDir = 3;

                // 定时移动
                moveTimer += dt;
                if (moveTimer >= moveInterval) {
                    moveTimer = 0;
                    dir = nextDir;

                    // 计算新头部位置
                    int newR = snakeR[0];
                    int newC = snakeC[0];
                    if (dir == 0) newR--;
                    if (dir == 1) newR++;
                    if (dir == 2) newC--;
                    if (dir == 3) newC++;

                    // 撞墙检测
                    if (newR < 0 || newR >= GRID_ROWS || newC < 0 || newC >= GRID_COLS) {
                        gameOver = true;
                    } else {
                        // 撞自己检测
                        for (int i = 0; i < snakeLen; i++) {
                            if (snakeR[i] == newR && snakeC[i] == newC) {
                                gameOver = true;
                                break;
                            }
                        }
                    }

                    if (!gameOver) {
                        // 吃食物？
                        bool ate = (newR == foodR && newC == foodC);

                        // 移动蛇身 (从尾部向前复制)
                        if (!ate) {
                            for (int i = snakeLen - 1; i > 0; i--) {
                                snakeR[i] = snakeR[i - 1];
                                snakeC[i] = snakeC[i - 1];
                            }
                        } else {
                            // 增长：先把所有节后移一格，空出头部
                            if (snakeLen < MAX_SNAKE) snakeLen++;
                            for (int i = snakeLen - 1; i > 0; i--) {
                                snakeR[i] = snakeR[i - 1];
                                snakeC[i] = snakeC[i - 1];
                            }
                            score += 10;
                            // 加速
                            if (moveInterval > 0.06f) moveInterval -= 0.003f;

                            // 新食物 (确保不在蛇身上)
                            bool onSnake;
                            do {
                                foodR = GameLib::Random(0, GRID_ROWS - 1);
                                foodC = GameLib::Random(0, GRID_COLS - 1);
                                onSnake = false;
                                for (int i = 0; i < snakeLen; i++) {
                                    if (snakeR[i] == foodR && snakeC[i] == foodC) {
                                        onSnake = true;
                                        break;
                                    }
                                }
                            } while (onSnake);
                        }
                        snakeR[0] = newR;
                        snakeC[0] = newC;
                    }
                }
            }
        } else {
            if (game.IsKeyPressed(KEY_R)) {
                snakeLen = 3;
                snakeR[0] = 10; snakeC[0] = 10;
                snakeR[1] = 10; snakeC[1] = 9;
                snakeR[2] = 10; snakeC[2] = 8;
                dir = 3; nextDir = 3;
                foodR = 5; foodC = 15;
                score = 0;
                moveInterval = 0.15f;
                gameOver = false;
            }
        }

        // --- 绘制 ---
        game.Clear(COLOR_BLACK);

        // 标题
        game.DrawTextScale(gridX, 5, "SNAKE", COLOR_GREEN, 2);

        // 网格
        game.DrawGrid(gridX, gridY, GRID_ROWS, GRID_COLS, CELL_SIZE, COLOR_DARK_GRAY);

        // 食物 (红色)
        game.FillCell(gridX, gridY, foodR, foodC, CELL_SIZE, COLOR_RED);

        // 蛇身
        for (int i = 0; i < snakeLen; i++) {
            uint32_t c = (i == 0) ? COLOR_GREEN : COLOR_DARK_GREEN;
            game.FillCell(gridX, gridY, snakeR[i], snakeC[i], CELL_SIZE, c);
        }

        // 右侧信息栏
        int infoX = gridX + gridW + 15;
        game.DrawText(infoX, 40, "Score:", COLOR_WHITE);
        game.DrawNumber(infoX, 55, score, COLOR_GOLD);

        game.DrawText(infoX, 85, "Length:", COLOR_WHITE);
        game.DrawNumber(infoX, 100, snakeLen, COLOR_CYAN);

        game.DrawText(infoX, 140, "Controls:", COLOR_GRAY);
        game.DrawText(infoX, 158, "Arrows", COLOR_LIGHT_GRAY);
        game.DrawText(infoX, 172, "P: Pause", COLOR_LIGHT_GRAY);

        if (paused && !gameOver) {
            game.FillRect(gridX + gridW / 2 - 50, gridY + gridH / 2 - 15, 100, 30, COLOR_DARK_GRAY);
            game.DrawText(gridX + gridW / 2 - 30, gridY + gridH / 2 - 7, "PAUSED", COLOR_YELLOW);
        }

        if (gameOver) {
            game.FillRect(gridX + gridW / 2 - 80, gridY + gridH / 2 - 30, 160, 70, COLOR_DARK_GRAY);
            game.DrawRect(gridX + gridW / 2 - 80, gridY + gridH / 2 - 30, 160, 70, COLOR_WHITE);
            game.DrawTextScale(gridX + gridW / 2 - 65, gridY + gridH / 2 - 22, "GAME OVER", COLOR_RED, 1);
            game.DrawPrintf(gridX + gridW / 2 - 50, gridY + gridH / 2 + 2, COLOR_WHITE, "Score: %d", score);
            game.DrawText(gridX + gridW / 2 - 55, gridY + gridH / 2 + 20, "R to restart", COLOR_YELLOW);
        }

        game.Update();
        game.WaitFrame(60);
    }
    return 0;
}
