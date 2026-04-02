// 03_shapes.cpp - Shape Gallery
//
// 展示 GameLib 支持的所有图形绘制 API。
// 学习：DrawLine, DrawRect, FillRect, DrawCircle, FillCircle,
//       DrawTriangle, FillTriangle, SetPixel
//
// 编译: g++ -o 03_shapes.exe 03_shapes.cpp -mwindows

#include "../GameLib.h"

int main()
{
    GameLib game;
    game.Open(640, 480, "03 - Shapes", true);

    while (!game.IsClosed()) {
        game.Clear(COLOR_DARK_GRAY);

        // --- 第一行: 点和线 ---
        game.DrawText(20, 15, "Pixels", COLOR_WHITE);
        for (int i = 0; i < 50; i++) {
            int px = 20 + GameLib::Random(0, 80);
            int py = 30 + GameLib::Random(0, 60);
            uint32_t c = COLOR_RGB(GameLib::Random(100,255),
                                   GameLib::Random(100,255),
                                   GameLib::Random(100,255));
            game.SetPixel(px, py, c);
        }

        game.DrawText(140, 15, "Lines", COLOR_WHITE);
        game.DrawLine(140, 35, 230, 35, COLOR_RED);
        game.DrawLine(140, 35, 230, 85, COLOR_GREEN);
        game.DrawLine(140, 35, 185, 85, COLOR_BLUE);
        game.DrawLine(140, 60, 230, 60, COLOR_YELLOW);

        // --- 第二行: 矩形 ---
        game.DrawText(20, 110, "DrawRect", COLOR_WHITE);
        game.DrawRect(20, 125, 80, 60, COLOR_RED);
        game.DrawRect(30, 135, 80, 60, COLOR_GREEN);

        game.DrawText(140, 110, "FillRect", COLOR_WHITE);
        game.FillRect(140, 125, 80, 60, COLOR_BLUE);
        game.FillRect(170, 145, 80, 60, COLOR_ORANGE);

        // --- 第三行: 圆 ---
        game.DrawText(20, 220, "DrawCircle", COLOR_WHITE);
        game.DrawCircle(65, 275, 30, COLOR_CYAN);
        game.DrawCircle(85, 275, 20, COLOR_MAGENTA);

        game.DrawText(140, 220, "FillCircle", COLOR_WHITE);
        game.FillCircle(185, 275, 30, COLOR_PURPLE);
        game.FillCircle(205, 265, 15, COLOR_PINK);

        // --- 第四行: 三角形 ---
        game.DrawText(20, 320, "DrawTriangle", COLOR_WHITE);
        game.DrawTriangle(60, 340, 20, 400, 100, 400, COLOR_GOLD);

        game.DrawText(140, 320, "FillTriangle", COLOR_WHITE);
        game.FillTriangle(180, 340, 140, 400, 220, 400, COLOR_SKY_BLUE);

        // --- 右侧: 组合图案 ---
        game.DrawText(350, 15, "Combined", COLOR_WHITE);

        // 靶心
        game.FillCircle(420, 100, 60, COLOR_RED);
        game.FillCircle(420, 100, 45, COLOR_WHITE);
        game.FillCircle(420, 100, 30, COLOR_RED);
        game.FillCircle(420, 100, 15, COLOR_WHITE);
        game.FillCircle(420, 100, 5, COLOR_RED);

        // 小房子
        game.FillRect(370, 280, 100, 80, COLOR_BROWN);     // 墙
        game.FillTriangle(370, 280, 420, 220, 470, 280, COLOR_DARK_RED); // 屋顶
        game.FillRect(400, 310, 30, 50, COLOR_DARK_BLUE);  // 门
        game.FillRect(380, 295, 15, 15, COLOR_SKY_BLUE);   // 左窗
        game.FillRect(445, 295, 15, 15, COLOR_SKY_BLUE);   // 右窗

        // 右下提示
        game.DrawText(350, 420, "All shapes are drawn", COLOR_LIGHT_GRAY);
        game.DrawText(350, 432, "with pure pixel math!", COLOR_LIGHT_GRAY);

        if (game.IsKeyPressed(KEY_ESCAPE)) break;

        game.Update();
        game.WaitFrame(30);
    }
    return 0;
}
