// demo_scene_save.cpp - Scene Management & Save/Load Data demo
// Build: g++ -o demo_scene_save.exe demo_scene_save.cpp -mwindows
#include "../GameLib.h"

enum { SCENE_MENU, SCENE_PLAY, SCENE_GAMEOVER };

int main()
{
    GameLib game;
    game.Open(640, 480, "Scene & Save Demo", true);
    game.SetScene(SCENE_MENU);

    int score = 0;
    int highscore = 0;
    int playerX = 320, playerY = 400;
    int targetX = 300, targetY = 100;
    bool newRecord = false;

    while (!game.IsClosed()) {
        switch (game.GetScene()) {
        case SCENE_MENU:
            if (game.IsSceneChanged()) {
                highscore = GameLib::LoadInt("save.dat", "highscore", 0);
            }
            game.Clear(COLOR_DARK_BLUE);
            game.DrawTextScale(180, 120, "Scene & Save Demo", COLOR_GOLD, 2);
            game.DrawText(220, 200, "Press ENTER to play", COLOR_WHITE);
            game.DrawPrintf(220, 240, COLOR_CYAN, "High Score: %d", highscore);
            game.DrawText(180, 300, "Press D to delete save file", COLOR_GRAY);
            game.DrawText(180, 320, "Press ESC to exit", COLOR_GRAY);

            if (game.IsKeyPressed(KEY_ENTER))
                game.SetScene(SCENE_PLAY);
            if (game.IsKeyPressed(KEY_D)) {
                GameLib::DeleteSave("save.dat");
                highscore = 0;
            }
            if (game.IsKeyPressed(KEY_ESCAPE))
                return 0;
            break;

        case SCENE_PLAY:
            if (game.IsSceneChanged()) {
                score = 0;
                playerX = 320;
                playerY = 400;
                targetX = GameLib::Random(50, 590);
                targetY = GameLib::Random(50, 300);
            }

            // Move player
            if (game.IsKeyDown(KEY_LEFT) && playerX > 15)   playerX -= 4;
            if (game.IsKeyDown(KEY_RIGHT) && playerX < 625) playerX += 4;
            if (game.IsKeyDown(KEY_UP) && playerY > 15)     playerY -= 4;
            if (game.IsKeyDown(KEY_DOWN) && playerY < 465)  playerY += 4;

            // Check collision with target
            if (GameLib::CircleOverlap(playerX, playerY, 15, targetX, targetY, 12)) {
                score += 10;
                targetX = GameLib::Random(50, 590);
                targetY = GameLib::Random(50, 300);
            }

            // Draw
            game.Clear(COLOR_BLACK);
            game.FillCircle(targetX, targetY, 12, COLOR_YELLOW);
            game.FillCircle(playerX, playerY, 15, COLOR_CYAN);
            game.DrawPrintf(10, 10, COLOR_WHITE, "Score: %d", score);
            game.DrawText(10, 30, "Arrow keys to move, ESC to end", COLOR_GRAY);

            if (game.IsKeyPressed(KEY_ESCAPE))
                game.SetScene(SCENE_GAMEOVER);
            break;

        case SCENE_GAMEOVER:
            if (game.IsSceneChanged()) {
                highscore = GameLib::LoadInt("save.dat", "highscore", 0);
                newRecord = false;
                if (score > highscore) {
                    GameLib::SaveInt("save.dat", "highscore", score);
                    GameLib::SaveString("save.dat", "name", "Player");
                    GameLib::SaveFloat("save.dat", "playtime", (float)game.GetTime());
                    highscore = score;
                    newRecord = true;
                }
            }

            game.Clear(COLOR_DARK_RED);
            game.DrawTextScale(220, 120, "GAME OVER", COLOR_WHITE, 2);
            game.DrawPrintf(230, 200, COLOR_WHITE, "Score: %d", score);
            game.DrawPrintf(230, 230, COLOR_CYAN, "High Score: %d", highscore);
            if (newRecord)
                game.DrawText(240, 260, "** NEW RECORD! **", COLOR_GOLD);

            // Show save data details
            const char *name = GameLib::LoadString("save.dat", "name", "(none)");
            float playtime = GameLib::LoadFloat("save.dat", "playtime", 0.0f);
            game.DrawPrintf(230, 300, COLOR_GRAY, "Name: %s", name);
            game.DrawPrintf(230, 320, COLOR_GRAY, "Play time: %.1f s", playtime);

            game.DrawText(200, 380, "Press ENTER for menu", COLOR_WHITE);
            game.DrawPrintf(200, 400, COLOR_DARK_GRAY,
                            "Previous scene: %d", game.GetPreviousScene());

            if (game.IsKeyPressed(KEY_ENTER))
                game.SetScene(SCENE_MENU);
            break;
        }

        game.Update();
        game.WaitFrame(60);
    }
    return 0;
}
