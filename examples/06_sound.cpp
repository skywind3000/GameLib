// 06_sound.cpp - Sound Demo
//
// Demo GameLib sound features: beep, multi-channel WAV, background music.
// Learn: PlayBeep, PlayWAV, PlayPCM, StopWAV, IsPlaying, SetVolume, StopAll,
//        SetMasterVolume, GetMasterVolume, PlayMusic, StopMusic
//
// Compile (Win32): g++ -o 06_sound.exe 06_sound.cpp -mwindows
// Compile (SDL):   g++ -std=c++11 -O2 -o 06_sound 06_sound.cpp -lSDL2

#if defined(_WIN32) && !defined(USE_SDL)
#include "../GameLib.h"
#else
#include "../GameLib.SDL.h"
#endif

static const char *ChooseExistingPath(const char *pathA, const char *pathB)
{
    FILE *file = fopen(pathA, "rb");
    if (file != NULL) {
        fclose(file);
        return pathA;
    }
    file = fopen(pathB, "rb");
    if (file != NULL) {
        fclose(file);
        return pathB;
    }
    return pathA;
}

int main()
{
    GameLib game;
    game.Open(640, 480, "06 - Sound Demo", true);

    int lastWavChannel = -1;
    bool lastMusicOk = true;
    const char *wavEffect = ChooseExistingPath("../assets/sound/explosion.wav",
                                               "assets/sound/explosion.wav");

    // MIDI music: Win32 always supports it via MCI; SDL supports it if mixer has MID decoder
#if defined(_WIN32) && !defined(USE_SDL)
    const char *musicFile = ChooseExistingPath("../assets/music/battle1.mid",
                                               "assets/music/battle1.mid");
    const char *musicLabel = "Background Music (MCI MIDI):";
    const char *musicHint  = "(uses assets/music/battle1.mid via MCI sequencer)";
    bool hasMusicSupport = true;
#else
    const char *musicFile = ChooseExistingPath("../assets/music/battle1.mid",
                                               "assets/music/battle1.mid");
    const char *musicLabel = "Background Music:";
    const char *musicHint  = "(MIDI: depends on SDL_mixer MID decoder; WAV/OGG always works)";
    bool hasMusicSupport = true;
#endif

    // Key note frequencies (C4 to B4)
    int notes[] = {262, 294, 330, 349, 392, 440, 494, 523};
    const char *noteNames[] = {"C4", "D4", "E4", "F4", "G4", "A4", "B4", "C5"};
    int activeNote = -1;

    while (!game.IsClosed()) {
        if (game.IsKeyPressed(KEY_ESCAPE)) break;

        game.Clear(COLOR_DARK_BLUE);

        // --- Title ---
        game.DrawTextScale(180, 20, "Sound Demo", COLOR_GOLD, 16, 16);

        // === Section 1: Beep Piano ===
        game.DrawText(40, 70, "Piano (Beep) - Press 1~8:", COLOR_WHITE);
        game.DrawText(40, 85, "Non-blocking, returns channel ID", COLOR_GREEN);

        activeNote = -1;
        for (int i = 0; i < 8; i++) {
            int bx = 40 + i * 70;
            int by = 105;
            bool pressed = game.IsKeyPressed(KEY_1 + i);
            if (pressed) activeNote = i;

            uint32_t keyColor = (activeNote == i) ? COLOR_YELLOW : COLOR_WHITE;
            game.FillRect(bx, by, 55, 100, keyColor);
            game.DrawRect(bx, by, 55, 100, COLOR_BLACK);
            game.DrawText(bx + 20, by + 75, noteNames[i], COLOR_BLACK);

            if (pressed)
                game.PlayBeep(notes[i], 150);
        }

        // === Section 2: Multi-Channel WAV Sound ===
        game.DrawText(40, 230, "WAV Sound Effect (multi-channel):", COLOR_WHITE);

        game.FillRect(40, 250, 200, 30, COLOR_GREEN);
        game.DrawText(55, 258, "W - Play WAV", COLOR_BLACK);
        if (game.IsKeyPressed(KEY_W))
            lastWavChannel = game.PlayWAV(wavEffect, 1, 800);

        game.FillRect(260, 250, 200, 30, COLOR_RED);
        game.DrawText(290, 258, "S - Stop WAV", COLOR_BLACK);
        if (game.IsKeyPressed(KEY_S)) {
            if (lastWavChannel > 0)
                game.StopWAV(lastWavChannel);
            lastWavChannel = -1;
        }

        game.FillRect(460, 250, 140, 30, COLOR_ORANGE);
        game.DrawText(480, 258, "A - Stop All", COLOR_BLACK);
        if (game.IsKeyPressed(KEY_A))
            game.StopAll();

        game.DrawText(40, 290, "(uses assets/sound/explosion.wav)", COLOR_GRAY);
        game.DrawPrintf(40, 305, COLOR_LIGHT_GRAY, "Last WAV channel: %d",
                        lastWavChannel);
        if (lastWavChannel > 0) {
            game.DrawPrintf(40, 320, COLOR_LIGHT_GRAY, "Playing: %s",
                            game.IsPlaying(lastWavChannel) == 1 ? "Yes" : "No");
        }

        // === Section 3: Master Volume ===
        game.DrawText(40, 340, "Master Volume: +/- to adjust", COLOR_WHITE);
        int masterVol = game.GetMasterVolume();
        if (game.IsKeyPressed(KEY_ADD))
            masterVol = game.SetMasterVolume(masterVol + 100);
        if (game.IsKeyPressed(KEY_SUBTRACT))  // numpad -
            masterVol = game.SetMasterVolume(masterVol - 100);
        game.DrawPrintf(40, 355, COLOR_LIGHT_GRAY, "Volume: %d/1000", masterVol);

        // === Section 4: Background Music ===
        game.DrawText(40, 380, musicLabel, COLOR_WHITE);

        bool musicPlaying = game.IsMusicPlaying();

        if (hasMusicSupport) {
            game.FillRect(40, 400, 200, 30, musicPlaying ? COLOR_DARK_GREEN : COLOR_GREEN);
            game.DrawText(55, 408, "M - Play Music", COLOR_BLACK);
            if (game.IsKeyPressed(KEY_M) && !musicPlaying) {
                lastMusicOk = game.PlayMusic(musicFile);
                musicPlaying = game.IsMusicPlaying();
            }

            game.FillRect(260, 400, 200, 30, COLOR_RED);
            game.DrawText(275, 408, "N - Stop Music", COLOR_BLACK);
            if (game.IsKeyPressed(KEY_N) && musicPlaying)
                game.StopMusic();

            game.DrawPrintf(40, 445, COLOR_LIGHT_GRAY, "Music: %s",
                            game.IsMusicPlaying() ? "Playing" : "Stopped");
            game.DrawPrintf(40, 460, COLOR_LIGHT_GRAY, "Last music start: %s",
                            lastMusicOk ? "OK" : "Failed");
        } else {
            game.DrawText(40, 408, "No music file available for this platform.", COLOR_LIGHT_GRAY);
        }

        game.DrawText(40, 430, musicHint, COLOR_GRAY);

        game.DrawText(40, 470, "ESC to exit", COLOR_DARK_GRAY);

        game.Update();
        game.WaitFrame(60);
    }
    return 0;
}
