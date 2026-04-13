#include "../GameLib.SDL.h"

int main()
{
    GameLib game;
    if (game.Open(960, 640, "GameLib.SDL Font Demo", true) != 0) {
        return 1;
    }
    game.ShowFps(true);

    int frame = 0;

#if GAMELIB_SDL_HAS_TTF
    const char *ttfStatus = "SDL_ttf support: ON";
#else
    const char *ttfStatus = "SDL_ttf support: OFF";
#endif

    while (!game.IsClosed()) {
        frame++;

        if (game.IsKeyPressed(KEY_ESCAPE)) {
            break;
        }

        game.Clear(COLOR_BLACK);

        game.DrawTextScale(16, 16, "SDL FONT DEMO", COLOR_WHITE, 2);
        game.DrawText(16, 40, "ESC EXIT  This page checks DrawTextFont + family lookup", COLOR_GRAY);
        game.DrawText(16, 58, ttfStatus, GAMELIB_SDL_HAS_TTF ? COLOR_GREEN : COLOR_RED);

        int defaultWidth = game.GetTextWidthFont("Default font sample", 28);
        int arialWidth = game.GetTextWidthFont("Arial sample", "Arial", 24);
        int timesWidth = game.GetTextWidthFont("Times New Roman sample", "Times New Roman", 24);
        int monoWidth = game.GetTextWidthFont("Courier New sample", "Courier New", 24);
        int yaheiWidth = game.GetTextWidthFont("Microsoft YaHei sample", "Microsoft YaHei", 24);
        int gothicWidth = game.GetTextWidthFont("MS Gothic sample", "MS Gothic", 24);

        int y = 80;

        game.DrawText(16, y, "1. Default font overload:", COLOR_WHITE);
        y += 18;
        game.DrawTextFont(24, y, "Default font sample", COLOR_GOLD, 28);
        y += 34;

        game.DrawText(16, y, "2. Family name lookup:", COLOR_WHITE);
        y += 18;
        game.DrawTextFont(24, y, "Arial sample", COLOR_GREEN, "Arial", 24);
        y += 30;
        game.DrawTextFont(24, y, "Times New Roman sample", COLOR_CYAN, "Times New Roman", 24);
        y += 30;
        game.DrawTextFont(24, y, "Courier New sample", COLOR_PINK, "Courier New", 24);
        y += 30;
        game.DrawTextFont(24, y, "Microsoft YaHei sample", COLOR_YELLOW, "Microsoft YaHei", 24);
        y += 30;
        game.DrawTextFont(24, y, "MS Gothic sample", COLOR_SKY_BLUE, "MS Gothic", 24);
        y += 36;

        game.DrawText(16, y, "3. Unicode lines:", COLOR_WHITE);
        y += 18;
        game.DrawTextFont(24, y, "中文测试：字体回退与默认字体", COLOR_ORANGE, 24);
        y += 30;
        game.DrawTextFont(24, y, "日本語テスト：MS Gothic / fallback", COLOR_MAGENTA, "MS Gothic", 22);
        y += 36;

        game.DrawText(16, y, "4. Measurement output:", COLOR_WHITE);
        y += 18;

        char buf[256];
        snprintf(buf, sizeof(buf), "default=%d  arial=%d  times=%d", defaultWidth, arialWidth, timesWidth);
        buf[sizeof(buf) - 1] = '\0';
        game.DrawText(24, y, buf, COLOR_LIGHT_GRAY);
        y += 16;
        snprintf(buf, sizeof(buf), "mono=%d  yahei=%d  gothic=%d  frame=%d", monoWidth, yaheiWidth, gothicWidth, frame);
        buf[sizeof(buf) - 1] = '\0';
        game.DrawText(24, y, buf, COLOR_LIGHT_GRAY);
        y += 26;

        game.FillRect(16, 540, 920, 72, COLOR_ARGB(255, 20, 20, 20));
        game.DrawRect(16, 540, 920, 72, COLOR_DARK_GRAY);
#if GAMELIB_SDL_HAS_TTF
        bool fontOk = (defaultWidth > 0) || (arialWidth > 0) || (timesWidth > 0) ||
                  (monoWidth > 0) || (yaheiWidth > 0) || (gothicWidth > 0);
        if (fontOk) {
            game.DrawText(24, 556, "TTF font rendering appears active. If some family names look identical, fallback mapping is probably in use.", COLOR_GREEN);
            game.DrawText(24, 574, "This is expected on Linux/macOS when Windows family names are redirected to platform defaults.", COLOR_GRAY);
        } else {
            game.DrawText(24, 556, "SDL_ttf is compiled in, but no usable system font was found at runtime.", COLOR_RED);
            game.DrawText(24, 574, "Check GAMELIB_SDL_DEFAULT_FONT or install a system font that matches the fallback candidate list.", COLOR_GRAY);
        }
#else
        game.DrawText(24, 556, "This build was compiled without SDL_ttf support, so DrawTextFont is intentionally disabled.", COLOR_RED);
        game.DrawText(24, 574, "Install SDL2_ttf and add -lsdl2_ttf to the link flags, then rebuild this demo.", COLOR_GRAY);
#endif

        game.Update();
        game.WaitFrame(60);
    }

    return 0;
}
