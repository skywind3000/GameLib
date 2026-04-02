// 01_hello.cpp - Hello World
//
// 最简单的 GameLib 程序：创建窗口，显示文字。
// 学习：Open, Clear, DrawText, DrawTextScale, Update, WaitFrame, IsClosed
//
// 编译: g++ -o 01_hello.exe 01_hello.cpp -mwindows

#include "../GameLib.h"

int main()
{
    GameLib game;
    game.Open(640, 480, "01 - Hello World", true);

    while (!game.IsClosed()) {
        game.Clear(COLOR_DARK_BLUE);

        // 大号标题
        game.DrawTextScale(160, 180, "Hello, GameLib!", COLOR_GOLD, 3);

        // 普通文字
        game.DrawText(230, 240, "Welcome to GameLib!", COLOR_WHITE);
        game.DrawText(210, 270, "Press ESC to exit, or close", COLOR_GRAY);
        game.DrawText(210, 280, "the window to quit.", COLOR_GRAY);

        // 显示运行时间
        game.DrawPrintf(10, 460, COLOR_LIGHT_GRAY, "Time: %.1f s", game.GetTime());

        if (game.IsKeyPressed(KEY_ESCAPE))
            break;

        game.Update();
        game.WaitFrame(60);
    }
    return 0;
}
