// 10_sound_demo.cpp - Sound Demo
//
// 演示 GameLib 的声音功能：蜂鸣、WAV 音效、MCI 背景音乐。
// 注意：PlayBeep 是阻塞的，按下时会暂停一小会。
// WAV/Music 需要对应文件存在，否则静默失败。
// 学习：PlayBeep, PlayWAV, StopWAV, PlayMusic, StopMusic
//
// 编译: g++ -o 10_sound_demo.exe 10_sound_demo.cpp -mwindows

#include "../GameLib.h"

int main()
{
    GameLib game;
    game.Open(640, 480, "10 - Sound Demo", true);

    bool musicPlaying = false;

    // 按键音符频率 (C4 到 B4)
    int notes[] = {262, 294, 330, 349, 392, 440, 494, 523};
    const char *noteNames[] = {"C4", "D4", "E4", "F4", "G4", "A4", "B4", "C5"};

    // 当前活动按钮 (用于视觉反馈)
    int activeNote = -1;

    while (!game.IsClosed()) {
        if (game.IsKeyPressed(KEY_ESCAPE)) break;

        game.Clear(COLOR_DARK_BLUE);

        // --- 标题 ---
        game.DrawTextScale(180, 20, "Sound Demo", COLOR_GOLD, 2);

        // === 区域 1: 蜂鸣器钢琴 ===
        game.DrawText(40, 70, "Piano (Beep) - Press 1~8:", COLOR_WHITE);
        game.DrawText(40, 85, "Warning: Beep is blocking!", COLOR_ORANGE);

        activeNote = -1;
        for (int i = 0; i < 8; i++) {
            int bx = 40 + i * 70;
            int by = 105;
            bool pressed = game.IsKeyPressed(KEY_1 + i);

            if (pressed) activeNote = i;

            // 钢琴键
            uint32_t keyColor = (activeNote == i) ? COLOR_YELLOW : COLOR_WHITE;
            game.FillRect(bx, by, 55, 100, keyColor);
            game.DrawRect(bx, by, 55, 100, COLOR_BLACK);
            game.DrawText(bx + 20, by + 75, noteNames[i], COLOR_BLACK);

            if (pressed)
                game.PlayBeep(notes[i], 150);
        }

        // === 区域 2: WAV 音效 ===
        game.DrawText(40, 230, "WAV Sound Effect:", COLOR_WHITE);

        game.FillRect(40, 250, 200, 30, COLOR_GREEN);
        game.DrawText(55, 258, "W - Play WAV", COLOR_BLACK);
        if (game.IsKeyPressed(KEY_W)) {
            // 尝试播放 assets 目录下的 WAV 文件
            game.PlayWAV("../assets/sound.wav");
        }

        game.FillRect(260, 250, 200, 30, COLOR_RED);
        game.DrawText(290, 258, "S - Stop WAV", COLOR_BLACK);
        if (game.IsKeyPressed(KEY_S))
            game.StopWAV();

        game.DrawText(40, 290, "(needs ../assets/sound.wav)", COLOR_GRAY);

        // === 区域 3: 背景音乐 ===
        game.DrawText(40, 330, "Background Music (MCI):", COLOR_WHITE);

        game.FillRect(40, 350, 200, 30, musicPlaying ? COLOR_DARK_GREEN : COLOR_GREEN);
        game.DrawText(55, 358, "M - Play Music", COLOR_BLACK);
        if (game.IsKeyPressed(KEY_M) && !musicPlaying) {
            game.PlayMusic("../assets/music.mp3");
            musicPlaying = true;
        }

        game.FillRect(260, 350, 200, 30, COLOR_RED);
        game.DrawText(275, 358, "N - Stop Music", COLOR_BLACK);
        if (game.IsKeyPressed(KEY_N) && musicPlaying) {
            game.StopMusic();
            musicPlaying = false;
        }

        game.DrawText(40, 390, "(needs ../assets/music.mp3)", COLOR_GRAY);

        // 状态
        game.DrawPrintf(40, 420, COLOR_LIGHT_GRAY, "Music: %s", musicPlaying ? "Playing" : "Stopped");

        // 底部提示
        game.DrawText(40, 460, "ESC to exit", COLOR_DARK_GRAY);

        game.Update();
        game.WaitFrame(60);
    }
    return 0;
}
