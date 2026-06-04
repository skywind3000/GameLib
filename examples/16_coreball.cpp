// 16_coreball.cpp - Coreball / Pin the Core
//
// Shoot all waiting balls onto the rotating core. If the new ball touches
// an existing ball, the level fails.
// Learn: circular motion, angle math, state machine, circle collision
//
// Compile (Win32): g++ -o 16_coreball.exe 16_coreball.cpp -mwindows
// Compile (SDL):   g++ -std=c++11 -O2 -o 16_coreball 16_coreball.cpp -lSDL2

#if defined(_WIN32) && !defined(USE_SDL)
#include "../GameLib.h"
#else
#include "../GameLib.SDL.h"
#endif

#include <math.h>
#include <stdio.h>

#define MAX_PINS 64
#define LEVEL_COUNT 10

static const double PI_VALUE = 3.14159265358979323846;

static const int WIN_W = 640;
static const int WIN_H = 480;
static const int CORE_X = 320;
static const int CORE_Y = 170;
static const int CORE_R = 44;
static const int BALL_R = 10;
static const int ORBIT_R = 130;
static const int LAUNCH_X = 320;
static const int LAUNCH_Y = 410;
static const double LAUNCH_SPEED = 520.0;

enum GameState {
    STATE_READY,
    STATE_LAUNCHING,
    STATE_LEVEL_CLEAR,
    STATE_GAME_OVER
};

struct LevelDef {
    int shots;
    int blockers;
    double speedDeg;
    double offsetDeg;
};

struct Pin {
    double localAngle;
    int playerShot;
};

static LevelDef levels[LEVEL_COUNT] = {
    {  9, 3,   92.0,  36.0 },
    { 10, 3, -102.0,   0.0 },
    { 10, 4,  112.0,  24.0 },
    { 11, 4, -124.0,  45.0 },
    { 11, 5,  136.0,  18.0 },
    { 12, 5, -148.0,  54.0 },
    { 12, 6,  160.0,  12.0 },
    { 13, 7, -172.0,  30.0 },
    { 14, 8,  184.0,  20.0 },
    { 15, 9, -196.0,  10.0 }
};

static double DegToRad(double deg)
{
    return deg * PI_VALUE / 180.0;
}

static const char *ChooseSavePath()
{
    FILE *file = fopen("../GameLib.h", "rb");
    if (file != NULL) {
        fclose(file);
        return "../coreball.save";
    }
    return "coreball.save";
}

static double NormalizeDeg(double deg)
{
    while (deg < 0.0) deg += 360.0;
    while (deg >= 360.0) deg -= 360.0;
    return deg;
}

static int PinX(double angleDeg)
{
    return CORE_X + (int)(cos(DegToRad(angleDeg)) * ORBIT_R);
}

static int PinY(double angleDeg)
{
    return CORE_Y + (int)(sin(DegToRad(angleDeg)) * ORBIT_R);
}

static void ResetLevel(int level, Pin pins[], int *pinCount, int *shotsLeft,
                       double *rotation, double *launchY, GameState *state)
{
    int i;
    LevelDef def = levels[level];
    *pinCount = 0;
    *shotsLeft = def.shots;
    *rotation = 0.0;
    *launchY = (double)LAUNCH_Y;
    *state = STATE_READY;

    for (i = 0; i < def.blockers && i < MAX_PINS; i++) {
        pins[*pinCount].localAngle = NormalizeDeg(def.offsetDeg + i * (360.0 / def.blockers));
        pins[*pinCount].playerShot = 0;
        (*pinCount)++;
    }
}

static void DrawPin(GameLib &game, double worldAngle, int playerShot)
{
    int x = PinX(worldAngle);
    int y = PinY(worldAngle);
    int lineX = CORE_X + (int)(cos(DegToRad(worldAngle)) * (CORE_R - 2));
    int lineY = CORE_Y + (int)(sin(DegToRad(worldAngle)) * (CORE_R - 2));
    uint32_t ballColor = playerShot ? COLOR_WHITE : COLOR_RGB(92, 112, 150);

    game.DrawLine(lineX, lineY, x, y, COLOR_RGB(118, 132, 160));
    game.FillCircle(x, y, BALL_R, ballColor);
    game.DrawCircle(x, y, BALL_R, COLOR_RGB(25, 31, 44));
}

static void DrawWaitingBalls(GameLib &game, int shotsLeft)
{
    int i;
    int visible = shotsLeft;
    if (visible > 8) visible = 8;

    for (i = 0; i < visible; i++) {
        int y = LAUNCH_Y + 28 + i * 18;
        uint32_t c = (i == 0) ? COLOR_WHITE : COLOR_RGB(150, 160, 178);
        if (y < WIN_H - 8) {
            game.FillCircle(LAUNCH_X, y, 6, c);
        }
    }

    if (shotsLeft > visible) {
        game.DrawPrintf(LAUNCH_X + 14, WIN_H - 28, COLOR_LIGHT_GRAY, "+%d", shotsLeft - visible);
    }
}

static int WouldHitExistingBall(Pin pins[], int pinCount, double rotation)
{
    int i;
    int newX = PinX(90.0);
    int newY = PinY(90.0);

    for (i = 0; i < pinCount; i++) {
        double worldAngle = NormalizeDeg(pins[i].localAngle + rotation);
        int x = PinX(worldAngle);
        int y = PinY(worldAngle);
        if (GameLib::CircleOverlap(newX, newY, BALL_R + 1, x, y, BALL_R + 1)) {
            return 1;
        }
    }
    return 0;
}

int main()
{
    GameLib game;
    game.Open(WIN_W, WIN_H, "16 - Coreball", true);
    game.AspectLock(true, COLOR_BLACK);
    game.ShowMouse(false);

    Pin pins[MAX_PINS];
    int pinCount = 0;
    int shotsLeft = 0;
    int level = 0;
    const char *savePath = ChooseSavePath();
    int bestLevel = GameLib::LoadInt(savePath, "best_level", 1);
    double rotation = 0.0;
    double launchY = (double)LAUNCH_Y;
    GameState state = STATE_READY;

    ResetLevel(level, pins, &pinCount, &shotsLeft, &rotation, &launchY, &state);

    while (!game.IsClosed()) {
        double dt = game.GetDeltaTime();
        int fire = 0;
        int i;

        if (game.IsKeyPressed(KEY_ESCAPE)) break;

        rotation = NormalizeDeg(rotation + levels[level].speedDeg * dt);

        if (state == STATE_READY) {
            if (game.IsKeyPressed(KEY_SPACE) ||
                game.IsMousePressed(MOUSE_LEFT) ||
                game.IsMousePressed(MOUSE_RIGHT)) {
                fire = 1;
            }
            if (fire && shotsLeft > 0) {
                state = STATE_LAUNCHING;
                launchY = (double)LAUNCH_Y;
                game.PlayBeep(520, 40, 1, 200);
            }
        } else if (state == STATE_LAUNCHING) {
            launchY -= LAUNCH_SPEED * dt;

            if (launchY <= PinY(90.0)) {
                if (WouldHitExistingBall(pins, pinCount, rotation)) {
                    state = STATE_GAME_OVER;
                    launchY = (double)PinY(90.0);
                    game.PlayBeep(160, 250, 1, 250);
                } else {
                    if (pinCount < MAX_PINS) {
                        pins[pinCount].localAngle = NormalizeDeg(90.0 - rotation);
                        pins[pinCount].playerShot = 1;
                        pinCount++;
                    }
                    shotsLeft--;
                    launchY = (double)LAUNCH_Y;
                    game.PlayBeep(740, 50, 1, 180);

                    if (shotsLeft <= 0) {
                        state = STATE_LEVEL_CLEAR;
                        if (level + 1 > bestLevel) {
                            bestLevel = level + 1;
                            GameLib::SaveInt(savePath, "best_level", bestLevel);
                        }
                        game.PlayBeep(980, 180, 1, 220);
                    } else {
                        state = STATE_READY;
                    }
                }
            }
        } else if (state == STATE_LEVEL_CLEAR) {
            if (game.IsKeyPressed(KEY_SPACE) ||
                game.IsMousePressed(MOUSE_LEFT) ||
                game.IsMousePressed(MOUSE_RIGHT)) {
                if (level + 1 < LEVEL_COUNT) level++;
                ResetLevel(level, pins, &pinCount, &shotsLeft, &rotation, &launchY, &state);
            }
        } else if (state == STATE_GAME_OVER) {
            if (game.IsKeyPressed(KEY_R) ||
                game.IsKeyPressed(KEY_SPACE) ||
                game.IsMousePressed(MOUSE_LEFT) ||
                game.IsMousePressed(MOUSE_RIGHT)) {
                ResetLevel(level, pins, &pinCount, &shotsLeft, &rotation, &launchY, &state);
                game.PlayBeep(420, 70, 1, 180);
            }
        }

        game.Clear(COLOR_RGB(16, 19, 28));

        for (i = 0; i < WIN_H; i += 40) {
            uint32_t stripe = ((i / 40) & 1) ? COLOR_RGB(18, 23, 34) : COLOR_RGB(14, 17, 26);
            game.FillRect(0, i, WIN_W, 40, stripe);
        }

        game.DrawPrintf(18, 16, COLOR_LIGHT_GRAY, "Level %d / %d", level + 1, LEVEL_COUNT);
        game.DrawPrintf(18, 32, COLOR_GRAY, "Best: %d", bestLevel);
        game.DrawPrintf(WIN_W - 112, 16, COLOR_LIGHT_GRAY, "Pins: %d", shotsLeft);

        for (i = 0; i < pinCount; i++) {
            double worldAngle = NormalizeDeg(pins[i].localAngle + rotation);
            DrawPin(game, worldAngle, pins[i].playerShot);
        }

        game.FillCircle(CORE_X, CORE_Y, CORE_R + 7, COLOR_RGB(36, 43, 62));
        game.FillCircle(CORE_X, CORE_Y, CORE_R, COLOR_RGB(232, 236, 242));
        game.DrawCircle(CORE_X, CORE_Y, CORE_R, COLOR_RGB(25, 31, 44));
        game.DrawPrintfScale(CORE_X - 12, CORE_Y - 13, COLOR_RGB(25, 31, 44), 14, 14, "%d", shotsLeft);

        if (state == STATE_LAUNCHING || state == STATE_GAME_OVER) {
            game.FillCircle(LAUNCH_X, (int)launchY, BALL_R, COLOR_WHITE);
            game.DrawCircle(LAUNCH_X, (int)launchY, BALL_R, COLOR_RGB(25, 31, 44));
        } else if (state == STATE_READY) {
            game.FillCircle(LAUNCH_X, LAUNCH_Y, BALL_R, COLOR_WHITE);
            game.DrawCircle(LAUNCH_X, LAUNCH_Y, BALL_R, COLOR_RGB(25, 31, 44));
        }

        DrawWaitingBalls(game, shotsLeft);

        game.DrawText(150, WIN_H - 34, "SPACE / LEFT / RIGHT CLICK: shoot    ESC: quit", COLOR_GRAY);

        if (state == STATE_LEVEL_CLEAR) {
            game.FillRect(190, 214, 260, 86, COLOR_RGB(32, 40, 58));
            game.DrawRect(190, 214, 260, 86, COLOR_RGB(130, 150, 190));
            if (level + 1 >= LEVEL_COUNT) {
                game.DrawTextScale(226, 228, "ALL CLEAR", COLOR_GOLD, 12, 12);
                game.DrawText(228, 268, "SPACE / CLICK to replay", COLOR_LIGHT_GRAY);
            } else {
                game.DrawTextScale(220, 228, "LEVEL CLEAR", COLOR_GREEN, 12, 12);
                game.DrawText(228, 268, "SPACE / CLICK for next", COLOR_LIGHT_GRAY);
            }
        } else if (state == STATE_GAME_OVER) {
            game.FillRect(198, 214, 244, 86, COLOR_RGB(54, 32, 38));
            game.DrawRect(198, 214, 244, 86, COLOR_RGB(190, 110, 120));
            game.DrawTextScale(228, 228, "GAME OVER", COLOR_RED, 12, 12);
            game.DrawText(230, 268, "R / SPACE to retry", COLOR_LIGHT_GRAY);
        }

        game.Update();
        game.WaitFrame(60);
    }

    return 0;
}
