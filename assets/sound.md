# Sound Assets

游戏音效资源列表，所有文件为 16-bit 单声道 WAV 格式，采样率 22050 Hz。
保存再 `assets/sound` 文件夹内。

## 音符 (Notes)

用于音乐游戏或音效提示的单音音符。

| 文件名 | 音名 | 频率 (Hz) | 时长 (s) | 描述 |
|--------|------|-----------|----------|------|
| note_do.wav | C4 | 261.6 | 0.5 | 中音 Do |
| note_re.wav | D4 | 293.7 | 0.5 | 中音 Re |
| note_mi.wav | E4 | 329.6 | 0.5 | 中音 Mi |
| note_fa.wav | F4 | 349.2 | 0.5 | 中音 Fa |
| note_sol.wav | G4 | 392.0 | 0.5 | 中音 Sol |
| note_la.wav | A4 | 440.0 | 0.5 | 中音 La |
| note_si.wav | B4 | 493.9 | 0.5 | 中音 Si |
| note_do_high.wav | C5 | 523.3 | 0.5 | 高音 Do |

## 游戏音效 (Game Effects)

常用游戏场景音效。

| 文件名 | 时长 (s) | 描述 |
|--------|----------|------|
| explosion.wav | 1.0 | 爆炸声，白噪声 + 低频震动，适合爆炸、炸弹等场景 |
| victory.wav | 0.8 | 胜利声，上升三和弦琶音 (C-E-G-C)，适合过关、胜利提示 |
| man_laugh.wav | 1.2 | 男人笑声，低音 "Ha-Ha-Ha" |
| woman_laugh.wav | 1.0 | 女人笑声，高音 "Ha-Ha-Ha" |

## 交互音效 (Interaction)

UI 和玩家交互音效。

| 文件名 | 时长 (s) | 描述 |
|--------|----------|------|
| click.wav | 0.05 | 点击声，适合按钮、菜单点击 |
| coin.wav | 0.3 | 金币拾取声，金属清脆音，适合收集物品 |
| jump.wav | 0.25 | 跳跃声，上升频率扫描，适合角色跳跃 |
| hit.wav | 0.15 | 打击声，低沉撞击音，适合攻击、碰撞 |
| game_over.wav | 1.2 | 游戏结束声，下降旋律 (C-B-A-G)，适合失败提示 |

## 使用示例

使用 GameLib.h 播放音效：

```cpp
#include "GameLib.h"

int main() {
    GameLib game;
    game.Open(640, 480, "Sound Demo", true);

    while (!game.IsClosed()) {
        game.Clear(COLOR_BLACK);

        // 按键播放音效
        if (game.IsKeyPressed(KEY_SPACE)) {
            game.PlayWAV("assets/sound/explosion.wav");  // 播放爆炸音效
        }
        if (game.IsKeyPressed(KEY_1)) {
            game.PlayWAV("assets/sound/note_do.wav");    // 播放音符 Do
        }
        if (game.IsKeyPressed(KEY_2)) {
            game.PlayWAV("assets/sound/note_re.wav");    // 播放音符 Re
        }
        if (game.IsKeyPressed(KEY_V)) {
            game.PlayWAV("assets/sound/victory.wav");    // 播放胜利音效
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
```

---

*生成脚本: sound/generate_sounds.py*