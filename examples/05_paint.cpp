// 05_paint.cpp - Simple Paint
//
// A simple paint program: draw with mouse, change color/brush/clear with keyboard.
// Learn: GetMouseX/Y, IsMouseDown, IsMouseReleased, GetMouseWheelDelta, IsActive
//
// Compile: g++ -o 05_paint.exe 05_paint.cpp -mwindows

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

    // Draw background once (don't Clear each frame, so drawings stay)
    game.Clear(COLOR_BLACK);

    while (!game.IsClosed()) {
        int mx = game.GetMouseX();
        int my = game.GetMouseY();
        int wheelSteps = game.GetMouseWheelDelta() / 120;

        // Switch color: 1-8 number keys
        for (int i = 0; i < colorCount; i++) {
            if (game.IsKeyPressed(KEY_1 + i))
                colorIndex = i;
        }

        // Brush size: Up/Down or mouse wheel
        if (game.IsKeyPressed(KEY_UP) && brushSize < 30) brushSize += 2;
        if (game.IsKeyPressed(KEY_DOWN) && brushSize > 1) brushSize -= 2;
        if (wheelSteps != 0) {
            brushSize += wheelSteps * 2;
            if (brushSize < 1) brushSize = 1;
            if (brushSize > 30) brushSize = 30;
        }

        // Clear screen: C key
        if (game.IsKeyPressed(KEY_C))
            game.Clear(COLOR_BLACK);

        // Click palette on release, avoid selecting colors while dragging.
        if (game.IsMouseReleased(MOUSE_LEFT) && my >= 4 && my < 22) {
            for (int i = 0; i < colorCount; i++) {
                int bx = 10 + i * 22;
                if (mx >= bx && mx < bx + 18) {
                    colorIndex = i;
                    break;
                }
            }
        }

        // Left button: draw
        if (game.IsMouseDown(MOUSE_LEFT) && my > 30 && game.IsActive()) {
            game.FillCircle(mx, my, brushSize, palette[colorIndex]);
        }

        // Right button: eraser
        if (game.IsMouseDown(MOUSE_RIGHT) && my > 30 && game.IsActive()) {
            game.FillCircle(mx, my, brushSize + 4, COLOR_BLACK);
        }

        // Top toolbar (redraw each frame)
        game.FillRect(0, 0, game.GetWidth(), 28, COLOR_DARK_GRAY);

        // Color palette
        for (int i = 0; i < colorCount; i++) {
            int bx = 10 + i * 22;
            game.FillRect(bx, 4, 18, 18, palette[i]);
            if (i == colorIndex)
                game.DrawRect(bx - 1, 3, 20, 20, COLOR_WHITE);
        }

        // Brush preview
        game.FillCircle(210, 13, brushSize, palette[colorIndex]);

        // Hints
        game.DrawText(240, 4, "1-8/Click:Color  Up/Down/Wheel:Size", COLOR_LIGHT_GRAY);
        game.DrawText(240, 16, "L:Draw  R:Erase  C:Clear", COLOR_LIGHT_GRAY);
        game.DrawText(520, 16, game.IsActive() ? "ACTIVE" : "PAUSED", game.IsActive() ? COLOR_GREEN : COLOR_YELLOW);

        game.Update();
        game.WaitFrame(60);
    }
    return 0;
}
