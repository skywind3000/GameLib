#include "../GameLib.SDL.h"

static int ClampInt(int value, int minValue, int maxValue)
{
    if (value < minValue) return minValue;
    if (value > maxValue) return maxValue;
    return value;
}

static void FillSpriteRect(GameLib &game, int spriteId, int x, int y, int w, int h, uint32_t color)
{
    for (int py = 0; py < h; py++) {
        for (int px = 0; px < w; px++) {
            game.SetSpritePixel(spriteId, x + px, y + py, color);
        }
    }
}

static void BuildTileset(GameLib &game, int spriteId, int tileSize)
{
    for (int y = 0; y < tileSize; y++) {
        for (int x = 0; x < tileSize; x++) {
            uint32_t grass = (((x / 4) + (y / 4)) % 2 == 0) ? COLOR_GREEN : COLOR_DARK_GREEN;
            uint32_t water = (((x + y) / 3) % 2 == 0) ? COLOR_BLUE : COLOR_SKY_BLUE;
            uint32_t wall = (((x / 4) + (y / 4)) % 2 == 0) ? COLOR_LIGHT_GRAY : COLOR_GRAY;
            uint32_t gold = (((x / 4) + (y / 4)) % 2 == 0) ? COLOR_GOLD : COLOR_ORANGE;

            if (x == 0 || y == 0 || x == tileSize - 1 || y == tileSize - 1) {
                grass = COLOR_DARK_GREEN;
                water = COLOR_DARK_BLUE;
                wall = COLOR_DARK_GRAY;
                gold = COLOR_BROWN;
            }

            game.SetSpritePixel(spriteId, x, y, grass);
            game.SetSpritePixel(spriteId, tileSize + x, y, water);
            game.SetSpritePixel(spriteId, tileSize * 2 + x, y, wall);
            game.SetSpritePixel(spriteId, tileSize * 3 + x, y, gold);
        }
    }

    for (int x = 0; x < tileSize; x += 4) {
        for (int y = 0; y < tileSize; y++) {
            game.SetSpritePixel(spriteId, tileSize * 2 + x, y, COLOR_DARK_GRAY);
        }
    }
    for (int y = 0; y < tileSize; y += 4) {
        for (int x = 0; x < tileSize; x++) {
            game.SetSpritePixel(spriteId, tileSize * 2 + x, y, COLOR_DARK_GRAY);
        }
    }
}

static void BuildPlayerSheet(GameLib &game, int spriteId)
{
    FillSpriteRect(game, spriteId, 0, 0, 32, 16, COLORKEY_DEFAULT);

    for (int frame = 0; frame < 2; frame++) {
        int ox = frame * 16;

        FillSpriteRect(game, spriteId, ox + 7, 2, 2, 11, COLOR_WHITE);
        FillSpriteRect(game, spriteId, ox + 5, 5, 6, 6, COLOR_LIGHT_GRAY);
        FillSpriteRect(game, spriteId, ox + 3, 7, 2, 2, COLOR_RED);
        FillSpriteRect(game, spriteId, ox + 11, 7, 2, 2, COLOR_RED);
        FillSpriteRect(game, spriteId, ox + 7, 5, 2, 2, COLOR_CYAN);
        FillSpriteRect(game, spriteId, ox + 6, 10, 4, 2, COLOR_GRAY);

        if (frame == 0) {
            FillSpriteRect(game, spriteId, ox + 6, 12, 4, 2, COLOR_ORANGE);
            FillSpriteRect(game, spriteId, ox + 7, 14, 2, 1, COLOR_YELLOW);
        } else {
            FillSpriteRect(game, spriteId, ox + 5, 12, 6, 2, COLOR_YELLOW);
            FillSpriteRect(game, spriteId, ox + 7, 14, 2, 1, COLOR_WHITE);
        }
    }
}

static void BuildMap(GameLib &game, int mapId, int cols, int rows)
{
    game.ClearTilemap(mapId, 0);

    game.FillTileRect(mapId, 0, 0, cols, 1, 2);
    game.FillTileRect(mapId, 0, rows - 1, cols, 1, 2);
    game.FillTileRect(mapId, 0, 0, 1, rows, 2);
    game.FillTileRect(mapId, cols - 1, 0, 1, rows, 2);

    game.FillTileRect(mapId, 8, 6, 14, 10, 1);
    game.FillTileRect(mapId, 30, 10, 18, 12, 1);
    game.FillTileRect(mapId, 54, 18, 10, 20, 2);
    game.FillTileRect(mapId, 12, 28, 22, 8, 3);

    for (int row = 8; row < rows - 8; row += 6) {
        game.FillTileRect(mapId, 40, row, 2, 3, 2);
    }

    for (int col = 4; col < cols - 4; col++) {
        if ((col % 5) != 0) {
            game.SetTile(mapId, col, rows / 2, 3);
        }
    }

    for (int i = 0; i < 80; i++) {
        int col = GameLib::Random(2, cols - 3);
        int row = GameLib::Random(2, rows - 3);
        if (game.GetTile(mapId, col, row) == 0) {
            game.SetTile(mapId, col, row, (i % 3 == 0) ? 3 : 1);
        }
    }
}

static bool HitsWall(GameLib &game, int mapId, float x, float y)
{
    int left = (int)x + 2;
    int right = (int)x + 13;
    int top = (int)y + 2;
    int bottom = (int)y + 13;

    return game.GetTileAtPixel(mapId, left, top) == 2 ||
           game.GetTileAtPixel(mapId, right, top) == 2 ||
           game.GetTileAtPixel(mapId, left, bottom) == 2 ||
           game.GetTileAtPixel(mapId, right, bottom) == 2;
}

int main()
{
    GameLib game;
    if (game.Open(960, 640, "GameLib.SDL Demo 1", true) != 0) {
        return 1;
    }
    game.ShowFps(true);

    const int tileSize = 16;
    const int mapCols = 80;
    const int mapRows = 60;

    int tileset = game.CreateSprite(tileSize * 4, tileSize);
    int playerSheet = game.CreateSprite(32, 16);
    if (tileset < 0 || playerSheet < 0) {
        return 1;
    }

    BuildTileset(game, tileset, tileSize);
    BuildPlayerSheet(game, playerSheet);

    int mapId = game.CreateTilemap(mapCols, mapRows, tileSize, tileset);
    if (mapId < 0) {
        return 1;
    }
    BuildMap(game, mapId, mapCols, mapRows);

    float playerX = 5.0f * tileSize;
    float playerY = 5.0f * tileSize;
    int playerScale = 2;
    int facing = 1;
    bool showGrid = false;
    int tick = 0;

    while (!game.IsClosed()) {
        tick++;

        if (game.IsKeyPressed(KEY_ESCAPE)) {
            break;
        }
        if (game.IsKeyPressed(KEY_SPACE)) {
            showGrid = !showGrid;
        }

        int wheel = game.GetMouseWheelDelta();
        if (wheel > 0 && playerScale < 4) playerScale++;
        if (wheel < 0 && playerScale > 1) playerScale--;

        int beepFrequency = 220 * playerScale;
        if (game.IsKeyPressed(KEY_B)) {
            game.PlayBeep(beepFrequency, 120);
        }

        float speed = game.IsKeyDown(KEY_SHIFT) ? 4.0f : 2.5f;
        float moveX = 0.0f;
        float moveY = 0.0f;
        if (game.IsKeyDown(KEY_A) || game.IsKeyDown(KEY_LEFT)) moveX -= speed;
        if (game.IsKeyDown(KEY_D) || game.IsKeyDown(KEY_RIGHT)) moveX += speed;
        if (game.IsKeyDown(KEY_W) || game.IsKeyDown(KEY_UP)) moveY -= speed;
        if (game.IsKeyDown(KEY_S) || game.IsKeyDown(KEY_DOWN)) moveY += speed;

        if (moveX < 0.0f) facing = -1;
        else if (moveX > 0.0f) facing = 1;

        float nextX = playerX + moveX;
        if (!HitsWall(game, mapId, nextX, playerY)) {
            playerX = nextX;
        }
        float nextY = playerY + moveY;
        if (!HitsWall(game, mapId, playerX, nextY)) {
            playerY = nextY;
        }

        int mapPixelWidth = mapCols * tileSize;
        int mapPixelHeight = mapRows * tileSize;
        if (playerX < tileSize) playerX = (float)tileSize;
        if (playerY < tileSize) playerY = (float)tileSize;
        if (playerX > (float)(mapPixelWidth - tileSize * 2)) playerX = (float)(mapPixelWidth - tileSize * 2);
        if (playerY > (float)(mapPixelHeight - tileSize * 2)) playerY = (float)(mapPixelHeight - tileSize * 2);

        int cameraX = ClampInt((int)playerX - game.GetWidth() / 2, 0, mapPixelWidth - game.GetWidth());
        int cameraY = ClampInt((int)playerY - game.GetHeight() / 2, 0, mapPixelHeight - game.GetHeight());

        int worldMouseX = game.GetMouseX() + cameraX;
        int worldMouseY = game.GetMouseY() + cameraY;
        int hoverCol = game.WorldToTileCol(mapId, worldMouseX);
        int hoverRow = game.WorldToTileRow(mapId, worldMouseY);
        int hoveredTile = game.GetTileAtPixel(mapId, worldMouseX, worldMouseY);

        if (game.IsMouseDown(MOUSE_LEFT)) {
            game.SetTile(mapId, hoverCol, hoverRow, 3);
        }
        if (game.IsMouseDown(MOUSE_RIGHT)) {
            game.SetTile(mapId, hoverCol, hoverRow, 0);
        }

        int screenPlayerX = (int)playerX - cameraX;
        int screenPlayerY = (int)playerY - cameraY;
        int playerDrawSize = tileSize * playerScale;
        int playerCenterX = screenPlayerX + playerDrawSize / 2;
        int playerCenterY = screenPlayerY + playerDrawSize / 2;

        bool cursorNearPlayer = GameLib::CircleOverlap(playerCenterX, playerCenterY, 14,
                                                       game.GetMouseX(), game.GetMouseY(), 10);
        int distanceToCursor = (int)GameLib::Distance((int)playerX, (int)playerY, worldMouseX, worldMouseY);

        game.Clear(COLOR_DARK_BLUE);
        game.DrawTilemap(mapId, -cameraX, -cameraY, 0);

        if (showGrid) {
            game.DrawGrid(-cameraX, -cameraY, mapRows, mapCols, tileSize, COLOR_DARK_GRAY);
        }

        if (hoverCol >= 0 && hoverCol < mapCols && hoverRow >= 0 && hoverRow < mapRows) {
            game.DrawRect(hoverCol * tileSize - cameraX, hoverRow * tileSize - cameraY,
                          tileSize, tileSize, COLOR_YELLOW);
        }

        game.DrawLine(playerCenterX, playerCenterY, game.GetMouseX(), game.GetMouseY(),
                      cursorNearPlayer ? COLOR_RED : COLOR_GOLD);
        game.DrawCircle(game.GetMouseX(), game.GetMouseY(), 10, COLOR_WHITE);
        game.FillCircle(playerCenterX, playerCenterY, 3, COLOR_YELLOW);
        game.FillTriangle(playerCenterX, screenPlayerY - 6,
                          playerCenterX - 6, screenPlayerY - 16,
                          playerCenterX + 6, screenPlayerY - 16,
                          COLOR_ORANGE);

        int frameIndex = (tick / 10) % 2;
        int spriteFlags = SPRITE_COLORKEY;
        if (facing < 0) spriteFlags |= SPRITE_FLIP_H;
        game.DrawSpriteFrameScaled(playerSheet, screenPlayerX, screenPlayerY,
                                   16, 16, frameIndex, playerDrawSize, playerDrawSize, spriteFlags);

        game.FillRect(8, 8, 420, 112, COLOR_ARGB(255, 0, 0, 0));
        game.DrawRect(8, 8, 420, 112, COLOR_LIGHT_GRAY);
        game.DrawTextScale(16, 16, "SDL DEMO 1", COLOR_WHITE, 2);

        char buf[160];
        snprintf(buf, sizeof(buf), "PLAYER %d,%d  CAMERA %d,%d  SCALE %dx",
                 (int)playerX, (int)playerY, cameraX, cameraY, playerScale);
        buf[sizeof(buf) - 1] = '\0';
        game.DrawText(16, 40, buf, COLOR_CYAN);

        snprintf(buf, sizeof(buf), "MOUSE TILE %d,%d  TILE ID %d  DIST %d",
                 hoverCol, hoverRow, hoveredTile, distanceToCursor);
        buf[sizeof(buf) - 1] = '\0';
        game.DrawText(16, 56, buf, COLOR_GOLD);

        if (game.IsActive()) {
            game.DrawText(16, 72, "WASD/ARROWS MOVE  SPACE GRID  WHEEL SCALE", COLOR_WHITE);
            snprintf(buf, sizeof(buf), "L/R MOUSE PAINT  B BEEP @ %d HZ", beepFrequency);
            buf[sizeof(buf) - 1] = '\0';
            game.DrawText(16, 88, buf, COLOR_WHITE);
        } else {
            game.DrawText(16, 72, "WINDOW NOT ACTIVE", COLOR_RED);
            game.DrawText(16, 88, "PRESS B TO TEST BEEP WHEN ACTIVE", COLOR_GRAY);
        }

        game.Update();
        game.WaitFrame(60);
    }

    return 0;
}
