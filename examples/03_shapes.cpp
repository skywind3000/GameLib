// 03_shapes.cpp - Shape Gallery
//
// Show the main shape drawing APIs, including ellipse drawing and
// ARGB alpha blending for primitive colors.
// Learn: SetPixel, DrawLine, DrawRect, FillRect, DrawCircle,
//        FillCircle, DrawEllipse, FillEllipse, DrawTriangle,
//        FillTriangle, COLOR_ARGB
//
// Compile: g++ -o 03_shapes.exe 03_shapes.cpp -mwindows

#include "../GameLib.h"

static void DrawCheckerboard(GameLib &game, int x, int y, int w, int h, int cell)
{
    for (int py = 0; py < h; py += cell) {
        for (int px = 0; px < w; px += cell) {
            bool dark = (((px / cell) + (py / cell)) & 1) != 0;
            uint32_t color = dark ? COLOR_RGB(55, 65, 80) : COLOR_RGB(85, 95, 110);
            int cellW = cell;
            int cellH = cell;
            if (px + cellW > w) cellW = w - px;
            if (py + cellH > h) cellH = h - py;
            game.FillRect(x + px, y + py, cellW, cellH, color);
        }
    }
}

int main()
{
    GameLib game;
    game.Open(640, 480, "03 - Shapes and Alpha", true);

    while (!game.IsClosed()) {
        double t = game.GetTime();
        int swing = (int)(sin(t * 2.0) * 20.0);

        game.Clear(COLOR_RGB(24, 28, 36));

        game.DrawText(16, 10, "03 - Shapes, Ellipses and Primitive Alpha", COLOR_WHITE);
        game.DrawText(16, 22, "ESC: Quit", COLOR_LIGHT_GRAY);

        // Panel 1: SetPixel + DrawLine
        game.DrawText(20, 44, "Pixels + Lines", COLOR_WHITE);
        game.DrawRect(16, 58, 192, 144, COLOR_LIGHT_GRAY);
        game.FillRect(20, 62, 88, 88, COLOR_RGB(15, 18, 24));
        for (int py = 0; py < 88; py++) {
            for (int px = 0; px < 88; px++) {
                if (((px + py) % 11) == 0) {
                    game.SetPixel(20 + px, 62 + py,
                                  COLOR_RGB(80 + (px * 2) % 175,
                                            80 + (py * 2) % 175,
                                            220));
                }
            }
        }
        game.FillRect(120, 62, 76, 88, COLOR_RGB(18, 22, 30));
        for (int i = 0; i < 10; i++) {
            int y = 70 + i * 7;
            game.DrawLine(124, 106 + swing / 2, 188, y, COLOR_ARGB(255, 255 - i * 18, 90 + i * 12, 80 + i * 10));
        }
        game.DrawLine(124, 146, 188, 76, COLOR_ARGB(110, 255, 255, 255));
        game.DrawText(26, 162, "SetPixel pattern", COLOR_LIGHT_GRAY);
        game.DrawText(26, 174, "DrawLine fan", COLOR_LIGHT_GRAY);

        // Panel 2: Rectangles + alpha blending
        game.DrawText(224, 44, "Rectangles + Alpha", COLOR_WHITE);
        game.DrawRect(220, 58, 192, 144, COLOR_LIGHT_GRAY);
        DrawCheckerboard(game, 224, 62, 184, 136, 16);
        game.DrawRect(236, 76, 74, 52, COLOR_RED);
        game.DrawRect(252, 92, 74, 52, COLOR_ARGB(120, 80, 255, 120));
        game.FillRect(326, 76, 58, 58, COLOR_ARGB(255, 70, 120, 255));
        game.FillRect(346, 96, 58, 58, COLOR_ARGB(120, 255, 210, 50));
        game.DrawText(232, 162, "DrawRect uses alpha too", COLOR_LIGHT_GRAY);
        game.DrawText(232, 174, "FillRect blends over grid", COLOR_LIGHT_GRAY);

        // Panel 3: Circles + ellipses
        game.DrawText(428, 44, "Circles + Ellipses", COLOR_WHITE);
        game.DrawRect(424, 58, 200, 144, COLOR_LIGHT_GRAY);
        DrawCheckerboard(game, 428, 62, 192, 136, 16);
        game.DrawCircle(474, 104, 32, COLOR_CYAN);
        game.FillCircle(496, 116, 24, COLOR_ARGB(140, 255, 80, 160));
        game.DrawEllipse(566, 102, 44, 24, COLOR_GOLD);
        game.FillEllipse(566, 132, 34, 18, COLOR_ARGB(135, 80, 220, 255));
        game.DrawText(438, 162, "DrawCircle / FillCircle", COLOR_LIGHT_GRAY);
        game.DrawText(438, 174, "DrawEllipse / FillEllipse", COLOR_LIGHT_GRAY);

        // Panel 4: Triangles
        game.DrawText(20, 228, "Triangles", COLOR_WHITE);
        game.DrawRect(16, 242, 288, 214, COLOR_LIGHT_GRAY);
        DrawCheckerboard(game, 20, 246, 280, 206, 20);
        game.DrawTriangle(52, 284, 26, 430, 128, 418, COLOR_GOLD);
        game.FillTriangle(172, 276, 116, 430, 242, 420, COLOR_ARGB(150, 120, 220, 255));
        game.FillTriangle(242, 266, 170, 392, 286, 436, COLOR_ARGB(110, 255, 120, 80));
        game.DrawText(28, 432, "Outline + layered fill triangles", COLOR_LIGHT_GRAY);

        // Panel 5: Combined scene
        game.DrawText(324, 228, "Combined Scene", COLOR_WHITE);
        game.DrawRect(320, 242, 304, 214, COLOR_LIGHT_GRAY);
        game.FillRect(324, 246, 296, 206, COLOR_RGB(30, 44, 72));
        game.FillEllipse(470, 292, 94, 48, COLOR_ARGB(120, 255, 210, 90));
        game.FillCircle(558, 296, 30, COLOR_ARGB(210, 255, 230, 80));
        game.FillRect(390, 344, 120, 70, COLOR_BROWN);
        game.FillTriangle(376, 344, 450, 286, 524, 344, COLOR_DARK_RED);
        game.FillRect(438, 372, 28, 42, COLOR_DARK_BLUE);
        game.FillRect(406, 360, 18, 18, COLOR_ARGB(170, 160, 230, 255));
        game.FillRect(478, 360, 18, 18, COLOR_ARGB(170, 160, 230, 255));
        game.DrawEllipse(548, 388, 46, 24, COLOR_WHITE);
        game.FillEllipse(548, 388, 30, 14, COLOR_ARGB(120, 255, 255, 255));
        game.DrawText(336, 432, "Opaque and translucent shapes can mix", COLOR_LIGHT_GRAY);

        if (game.IsKeyPressed(KEY_ESCAPE)) break;

        game.Update();
        game.WaitFrame(30);
    }
    return 0;
}
