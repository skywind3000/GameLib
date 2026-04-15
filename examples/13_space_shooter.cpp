// 13_space_shooter.cpp - Space Shooter
//
// The most comprehensive example: a space shooting game.
// Features: asset-based player/bullets/explosions, scrolling starfield background,
//            enemy formations, bullet system, collision detection, sound effects,
//            scoring, difficulty scaling.
// Learn: comprehensive use of all core GameLib APIs
//
// Compile: g++ -o 13_space_shooter.exe 13_space_shooter.cpp -mwindows

#include "../GameLib.h"
#include <stdio.h>

// ============ Constants ============

#define W 480
#define H 640

#define MAX_STARS      80
#define MAX_BULLETS    30
#define MAX_ENEMIES    20
#define MAX_EXPLOSIONS 15
#define MAX_ENEMY_BULLETS 20

// ============ Data Structures ============

struct Star   { float x, y, speed; uint32_t color; };
struct Bullet { float x, y; bool active; };
struct Enemy  { float x, y, vx, vy; int hp; bool active; int type; };

struct Explosion {
    float x, y;
    int timer;
    bool active;
};

struct EnemyBullet { float x, y, vy; bool active; };

// ============ Fallback helpers ============

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

static void QueueSound(const char *&queuedSound, int &queuedPriority,
                       const char *candidate, int candidatePriority)
{
    if (candidate == NULL) return;
    if (candidatePriority >= queuedPriority) {
        queuedSound = candidate;
        queuedPriority = candidatePriority;
    }
}

static void SpawnExplosion(Explosion explosions[], float x, float y, int timer)
{
    for (int i = 0; i < MAX_EXPLOSIONS; i++) {
        if (!explosions[i].active) {
            explosions[i].active = true;
            explosions[i].x = x;
            explosions[i].y = y;
            explosions[i].timer = timer;
            break;
        }
    }
}

// ============ Sprite Creation (fallback only) ============

int CreatePlayerSprite(GameLib &game)
{
    int id = game.CreateSprite(24, 24);
    if (id < 0) return -1;
    for (int y = 0; y < 24; y++)
        for (int x = 0; x < 24; x++)
            game.SetSpritePixel(id, x, y, 0);

    // Body
    for (int y = 4; y < 20; y++)
        for (int x = 9; x < 15; x++)
            game.SetSpritePixel(id, x, y, COLOR_CYAN);
    // Nose
    for (int x = 10; x < 14; x++) {
        game.SetSpritePixel(id, x, 2, COLOR_WHITE);
        game.SetSpritePixel(id, x, 3, COLOR_WHITE);
    }
    game.SetSpritePixel(id, 11, 0, COLOR_WHITE);
    game.SetSpritePixel(id, 12, 0, COLOR_WHITE);
    game.SetSpritePixel(id, 11, 1, COLOR_WHITE);
    game.SetSpritePixel(id, 12, 1, COLOR_WHITE);
    // Wings
    for (int x = 2; x < 9; x++)
        for (int y = 13; y < 17; y++)
            game.SetSpritePixel(id, x, y, COLOR_GRAY);
    for (int x = 15; x < 22; x++)
        for (int y = 13; y < 17; y++)
            game.SetSpritePixel(id, x, y, COLOR_GRAY);
    // Engine
    game.SetSpritePixel(id, 11, 20, COLOR_ORANGE);
    game.SetSpritePixel(id, 12, 20, COLOR_ORANGE);
    game.SetSpritePixel(id, 11, 21, COLOR_YELLOW);
    game.SetSpritePixel(id, 12, 21, COLOR_YELLOW);

    return id;
}

int CreateEnemySprite(GameLib &game, uint32_t bodyColor)
{
    int id = game.CreateSprite(20, 20);
    if (id < 0) return -1;
    for (int y = 0; y < 20; y++)
        for (int x = 0; x < 20; x++)
            game.SetSpritePixel(id, x, y, 0);

    // Body (inverted triangle)
    for (int y = 2; y < 14; y++) {
        int half = (14 - y);
        int cx = 10;
        for (int x = cx - half; x < cx + half; x++)
            if (x >= 0 && x < 20)
                game.SetSpritePixel(id, x, y, bodyColor);
    }
    // Wing tips
    for (int y = 3; y < 8; y++) {
        game.SetSpritePixel(id, 1, y, COLOR_DARK_GRAY);
        game.SetSpritePixel(id, 18, y, COLOR_DARK_GRAY);
    }
    // Cockpit
    game.SetSpritePixel(id, 9,  5, COLOR_YELLOW);
    game.SetSpritePixel(id, 10, 5, COLOR_YELLOW);
    game.SetSpritePixel(id, 9,  6, COLOR_YELLOW);
    game.SetSpritePixel(id, 10, 6, COLOR_YELLOW);

    return id;
}

// ============ main ============

int main()
{
    GameLib game;
    game.Open(W, H, "13 - Space Shooter", true);

    const char *playerSpritePath = ChooseExistingPath("../assets/plane0.png", "assets/plane0.png");
    const char *bulletSpritePath = ChooseExistingPath("../assets/bullet.png", "assets/bullet.png");
    const char *explosionSpritePath = ChooseExistingPath("../assets/explosion.png", "assets/explosion.png");

    const char *shootSfx = ChooseExistingPath("../assets/sound/click.wav", "assets/sound/click.wav");
    const char *enemyHitSfx = ChooseExistingPath("../assets/sound/hit.wav", "assets/sound/hit.wav");
    const char *enemyDownSfx = ChooseExistingPath("../assets/sound/coin.wav", "assets/sound/coin.wav");
    const char *levelUpSfx = ChooseExistingPath("../assets/sound/note_do_high.wav", "assets/sound/note_do_high.wav");
    const char *playerHitSfx = ChooseExistingPath("../assets/sound/explosion.wav", "assets/sound/explosion.wav");
    const char *gameOverSfx = ChooseExistingPath("../assets/sound/game_over.wav", "assets/sound/game_over.wav");
    const char *restartSfx = ChooseExistingPath("../assets/sound/click.wav", "assets/sound/click.wav");

    // Sprites
    int sprPlayer = game.LoadSprite(playerSpritePath);
    if (sprPlayer >= 0) {
        game.SetSpriteColorKey(sprPlayer, COLORKEY_DEFAULT);
    } else {
        sprPlayer = CreatePlayerSprite(game);
        if (sprPlayer >= 0) game.SetSpriteColorKey(sprPlayer, 0);
    }

    int sprBullet = game.LoadSprite(bulletSpritePath);
    if (sprBullet >= 0) game.SetSpriteColorKey(sprBullet, COLORKEY_DEFAULT);

    int sprExplosion = game.LoadSprite(explosionSpritePath);
    if (sprExplosion >= 0) game.SetSpriteColorKey(sprExplosion, COLORKEY_DEFAULT);

    int sprEnemy1 = CreateEnemySprite(game, COLOR_RED);
    int sprEnemy2 = CreateEnemySprite(game, COLOR_MAGENTA);
    if (sprEnemy1 >= 0) game.SetSpriteColorKey(sprEnemy1, 0);
    if (sprEnemy2 >= 0) game.SetSpriteColorKey(sprEnemy2, 0);

    const int playerW = (sprPlayer >= 0) ? game.GetSpriteWidth(sprPlayer) : 24;
    const int playerH = (sprPlayer >= 0) ? game.GetSpriteHeight(sprPlayer) : 24;
    const int enemyW = 20;
    const int enemyH = 20;
    const int playerBulletW = 10;
    const int playerBulletH = 20;
    const int enemyBulletW = 8;
    const int enemyBulletH = 18;
    const int explosionLife = 16;

    // Starfield
    Star stars[MAX_STARS];
    for (int i = 0; i < MAX_STARS; i++) {
        stars[i].x = (float)GameLib::Random(0, W - 1);
        stars[i].y = (float)GameLib::Random(0, H - 1);
        stars[i].speed = (float)GameLib::Random(1, 4);
        int b = 80 + (int)(stars[i].speed * 40);
        if (b > 255) b = 255;
        stars[i].color = COLOR_RGB(b, b, b);
    }

    // Player
    float px = W / 2.0f - playerW / 2.0f, py = H - playerH - 28.0f;

    // Bullets
    Bullet bullets[MAX_BULLETS];
    for (int i = 0; i < MAX_BULLETS; i++) bullets[i].active = false;
    int shootTimer = 0;
    int shootSfxCooldown = 0;

    // Enemies
    Enemy enemies[MAX_ENEMIES];
    for (int i = 0; i < MAX_ENEMIES; i++) enemies[i].active = false;
    int spawnTimer = 0;

    // Enemy bullets
    EnemyBullet eBullets[MAX_ENEMY_BULLETS];
    for (int i = 0; i < MAX_ENEMY_BULLETS; i++) eBullets[i].active = false;

    // Explosions
    Explosion explosions[MAX_EXPLOSIONS];
    for (int i = 0; i < MAX_EXPLOSIONS; i++) explosions[i].active = false;

    int score = 0;
    int lives = 3;
    int level = 1;
    int killCount = 0;
    bool gameOver = false;
    int invincible = 0; // invincibility frames (blink after being hit)

    while (!game.IsClosed()) {
        const char *sfxToPlay = NULL;
        int sfxPriority = 0;

        if (game.IsKeyPressed(KEY_ESCAPE)) break;

        if (!gameOver) {
            if (shootSfxCooldown > 0) shootSfxCooldown--;

            // --- Player movement ---
            float spd = 5.0f;
            if (game.IsKeyDown(KEY_LEFT))  px -= spd;
            if (game.IsKeyDown(KEY_RIGHT)) px += spd;
            if (game.IsKeyDown(KEY_UP))    py -= spd;
            if (game.IsKeyDown(KEY_DOWN))  py += spd;
            if (px < 0) px = 0;
            if (px > W - playerW) px = (float)(W - playerW);
            if (py < 0) py = 0;
            if (py > H - playerH) py = (float)(H - playerH);

            // --- Auto fire (hold space) ---
            if (game.IsKeyDown(KEY_SPACE)) {
                shootTimer++;
                if (shootTimer >= 6) {
                    shootTimer = 0;
                    for (int i = 0; i < MAX_BULLETS; i++) {
                        if (!bullets[i].active) {
                            bullets[i].active = true;
                            bullets[i].x = px + playerW / 2.0f - playerBulletW / 2.0f;
                            bullets[i].y = py - playerBulletH + 6;
                            if (shootSfxCooldown <= 0) {
                                QueueSound(sfxToPlay, sfxPriority, shootSfx, 1);
                                shootSfxCooldown = 10;
                            }
                            break;
                        }
                    }
                }
            } else {
                shootTimer = 5; // next press fires immediately
            }

            // --- Update bullets ---
            for (int i = 0; i < MAX_BULLETS; i++) {
                if (!bullets[i].active) continue;
                bullets[i].y -= 10;
                if (bullets[i].y < -playerBulletH) bullets[i].active = false;
            }

            // --- Spawn enemies ---
            spawnTimer++;
            int rate = 50 - level * 5;
            if (rate < 15) rate = 15;
            if (spawnTimer >= rate) {
                spawnTimer = 0;
                for (int i = 0; i < MAX_ENEMIES; i++) {
                    if (!enemies[i].active) {
                        enemies[i].active = true;
                        enemies[i].x = (float)GameLib::Random(10, W - 30);
                        enemies[i].y = (float)GameLib::Random(-80, -20);
                        enemies[i].vx = (float)(GameLib::Random(-2, 2));
                        enemies[i].vy = (float)(GameLib::Random(1, 2 + level / 2));
                        enemies[i].type = GameLib::Random(0, 1);
                        enemies[i].hp = enemies[i].type == 0 ? 1 : 2;
                        break;
                    }
                }
            }

            // --- Update enemies ---
            for (int i = 0; i < MAX_ENEMIES; i++) {
                if (!enemies[i].active) continue;
                enemies[i].x += enemies[i].vx;
                enemies[i].y += enemies[i].vy;

                // Bounce off left/right
                if (enemies[i].x < 0 || enemies[i].x > W - enemyW)
                    enemies[i].vx = -enemies[i].vx;

                // Off screen
                if (enemies[i].y > H + enemyH) enemies[i].active = false;

                // Enemy shooting (random)
                if (GameLib::Random(0, 200) < 1 + level) {
                    for (int j = 0; j < MAX_ENEMY_BULLETS; j++) {
                        if (!eBullets[j].active) {
                            eBullets[j].active = true;
                            eBullets[j].x = enemies[i].x + enemyW / 2.0f - enemyBulletW / 2.0f;
                            eBullets[j].y = enemies[i].y + enemyH - 4;
                            eBullets[j].vy = 4.0f + level * 0.5f;
                            break;
                        }
                    }
                }
            }

            // --- Update enemy bullets ---
            for (int i = 0; i < MAX_ENEMY_BULLETS; i++) {
                if (!eBullets[i].active) continue;
                eBullets[i].y += eBullets[i].vy;
                if (eBullets[i].y > H + enemyBulletH) eBullets[i].active = false;
            }

            // --- Collision: player bullets vs enemies ---
            for (int i = 0; i < MAX_BULLETS; i++) {
                if (!bullets[i].active) continue;
                for (int j = 0; j < MAX_ENEMIES; j++) {
                    if (!enemies[j].active) continue;
                    if (GameLib::RectOverlap(
                            (int)bullets[i].x, (int)bullets[i].y, playerBulletW, playerBulletH,
                            (int)enemies[j].x, (int)enemies[j].y, enemyW, enemyH)) {
                        bullets[i].active = false;
                        enemies[j].hp--;
                        if (enemies[j].hp <= 0) {
                            enemies[j].active = false;
                            score += (enemies[j].type + 1) * 100;
                            killCount++;
                            QueueSound(sfxToPlay, sfxPriority, enemyDownSfx, 3);
                            // Level up
                            if (killCount >= 10 + level * 5) {
                                level++;
                                killCount = 0;
                                QueueSound(sfxToPlay, sfxPriority, levelUpSfx, 4);
                            }
                            SpawnExplosion(explosions,
                                           enemies[j].x + enemyW / 2.0f,
                                           enemies[j].y + enemyH / 2.0f,
                                           explosionLife);
                        } else {
                            QueueSound(sfxToPlay, sfxPriority, enemyHitSfx, 2);
                        }
                        break;
                    }
                }
            }

            // --- Collision: enemy bullets vs player ---
            if (invincible > 0) {
                invincible--;
            } else {
                for (int i = 0; i < MAX_ENEMY_BULLETS; i++) {
                    if (!eBullets[i].active) continue;
                    if (GameLib::RectOverlap(
                            (int)eBullets[i].x, (int)eBullets[i].y, enemyBulletW, enemyBulletH,
                            (int)px + 6, (int)py + 6, playerW - 12, playerH - 12)) {
                        eBullets[i].active = false;
                        lives--;
                        invincible = 90; // 1.5 seconds invincibility
                        SpawnExplosion(explosions,
                                       px + playerW / 2.0f,
                                       py + playerH / 2.0f,
                                       explosionLife);
                        if (lives <= 0) {
                            gameOver = true;
                            QueueSound(sfxToPlay, sfxPriority, gameOverSfx, 6);
                        } else {
                            QueueSound(sfxToPlay, sfxPriority, playerHitSfx, 5);
                        }
                        break;
                    }
                }

                // --- Collision: enemies vs player ---
                for (int i = 0; i < MAX_ENEMIES; i++) {
                    if (!enemies[i].active) continue;
                    if (GameLib::RectOverlap(
                            (int)enemies[i].x, (int)enemies[i].y, enemyW, enemyH,
                            (int)px + 6, (int)py + 6, playerW - 12, playerH - 12)) {
                        enemies[i].active = false;
                        lives--;
                        invincible = 90;
                        SpawnExplosion(explosions,
                                       enemies[i].x + enemyW / 2.0f,
                                       enemies[i].y + enemyH / 2.0f,
                                       explosionLife);
                        SpawnExplosion(explosions,
                                       px + playerW / 2.0f,
                                       py + playerH / 2.0f,
                                       explosionLife);
                        if (lives <= 0) {
                            gameOver = true;
                            QueueSound(sfxToPlay, sfxPriority, gameOverSfx, 6);
                        } else {
                            QueueSound(sfxToPlay, sfxPriority, playerHitSfx, 5);
                        }
                        break;
                    }
                }
            }

            // --- Update explosions ---
            for (int i = 0; i < MAX_EXPLOSIONS; i++) {
                if (!explosions[i].active) continue;
                explosions[i].timer--;
                if (explosions[i].timer <= 0) explosions[i].active = false;
            }

        } else {
            // Restart
            if (game.IsKeyPressed(KEY_R)) {
                px = W / 2.0f - playerW / 2.0f;
                py = H - playerH - 28.0f;
                for (int i = 0; i < MAX_BULLETS; i++) bullets[i].active = false;
                for (int i = 0; i < MAX_ENEMIES; i++) enemies[i].active = false;
                for (int i = 0; i < MAX_ENEMY_BULLETS; i++) eBullets[i].active = false;
                for (int i = 0; i < MAX_EXPLOSIONS; i++) explosions[i].active = false;
                score = 0; lives = 3; level = 1; killCount = 0;
                spawnTimer = 0; shootTimer = 0; shootSfxCooldown = 0;
                gameOver = false; invincible = 0;
                QueueSound(sfxToPlay, sfxPriority, restartSfx, 1);
            }
        }

        if (sfxToPlay)
            game.PlayWAV(sfxToPlay);

        // --- Update starfield ---
        for (int i = 0; i < MAX_STARS; i++) {
            stars[i].y += stars[i].speed;
            if (stars[i].y > H) {
                stars[i].y = 0;
                stars[i].x = (float)GameLib::Random(0, W - 1);
            }
        }

        // ============ Drawing ============
        game.Clear(COLOR_BLACK);

        // Starfield
        for (int i = 0; i < MAX_STARS; i++)
            game.SetPixel((int)stars[i].x, (int)stars[i].y, stars[i].color);

        // Player bullets
        for (int i = 0; i < MAX_BULLETS; i++) {
            if (!bullets[i].active) continue;
            if (sprBullet >= 0) {
                game.DrawSpriteScaled(sprBullet,
                                      (int)bullets[i].x, (int)bullets[i].y,
                                      playerBulletW, playerBulletH,
                                      SPRITE_COLORKEY);
            } else {
                game.FillRect((int)bullets[i].x, (int)bullets[i].y,
                              playerBulletW, playerBulletH, COLOR_YELLOW);
            }
        }

        // Enemy bullets
        for (int i = 0; i < MAX_ENEMY_BULLETS; i++) {
            if (!eBullets[i].active) continue;
            if (sprBullet >= 0) {
                game.DrawSpriteScaled(sprBullet,
                                      (int)eBullets[i].x, (int)eBullets[i].y,
                                      enemyBulletW, enemyBulletH,
                                      SPRITE_COLORKEY | SPRITE_FLIP_V);
            } else {
                game.FillRect((int)eBullets[i].x, (int)eBullets[i].y,
                              enemyBulletW, enemyBulletH, COLOR_RED);
            }
        }

        // Enemies
        for (int i = 0; i < MAX_ENEMIES; i++) {
            if (!enemies[i].active) continue;
            int spr = enemies[i].type == 0 ? sprEnemy1 : sprEnemy2;
            game.DrawSpriteEx(spr, (int)enemies[i].x, (int)enemies[i].y, SPRITE_COLORKEY);
        }

        // Explosions
        for (int i = 0; i < MAX_EXPLOSIONS; i++) {
            if (!explosions[i].active) continue;
            if (sprExplosion >= 0) {
                int frame = (explosionLife - explosions[i].timer) / 4;
                if (frame < 0) frame = 0;
                if (frame > 3) frame = 3;
                game.DrawSpriteFrameScaled(sprExplosion,
                                           (int)explosions[i].x - 16,
                                           (int)explosions[i].y - 16,
                                           32, 32, frame,
                                           32, 32,
                                           SPRITE_COLORKEY);
            } else {
                int r = explosionLife - explosions[i].timer + 5;
                uint32_t c;
                if (explosions[i].timer > 10) c = COLOR_WHITE;
                else if (explosions[i].timer > 5) c = COLOR_YELLOW;
                else c = COLOR_ORANGE;
                game.FillCircle((int)explosions[i].x, (int)explosions[i].y, r, c);
                if (r > 3)
                    game.FillCircle((int)explosions[i].x, (int)explosions[i].y, r - 3, COLOR_RED);
            }
        }

        // Player (blink when invincible)
        if (invincible == 0 || (invincible / 4) % 2 == 0)
            game.DrawSpriteEx(sprPlayer, (int)px, (int)py, SPRITE_COLORKEY);

        // HUD
        game.DrawPrintf(10, 10, COLOR_WHITE, "SCORE: %d", score);
        game.DrawPrintf(W - 100, 10, COLOR_GREEN, "LIVES: %d", lives);
        game.DrawPrintf(W / 2 - 30, 10, COLOR_YELLOW, "LV.%d", level);

        game.DrawText(10, H - 15, "Arrows:Move  Space:Shoot", COLOR_DARK_GRAY);

        if (gameOver) {
            game.FillRect(W / 2 - 120, H / 2 - 50, 240, 100, COLOR_DARK_GRAY);
            game.DrawRect(W / 2 - 120, H / 2 - 50, 240, 100, COLOR_WHITE);
            game.DrawTextScale(W / 2 - 65, H / 2 - 40, "GAME OVER", COLOR_RED, 2);
            game.DrawPrintf(W / 2 - 55, H / 2 + 0, COLOR_WHITE, "Final Score: %d", score);
            game.DrawPrintf(W / 2 - 45, H / 2 + 15, COLOR_WHITE, "Level: %d", level);
            game.DrawText(W / 2 - 50, H / 2 + 33, "R to restart", COLOR_YELLOW);
        }

        game.Update();
        game.WaitFrame(60);
    }

    if (sprPlayer >= 0) game.FreeSprite(sprPlayer);
    if (sprBullet >= 0) game.FreeSprite(sprBullet);
    if (sprExplosion >= 0) game.FreeSprite(sprExplosion);
    if (sprEnemy1 >= 0) game.FreeSprite(sprEnemy1);
    if (sprEnemy2 >= 0) game.FreeSprite(sprEnemy2);
    return 0;
}
