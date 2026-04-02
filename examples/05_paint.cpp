// 05_paint.cpp - Simple Paint
//
// 简易画板：鼠标画画，键盘换颜色/换笔刷/清屏。
// 学习：GetMouseX/Y, IsMouseDown, FillCircle, IsKeyPressed
//
// 编译: g++ -o 05_paint.exe 05_paint.cpp -mwindows

#include "../GameLib.h"

int main()
{
    GameLib game;
    game.Open(640, 480, "05 - Paint", true);

    uint32_t palette[] = {
        COLOR_WHITE, COLOR_RED, COLOR_GREEN, COLOR_BLUE,
        COLOR_YELLOW, COLOR_CYAN, COLOR_ORANGE, COLOR_PINK
    };
    int colorCount = 8;
    int colorIndex = 0;
    int brushSize = 4;

    // 先画一次背景 (不每帧 Clear，这样画的东西会保留)
    game.Clear(COLOR_BLACK);

    while (!game.IsClosed()) {
        int mx = game.GetMouseX();
        int my = game.GetMouseY();

        // 切换颜色: 1-8 数字键
        for (int i = 0; i < colorCount; i++) {
            if (game.IsKeyPressed(KEY_1 + i))
                colorIndex = i;
        }

        // 笔刷大小: +/-
        if (game.IsKeyPressed(KEY_UP) && brushSize < 30) brushSize += 2;
        if (game.IsKeyPressed(KEY_DOWN) && brushSize > 1) brushSize -= 2;

        // 清屏: C 键
        if (game.IsKeyPressed(KEY_C))
            game.Clear(COLOR_BLACK);

        // 左键画画
        if (game.IsMouseDown(MOUSE_LEFT) && my > 30) {
            game.FillCircle(mx, my, brushSize, palette[colorIndex]);
        }

        // 右键橡皮擦
        if (game.IsMouseDown(MOUSE_RIGHT) && my > 30) {
            game.FillCircle(mx, my, brushSize + 4, COLOR_BLACK);
        }

        // 顶部工具栏 (每帧重绘)
        game.FillRect(0, 0, game.GetWidth(), 28, COLOR_DARK_GRAY);

        // 色板
        for (int i = 0; i < colorCount; i++) {
            int bx = 10 + i * 22;
            game.FillRect(bx, 4, 18, 18, palette[i]);
            if (i == colorIndex)
                game.DrawRect(bx - 1, 3, 20, 20, COLOR_WHITE);
        }

        // 笔刷预览
        game.FillCircle(210, 13, brushSize, palette[colorIndex]);

        // 提示
        game.DrawText(240, 4, "1-8:Color  Up/Down:Size", COLOR_LIGHT_GRAY);
        game.DrawText(240, 16, "L:Draw  R:Erase  C:Clear", COLOR_LIGHT_GRAY);

        game.Update();
        game.WaitFrame(60);
    }
    return 0;
}
