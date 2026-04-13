// 09_sprite_animation.cpp - Sprite Animation
//
// Use DrawSpriteFrameScaled for sprite sheet frame animation:
// A little character walks on screen, switching animation frames by direction.
// Sprite is generated with code (no external BMP files needed).
// Learn: DrawSpriteFrameScaled, DrawSpriteScaled, GetDeltaTime, frame animation basics
//
// Compile: g++ -o 09_sprite_animation.exe 09_sprite_animation.cpp -mwindows

#include "../GameLib.h"

// Create a simple character sprite sheet: 4 directions x 3 frames = 12 frames
// Each frame 12x16, whole sheet 36x64
// Rows: down(0) left(1) right(2) up(3)
// Columns: 3 walking animation frames
int CreateCharSheet(GameLib &game)
{
    int fw = 12, fh = 16;
    int cols = 3, rows = 4;
    int id = game.CreateSprite(fw * cols, fh * rows);
    if (id < 0) return -1;

    // All transparent
    for (int y = 0; y < fh * rows; y++)
        for (int x = 0; x < fw * cols; x++)
            game.SetSpritePixel(id, x, y, 0x00000000);

    uint32_t skin = COLOR_RGB(255, 200, 150);
    uint32_t hair = COLOR_BROWN;
    uint32_t shirt = COLOR_BLUE;
    uint32_t pants = COLOR_DARK_BLUE;
    uint32_t shoe = COLOR_DARK_GRAY;

    // Draw a stick figure for each direction and frame
    for (int dir = 0; dir < 4; dir++) {
        for (int f = 0; f < 3; f++) {
            int ox = f * fw;
            int oy = dir * fh;

            // Head (4x4)
            for (int dy = 0; dy < 4; dy++)
                for (int dx = 0; dx < 4; dx++)
                    game.SetSpritePixel(id, ox + 4 + dx, oy + dy, skin);

            // Hair (top row)
            for (int dx = 0; dx < 4; dx++)
                game.SetSpritePixel(id, ox + 4 + dx, oy, hair);

            // Body (6x5)
            for (int dy = 4; dy < 9; dy++)
                for (int dx = 0; dx < 6; dx++)
                    game.SetSpritePixel(id, ox + 3 + dx, oy + dy, shirt);

            // Pants (6x3)
            for (int dy = 9; dy < 12; dy++)
                for (int dx = 0; dx < 6; dx++)
                    game.SetSpritePixel(id, ox + 3 + dx, oy + dy, pants);

            // Feet - walking animation based on frame
            int leftFootX, rightFootX;
            if (f == 0) { leftFootX = 3; rightFootX = 7; }       // standing
            else if (f == 1) { leftFootX = 2; rightFootX = 8; }  // left foot forward
            else { leftFootX = 4; rightFootX = 6; }              // right foot forward

            for (int dy = 12; dy < 14; dy++) {
                game.SetSpritePixel(id, ox + leftFootX, oy + dy, shoe);
                game.SetSpritePixel(id, ox + leftFootX + 1, oy + dy, shoe);
                game.SetSpritePixel(id, ox + rightFootX, oy + dy, shoe);
                game.SetSpritePixel(id, ox + rightFootX + 1, oy + dy, shoe);
            }

            // Eyes (position varies by direction)
            if (dir == 0) { // down
                game.SetSpritePixel(id, ox + 5, oy + 2, COLOR_BLACK);
                game.SetSpritePixel(id, ox + 7, oy + 2, COLOR_BLACK);
            } else if (dir == 1) { // left
                game.SetSpritePixel(id, ox + 4, oy + 2, COLOR_BLACK);
            } else if (dir == 2) { // right
                game.SetSpritePixel(id, ox + 7, oy + 2, COLOR_BLACK);
            } else { // up - no eyes, draw back hair color
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
    int fw = 12, fh = 16;  // frame size
    int scale = 3;          // display scale

    float px = 300, py = 220;
    float speed = 100.0f;   // pixels per second
    int dir = 0;            // 0=down 1=left 2=right 3=up
    int frame = 0;
    float animTimer = 0;
    bool moving = false;

    while (!game.IsClosed()) {
        if (game.IsKeyPressed(KEY_ESCAPE)) break;

        float dt = game.GetDeltaTime();
        moving = false;

        // Movement
        if (game.IsKeyDown(KEY_DOWN)  || game.IsKeyDown(KEY_S)) { py += speed * dt; dir = 0; moving = true; }
        if (game.IsKeyDown(KEY_LEFT)  || game.IsKeyDown(KEY_A)) { px -= speed * dt; dir = 1; moving = true; }
        if (game.IsKeyDown(KEY_RIGHT) || game.IsKeyDown(KEY_D)) { px += speed * dt; dir = 2; moving = true; }
        if (game.IsKeyDown(KEY_UP)    || game.IsKeyDown(KEY_W)) { py -= speed * dt; dir = 3; moving = true; }

        // Boundary check
        if (px < 0) px = 0;
        if (py < 0) py = 0;
        if (px > game.GetWidth() - fw * scale) px = (float)(game.GetWidth() - fw * scale);
        if (py > game.GetHeight() - fh * scale) py = (float)(game.GetHeight() - fh * scale);

        // Animation frame update
        if (moving) {
            animTimer += dt;
            if (animTimer >= 0.15f) {
                animTimer = 0;
                frame = (frame + 1) % 3;
            }
        } else {
            frame = 0;  // standing frame
            animTimer = 0;
        }

        // --- Drawing ---
        game.Clear(COLOR_DARK_GREEN);

        // Ground decoration
        for (int i = 0; i < 30; i++) {
            int gx = (i * 97 + 13) % game.GetWidth();
            int gy = (i * 173 + 47) % game.GetHeight();
            game.FillRect(gx, gy, 3, 3, COLOR_GREEN);
        }

        // Character (scaled frame drawing)
        int frameIndex = dir * 3 + frame;
        game.DrawSpriteFrameScaled(charSheet, (int)px, (int)py,
                                   fw, fh, frameIndex,
                                   fw * scale, fh * scale);

        // Sprite Sheet preview (top right)
        game.DrawText(470, 10, "Sprite Sheet:", COLOR_WHITE);
        int previewScale = 2;
        int sheetW = fw * 3, sheetH = fh * 4;
        int pvX = 470, pvY = 25;
        game.DrawSpriteScaled(charSheet, pvX, pvY, sheetW * previewScale, sheetH * previewScale);
        game.DrawRect(pvX, pvY, sheetW * previewScale, sheetH * previewScale, COLOR_GRAY);
        // Highlight current frame
        game.DrawRect(pvX + frame * fw * previewScale, pvY + dir * fh * previewScale,
                      fw * previewScale, fh * previewScale, COLOR_YELLOW);

        // Info
        const char *dirNames[] = {"Down", "Left", "Right", "Up"};
        game.DrawText(10, 10, "WASD / Arrow keys to move", COLOR_WHITE);
        game.DrawPrintf(10, 25, COLOR_GRAY, "Dir: %s  Frame: %d", dirNames[dir], frame);

        game.Update();
        game.WaitFrame(60);
    }

    game.FreeSprite(charSheet);
    return 0;
}
