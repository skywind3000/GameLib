// 20_sprite_rotation.cpp - Sprite Rotation Demo
//
// Draw procedurally created sprites with center-based rotation.
//
// Controls:
//   Mouse wheel / Q E : change angular speed
//   A D / Left Right  : change pulse frame
//   F                 : toggle horizontal flip
//   SPACE             : pause / resume spin
//   R                 : reset angle, speed, frame and flip
//   ESC               : quit
//
// Learn: CreateSprite, SetSpritePixel, DrawSpriteRotated,
//        DrawSpriteFrameRotated, SetSpriteColorKey,
//        center-based rotation with current sprite flags
//
// Compile: g++ -o 20_sprite_rotation.exe 20_sprite_rotation.cpp -mwindows

#include "../GameLib.h"

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

static void DrawRotationBox(GameLib &game, int spriteId,
                            int x, int y, int w, int h,
                            double angleDeg, int flags, const char *label)
{
    DrawCheckerboard(game, x, y, w, h, 10);
    game.DrawRect(x, y, w, h, COLOR_RGB(98, 110, 138));
    DrawCrosshair(game, x + w / 2, y + h / 2, 14, COLOR_RGB(120, 138, 168));
    game.DrawSpriteRotated(spriteId, x + w / 2, y + h / 2, angleDeg, flags);
    game.DrawText(x + 8, y + h - 18, label, COLOR_LIGHT_GRAY);
}

static void DrawFrameRotationBox(GameLib &game, int spriteId, int frameIndex,
                                 int x, int y, int w, int h,
                                 int frameW, int frameH,
                                 double angleDeg, int flags, const char *label)
{
    DrawCheckerboard(game, x, y, w, h, 10);
    game.DrawRect(x, y, w, h, COLOR_RGB(98, 110, 138));
    DrawCrosshair(game, x + w / 2, y + h / 2, 12, COLOR_RGB(120, 138, 168));
    game.DrawSpriteFrameRotated(spriteId, x + w / 2, y + h / 2,
                                frameW, frameH, frameIndex, angleDeg, flags);
    game.DrawText(x + 8, y + h - 18, label, COLOR_LIGHT_GRAY);
}

int main()
{
    GameLib game;
    const int SW = 920;
    const int SH = 620;
    game.Open(SW, SH, "20 - Sprite Rotation", true);

    int ship = CreateShipSprite(game);
    int pulse = CreatePulseSheet(game);
    int angleSpeed = 3;
    int frame = 0;
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
            angle = 0.0;
            angleSpeed = 3;
            frame = 0;
            flip = false;
            running = true;
        }

        if (running) {
            angle += (double)angleSpeed;
            WrapAngle(&angle);
        }

        game.Clear(COLOR_RGB(18, 22, 36));
        game.FillRect(0, 0, SW, 56, COLOR_RGB(10, 14, 24));
        game.DrawText(20, 16, "Wheel/Q/E speed   A/D change frame   F flip   SPACE pause   R reset   ESC quit", COLOR_WHITE);

        if (ship < 0 || pulse < 0) {
            game.DrawText(20, 90, "Failed to create demo sprites.", COLOR_RED);
            game.Update();
            game.WaitFrame(60);
            continue;
        }

        int flags = SPRITE_COLORKEY | (flip ? SPRITE_FLIP_H : 0);
        const char *flipText = flip ? "On" : "Off";
        const char *spinText = running ? "Running" : "Paused";

        game.DrawPrintf(20, 34, COLOR_LIGHT_GRAY,
                        "Angle: %.1f   Speed: %d deg/frame   Frame: %d   Flip: %s   Spin: %s",
                        angle, angleSpeed, frame, flipText, spinText);

        DrawPanel(game, 20, 72, 200, 528, "Source Sprites");
        game.DrawText(36, 104, "CreateSprite ship", COLOR_LIGHT_GRAY);
        DrawCheckerboard(game, 36, 124, 168, 120, 8);
        game.DrawRect(36, 124, 168, 120, COLOR_RGB(98, 110, 138));
        game.DrawSpriteEx(ship, 36 + (168 - 17) / 2, 124 + (120 - 17) / 2, SPRITE_COLORKEY);
        game.DrawText(36, 254, "17x17, ColorKey background", COLOR_GRAY);

        game.DrawText(36, 294, "Pulse sprite sheet", COLOR_LIGHT_GRAY);
        DrawCheckerboard(game, 36, 314, 168, 92, 8);
        game.DrawRect(36, 314, 168, 92, COLOR_RGB(98, 110, 138));
        game.DrawSpriteScaled(pulse, 56, 344, 128, 32, SPRITE_COLORKEY);
        game.DrawRect(56 + frame * 32, 344, 32, 32, COLOR_GOLD);
        game.DrawText(36, 416, "4 frames, each 16x16", COLOR_GRAY);

        game.DrawText(36, 462, "Uses:", COLOR_WHITE);
        game.DrawText(36, 482, "Center-based rotation", COLOR_LIGHT_GRAY);
        game.DrawText(36, 498, "Flags still apply", COLOR_LIGHT_GRAY);
        game.DrawText(36, 514, "No external assets needed", COLOR_LIGHT_GRAY);

        DrawPanel(game, 240, 72, 660, 260, "DrawSpriteRotated");
        DrawRotationBox(game, ship, 256, 124, 280, 180, angle, flags, "Animated ship");
        DrawRotationBox(game, ship, 560, 124, 146, 80, 0.0, flags, "0 deg");
        DrawRotationBox(game, ship, 724, 124, 146, 80, 45.0, flags, "45 deg");
        DrawRotationBox(game, ship, 560, 224, 146, 80, 90.0, flags, "90 deg");
        DrawRotationBox(game, ship, 724, 224, 146, 80, 135.0, flags, "135 deg");

        DrawPanel(game, 240, 348, 660, 252, "DrawSpriteFrameRotated");
        game.DrawText(256, 380, "Current frame from sprite sheet", COLOR_LIGHT_GRAY);
        DrawFrameRotationBox(game, pulse, frame, 256, 404, 220, 164,
                             16, 16, angle, flags, "Current pulse");
        DrawFrameRotationBox(game, pulse, 0, 504, 404, 92, 72,
                             16, 16, angle, flags, "Frame 0");
        DrawFrameRotationBox(game, pulse, 1, 612, 404, 92, 72,
                             16, 16, angle + 30.0, flags, "Frame 1");
        DrawFrameRotationBox(game, pulse, 2, 720, 404, 92, 72,
                             16, 16, angle + 60.0, flags, "Frame 2");
        DrawFrameRotationBox(game, pulse, 3, 504, 496, 92, 72,
                             16, 16, angle + 90.0, flags, "Frame 3");
        DrawFrameRotationBox(game, pulse, frame, 612, 496, 200, 72,
                             16, 16, angle, flags, "A/D changes selected frame");

        game.Update();
        game.WaitFrame(60);
    }

    if (ship >= 0) game.FreeSprite(ship);
    if (pulse >= 0) game.FreeSprite(pulse);
    return 0;
}