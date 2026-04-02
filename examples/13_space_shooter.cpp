// 13_space_shooter.cpp - Space Shooter
//
// 综合性最强的示例：太空射击游戏。
// 包含：精灵(代码生成)、滚动星空背景、玩家飞船、敌机编队、
//       子弹系统、碰撞检测、爆炸效果、计分、难度递增。
// 学习：综合运用 GameLib 所有核心 API
//
// 编译: g++ -o 13_space_shooter.exe 13_space_shooter.cpp -mwindows

#include "../GameLib.h"

// ============ 常量 ============

#define W 480
#define H 640

#define MAX_STARS      80
#define MAX_BULLETS    30
#define MAX_ENEMIES    20
#define MAX_EXPLOSIONS 15
#define MAX_ENEMY_BULLETS 20

// ============ 数据结构 ============

struct Star   { float x, y, speed; uint32_t color; };
struct Bullet { float x, y; bool active; };
struct Enemy  { float x, y, vx, vy; int hp; bool active; int type; };

struct Explosion {
    float x, y;
    int timer;
    bool active;
};

struct EnemyBullet { float x, y, vy; bool active; };

// ============ 精灵创建 (代码生成) ============

int CreatePlayerSprite(GameLib &game)
{
    int id = game.CreateSprite(24, 24);
    if (id < 0) return -1;
    for (int y = 0; y < 24; y++)
        for (int x = 0; x < 24; x++)
            game.SetSpritePixel(id, x, y, 0);

    // 机身
    for (int y = 4; y < 20; y++)
        for (int x = 9; x < 15; x++)
            game.SetSpritePixel(id, x, y, COLOR_CYAN);
    // 机头
    for (int x = 10; x < 14; x++) {
        game.SetSpritePixel(id, x, 2, COLOR_WHITE);
        game.SetSpritePixel(id, x, 3, COLOR_WHITE);
    }
    game.SetSpritePixel(id, 11, 0, COLOR_WHITE);
    game.SetSpritePixel(id, 12, 0, COLOR_WHITE);
    game.SetSpritePixel(id, 11, 1, COLOR_WHITE);
    game.SetSpritePixel(id, 12, 1, COLOR_WHITE);
    // 翼
    for (int x = 2; x < 9; x++)
        for (int y = 13; y < 17; y++)
            game.SetSpritePixel(id, x, y, COLOR_GRAY);
    for (int x = 15; x < 22; x++)
        for (int y = 13; y < 17; y++)
            game.SetSpritePixel(id, x, y, COLOR_GRAY);
    // 引擎
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

    // 身体 (倒三角)
    for (int y = 2; y < 14; y++) {
        int half = (14 - y);
        int cx = 10;
        for (int x = cx - half; x < cx + half; x++)
            if (x >= 0 && x < 20)
                game.SetSpritePixel(id, x, y, bodyColor);
    }
    // 翼尖
    for (int y = 3; y < 8; y++) {
        game.SetSpritePixel(id, 1, y, COLOR_DARK_GRAY);
        game.SetSpritePixel(id, 18, y, COLOR_DARK_GRAY);
    }
    // 驾驶舱
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

    // 精灵
    int sprPlayer = CreatePlayerSprite(game);
    int sprEnemy1 = CreateEnemySprite(game, COLOR_RED);
    int sprEnemy2 = CreateEnemySprite(game, COLOR_MAGENTA);

    // 星空
    Star stars[MAX_STARS];
    for (int i = 0; i < MAX_STARS; i++) {
        stars[i].x = (float)GameLib::Random(0, W - 1);
        stars[i].y = (float)GameLib::Random(0, H - 1);
        stars[i].speed = (float)GameLib::Random(1, 4);
        int b = 80 + (int)(stars[i].speed * 40);
        if (b > 255) b = 255;
        stars[i].color = COLOR_RGB(b, b, b);
    }

    // 玩家
    float px = W / 2.0f - 12, py = H - 60.0f;

    // 子弹
    Bullet bullets[MAX_BULLETS];
    for (int i = 0; i < MAX_BULLETS; i++) bullets[i].active = false;
    int shootTimer = 0;

    // 敌人
    Enemy enemies[MAX_ENEMIES];
    for (int i = 0; i < MAX_ENEMIES; i++) enemies[i].active = false;
    int spawnTimer = 0;

    // 敌人子弹
    EnemyBullet eBullets[MAX_ENEMY_BULLETS];
    for (int i = 0; i < MAX_ENEMY_BULLETS; i++) eBullets[i].active = false;

    // 爆炸
    Explosion explosions[MAX_EXPLOSIONS];
    for (int i = 0; i < MAX_EXPLOSIONS; i++) explosions[i].active = false;

    int score = 0;
    int lives = 3;
    int level = 1;
    int killCount = 0;
    bool gameOver = false;
    int invincible = 0; // 无敌帧 (被击中后闪烁)

    while (!game.IsClosed()) {
        if (game.IsKeyPressed(KEY_ESCAPE)) break;

        if (!gameOver) {
            // --- 玩家移动 ---
            float spd = 5.0f;
            if (game.IsKeyDown(KEY_LEFT))  px -= spd;
            if (game.IsKeyDown(KEY_RIGHT)) px += spd;
            if (game.IsKeyDown(KEY_UP))    py -= spd;
            if (game.IsKeyDown(KEY_DOWN))  py += spd;
            if (px < 0) px = 0;
            if (px > W - 24) px = (float)(W - 24);
            if (py < 0) py = 0;
            if (py > H - 24) py = (float)(H - 24);

            // --- 自动射击 (按住空格) ---
            if (game.IsKeyDown(KEY_SPACE)) {
                shootTimer++;
                if (shootTimer >= 6) {
                    shootTimer = 0;
                    for (int i = 0; i < MAX_BULLETS; i++) {
                        if (!bullets[i].active) {
                            bullets[i].active = true;
                            bullets[i].x = px + 10;
                            bullets[i].y = py - 5;
                            break;
                        }
                    }
                }
            } else {
                shootTimer = 5; // 松开后下次按立即发射
            }

            // --- 更新子弹 ---
            for (int i = 0; i < MAX_BULLETS; i++) {
                if (!bullets[i].active) continue;
                bullets[i].y -= 10;
                if (bullets[i].y < -10) bullets[i].active = false;
            }

            // --- 生成敌人 ---
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

            // --- 更新敌人 ---
            for (int i = 0; i < MAX_ENEMIES; i++) {
                if (!enemies[i].active) continue;
                enemies[i].x += enemies[i].vx;
                enemies[i].y += enemies[i].vy;

                // 左右反弹
                if (enemies[i].x < 0 || enemies[i].x > W - 20)
                    enemies[i].vx = -enemies[i].vx;

                // 飞出屏幕
                if (enemies[i].y > H + 20) enemies[i].active = false;

                // 敌人射击 (随机)
                if (GameLib::Random(0, 200) < 1 + level) {
                    for (int j = 0; j < MAX_ENEMY_BULLETS; j++) {
                        if (!eBullets[j].active) {
                            eBullets[j].active = true;
                            eBullets[j].x = enemies[i].x + 10;
                            eBullets[j].y = enemies[i].y + 20;
                            eBullets[j].vy = 4.0f + level * 0.5f;
                            break;
                        }
                    }
                }
            }

            // --- 更新敌人子弹 ---
            for (int i = 0; i < MAX_ENEMY_BULLETS; i++) {
                if (!eBullets[i].active) continue;
                eBullets[i].y += eBullets[i].vy;
                if (eBullets[i].y > H + 10) eBullets[i].active = false;
            }

            // --- 碰撞: 玩家子弹 vs 敌人 ---
            for (int i = 0; i < MAX_BULLETS; i++) {
                if (!bullets[i].active) continue;
                for (int j = 0; j < MAX_ENEMIES; j++) {
                    if (!enemies[j].active) continue;
                    if (GameLib::RectOverlap(
                            (int)bullets[i].x - 1, (int)bullets[i].y - 4, 4, 8,
                            (int)enemies[j].x, (int)enemies[j].y, 20, 20)) {
                        bullets[i].active = false;
                        enemies[j].hp--;
                        if (enemies[j].hp <= 0) {
                            enemies[j].active = false;
                            score += (enemies[j].type + 1) * 100;
                            killCount++;
                            // 升级
                            if (killCount >= 10 + level * 5) {
                                level++;
                                killCount = 0;
                            }
                            // 爆炸效果
                            for (int k = 0; k < MAX_EXPLOSIONS; k++) {
                                if (!explosions[k].active) {
                                    explosions[k].active = true;
                                    explosions[k].x = enemies[j].x + 10;
                                    explosions[k].y = enemies[j].y + 10;
                                    explosions[k].timer = 15;
                                    break;
                                }
                            }
                        }
                        break;
                    }
                }
            }

            // --- 碰撞: 敌人子弹 vs 玩家 ---
            if (invincible > 0) {
                invincible--;
            } else {
                for (int i = 0; i < MAX_ENEMY_BULLETS; i++) {
                    if (!eBullets[i].active) continue;
                    if (GameLib::RectOverlap(
                            (int)eBullets[i].x - 2, (int)eBullets[i].y - 2, 4, 8,
                            (int)px + 4, (int)py + 2, 16, 20)) {
                        eBullets[i].active = false;
                        lives--;
                        invincible = 90; // 1.5 秒无敌
                        if (lives <= 0) gameOver = true;
                        break;
                    }
                }

                // --- 碰撞: 敌机 vs 玩家 ---
                for (int i = 0; i < MAX_ENEMIES; i++) {
                    if (!enemies[i].active) continue;
                    if (GameLib::RectOverlap(
                            (int)enemies[i].x, (int)enemies[i].y, 20, 20,
                            (int)px + 2, (int)py + 2, 20, 20)) {
                        enemies[i].active = false;
                        lives--;
                        invincible = 90;
                        if (lives <= 0) gameOver = true;
                        break;
                    }
                }
            }

            // --- 更新爆炸 ---
            for (int i = 0; i < MAX_EXPLOSIONS; i++) {
                if (!explosions[i].active) continue;
                explosions[i].timer--;
                if (explosions[i].timer <= 0) explosions[i].active = false;
            }

        } else {
            // 重新开始
            if (game.IsKeyPressed(KEY_R)) {
                px = W / 2.0f - 12; py = H - 60.0f;
                for (int i = 0; i < MAX_BULLETS; i++) bullets[i].active = false;
                for (int i = 0; i < MAX_ENEMIES; i++) enemies[i].active = false;
                for (int i = 0; i < MAX_ENEMY_BULLETS; i++) eBullets[i].active = false;
                for (int i = 0; i < MAX_EXPLOSIONS; i++) explosions[i].active = false;
                score = 0; lives = 3; level = 1; killCount = 0;
                gameOver = false; invincible = 0;
            }
        }

        // --- 更新星空 ---
        for (int i = 0; i < MAX_STARS; i++) {
            stars[i].y += stars[i].speed;
            if (stars[i].y > H) {
                stars[i].y = 0;
                stars[i].x = (float)GameLib::Random(0, W - 1);
            }
        }

        // ============ 绘制 ============
        game.Clear(COLOR_BLACK);

        // 星空
        for (int i = 0; i < MAX_STARS; i++)
            game.SetPixel((int)stars[i].x, (int)stars[i].y, stars[i].color);

        // 玩家子弹
        for (int i = 0; i < MAX_BULLETS; i++) {
            if (!bullets[i].active) continue;
            game.FillRect((int)bullets[i].x, (int)bullets[i].y - 4, 3, 8, COLOR_YELLOW);
        }

        // 敌人子弹
        for (int i = 0; i < MAX_ENEMY_BULLETS; i++) {
            if (!eBullets[i].active) continue;
            game.FillRect((int)eBullets[i].x - 1, (int)eBullets[i].y, 3, 6, COLOR_RED);
        }

        // 敌人
        for (int i = 0; i < MAX_ENEMIES; i++) {
            if (!enemies[i].active) continue;
            int spr = enemies[i].type == 0 ? sprEnemy1 : sprEnemy2;
            game.DrawSprite(spr, (int)enemies[i].x, (int)enemies[i].y);
        }

        // 爆炸
        for (int i = 0; i < MAX_EXPLOSIONS; i++) {
            if (!explosions[i].active) continue;
            int r = 15 - explosions[i].timer + 5;
            uint32_t c;
            if (explosions[i].timer > 10) c = COLOR_WHITE;
            else if (explosions[i].timer > 5) c = COLOR_YELLOW;
            else c = COLOR_ORANGE;
            game.FillCircle((int)explosions[i].x, (int)explosions[i].y, r, c);
            if (r > 3)
                game.FillCircle((int)explosions[i].x, (int)explosions[i].y, r - 3, COLOR_RED);
        }

        // 玩家 (无敌时闪烁)
        if (invincible == 0 || (invincible / 4) % 2 == 0)
            game.DrawSprite(sprPlayer, (int)px, (int)py);

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

    game.FreeSprite(sprPlayer);
    game.FreeSprite(sprEnemy1);
    game.FreeSprite(sprEnemy2);
    return 0;
}
