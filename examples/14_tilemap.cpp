// 14_tilemap.cpp - Tilemap Two-Layer Scrolling
//
// Two-layer scrolling demo:
//   - Background layer: far view (sky, clouds, hills), half speed for parallax effect
//   - Foreground layer: ground, platforms, bricks, stairs, full speed
//   - Character sprite loaded from assets/, camera follows
//   - Decorative trees loaded from assets/
// Foreground tileset and sprites loaded from assets/ folder (PNG with 0xFF00FF color key).
// Background tileset is still made by code (no matching asset).
//
// Controls: <- -> move character, ESC to quit
// Learn: CreateTilemap, FillTileRect, ClearTilemap, WorldToTileCol/Row,
//        GetTileAtPixel, DrawTilemap, LoadSprite, DrawSpriteEx,
//        SPRITE_COLORKEY, SPRITE_FLIP_H, parallax scrolling
//
// Compile: g++ -o 14_tilemap.exe 14_tilemap.cpp -mwindows

#include "../GameLib.h"

// Tile size
static const int TS = 16;

// Character sprite dimensions
static const int CHAR_W = 32;
static const int CHAR_FOOT = 44;  // y of boot soles in sprite

// Foreground tile IDs
enum { FG_GRASS = 0, FG_DIRT = 1, FG_BRICK = 2, FG_STONE = 3 };

// Background tile IDs
enum { BG_SKY = 0, BG_CLOUD_L = 1, BG_CLOUD_R = 2, BG_HILL_TOP = 3, BG_HILL = 4 };

// Helper: fill a rectangle inside a sprite
static void spriteFill(GameLib &g, int id, int x0, int y0, int w, int h, uint32_t c)
{
    for (int y = y0; y < y0 + h; y++)
        for (int x = x0; x < x0 + w; x++)
            g.SetSpritePixel(id, x, y, c);
}

// Make background tileset (5 tiles in a row = 80x16 sprite)
static int makeBgTileset(GameLib &game)
{
    int id = game.CreateSprite(5 * TS, TS);
    uint32_t sky = COLOR_RGB(135, 206, 235);
    int ox;

    // Sky
    spriteFill(game, id, BG_SKY * TS, 0, TS, TS, sky);

    // Cloud (left half)
    ox = BG_CLOUD_L * TS;
    spriteFill(game, id, ox, 0, TS, TS, sky);
    spriteFill(game, id, ox + 4, 6, 12, 6, COLOR_WHITE);
    spriteFill(game, id, ox + 6, 3, 8, 5, COLOR_WHITE);

    // Cloud (right half)
    ox = BG_CLOUD_R * TS;
    spriteFill(game, id, ox, 0, TS, TS, sky);
    spriteFill(game, id, ox, 6, 12, 6, COLOR_WHITE);
    spriteFill(game, id, ox + 2, 3, 8, 5, COLOR_WHITE);

    // Hill top (triangle shape)
    ox = BG_HILL_TOP * TS;
    spriteFill(game, id, ox, 0, TS, TS, sky);
    for (int y = 0; y < TS; y++) {
        int hw = 1 + y * 7 / 15;
        for (int x = 8 - hw; x < 8 + hw; x++)
            if (x >= 0 && x < TS)
                game.SetSpritePixel(id, ox + x, y, COLOR_RGB(70, 130, 70));
    }

    // Hill body
    spriteFill(game, id, BG_HILL * TS, 0, TS, TS, COLOR_RGB(70, 130, 70));

    return id;
}

int main()
{
    GameLib game;
    const int SW = 640, SH = 480;
    game.Open(SW, SH, "14 - Tilemap Two-Layer Scrolling", true);
    game.ShowFps(true);

    // ---- Load tilesets and sprites ----
    int fgTS = game.LoadSprite("../assets/tileset.png");      // 64x16, 4 tiles
    int bgTS = makeBgTileset(game);
    int charSpr = game.LoadSprite("../assets/character.png"); // 32x48
    int treeSpr = game.LoadSprite("../assets/tree.png");      // 32x64

    // ---- Create maps ----
    const int FG_C = 80, FG_R = 30;   // Foreground 80x30 = 1280x480
    const int BG_C = 60, BG_R = 30;   // Background 60x30 = 960x480
    int fgMap = game.CreateTilemap(FG_C, FG_R, TS, fgTS);
    int bgMap = game.CreateTilemap(BG_C, BG_R, TS, bgTS);
    int fgCols = game.GetTilemapCols(fgMap);
    int fgRows = game.GetTilemapRows(fgMap);
    int fgTileSize = game.GetTileSize(fgMap);
    int bgRows = game.GetTilemapRows(bgMap);
    int fgWorldWidth = fgCols * fgTileSize;
    int fgWorldHeight = fgRows * fgTileSize;

    // ---- Fill background ----
    game.ClearTilemap(bgMap, BG_SKY);
    // Clouds
    int cloudPos[][2] = { {5,4}, {18,3}, {33,5}, {48,2}, {55,6} };
    for (int i = 0; i < 5; i++) {
        game.SetTile(bgMap, cloudPos[i][0],     cloudPos[i][1], BG_CLOUD_L);
        game.SetTile(bgMap, cloudPos[i][0] + 1, cloudPos[i][1], BG_CLOUD_R);
    }
    // Hills (3 groups at different heights)
    int hills[][3] = { {8, 13, 24}, {22, 30, 22}, {40, 47, 25} }; // {start col, end col, top row}
    for (int i = 0; i < 3; i++) {
        int hillCols = hills[i][1] - hills[i][0];
        game.FillTileRect(bgMap, hills[i][0], hills[i][2], hillCols, 1, BG_HILL_TOP);
        game.FillTileRect(bgMap, hills[i][0], hills[i][2] + 1, hillCols,
                          bgRows - (hills[i][2] + 1), BG_HILL);
    }

    // ---- Fill foreground ----
    game.ClearTilemap(fgMap);
    // Ground: bottom 3 rows (grass + dirt + dirt)
    game.FillTileRect(fgMap, 0, fgRows - 3, fgCols, 1, FG_GRASS);
    game.FillTileRect(fgMap, 0, fgRows - 2, fgCols, 2, FG_DIRT);
    // Holes
    game.FillTileRect(fgMap, 20, fgRows - 3, 3, 3, -1);
    game.FillTileRect(fgMap, 48, fgRows - 3, 3, 3, -1);
    // Floating brick platforms
    game.FillTileRect(fgMap, 12, 23, 6, 1, FG_BRICK);
    game.FillTileRect(fgMap, 30, 20, 6, 1, FG_BRICK);
    game.FillTileRect(fgMap, 42, 24, 5, 1, FG_BRICK);
    // Stone stairs (5 steps up from ground)
    for (int step = 0; step < 5; step++)
        game.FillTileRect(fgMap, 60, fgRows - 4 - step, step + 1, 1, FG_STONE);
    // High stone platform
    game.FillTileRect(fgMap, 66, 17, 7, 1, FG_STONE);

    // ---- Trees (decoration) ----
    int treeX[] = { 5*TS, 25*TS, 38*TS, 55*TS, 70*TS };
    int treeCount = 5;
    int treeY = (FG_R - 3) * TS - 61;

    // ---- Player ----
    float playerX = 48.0f;
    float playerY = (float)((fgRows - 3) * fgTileSize - CHAR_FOOT); // Stand on ground
    float speed = 180.0f;
    int facing = 1;    // 1=right, -1=left

    while (!game.IsClosed()) {
        float dt = game.GetDeltaTime();
        if (dt > 0.05f) dt = 0.05f; // Prevent first frame jump

        // ---- Input ----
        if (game.IsKeyDown(KEY_RIGHT) || game.IsKeyDown(KEY_D)) {
            playerX += speed * dt;
            facing = 1;
        }
        if (game.IsKeyDown(KEY_LEFT) || game.IsKeyDown(KEY_A)) {
            playerX -= speed * dt;
            facing = -1;
        }
        // Limit player range
        if (playerX < 0) playerX = 0;
        if (playerX > fgWorldWidth - CHAR_W) playerX = (float)(fgWorldWidth - CHAR_W);

        // ---- Camera follow ----
        float cameraX = playerX - SW / 2 + CHAR_W / 2;
        float maxCam = (float)(fgWorldWidth - SW);
        if (cameraX < 0) cameraX = 0;
        if (cameraX > maxCam) cameraX = maxCam;

        int footX = (int)playerX + CHAR_W / 2;
        int footY = (int)playerY + CHAR_FOOT;
        int footCol = game.WorldToTileCol(fgMap, footX);
        int footRow = game.WorldToTileRow(fgMap, footY);
        int footTile = game.GetTileAtPixel(fgMap, footX, footY);

        // ---- Draw ----
        game.Clear(COLOR_RGB(135, 206, 235));

        // Background layer (half speed = parallax)
        game.DrawTilemap(bgMap, -(int)(cameraX * 0.5f), 0);

        // Foreground layer (full speed)
        int camX = (int)cameraX;
        game.DrawTilemap(fgMap, -camX, 0);

        // ---- Draw trees ----
        for (int i = 0; i < treeCount; i++)
            game.DrawSpriteEx(treeSpr, treeX[i] - camX, treeY, SPRITE_COLORKEY);

        // ---- Draw player ----
        int px = (int)playerX - camX;
        int py = (int)playerY;
        int pflags = SPRITE_COLORKEY | ((facing < 0) ? SPRITE_FLIP_H : 0);
        game.DrawSpriteEx(charSpr, px, py, pflags);

        // ---- HUD ----
        game.FillRect(0, 0, 300, 50, COLOR_RGB(15, 15, 25));
        game.DrawText(8, 6, "<- -> Move    ESC Quit", COLOR_WHITE);
        game.DrawPrintf(8, 20, COLOR_LIGHT_GRAY,
            "Camera: %d   World: %dx%d", camX, fgWorldWidth, fgWorldHeight);
        game.DrawPrintf(8, 34, COLOR_LIGHT_GRAY,
            "Foot tile: (%d,%d) = %d", footCol, footRow, footTile);

        if (game.IsKeyPressed(KEY_ESCAPE)) break;
        game.Update();
        game.WaitFrame(60);
    }

    game.FreeTilemap(fgMap);
    game.FreeTilemap(bgMap);
    game.FreeSprite(charSpr);
    game.FreeSprite(treeSpr);
    return 0;
}
