# /game — 用 GameLib 做游戏

When the user says they want to make a game / write a game / create a game, follow this 6-step flow:

## Steps

1. **Understand** — Ask the user what kind of game they want, core mechanics, controls. Clarify if unsure.
2. **Read API docs** — Read `docs/manual.md` or `docs/quickref.md` to know available APIs.
3. **Pick example** — Check the example list (see below) for the closest starting point. Copy it or base new file on it.
4. **Check assets** — Read `assets/sprites.md` and `assets/sound.md` for available image/sound assets. Use existing assets first.
5. **Generate skeleton** — Create a minimal playable prototype in `examples/`. Include `#include "../GameLib.h"`. Make sure basic mechanics work before adding polish.
6. **Compile verify** — Compile with `g++ -o output.exe examples/yourgame.cpp -mwindows` (from project root). Fix errors if any.

## Example reference

| # | File | Topic |
|---|------|-------|
| 01 | 01_hello.cpp | Window, text display |
| 02 | 02_movement.cpp | Keyboard movement, physics |
| 03 | 03_shapes.cpp | Primitives (ellipse, triangle, alpha) |
| 04 | 04_paint.cpp | Mouse input, drawing board |
| 05 | 05_sprites.cpp | Sprites, frame animation |
| 06 | 06_sound.cpp | Sound effects, BGM, PlayBeep |
| 07 | 07_shooting.cpp | Shooting, bullets, collision |
| 08 | 08_breakout.cpp | Breakout game |
| 09 | 09_snake.cpp | Snake game |
| 10 | 10_tilemap.cpp | Tilemap, parallax, camera |
| 11 | 11_font_text.cpp | TTF font, text measurement |
| 12 | 12_sprite_transform.cpp | Sprite scale, rotate |
| 13 | 13_clip_rect.cpp | Clip rectangle |
| 14 | 14_space_shooter.cpp | Full mini-game (enemies, scoring) |
| 15 | 15_ui_controls.cpp | Button, Checkbox, RadioBox |

## Rules

- Game files go in `examples/` directory.
- Always `#include "../GameLib.h"` (Win32 line).
- Follow AGENTS.md Code Constraints (C++11 only, no C++14+).
- Prefer a minimal prototype first; let user iterate before adding complexity.
- Check assets/sprites.md and assets/sound.md before suggesting new asset needs.
