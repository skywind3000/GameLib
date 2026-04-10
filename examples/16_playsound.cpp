#include "../GameLib.h"

int main() {
    GameLib game;
    game.Open(640, 480, "Sound Demo", true);

    while (!game.IsClosed()) {
        game.Clear(COLOR_BLACK);

        // 按键播放音效
        if (game.IsKeyPressed(KEY_SPACE)) {
            game.PlayWAV("../assets/sound/explosion.wav");  // 播放爆炸音效
        }
        if (game.IsKeyPressed(KEY_1)) {
            game.PlayWAV("../assets/sound/note_do.wav");    // 播放音符 Do
        }
        if (game.IsKeyPressed(KEY_2)) {
            game.PlayWAV("../assets/sound/note_re.wav");    // 播放音符 Re
        }
        if (game.IsKeyPressed(KEY_V)) {
            game.PlayWAV("../assets/sound/victory.wav");    // 播放胜利音效
        }

        // 显示提示
        game.DrawText(10, 10, "SPACE: Explosion", COLOR_WHITE);
        game.DrawText(10, 22, "1/2: Note Do/Re", COLOR_WHITE);
        game.DrawText(10, 34, "V: Victory", COLOR_WHITE);

        game.Update();
        game.WaitFrame(60);
    }
    return 0;
}
