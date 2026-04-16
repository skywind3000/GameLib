#define GAMELIB_SDL_DISABLE_IMAGE 1
#define GAMELIB_SDL_DISABLE_TTF 1
#define GAMELIB_SDL_DISABLE_MIXER 1
#include "../GameLib.SDL.h"

static int ClampInt(int value, int minValue, int maxValue)
{
    if (value < minValue) return minValue;
    if (value > maxValue) return maxValue;
    return value;
}

static void WrapAngle(double *angle)
{
    while (*angle >= 360.0) *angle -= 360.0;
    while (*angle < 0.0) *angle += 360.0;
}

static void DrawPanel(GameLib &game, int x, int y, int w, int h, const char *title)
{
    game.FillRect(x, y, w, h, COLOR_RGB(28, 34, 50));
    game.DrawRect(x, y, w, h, COLOR_RGB(84, 94, 120));
    game.FillRect(x + 1, y + 1, w - 2, 22, COLOR_RGB(38, 48, 72));
    game.DrawText(x + 8, y + 7, title, COLOR_WHITE);
}

static void DrawCheckerboard(GameLib &game, int x, int y, int w, int h, int cell)
{
    uint32_t c0 = COLOR_RGB(48, 54, 70);
    uint32_t c1 = COLOR_RGB(62, 70, 90);

    for (int py = y; py < y + h; py += cell) {
        for (int px = x; px < x + w; px += cell) {
            int cw = cell;
            int ch = cell;
            if (px + cw > x + w) cw = x + w - px;
            if (py + ch > y + h) ch = y + h - py;
            game.FillRect(px, py, cw, ch,
                          (((px - x) / cell + (py - y) / cell) & 1) ? c0 : c1);
        }
    }
}

static void DrawCrosshair(GameLib &game, int cx, int cy, int size, uint32_t color)
{
    game.DrawLine(cx - size, cy, cx + size, cy, color);
    game.DrawLine(cx, cy - size, cx, cy + size, color);
}

static int CreateShipSprite(GameLib &game)
{
    int id = game.CreateSprite(17, 17);
    if (id < 0) return -1;

    for (int y = 0; y < 17; y++) {
        for (int x = 0; x < 17; x++) {
            game.SetSpritePixel(id, x, y, COLORKEY_DEFAULT);
        }
    }

    for (int y = 1; y <= 6; y++) {
        int span = y - 1;
        for (int x = 8 - span; x <= 8 + span; x++) {
            game.SetSpritePixel(id, x, y, COLOR_WHITE);
        }
    }

    for (int y = 6; y <= 12; y++) {
        for (int x = 6; x <= 10; x++) {
            game.SetSpritePixel(id, x, y, COLOR_CYAN);
        }
    }

    for (int x = 2; x <= 5; x++) {
        game.SetSpritePixel(id, x, 9, COLOR_GRAY);
        game.SetSpritePixel(id, x, 10, COLOR_GRAY);
    }
    for (int x = 11; x <= 14; x++) {
        game.SetSpritePixel(id, x, 9, COLOR_GRAY);
        game.SetSpritePixel(id, x, 10, COLOR_GRAY);
    }

    game.SetSpritePixel(id, 6, 13, COLOR_DARK_GRAY);
    game.SetSpritePixel(id, 10, 13, COLOR_DARK_GRAY);
    game.SetSpritePixel(id, 7, 13, COLOR_DARK_GRAY);
    game.SetSpritePixel(id, 9, 13, COLOR_DARK_GRAY);
    game.SetSpritePixel(id, 7, 14, COLOR_ORANGE);
    game.SetSpritePixel(id, 8, 14, COLOR_ORANGE);
    game.SetSpritePixel(id, 9, 14, COLOR_ORANGE);
    game.SetSpritePixel(id, 7, 15, COLOR_YELLOW);
    game.SetSpritePixel(id, 8, 16, COLOR_YELLOW);
    game.SetSpritePixel(id, 9, 15, COLOR_YELLOW);

    game.SetSpriteColorKey(id, COLORKEY_DEFAULT);
    return id;
}

static int CreatePulseSheet(GameLib &game)
{
    int id = game.CreateSprite(64, 16);
    if (id < 0) return -1;

    uint32_t colors[4] = { COLOR_RED, COLOR_ORANGE, COLOR_YELLOW, COLOR_WHITE };
    int radii[4] = { 2, 3, 4, 3 };

    for (int y = 0; y < 16; y++) {
        for (int x = 0; x < 64; x++) {
            game.SetSpritePixel(id, x, y, COLORKEY_DEFAULT);
        }
    }

    for (int frame = 0; frame < 4; frame++) {
        int cx = frame * 16 + 8;
        int cy = 8;
        int radius = radii[frame];

        for (int dy = -radius; dy <= radius; dy++) {
            for (int dx = -radius; dx <= radius; dx++) {
                int absX = dx < 0 ? -dx : dx;
                int absY = dy < 0 ? -dy : dy;
                if (absX + absY <= radius) {
                    game.SetSpritePixel(id, cx + dx, cy + dy, colors[frame]);
                }
            }
        }

        game.SetSpritePixel(id, cx, cy, COLOR_WHITE);
        game.SetSpritePixel(id, cx - 1, cy, COLOR_WHITE);
        game.SetSpritePixel(id, cx + 1, cy, COLOR_WHITE);
        game.SetSpritePixel(id, cx, cy - 1, COLOR_WHITE);
        game.SetSpritePixel(id, cx, cy + 1, COLOR_WHITE);
    }

    game.SetSpriteColorKey(id, COLORKEY_DEFAULT);
    return id;
}

int main()
{
    GameLib game;
    if (game.Open(820, 560, "SDL Demo 16 - Sprite Rotation", true) != 0) {
        return 1;
    }

    int ship = CreateShipSprite(game);
    int pulse = CreatePulseSheet(game);
    if (ship < 0 || pulse < 0) {
        if (ship >= 0) game.FreeSprite(ship);
        if (pulse >= 0) game.FreeSprite(pulse);
        return 1;
    }

    int frame = 0;
    int angleSpeed = 3;
    bool flip = false;
    bool running = true;
    double angle = 0.0;

    while (!game.IsClosed()) {
        if (game.IsKeyPressed(KEY_ESCAPE)) break;

        int wheel = game.GetMouseWheelDelta();
        if (wheel > 0) angleSpeed++;
        if (wheel < 0) angleSpeed--;
        if (game.IsKeyPressed(KEY_Q)) angleSpeed--;
        if (game.IsKeyPressed(KEY_E)) angleSpeed++;
        angleSpeed = ClampInt(angleSpeed, -12, 12);

        if (game.IsKeyPressed(KEY_A) || game.IsKeyPressed(KEY_LEFT))
            frame = (frame + 3) % 4;
        if (game.IsKeyPressed(KEY_D) || game.IsKeyPressed(KEY_RIGHT))
            frame = (frame + 1) % 4;
        if (game.IsKeyPressed(KEY_F)) flip = !flip;
        if (game.IsKeyPressed(KEY_SPACE)) running = !running;
        if (game.IsKeyPressed(KEY_R)) {
            frame = 0;
            angleSpeed = 3;
            flip = false;
            running = true;
            angle = 0.0;
        }

        if (running) {
            angle += (double)angleSpeed;
            WrapAngle(&angle);
        }

        int flags = SPRITE_COLORKEY | (flip ? SPRITE_FLIP_H : 0);

        game.Clear(COLOR_BLACK);
        game.FillRect(0, 0, 820, 54, COLOR_RGB(10, 14, 24));
        game.DrawText(16, 16, "SDL rotation demo: Wheel/Q/E speed  A/D frame  F flip  SPACE pause  R reset  ESC quit", COLOR_WHITE);
        game.DrawPrintf(16, 34, COLOR_LIGHT_GRAY,
                        "Angle: %.1f   Speed: %d   Frame: %d   Flip: %s",
                        angle, angleSpeed, frame, flip ? "On" : "Off");

        DrawPanel(game, 16, 72, 180, 468, "Source");
        DrawCheckerboard(game, 32, 112, 148, 120, 8);
        game.DrawRect(32, 112, 148, 120, COLOR_RGB(98, 110, 138));
        game.DrawSpriteEx(ship, 32 + (148 - 17) / 2, 112 + (120 - 17) / 2, SPRITE_COLORKEY);
        game.DrawText(32, 244, "17x17 ship", COLOR_GRAY);

        DrawCheckerboard(game, 32, 290, 148, 90, 8);
        game.DrawRect(32, 290, 148, 90, COLOR_RGB(98, 110, 138));
        game.DrawSpriteScaled(pulse, 42, 318, 128, 32, SPRITE_COLORKEY);
        game.DrawRect(42 + frame * 32, 318, 32, 32, COLOR_GOLD);
        game.DrawText(32, 392, "Pulse sheet", COLOR_GRAY);
        game.DrawText(32, 430, "DrawSpriteRotated", COLOR_WHITE);
        game.DrawText(32, 446, "DrawSpriteFrameRotated", COLOR_WHITE);

        DrawPanel(game, 214, 72, 590, 220, "DrawSpriteRotated");
        DrawCheckerboard(game, 232, 112, 264, 156, 10);
        game.DrawRect(232, 112, 264, 156, COLOR_RGB(98, 110, 138));
        DrawCrosshair(game, 364, 190, 18, COLOR_RGB(120, 138, 168));
        game.DrawSpriteRotated(ship, 364, 190, angle, flags);
        game.DrawText(240, 244, "Animated ship", COLOR_LIGHT_GRAY);

        DrawCheckerboard(game, 530, 112, 118, 70, 10);
        game.DrawRect(530, 112, 118, 70, COLOR_RGB(98, 110, 138));
        DrawCrosshair(game, 589, 147, 12, COLOR_RGB(120, 138, 168));
        game.DrawSpriteRotated(ship, 589, 147, 45.0, flags);
        game.DrawText(538, 160, "45 deg", COLOR_LIGHT_GRAY);

        DrawCheckerboard(game, 668, 112, 118, 70, 10);
        game.DrawRect(668, 112, 118, 70, COLOR_RGB(98, 110, 138));
        DrawCrosshair(game, 727, 147, 12, COLOR_RGB(120, 138, 168));
        game.DrawSpriteRotated(ship, 727, 147, 90.0, flags);
        game.DrawText(676, 160, "90 deg", COLOR_LIGHT_GRAY);

        DrawCheckerboard(game, 530, 198, 118, 70, 10);
        game.DrawRect(530, 198, 118, 70, COLOR_RGB(98, 110, 138));
        DrawCrosshair(game, 589, 233, 12, COLOR_RGB(120, 138, 168));
        game.DrawSpriteRotated(ship, 589, 233, 135.0, flags);
        game.DrawText(538, 246, "135 deg", COLOR_LIGHT_GRAY);

        DrawCheckerboard(game, 668, 198, 118, 70, 10);
        game.DrawRect(668, 198, 118, 70, COLOR_RGB(98, 110, 138));
        DrawCrosshair(game, 727, 233, 12, COLOR_RGB(120, 138, 168));
        game.DrawSpriteRotated(ship, 727, 233, 0.0, flags);
        game.DrawText(676, 246, "0 deg", COLOR_LIGHT_GRAY);

        DrawPanel(game, 214, 310, 590, 230, "DrawSpriteFrameRotated");
        DrawCheckerboard(game, 232, 350, 220, 160, 10);
        game.DrawRect(232, 350, 220, 160, COLOR_RGB(98, 110, 138));
        DrawCrosshair(game, 342, 430, 16, COLOR_RGB(120, 138, 168));
        game.DrawSpriteFrameRotated(pulse, 342, 430, 16, 16, frame, angle, flags);
        game.DrawText(240, 486, "Selected frame", COLOR_LIGHT_GRAY);

        DrawCheckerboard(game, 486, 350, 82, 70, 10);
        game.DrawRect(486, 350, 82, 70, COLOR_RGB(98, 110, 138));
        DrawCrosshair(game, 527, 385, 10, COLOR_RGB(120, 138, 168));
        game.DrawSpriteFrameRotated(pulse, 527, 385, 16, 16, 0, angle, flags);
        game.DrawText(494, 402, "F0", COLOR_LIGHT_GRAY);

        DrawCheckerboard(game, 584, 350, 82, 70, 10);
        game.DrawRect(584, 350, 82, 70, COLOR_RGB(98, 110, 138));
        DrawCrosshair(game, 625, 385, 10, COLOR_RGB(120, 138, 168));
        game.DrawSpriteFrameRotated(pulse, 625, 385, 16, 16, 1, angle + 30.0, flags);
        game.DrawText(592, 402, "F1", COLOR_LIGHT_GRAY);

        DrawCheckerboard(game, 682, 350, 82, 70, 10);
        game.DrawRect(682, 350, 82, 70, COLOR_RGB(98, 110, 138));
        DrawCrosshair(game, 723, 385, 10, COLOR_RGB(120, 138, 168));
        game.DrawSpriteFrameRotated(pulse, 723, 385, 16, 16, 2, angle + 60.0, flags);
        game.DrawText(690, 402, "F2", COLOR_LIGHT_GRAY);

        DrawCheckerboard(game, 486, 440, 82, 70, 10);
        game.DrawRect(486, 440, 82, 70, COLOR_RGB(98, 110, 138));
        DrawCrosshair(game, 527, 475, 10, COLOR_RGB(120, 138, 168));
        game.DrawSpriteFrameRotated(pulse, 527, 475, 16, 16, 3, angle + 90.0, flags);
        game.DrawText(494, 492, "F3", COLOR_LIGHT_GRAY);

        game.DrawText(584, 454, "Core SDL2-only rotation demo", COLOR_LIGHT_GRAY);
        game.DrawText(584, 470, "No SDL_image / TTF / mixer needed", COLOR_LIGHT_GRAY);
        game.DrawText(584, 486, "Uses CreateSprite + ColorKey", COLOR_LIGHT_GRAY);

        game.Update();
        game.WaitFrame(60);
    }

    game.FreeSprite(ship);
    game.FreeSprite(pulse);
    return 0;
}