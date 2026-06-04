// 08_breakout.cpp - Breakout
//
// Classic Breakout: bounce the ball with paddle, destroy all bricks to win.
// Learn: in-depth collision detection, multi-object management, game state
//
// Compile (Win32): g++ -o 08_breakout.exe 08_breakout.cpp -mwindows
// Compile (SDL):   g++ -std=c++11 -O2 -o 08_breakout 08_breakout.cpp -lSDL2

#if defined(_WIN32) && !defined(USE_SDL)
#include "../GameLib.h"
#else
#include "../GameLib.SDL.h"
#endif

#include <math.h>
#include <stdio.h>

#define BRICK_ROWS 6
#define BRICK_COLS 10
#define BRICK_W 58
#define BRICK_H 18
#define BRICK_GAP 4
#define BRICK_OFFSET_X 12
#define BRICK_OFFSET_Y 50

static const int PADDLE_W = 80;
static const int PADDLE_H = 12;
static const int PADDLE_START_X = 280;
static const int PADDLE_Y = 450;
static const int PADDLE_SPEED = 6;

static const float BALL_START_VY = -4.0f;
static const float BALL_START_MIN_ABS_VX = 1.5f;
static const float BALL_START_MAX_ABS_VX = 4.0f;
static const int BALL_R = 5;
static const float BALL_PADDLE_MAX_VX = 8.0f;
static const int LIVES_START = 1;

static const char *ChooseExistingPath(const char *pathA, const char *pathB)
{
    FILE *file = fopen(pathA, "rb");
    if (file != NULL) { fclose(file); return pathA; }
    file = fopen(pathB, "rb");
    if (file != NULL) { fclose(file); return pathB; }
    return pathA;
}

static void ResetBallVelocity(float *vx, float *vy)
{
    int minSpeedX10 = (int)(BALL_START_MIN_ABS_VX * 10.0f);
    int maxSpeedX10 = (int)(BALL_START_MAX_ABS_VX * 10.0f);
    float speed = (float)GameLib::Random(minSpeedX10, maxSpeedX10) / 10.0f;
    if (GameLib::Random(0, 1) == 0) speed = -speed;
    *vx = speed;
    *vy = BALL_START_VY;
}
int main()
{
    GameLib game;
    game.Open(640, 480, "08 - Breakout", true);

    const char *launchSfx  = ChooseExistingPath("../assets/sound/jump.wav", "assets/sound/jump.wav");
    const char *bounceSfx  = ChooseExistingPath("../assets/sound/hit.wav", "assets/sound/hit.wav");
    const char *brickRowSfx[BRICK_ROWS] = {
        ChooseExistingPath("../assets/sound/note_do_high.wav", "assets/sound/note_do_high.wav"),
        ChooseExistingPath("../assets/sound/note_si.wav", "assets/sound/note_si.wav"),
        ChooseExistingPath("../assets/sound/note_la.wav", "assets/sound/note_la.wav"),
        ChooseExistingPath("../assets/sound/note_sol.wav", "assets/sound/note_sol.wav"),
        ChooseExistingPath("../assets/sound/note_fa.wav", "assets/sound/note_fa.wav"),
        ChooseExistingPath("../assets/sound/note_mi.wav", "assets/sound/note_mi.wav")
    };
    const char *loseLifeSfx = ChooseExistingPath("../assets/sound/explosion.wav", "assets/sound/explosion.wav");
    const char *restartSfx  = ChooseExistingPath("../assets/sound/click.wav", "assets/sound/click.wav");
    const char *gameOverSfx = ChooseExistingPath("../assets/sound/game_over.wav", "assets/sound/game_over.wav");
    const char *winSfx      = ChooseExistingPath("../assets/sound/victory.wav", "assets/sound/victory.wav");

    bool bricks[BRICK_ROWS][BRICK_COLS];
    uint32_t brickColors[] = {COLOR_RED, COLOR_ORANGE, COLOR_YELLOW, COLOR_GREEN, COLOR_CYAN, COLOR_PURPLE};

    int padW = PADDLE_W, padH = PADDLE_H;
    int padX = PADDLE_START_X, padY = PADDLE_Y;

    float ballX = 320, ballY = 430;
    float ballVX = 0.0f, ballVY = BALL_START_VY;
    int ballR = BALL_R;

    int score = 0, lives = LIVES_START, totalBricks = 0;
    bool started = false, gameOver = false, gameWin = false;

    for (int r = 0; r < BRICK_ROWS; r++)
        for (int c = 0; c < BRICK_COLS; c++)
            bricks[r][c] = true;
    totalBricks = BRICK_ROWS * BRICK_COLS;

    while (!game.IsClosed()) {
        const char *sfxToPlay = NULL;
        if (game.IsKeyPressed(KEY_ESCAPE)) break;

        if (!gameOver && !gameWin) {
            if (game.IsKeyDown(KEY_LEFT))  padX -= PADDLE_SPEED;
            if (game.IsKeyDown(KEY_RIGHT)) padX += PADDLE_SPEED;
            if (padX < 0) padX = 0;
            if (padX + padW > game.GetWidth()) padX = game.GetWidth() - padW;

            if (!started) {
                ballX = (float)(padX + padW / 2);
                ballY = (float)(padY - ballR - 1);
                if (game.IsKeyPressed(KEY_SPACE)) {
                    started = true;
                    ResetBallVelocity(&ballVX, &ballVY);
                    sfxToPlay = launchSfx;
                }
            } else {
                ballX += ballVX;
                ballY += ballVY;

                if (ballX - ballR < 0) {
                    ballX = (float)ballR; ballVX = -ballVX;
                    if (!sfxToPlay) sfxToPlay = bounceSfx;
                }
                if (ballX + ballR > game.GetWidth()) {
                    ballX = (float)(game.GetWidth() - ballR); ballVX = -ballVX;
                    if (!sfxToPlay) sfxToPlay = bounceSfx;
                }
                if (ballY - ballR < 0) {
                    ballY = (float)ballR; ballVY = -ballVY;
                    if (!sfxToPlay) sfxToPlay = bounceSfx;
                }
                if (ballY + ballR > game.GetHeight()) {
                    lives--;
                    if (lives <= 0) { gameOver = true; sfxToPlay = gameOverSfx; }
                    else { started = false; ballVX = 0.0f; ballVY = BALL_START_VY; sfxToPlay = loseLifeSfx; }
                }

                if (ballVY > 0 &&
                    ballX + ballR > padX && ballX - ballR < padX + padW &&
                    ballY + ballR >= padY && ballY + ballR <= padY + padH + 4) {
                    ballVY = -ballVY;
                    ballY = (float)(padY - ballR);
                    float hitPos = (ballX - padX) / padW;
                    ballVX = (hitPos - 0.5f) * BALL_PADDLE_MAX_VX;
                    if (!sfxToPlay) sfxToPlay = bounceSfx;
                }

                for (int r = 0; r < BRICK_ROWS; r++) {
                    for (int c = 0; c < BRICK_COLS; c++) {
                        if (!bricks[r][c]) continue;
                        int bx = BRICK_OFFSET_X + c * (BRICK_W + BRICK_GAP);
                        int by = BRICK_OFFSET_Y + r * (BRICK_H + BRICK_GAP);

                        if (ballX + ballR > bx && ballX - ballR < bx + BRICK_W &&
                            ballY + ballR > by && ballY - ballR < by + BRICK_H) {
                            bricks[r][c] = false;
                            totalBricks--;
                            score += 10 * (BRICK_ROWS - r);

                            float overlapLeft   = (ballX + ballR) - bx;
                            float overlapRight  = (bx + BRICK_W) - (ballX - ballR);
                            float overlapTop    = (ballY + ballR) - by;
                            float overlapBottom = (by + BRICK_H) - (ballY - ballR);
                            float minOverlapX = (overlapLeft < overlapRight) ? overlapLeft : overlapRight;
                            float minOverlapY = (overlapTop < overlapBottom) ? overlapTop : overlapBottom;

                            if (minOverlapX < minOverlapY) ballVX = -ballVX;
                            else ballVY = -ballVY;

                            if (totalBricks <= 0) { gameWin = true; sfxToPlay = winSfx; }
                            else sfxToPlay = brickRowSfx[r];
                            goto done_collision;
                        }
                    }
                }
                done_collision:;
            }
        } else {
            if (game.IsKeyPressed(KEY_R)) {
                for (int r = 0; r < BRICK_ROWS; r++)
                    for (int c = 0; c < BRICK_COLS; c++)
                        bricks[r][c] = true;
                totalBricks = BRICK_ROWS * BRICK_COLS;
                score = 0; lives = LIVES_START; padX = PADDLE_START_X;
                started = false; gameOver = false; gameWin = false;
                sfxToPlay = restartSfx;
            }
        }

        if (sfxToPlay) game.PlayWAV(sfxToPlay);

        game.Clear(COLOR_BLACK);

        game.DrawPrintf(10, 10, COLOR_WHITE, "Score: %d", score);
        game.DrawPrintf(10, 25, COLOR_GREEN, "Lives: %d", lives);
        game.DrawPrintf(game.GetWidth() - 130, 10, COLOR_GRAY, "Bricks: %d", totalBricks);

        for (int r = 0; r < BRICK_ROWS; r++) {
            for (int c = 0; c < BRICK_COLS; c++) {
                if (!bricks[r][c]) continue;
                int bx = BRICK_OFFSET_X + c * (BRICK_W + BRICK_GAP);
                int by = BRICK_OFFSET_Y + r * (BRICK_H + BRICK_GAP);
                game.FillRect(bx, by, BRICK_W, BRICK_H, brickColors[r]);
                game.DrawRect(bx, by, BRICK_W, BRICK_H, COLOR_WHITE);
            }
        }

        game.FillRect(padX, padY, padW, padH, COLOR_WHITE);
        game.FillCircle((int)ballX, (int)ballY, ballR, COLOR_WHITE);

        if (!started && !gameOver && !gameWin)
            game.DrawText(240, 420, "SPACE to launch", COLOR_YELLOW);

        if (gameOver) {
            game.FillRect(200, 200, 240, 80, COLOR_DARK_GRAY);
            game.DrawRect(200, 200, 240, 80, COLOR_WHITE);
            game.DrawTextScale(230, 210, "GAME OVER", COLOR_RED, 16, 16);
            game.DrawPrintf(260, 245, COLOR_WHITE, "Score: %d", score);
            game.DrawText(245, 262, "R to restart", COLOR_YELLOW);
        }
        if (gameWin) {
            game.FillRect(200, 200, 240, 80, COLOR_DARK_GRAY);
            game.DrawRect(200, 200, 240, 80, COLOR_WHITE);
            game.DrawTextScale(240, 210, "YOU WIN!", COLOR_GREEN, 16, 16);
            game.DrawPrintf(260, 245, COLOR_WHITE, "Score: %d", score);
            game.DrawText(245, 262, "R to restart", COLOR_YELLOW);
        }

        game.Update();
        game.WaitFrame(60);
    }
    return 0;
}
