#if defined(_WIN32) && !defined(GAMELIB_SDL_DEFAULT_FONT)
#define GAMELIB_SDL_DEFAULT_FONT "C:/Windows/Fonts/msyh.ttc"
#endif

#include "../GameLib.SDL.h"

static const int TS = 16;
static const int CHAR_W = 32;
static const int CHAR_FOOT = 44;

enum { FG_GRASS = 0, FG_DIRT = 1, FG_BRICK = 2, FG_STONE = 3 };
enum { BG_SKY = 0, BG_CLOUD_L = 1, BG_CLOUD_R = 2, BG_HILL_TOP = 3, BG_HILL = 4 };

static void spriteFill(GameLib &g, int id, int x0, int y0, int w, int h, uint32_t c)
{
    for (int y = y0; y < y0 + h; y++) {
        for (int x = x0; x < x0 + w; x++) {
            g.SetSpritePixel(id, x, y, c);
        }
    }
}

static int makeBgTileset(GameLib &game)
{
    int id = game.CreateSprite(5 * TS, TS);
    if (id < 0) return -1;

    uint32_t sky = COLOR_RGB(135, 206, 235);
    int ox = 0;

    spriteFill(game, id, BG_SKY * TS, 0, TS, TS, sky);

    ox = BG_CLOUD_L * TS;
    spriteFill(game, id, ox, 0, TS, TS, sky);
    spriteFill(game, id, ox + 4, 6, 12, 6, COLOR_WHITE);
    spriteFill(game, id, ox + 6, 3, 8, 5, COLOR_WHITE);

    ox = BG_CLOUD_R * TS;
    spriteFill(game, id, ox, 0, TS, TS, sky);
    spriteFill(game, id, ox, 6, 12, 6, COLOR_WHITE);
    spriteFill(game, id, ox + 2, 3, 8, 5, COLOR_WHITE);

    ox = BG_HILL_TOP * TS;
    spriteFill(game, id, ox, 0, TS, TS, sky);
    for (int y = 0; y < TS; y++) {
        int halfWidth = 1 + y * 7 / 15;
        for (int x = 8 - halfWidth; x < 8 + halfWidth; x++) {
            if (x >= 0 && x < TS) {
                game.SetSpritePixel(id, ox + x, y, COLOR_RGB(70, 130, 70));
            }
        }
    }

    spriteFill(game, id, BG_HILL * TS, 0, TS, TS, COLOR_RGB(70, 130, 70));
    return id;
}

static int loadSpriteWithFallbacks(GameLib &game, const char *pathA, const char *pathB)
{
    int id = game.LoadSprite(pathA);
    if (id >= 0) return id;
    if (pathB && pathB[0]) return game.LoadSprite(pathB);
    return -1;
}

int main()
{
    GameLib game;
    const int SW = 640;
    const int SH = 480;
    const int SCENE_X = 24;
    const int SCENE_Y = 24;
    const int SCENE_W = 592;
    const int SCENE_H = 276;
    const int SHAPE_X = 24;
    const int SHAPE_Y = 320;
    const int SHAPE_W = 284;
    const int SHAPE_H = 136;
    const int TEXT_X = 332;
    const int TEXT_Y = 320;
    const int TEXT_W = 284;
    const int TEXT_H = 136;

    if (game.Open(SW, SH, "SDL Demo 1 - Clip Rectangle Tilemap", true, true) != 0) {
        return 1;
    }
    game.ShowFps(true);

    int fgTileset = loadSpriteWithFallbacks(game, "../assets/tileset.png", "assets/tileset.png");
    int bgTileset = makeBgTileset(game);
    int charSpr = loadSpriteWithFallbacks(game, "../assets/character.png", "assets/character.png");
    int treeSpr = loadSpriteWithFallbacks(game, "../assets/tree.png", "assets/tree.png");

    if (fgTileset < 0 || bgTileset < 0 || charSpr < 0 || treeSpr < 0) {
        while (!game.IsClosed()) {
            if (game.IsKeyPressed(KEY_ESCAPE)) break;

            game.Clear(COLOR_DARK_RED);
            game.DrawTextScale(84, 96, "SDL DEMO 1", COLOR_WHITE, 2);
            game.DrawText(64, 150, "Required sprite assets could not be loaded.", COLOR_WHITE);
            game.DrawText(64, 172, "This demo tries both ../assets/*.png and assets/*.png.", COLOR_LIGHT_GRAY);
            game.DrawPrintf(64, 206, COLOR_YELLOW, "tileset.png: %s", fgTileset >= 0 ? "OK" : "Missing");
            game.DrawPrintf(64, 222, COLOR_YELLOW, "character.png: %s", charSpr >= 0 ? "OK" : "Missing");
            game.DrawPrintf(64, 238, COLOR_YELLOW, "tree.png: %s", treeSpr >= 0 ? "OK" : "Missing");
            game.DrawText(64, 282, "Press ESC to exit.", COLOR_WHITE);
            game.Update();
            game.WaitFrame(60);
        }

        game.FreeSprite(fgTileset);
        game.FreeSprite(bgTileset);
        game.FreeSprite(charSpr);
        game.FreeSprite(treeSpr);
        return 1;
    }

    const int FG_C = 80;
    const int FG_R = 30;
    const int BG_C = 60;
    const int BG_R = 30;
    int fgMap = game.CreateTilemap(FG_C, FG_R, TS, fgTileset);
    int bgMap = game.CreateTilemap(BG_C, BG_R, TS, bgTileset);
    if (fgMap < 0 || bgMap < 0) {
        game.FreeTilemap(fgMap);
        game.FreeTilemap(bgMap);
        game.FreeSprite(fgTileset);
        game.FreeSprite(bgTileset);
        game.FreeSprite(charSpr);
        game.FreeSprite(treeSpr);
        return 1;
    }

    int fgCols = game.GetTilemapCols(fgMap);
    int fgRows = game.GetTilemapRows(fgMap);
    int bgRows = game.GetTilemapRows(bgMap);
    int fgTileSize = game.GetTileSize(fgMap);
    int fgWorldWidth = fgCols * fgTileSize;
    int fgWorldHeight = fgRows * fgTileSize;
    int worldScreenY = SCENE_Y + SCENE_H - fgWorldHeight;

    game.ClearTilemap(bgMap, BG_SKY);
    {
        int cloudPos[][2] = { {5, 4}, {18, 3}, {33, 5}, {48, 2}, {55, 6} };
        int hills[][3] = { {8, 13, 24}, {22, 30, 22}, {40, 47, 25} };
        for (int i = 0; i < 5; i++) {
            game.SetTile(bgMap, cloudPos[i][0], cloudPos[i][1], BG_CLOUD_L);
            game.SetTile(bgMap, cloudPos[i][0] + 1, cloudPos[i][1], BG_CLOUD_R);
        }

        for (int i = 0; i < 3; i++) {
            int hillCols = hills[i][1] - hills[i][0];
            game.FillTileRect(bgMap, hills[i][0], hills[i][2], hillCols, 1, BG_HILL_TOP);
            game.FillTileRect(bgMap, hills[i][0], hills[i][2] + 1, hillCols,
                              bgRows - (hills[i][2] + 1), BG_HILL);
        }
    }

    game.ClearTilemap(fgMap);
    game.FillTileRect(fgMap, 0, fgRows - 3, fgCols, 1, FG_GRASS);
    game.FillTileRect(fgMap, 0, fgRows - 2, fgCols, 2, FG_DIRT);
    game.FillTileRect(fgMap, 20, fgRows - 3, 3, 3, -1);
    game.FillTileRect(fgMap, 48, fgRows - 3, 3, 3, -1);
    game.FillTileRect(fgMap, 12, 23, 6, 1, FG_BRICK);
    game.FillTileRect(fgMap, 30, 20, 6, 1, FG_BRICK);
    game.FillTileRect(fgMap, 42, 24, 5, 1, FG_BRICK);
    for (int step = 0; step < 5; step++) {
        game.FillTileRect(fgMap, 60, fgRows - 4 - step, step + 1, 1, FG_STONE);
    }
    game.FillTileRect(fgMap, 66, 17, 7, 1, FG_STONE);

    int treeX[] = { 5 * TS, 25 * TS, 38 * TS, 55 * TS, 70 * TS };
    int treeCount = 5;
    int treeY = (FG_R - 3) * TS - 61;

    double playerX = 48.0;
    double playerY = (double)((fgRows - 3) * fgTileSize - CHAR_FOOT);
    double speed = 180.0;
    int facing = 1;

    while (!game.IsClosed()) {
        double dt = game.GetDeltaTime();
        if (dt > 0.05) dt = 0.05;

        if (game.IsKeyDown(KEY_RIGHT) || game.IsKeyDown(KEY_D)) {
            playerX += speed * dt;
            facing = 1;
        }
        if (game.IsKeyDown(KEY_LEFT) || game.IsKeyDown(KEY_A)) {
            playerX -= speed * dt;
            facing = -1;
        }
        if (playerX < 0) playerX = 0;
        if (playerX > fgWorldWidth - CHAR_W) playerX = (double)(fgWorldWidth - CHAR_W);

        {
            double maxCamera = (double)(fgWorldWidth - SCENE_W);
            double cameraX = playerX - SCENE_W / 2 + CHAR_W / 2;
            int camX;
            int bgCamX;
            int footX = (int)playerX + CHAR_W / 2;
            int footY = (int)playerY + CHAR_FOOT;
            int footCol;
            int footRow;
            int footTile;
            double t = game.GetTime();

            if (maxCamera < 0) maxCamera = 0;
            if (cameraX < 0) cameraX = 0;
            if (cameraX > maxCamera) cameraX = maxCamera;

            camX = (int)cameraX;
            bgCamX = (int)(cameraX * 0.5);
            footCol = game.WorldToTileCol(fgMap, footX);
            footRow = game.WorldToTileRow(fgMap, footY);
            footTile = game.GetTileAtPixel(fgMap, footX, footY);

            game.Clear(COLOR_RGB(18, 20, 28));
            game.DrawText(24, 6, "SDL Demo 1 - Clip windows for scene, shapes, and text", COLOR_WHITE);
            game.DrawText(24, 18, "Move with A/D or arrows. ESC quits.", COLOR_LIGHT_GRAY);

            game.SetClip(SCENE_X, SCENE_Y, SCENE_W, SCENE_H);
            game.Clear(COLOR_RGB(135, 206, 235));
            game.DrawTilemap(bgMap, SCENE_X - bgCamX, worldScreenY);
            game.DrawTilemap(fgMap, SCENE_X - camX, worldScreenY);
            for (int i = 0; i < treeCount; i++) {
                game.DrawSpriteEx(treeSpr, SCENE_X + treeX[i] - camX, worldScreenY + treeY, SPRITE_COLORKEY);
            }

            {
                int px = SCENE_X + (int)playerX - camX;
                int py = worldScreenY + (int)playerY;
                int flags = SPRITE_COLORKEY | ((facing < 0) ? SPRITE_FLIP_H : 0);
                game.DrawSpriteEx(charSpr, px, py, flags);
                game.DrawRect(SCENE_X - 12, SCENE_Y - 8, SCENE_W + 24, SCENE_H + 16,
                              COLOR_ARGB(140, 255, 255, 255));
                game.DrawLine(SCENE_X - 32, SCENE_Y + 16, px + 16, py + 8,
                              COLOR_ARGB(180, 255, 240, 0));
                game.FillCircle(SCENE_X + SCENE_W - 20 + (int)(cos(t * 1.3) * 44.0),
                                SCENE_Y + 34, 18, COLOR_ARGB(180, 255, 120, 80));
            }
            game.ClearClip();
            game.DrawRect(SCENE_X - 1, SCENE_Y - 1, SCENE_W + 2, SCENE_H + 2, COLOR_WHITE);
            game.DrawText(SCENE_X + 8, SCENE_Y + 8, "Scene Clip", COLOR_WHITE);

            game.SetClip(SHAPE_X, SHAPE_Y, SHAPE_W, SHAPE_H);
            game.Clear(COLOR_RGB(26, 36, 52));
            game.FillTriangle(SHAPE_X + SHAPE_W / 2, SHAPE_Y - 26,
                              SHAPE_X + SHAPE_W + 54, SHAPE_Y + SHAPE_H - 12,
                              SHAPE_X - 54, SHAPE_Y + SHAPE_H + 18,
                              COLOR_ARGB(120, 255, 180, 40));
            game.DrawRect(SHAPE_X - 16, SHAPE_Y + 12, SHAPE_W + 32, SHAPE_H - 24, COLOR_CYAN);
            for (int i = 0; i < 6; i++) {
                int sweep = ((int)(t * 120.0) + i * 46) % (SHAPE_W + 140);
                int x0 = SHAPE_X - 70 + sweep;
                game.DrawLine(x0, SHAPE_Y - 10, x0 + 120, SHAPE_Y + SHAPE_H + 18,
                              COLOR_ARGB(180, 120 + i * 20, 220 - i * 24, 255));
            }
            game.FillCircle(SHAPE_X + SHAPE_W / 2 + (int)(cos(t * 1.7) * 168.0),
                            SHAPE_Y + SHAPE_H / 2, 26, COLOR_ARGB(180, 255, 90, 120));
            game.DrawEllipse(SHAPE_X + SHAPE_W / 2, SHAPE_Y + SHAPE_H / 2,
                             SHAPE_W / 2 + 30, 34, COLOR_GOLD);
            game.ClearClip();
            game.DrawRect(SHAPE_X - 1, SHAPE_Y - 1, SHAPE_W + 2, SHAPE_H + 2, COLOR_WHITE);
            game.DrawText(SHAPE_X + 8, SHAPE_Y + 8, "Shape Clip", COLOR_WHITE);

            game.SetClip(TEXT_X, TEXT_Y, TEXT_W, TEXT_H);
            game.Clear(COLOR_RGB(44, 30, 22));
            {
                int clipX = 0;
                int clipY = 0;
                int clipW = 0;
                int clipH = 0;
                int asciiX = TEXT_X - 72 + ((int)(t * 70.0) % 144);
                game.GetClip(&clipX, &clipY, &clipW, &clipH);

#if GAMELIB_SDL_HAS_TTF
                int marqueeX = TEXT_X + TEXT_W - ((int)(t * 110.0) % (TEXT_W + 340));
                game.DrawTextFont(marqueeX, TEXT_Y + 12,
                                  "DrawTextFont stays inside the clip window.",
                                  COLOR_GOLD, 20);
#else
                game.DrawText(TEXT_X + 8, TEXT_Y + 12,
                              "Build with SDL_ttf to test DrawTextFont clipping.",
                              COLOR_GOLD);
#endif
                game.DrawText(asciiX, TEXT_Y + 54,
                              "Built-in DrawText clips too.", COLOR_WHITE);
                game.DrawPrintf(TEXT_X + 8, TEXT_Y + 78, COLOR_LIGHT_GRAY,
                                "Clip rect: %d,%d  %dx%d", clipX, clipY, clipW, clipH);
                game.DrawPrintf(TEXT_X + 8, TEXT_Y + 94, COLOR_LIGHT_GRAY,
                                "Foot tile: (%d,%d) = %d", footCol, footRow, footTile);
                game.DrawPrintf(TEXT_X + 8, TEXT_Y + 110, COLOR_LIGHT_GRAY,
                                "GetClip + SetClip + ClearClip in one demo.");
            }
            game.ClearClip();
            game.DrawRect(TEXT_X - 1, TEXT_Y - 1, TEXT_W + 2, TEXT_H + 2, COLOR_WHITE);
            game.DrawText(TEXT_X + 8, TEXT_Y + 8, "Text Clip", COLOR_WHITE);

			if (game.IsKeyPressed(KEY_F12)) game.Screenshot("screenshot.bmp");
            if (game.IsKeyPressed(KEY_ESCAPE)) break;
            game.Update();
            game.WaitFrame(60);
        }
    }

    game.FreeTilemap(fgMap);
    game.FreeTilemap(bgMap);
    game.FreeSprite(treeSpr);
    game.FreeSprite(charSpr);
    game.FreeSprite(bgTileset);
    game.FreeSprite(fgTileset);
    return 0;
}
