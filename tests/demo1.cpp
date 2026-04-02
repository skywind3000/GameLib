#include "../GameLib.h"

int main()
{
    GameLib game;
    game.Open(800, 600, "Hello GameLib", true);
	game.ShowFps(true);
    while (!game.IsClosed()) {
        game.Clear(COLOR_DARK_BLUE);
        game.DrawTextScale(200, 250, "Hello, World!!", COLOR_GOLD, 3);
        game.DrawText(280, 320, "Press ESC to exit", COLOR_GRAY);

        if (game.IsKeyPressed(KEY_ESCAPE))
            break;

        game.Update();
        game.WaitFrame(60);
    }
    return 0;
}
