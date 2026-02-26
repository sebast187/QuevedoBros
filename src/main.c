#include "raylib.h"
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include "levels.h"

// --- CONSTANTS ---
#define TILE_SIZE 40
#define GRAVITY 1800.0f         
#define JUMP_SPEED -720.0f      

// --- PHYSICS CONSTANTS (Mario Style) ---
#define PLAYER_MAX_SPEED 350.0f      
#define PLAYER_ACCEL 2000.0f         
#define PLAYER_FRICTION 1400.0f      
#define PLAYER_AIR_DRAG 300.0f       
#define SKID_THRESHOLD 100.0f        

#define MAX_ENEMIES 80
#define MAX_PARTICLES 200
#define MAX_HIGHSCORES 5
#define NUM_MAIN_LEVELS 8

// --- ENUMS & STRUCTS ---
typedef enum { 
    STATE_MENU, STATE_HIGHSCORES, STATE_READY, STATE_PLAYING, 
    STATE_PAUSED, STATE_DEAD, STATE_GAME_OVER, STATE_LEVEL_CLEAR, 
    STATE_SECRET_CLEAR, STATE_VICTORY 
} GameState;

typedef enum { THEME_GRASS, THEME_CAVE, THEME_SNOW, THEME_CASTLE } LevelTheme;
typedef enum { ENEMY_WADDLER, ENEMY_HOPPER, ENEMY_DROPPER } EnemyType;

typedef struct {
    Vector2 pos;
    Vector2 vel;
    int width, height;
    bool isGrounded;
    bool isDead;
    bool facingRight;
    int score;
    int lives;
} Player;

typedef struct {
    Vector2 pos;
    Vector2 vel;
    bool active;
    int width, height;
    float timer;
    EnemyType type;
    bool isGrounded;
} Enemy;

typedef struct {
    Vector2 pos;
    Vector2 vel;
    float life;
    Color color;
    bool active;
    float size;
    float rotation; 
} Particle;

typedef struct {
    int score;
    int level;
} HighScore;

// --- GLOBALS ---
char current_map[MAP_HEIGHT][MAP_WIDTH];
Player player;
Enemy enemies[MAX_ENEMIES];
Particle particles[MAX_PARTICLES];
HighScore highscores[MAX_HIGHSCORES];

int current_level = 0;
GameState state = STATE_MENU;
Camera2D camera = { 0 };
float targetCamY = 0.0f; 
LevelTheme currentTheme = THEME_GRASS;

// --- AUDIO GLOBALS ---
Music themeMusic[4];         
Music* currentMusic = NULL;  
Sound sfxJump;
Sound sfxCoin;
Sound sfxBreak;
Sound sfxDeath;
Sound sfxClear;

// --- FLAGPOLE GLOBALS ---
Vector2 flagPos = {0};
bool hasFlag = false;

// UI Selection
int menuSelection = 0;   
int pauseSelection = 0;  
float saveMessageTimer = 0.0f; 
float skidTimer = 0.0f; 

// --- MULTIPLAYER GLOBALS ---
int num_players = 1;
int current_player_idx = 0; 
int player_lives[2];
int player_scores[2];
int player_levels[2];
int player_return_levels[2] = {-1, -1}; 

// --- MAC BUNDLE PATH FIX ---
void SetupMacPaths() {
    // If the game can already see the files (like when running from Terminal), DO NOT change paths!
    if (FileExists("resources/bgm_grass.mp3")) return;

    ChangeDirectory(GetApplicationDirectory());
    if (DirectoryExists("../Resources")) ChangeDirectory("../Resources"); // .app Bundle
    else if (DirectoryExists("../resources")) ChangeDirectory("../"); // Build folder fallback
}

void StopGameMusic() {
    if (currentMusic != NULL) {
        StopMusicStream(*currentMusic);
    }
}

// --- HIGHSCORE SYSTEM ---
void LoadHighscores() {
    for(int i=0; i<MAX_HIGHSCORES; i++) { highscores[i].score = 0; highscores[i].level = 0; }
    FILE *f = fopen("quevedo_highscores.txt", "r");
    if(f) {
        for(int i=0; i<MAX_HIGHSCORES; i++) fscanf(f, "%d %d", &highscores[i].score, &highscores[i].level);
        fclose(f);
    }
}

void SaveHighscores() {
    FILE *f = fopen("quevedo_highscores.txt", "w");
    if(f) {
        for(int i=0; i<MAX_HIGHSCORES; i++) fprintf(f, "%d %d\n", highscores[i].score, highscores[i].level);
        fclose(f);
    }
}

void SubmitHighscore(int score, int level) {
    if (score == 0) return;
    for(int i=0; i<MAX_HIGHSCORES; i++) {
        if(score > highscores[i].score) {
            for(int j=MAX_HIGHSCORES-1; j>i; j--) highscores[j] = highscores[j-1]; 
            highscores[i].score = score;
            highscores[i].level = level;
            break;
        }
    }
    SaveHighscores();
}

// --- SAVE SYSTEM ---
void SaveGame() {
    FILE *file = fopen("quevedo_save.txt", "w");
    if (file) {
        fprintf(file, "%d %d\n", num_players, current_player_idx);
        fprintf(file, "%d %d %d %d\n", player_lives[0], player_scores[0], player_levels[0], player_return_levels[0]);
        fprintf(file, "%d %d %d %d\n", player_lives[1], player_scores[1], player_levels[1], player_return_levels[1]);
        fclose(file);
        saveMessageTimer = 2.0f; 
    }
}

bool LoadGame() {
    FILE *file = fopen("quevedo_save.txt", "r");
    if (file) {
        fscanf(file, "%d %d", &num_players, &current_player_idx);
        fscanf(file, "%d %d %d %d", &player_lives[0], &player_scores[0], &player_levels[0], &player_return_levels[0]);
        fscanf(file, "%d %d %d %d", &player_lives[1], &player_scores[1], &player_levels[1], &player_return_levels[1]);
        fclose(file);
        return true;
    }
    return false;
}

// --- HELPER MATH ---
float Lerp(float start, float end, float amount) { return start + amount * (end - start); }
float MoveTowards(float current, float target, float maxDelta) {
    if (fabs(target - current) <= maxDelta) return target;
    return current + (target > current ? maxDelta : -maxDelta);
}

// --- THEME & FX FUNCTIONS ---
LevelTheme GetLevelTheme(int level) {
    if (level == 8) return THEME_CAVE;   
    if (level == 9) return THEME_SNOW;   
    if (level == 10) return THEME_CASTLE; 
    if (level <= 1) return THEME_GRASS;
    if (level <= 3) return THEME_CAVE;
    if (level <= 5) return THEME_SNOW;
    return THEME_CASTLE;
}

void SpawnParticle(Vector2 pos, Color color, float speedMult, float size) {
    for (int i = 0; i < MAX_PARTICLES; i++) {
        if (!particles[i].active) {
            particles[i].active = true;
            particles[i].pos = pos;
            particles[i].vel = (Vector2){ (float)(GetRandomValue(-200, 200)) * speedMult, (float)(GetRandomValue(-300, -50)) * speedMult };
            particles[i].life = 1.0f;
            particles[i].color = color;
            particles[i].size = size;
            particles[i].rotation = 0;
            break;
        }
    }
}

void SpawnDebris(Vector2 pos) {
    for (int i = 0; i < 4; i++) SpawnParticle(pos, (Color){139, 69, 19, 255}, 1.5f, 12);
}

void LoadLevel(int level_idx) {    
    LevelTheme oldTheme = currentTheme;          
    currentTheme = GetLevelTheme(level_idx);     
    int enemy_count = 0;

    // --- DYNAMIC MUSIC SWAPPER ---
    if (currentMusic != NULL && oldTheme != currentTheme) {
        StopMusicStream(*currentMusic);          
    }
    currentMusic = &themeMusic[currentTheme]; 
    if (!IsMusicStreamPlaying(*currentMusic)) {
        PlayMusicStream(*currentMusic);          
    }
    // -----------------------------
    
    for (int y = 0; y < MAP_HEIGHT; y++) {
        for (int x = 0; x < MAP_WIDTH; x++) current_map[y][x] = ' ';
    }

    for (int y = 0; y < MAP_HEIGHT; y++) {
        const char* rowStr = GAME_LEVELS[level_idx][y];
        if (rowStr == NULL) continue; 
        
        int rowLen = strlen(rowStr);
        for (int x = 0; x < MAP_WIDTH; x++) {
            if (x >= rowLen) continue; 
            
            char tile = GAME_LEVELS[level_idx][y][x];
            current_map[y][x] = tile;
            
            if (tile == 'P') {
                player.pos = (Vector2){ x * TILE_SIZE, y * TILE_SIZE };
                player.vel = (Vector2){ 0, 0 };
                player.isDead = false;
                player.facingRight = true;
                current_map[y][x] = ' '; 
                
                targetCamY = player.pos.y - 100;
                camera.target = (Vector2){ player.pos.x, targetCamY };
            }
            else if (tile == 'E' || tile == 'H' || tile == 'D') {
                if (enemy_count < MAX_ENEMIES) {
                    enemies[enemy_count].width = 30;
                    enemies[enemy_count].height = 30;
                    enemies[enemy_count].pos = (Vector2){ x * TILE_SIZE + 5, y * TILE_SIZE + 10 };
                    enemies[enemy_count].vel = (Vector2){ -80.0f, 0 };
                    enemies[enemy_count].active = true;
                    enemies[enemy_count].timer = GetRandomValue(0, 100);
                    
                    if (tile == 'E') enemies[enemy_count].type = ENEMY_WADDLER;
                    else if (tile == 'H') enemies[enemy_count].type = ENEMY_HOPPER;
                    else { enemies[enemy_count].type = ENEMY_DROPPER; enemies[enemy_count].vel.x = 0; }
                    enemy_count++;
                }
                current_map[y][x] = ' '; 
            }
            else if (tile == 'F') {
                flagPos = (Vector2){ x * TILE_SIZE, y * TILE_SIZE };
                hasFlag = true;
                current_map[y][x] = ' '; 
            }
        }
    }
    for (int i = enemy_count; i < MAX_ENEMIES; i++) enemies[i].active = false;
    for (int i = 0; i < MAX_PARTICLES; i++) particles[i].active = false;
}

bool IsTileSolid(char tile) { return (tile == '#' || tile == '?' || tile == 'X' || tile == 'W' || tile == 'V' || tile == 'U' || tile == 'B'); }

// --- PHYSICS ---
void UpdatePhysics(float dt) {
    float currentGravity = GRAVITY;
    if (player.vel.y < 0 && !(IsKeyDown(KEY_UP) || IsKeyDown(KEY_SPACE))) currentGravity *= 2.5f; 
    
    player.vel.y += currentGravity * dt;
    player.isGrounded = false;

    // --- X Axis Movement ---
    player.pos.x += player.vel.x * dt;
    
    int leftTileX = player.pos.x / TILE_SIZE;
    int rightTileX = (player.pos.x + player.width - 0.01f) / TILE_SIZE;
    int topTileX = (player.pos.y + 4.0f) / TILE_SIZE; 
    int bottomTileX = (player.pos.y + player.height - 4.0f) / TILE_SIZE; 

    for (int y = topTileX; y <= bottomTileX; y++) {
        for (int x = leftTileX; x <= rightTileX; x++) {
            if (x >= 0 && x < MAP_WIDTH && y >= 0 && y < MAP_HEIGHT) {
                char t = current_map[y][x];
                if (IsTileSolid(t)) {
                    if (player.vel.x > 0) { player.pos.x = x * TILE_SIZE - player.width; player.vel.x = 0; }
                    else if (player.vel.x < 0) { player.pos.x = x * TILE_SIZE + TILE_SIZE; player.vel.x = 0; }
                }
                if (t == '^') player.isDead = true;
            }
        }
    }
    if (player.pos.x < 0) { player.pos.x = 0; player.vel.x = 0; }

    // --- Y Axis Movement ---
    player.pos.y += player.vel.y * dt;
    
    int leftTileY = (player.pos.x + 4.0f) / TILE_SIZE;
    int rightTileY = (player.pos.x + player.width - 4.0f) / TILE_SIZE;
    int topTileY = player.pos.y / TILE_SIZE;
    int bottomTileY = (player.pos.y + player.height - 0.01f) / TILE_SIZE;

    for (int y = topTileY; y <= bottomTileY; y++) {
        for (int x = leftTileY; x <= rightTileY; x++) {
            if (x >= 0 && x < MAP_WIDTH && y >= 0 && y < MAP_HEIGHT) {
                char t = current_map[y][x];
                
                if (IsTileSolid(t)) {
                    if (player.vel.y > 0) { 
                        player.pos.y = y * TILE_SIZE - player.height;
                        player.vel.y = 0;
                        player.isGrounded = true;
                    } else if (player.vel.y < 0) { 
                        player.pos.y = y * TILE_SIZE + TILE_SIZE;
                        player.vel.y = 0;
                        
                        if (t == '?') {
                            current_map[y][x] = 'X';
                            player.score += 100;
                            for (int i=0; i<8; i++) SpawnParticle((Vector2){x*TILE_SIZE+20, y*TILE_SIZE}, GOLD, 1.0f, 6);
                        }
                        else if (t == 'B') { 
                            current_map[y][x] = ' '; 
                            player.score += 50;
                            PlaySound(sfxBreak);
                            SpawnDebris((Vector2){x*TILE_SIZE + 20, y*TILE_SIZE + 20});
                        }
                    }
                } else if (t == '^') {
                    player.isDead = true; 
                } else if (t == 'C') {
                    current_map[y][x] = ' ';
                    player.score += 50;
                    PlaySound(sfxCoin);
                    for (int i=0; i<6; i++) SpawnParticle((Vector2){x*TILE_SIZE+20, y*TILE_SIZE+20}, YELLOW, 0.5f, 4);
                }
            }
        }
    }
    if (player.pos.y > MAP_HEIGHT * TILE_SIZE + 200) player.isDead = true;
    
    // --- TALL FLAGPOLE COLLISION & POINTS SYSTEM ---
    if (hasFlag) {
        Rectangle playerRec = { player.pos.x, player.pos.y, player.width, player.height };
        Rectangle flagRec = { flagPos.x + 16, flagPos.y - 120, 6, 160 }; 
        
        if (CheckCollisionRecs(playerRec, flagRec)) {
            float hitY = player.pos.y + player.height; 
            float poleBottom = flagPos.y + 40;
            float poleTop = flagPos.y - 120;
            
            float percent = (poleBottom - hitY) / (poleBottom - poleTop);
            if (percent < 0.0f) percent = 0.0f;
            if (percent > 1.0f) percent = 1.0f;
            
            int points = 200;
            if (percent > 0.85f) points = 5000;
            else if (percent > 0.60f) points = 2000;
            else if (percent > 0.30f) points = 800;
            else if (percent > 0.10f) points = 400;
            
            player.score += points;
            
            Color burstColor = (points == 5000) ? GOLD : (points >= 800) ? YELLOW : LIGHTGRAY;
            for (int i = 0; i < 30; i++) {
                SpawnParticle((Vector2){flagRec.x, hitY}, burstColor, 2.0f, 6);
            }
            
            PlaySound(sfxClear);
            state = STATE_LEVEL_CLEAR;
            hasFlag = false; 
        }
    }
}

void UpdateEnemies(float dt) {
    for (int i = 0; i < MAX_ENEMIES; i++) {
        if (!enemies[i].active) continue;
        enemies[i].timer += dt;

        if (enemies[i].type == ENEMY_WADDLER) {
            enemies[i].vel.y += GRAVITY * dt;
        }
        else if (enemies[i].type == ENEMY_HOPPER) {
            enemies[i].vel.y += GRAVITY * dt;
            if (enemies[i].isGrounded && enemies[i].timer > 2.0f) { 
                enemies[i].vel.y = JUMP_SPEED * 0.8f; 
                enemies[i].timer = 0;
            }
        }
        else if (enemies[i].type == ENEMY_DROPPER) {
            if (enemies[i].vel.y == 0 && fabs(player.pos.x - enemies[i].pos.x) < 60 && player.pos.y > enemies[i].pos.y) {
                enemies[i].vel.y = 800.0f;
            } else if (enemies[i].vel.y != 0) {
                enemies[i].vel.y += GRAVITY * dt;
            }
        }

        enemies[i].pos.x += enemies[i].vel.x * dt;
        
        int leftTileX = enemies[i].pos.x / TILE_SIZE;
        int rightTileX = (enemies[i].pos.x + enemies[i].width - 0.01f) / TILE_SIZE;
        int topTileX = (enemies[i].pos.y + 4.0f) / TILE_SIZE;
        int bottomTileX = (enemies[i].pos.y + enemies[i].height - 4.0f) / TILE_SIZE;

        bool hitWall = false;
        for (int y = topTileX; y <= bottomTileX; y++) {
            for (int x = leftTileX; x <= rightTileX; x++) {
                if (x >= 0 && x < MAP_WIDTH && y >= 0 && y < MAP_HEIGHT) {
                    if (IsTileSolid(current_map[y][x])) {
                        if (enemies[i].type != ENEMY_DROPPER) {
                            if (enemies[i].vel.x > 0) { 
                                enemies[i].pos.x = x * TILE_SIZE - enemies[i].width; 
                            } else if (enemies[i].vel.x < 0) { 
                                enemies[i].pos.x = x * TILE_SIZE + TILE_SIZE; 
                            }
                            hitWall = true;
                        }
                    }
                }
            }
        }

        bool hitLedge = false;
        if (enemies[i].type == ENEMY_HOPPER && enemies[i].isGrounded) {
            float probeX = (enemies[i].vel.x > 0) ? (enemies[i].pos.x + enemies[i].width + 1.0f) : (enemies[i].pos.x - 1.0f);
            float probeY = enemies[i].pos.y + enemies[i].height + 1.0f;
            int pTX = probeX / TILE_SIZE;
            int pTY = probeY / TILE_SIZE;
            if (pTX >= 0 && pTX < MAP_WIDTH && pTY < MAP_HEIGHT) {
                if (!IsTileSolid(current_map[pTY][pTX])) {
                    hitLedge = true;
                    if (enemies[i].vel.x > 0) enemies[i].pos.x = (pTX * TILE_SIZE) - enemies[i].width;
                    else enemies[i].pos.x = (pTX + 1) * TILE_SIZE;
                }
            }
        }

        if (hitWall || hitLedge) enemies[i].vel.x *= -1; 
        enemies[i].isGrounded = false;
        
        enemies[i].pos.y += enemies[i].vel.y * dt;
        int leftTileY = (enemies[i].pos.x + 4.0f) / TILE_SIZE;
        int rightTileY = (enemies[i].pos.x + enemies[i].width - 4.0f) / TILE_SIZE;
        int topTileY = enemies[i].pos.y / TILE_SIZE;
        int bottomTileY = (enemies[i].pos.y + enemies[i].height - 0.01f) / TILE_SIZE;

        for (int y = topTileY; y <= bottomTileY; y++) {
            for (int x = leftTileY; x <= rightTileY; x++) {
                if (x >= 0 && x < MAP_WIDTH && y >= 0 && y < MAP_HEIGHT) {
                    if (IsTileSolid(current_map[y][x])) {
                        if (enemies[i].vel.y > 0) {
                            enemies[i].pos.y = y * TILE_SIZE - enemies[i].height;
                            enemies[i].vel.y = 0;
                            enemies[i].isGrounded = true; 
                            if (enemies[i].type == ENEMY_DROPPER) {
                                for (int p=0; p<10; p++) SpawnParticle(enemies[i].pos, GRAY, 1.0f, 4);
                            }
                        }
                    } else if (current_map[y][x] == '^') {
                        enemies[i].active = false;
                        for (int p=0; p<10; p++) SpawnParticle(enemies[i].pos, MAROON, 1.2f, 5);
                    }
                }
            }
        }

        Rectangle pRec = { player.pos.x, player.pos.y, player.width, player.height };
        Rectangle eRec = { enemies[i].pos.x, enemies[i].pos.y, enemies[i].width, enemies[i].height };
        
        if (CheckCollisionRecs(pRec, eRec)) {
            if (enemies[i].type != ENEMY_DROPPER && player.vel.y > 0 && player.pos.y + player.height < enemies[i].pos.y + 20) {
                enemies[i].active = false;
                player.vel.y = JUMP_SPEED * 0.7f;
                player.score += (enemies[i].type == ENEMY_HOPPER) ? 400 : 200;
                for (int p=0; p<15; p++) SpawnParticle(enemies[i].pos, MAROON, 1.2f, 5);
            } else { player.isDead = true; }
        }
        
        if (enemies[i].pos.y > MAP_HEIGHT * TILE_SIZE + 200) enemies[i].active = false;
    }
}

// --- RENDERING ---
void DrawGame() {
    Color skyTop, skyBot;
    if (currentTheme == THEME_GRASS) { skyTop = (Color){ 60, 160, 240, 255 }; skyBot = (Color){ 160, 220, 255, 255 }; }
    else if (currentTheme == THEME_CAVE) { skyTop = (Color){ 20, 10, 30, 255 }; skyBot = (Color){ 60, 30, 70, 255 }; }
    else if (currentTheme == THEME_SNOW) { skyTop = (Color){ 150, 200, 255, 255 }; skyBot = (Color){ 230, 240, 255, 255 }; }
    else { skyTop = (Color){ 60, 0, 0, 255 }; skyBot = (Color){ 180, 40, 0, 255 }; } 

    DrawRectangleGradientV(0, 0, 800, 600, skyTop, skyBot);

    float time = GetTime();
    float wrapWidth = (MAP_WIDTH * TILE_SIZE) + 1600; 
    
    for(int i = 0; i < 40; i++) {
        float bgX = fmod((i * 120) - (camera.target.x * 0.15f) + 8000, wrapWidth) - 800; 
        if (currentTheme == THEME_GRASS) {
            DrawCircle(bgX, 80 + (i%4)*30, 45, (Color){255, 255, 255, 180});
            DrawCircle(bgX+30, 70 + (i%4)*30, 55, (Color){255, 255, 255, 180});
        } else if (currentTheme == THEME_CAVE) { 
            DrawTriangle((Vector2){bgX, 0}, (Vector2){bgX-30, 0}, (Vector2){bgX-15, 200 + (i%5)*50}, (Color){40, 20, 50, 200});
        } else if (currentTheme == THEME_SNOW) { 
            DrawCircle(bgX, fmod((i*150) + time*80, 600), 4 + (i%3), WHITE);
        } else if (currentTheme == THEME_CASTLE) { 
            DrawCircle(bgX, 600 - fmod((i*90) + time*120, 600), 5 + (i%4), ORANGE);
        }
    }

    BeginMode2D(camera);
    
    if (hasFlag) {
        DrawRectangle(flagPos.x + 16, flagPos.y - 120, 6, 160, LIGHTGRAY);
        DrawCircle(flagPos.x + 19, flagPos.y - 120, 8, GOLD);
        float wave = sinf(time * 8.0f) * 5.0f;
        DrawTriangle((Vector2){flagPos.x+22, flagPos.y-110}, (Vector2){flagPos.x+22, flagPos.y-70}, (Vector2){flagPos.x+55+wave, flagPos.y-90}, RED);
    }

    for (int y = 0; y < MAP_HEIGHT; y++) {
        for (int x = 0; x < MAP_WIDTH; x++) {
            char t = current_map[y][x];
            if (t == ' ') continue;
            
            int px = x * TILE_SIZE;
            int py = y * TILE_SIZE;

            if (t == '#') {
                Color topCol, dirtCol;
                if (currentTheme == THEME_GRASS) { topCol = (Color){34, 139, 34, 255}; dirtCol = (Color){139, 69, 19, 255}; }
                else if (currentTheme == THEME_CAVE) { topCol = (Color){100, 100, 100, 255}; dirtCol = (Color){60, 60, 60, 255}; }
                else if (currentTheme == THEME_SNOW) { topCol = (Color){255, 250, 250, 255}; dirtCol = (Color){100, 150, 180, 255}; }
                else { topCol = (Color){100, 20, 20, 255}; dirtCol = (Color){50, 10, 10, 255}; }
                DrawRectangle(px, py, TILE_SIZE, TILE_SIZE, dirtCol);
                DrawRectangle(px, py, TILE_SIZE, 8, topCol);
            } 
            else if (t == 'X') {
                DrawRectangle(px, py, TILE_SIZE, TILE_SIZE, GRAY);
                DrawRectangle(px+4, py+4, TILE_SIZE-8, TILE_SIZE-8, LIGHTGRAY);
                DrawRectangleLines(px, py, TILE_SIZE, TILE_SIZE, BLACK);
            }
            else if (t == 'B') { 
                DrawRectangle(px, py, TILE_SIZE, TILE_SIZE, (Color){139, 69, 19, 255}); 
                DrawRectangleLines(px, py, TILE_SIZE, TILE_SIZE, BLACK);
                DrawLine(px, py+20, px+40, py+20, BLACK);
                DrawLine(px+20, py, px+20, py+20, BLACK);
            }
            else if (t == '?') {
                float pulse = (sinf(time * 5.0f) + 1.0f) * 0.5f;
                DrawRectangle(px, py, TILE_SIZE, TILE_SIZE, (Color){200 + pulse*55, 150 + pulse*50, 0, 255});
                DrawRectangleLines(px, py, TILE_SIZE, TILE_SIZE, BLACK);
                DrawText("?", px + 12, py + 6, 28, WHITE);
            }
            else if (t == '^') {
                Color spikeCol = (currentTheme == THEME_CASTLE) ? ORANGE : DARKGRAY;
                DrawTriangle((Vector2){px+20, py+5}, (Vector2){px+5, py+40}, (Vector2){px+35, py+40}, spikeCol);
            }
            else if (t == 'W' || t == 'V' || t == 'U') { 
                DrawRectangle(px, py, TILE_SIZE, TILE_SIZE, LIME);
                DrawRectangle(px-4, py, TILE_SIZE+8, 10, GREEN);
                DrawRectangleLines(px-4, py, TILE_SIZE+8, 10, DARKGREEN);
            }
            else if (t == 'C') {
                float spinWidth = fabs(sinf(time * 3.0f + x)) * 14.0f;
                DrawEllipse(px + 20, py + 20, spinWidth, 14.0f, GOLD);
                DrawEllipse(px + 20, py + 20, spinWidth * 0.6f, 8.0f, YELLOW);
            }
        }
    }

    for (int i = 0; i < MAX_ENEMIES; i++) {
        if (!enemies[i].active) continue;
        float ex = enemies[i].pos.x; float ey = enemies[i].pos.y;
        if (enemies[i].type == ENEMY_WADDLER) {
            float waddle = sinf(enemies[i].timer * 10.0f) * 3.0f;
            DrawRectangle(ex + 4, ey + 10 + fabs(waddle), 22, 16, DARKBROWN);
            DrawCircle(ex + 15, ey + 10 + fabs(waddle), 14, MAROON);
            DrawRectangle(ex + 6, ey + 6 + fabs(waddle), 4, 8, WHITE);
            DrawRectangle(ex + 20, ey + 6 + fabs(waddle), 4, 8, WHITE);
        } 
        else if (enemies[i].type == ENEMY_HOPPER) {
            DrawRectangle(ex + 4, ey + 10, 22, 20, DARKGREEN); 
            DrawCircle(ex + 15, ey + 5, 12, GREEN);
            DrawCircle(ex + 8, ey + 2, 4, WHITE); 
            DrawCircle(ex + 22, ey + 2, 4, WHITE);
        }
        else if (enemies[i].type == ENEMY_DROPPER) {
            DrawRectangle(ex, ey, 30, 30, GRAY); 
            DrawRectangleLines(ex, ey, 30, 30, DARKGRAY);
            DrawRectangle(ex + 5, ey + 10, 6, 6, RED); 
            DrawRectangle(ex + 19, ey + 10, 6, 6, RED);
        }
    }

    if (!player.isDead) {
        float px = player.pos.x; float py = player.pos.y;
        float walkCycle = (fabs(player.vel.x) > 10.0f && player.isGrounded) ? sinf(time * 20.0f) * 8.0f : 0.0f;
        if (!player.isGrounded) walkCycle = -5.0f;

        Color pShirt = (current_player_idx == 0) ? RED : GREEN;
        Color pOveralls = BLUE;
        Color pHat = (current_player_idx == 0) ? RED : GREEN;

        DrawRectangle(px + 10 - walkCycle, py + 14, 6, 12, pShirt); 
        DrawRectangle(px + 6 + walkCycle, py + 26, 8, 14, pOveralls); 
        DrawRectangle(px + 16 - walkCycle, py + 26, 8, 14, pOveralls); 
        DrawRectangle(px + 4, py + 16, 22, 12, pOveralls); 
        DrawRectangle(px + 2, py + 8, 26, 8, pShirt); 
        DrawRectangle(px + 10 + walkCycle, py + 12, 8, 14, pShirt); 
        DrawRectangle(px + 4, py - 6, 22, 16, BEIGE); 
        
        if (player.facingRight) {
            DrawRectangle(px + 22, py - 2, 8, 6, BEIGE); 
            DrawRectangle(px + 18, py - 4, 4, 6, BLACK); 
            DrawRectangle(px + 16, py + 4, 10, 4, BLACK); 
            DrawRectangle(px + 12, py - 8, 20, 4, pHat); 
        } else {
            DrawRectangle(px, py - 2, 8, 6, BEIGE);
            DrawRectangle(px + 8, py - 4, 4, 6, BLACK);
            DrawRectangle(px + 4, py + 4, 10, 4, BLACK);
            DrawRectangle(px - 2, py - 8, 20, 4, pHat);
        }
        DrawRectangle(px + 2, py - 12, 26, 6, pHat); 
    }

    for (int i = 0; i < MAX_PARTICLES; i++) {
        if (particles[i].active) {
            DrawRectanglePro((Rectangle){particles[i].pos.x, particles[i].pos.y, particles[i].size, particles[i].size}, (Vector2){particles[i].size/2, particles[i].size/2}, particles[i].rotation, Fade(particles[i].color, particles[i].life));
        }
    }
    EndMode2D();

    DrawText(LEVEL_TITLES[current_level], 20, 20, 20, WHITE);
    DrawText(TextFormat("PLAYER %d SCORE: %d", current_player_idx+1, player.score), 20, 50, 20, WHITE);
    DrawText(TextFormat("LIVES: %d", player.lives), 680, 20, 20, RED);
}

// --- MAIN LOOP ---
int main(void) {
    SetTraceLogLevel(LOG_INFO); 
    InitWindow(800, 600, "Quevedo Bros Ultimate");
    SetTargetFPS(60);

    SetupMacPaths();

    InitAudioDevice();
    themeMusic[THEME_GRASS] = LoadMusicStream("resources/bgm_grass.mp3");
    themeMusic[THEME_CAVE]  = LoadMusicStream("resources/bgm_cave.mp3");
    themeMusic[THEME_SNOW]  = LoadMusicStream("resources/bgm_snow.mp3");
    themeMusic[THEME_CASTLE]= LoadMusicStream("resources/bgm_castle.mp3");

    sfxJump = LoadSound("resources/jump.wav");
    sfxCoin = LoadSound("resources/coin.wav");
    sfxBreak = LoadSound("resources/break.wav");
    sfxDeath = LoadSound("resources/death.wav");
    sfxClear = LoadSound("resources/clear.wav");
    
    for (int i = 0; i < 4; i++) SetMusicVolume(themeMusic[i], 0.6f);

    LoadHighscores();

    player.width = 30; player.height = 40;
    camera.offset = (Vector2){ 400.0f, 350.0f }; 
    camera.zoom = 1.0f;

    while (!WindowShouldClose()) {
        float dt = GetFrameTime();

        switch (state) {
            case STATE_MENU:
                if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S)) menuSelection = (menuSelection + 1) % 4;
                if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W)) menuSelection = (menuSelection - 1 + 4) % 4;

                if (IsKeyPressed(KEY_ENTER)) {
                    if (menuSelection == 0 || menuSelection == 1) { 
                        num_players = (menuSelection == 0) ? 1 : 2;
                        player_lives[0] = 3; player_scores[0] = 0; player_levels[0] = 0; player_return_levels[0] = -1;
                        player_lives[1] = 3; player_scores[1] = 0; player_levels[1] = 0; player_return_levels[1] = -1;
                        current_player_idx = 0;
                        state = STATE_READY;
                    } 
                    else if (menuSelection == 2) { 
                        if (LoadGame()) state = STATE_READY;
                    }
                    else if (menuSelection == 3) { 
                        state = STATE_HIGHSCORES;
                    }
                }
                break;

            case STATE_HIGHSCORES:
                if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_ESCAPE)) state = STATE_MENU;
                break;

            case STATE_READY:
                current_level = player_levels[current_player_idx];
                player.score = player_scores[current_player_idx];
                player.lives = player_lives[current_player_idx];

                if (IsKeyPressed(KEY_ENTER)) {
                    LoadLevel(current_level);
                    state = STATE_PLAYING;
                }
                break;

            case STATE_PLAYING:
                if (currentMusic != NULL) UpdateMusicStream(*currentMusic);
                
                if (IsKeyPressed(KEY_N)) state = STATE_LEVEL_CLEAR;
                if (IsKeyPressed(KEY_P) || IsKeyPressed(KEY_ESCAPE)) { state = STATE_PAUSED; pauseSelection = 0; }

                if ((IsKeyDown(KEY_DOWN) || IsKeyDown(KEY_S)) && player.isGrounded) {
                    int pX = (player.pos.x + player.width/2) / TILE_SIZE;
                    int pY = (player.pos.y + player.height + 2.0f) / TILE_SIZE;
                    if (pX >= 0 && pX < MAP_WIDTH && pY >= 0 && pY < MAP_HEIGHT) {
                        char t = current_map[pY][pX];
                        if (t == 'W' || t == 'V' || t == 'U') {
                            PlaySound(sfxClear); 
                            player.score += 5000;
                            player_return_levels[current_player_idx] = current_level;
                            
                            if (t == 'W') current_level = 8;
                            else if (t == 'V') current_level = 9;
                            else if (t == 'U') current_level = 10;
                            
                            player_levels[current_player_idx] = current_level;
                            state = STATE_SECRET_CLEAR; 
                        }
                    }
                }

                if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A)) { player.vel.x -= PLAYER_ACCEL * dt; player.facingRight = false; }
                if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D)) { player.vel.x += PLAYER_ACCEL * dt; player.facingRight = true; }

                if (!IsKeyDown(KEY_LEFT) && !IsKeyDown(KEY_A) && !IsKeyDown(KEY_RIGHT) && !IsKeyDown(KEY_D)) {
                    float friction = player.isGrounded ? PLAYER_FRICTION : PLAYER_AIR_DRAG;
                    player.vel.x = MoveTowards(player.vel.x, 0, friction * dt);
                }
                
                if ((player.vel.x > SKID_THRESHOLD && (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A))) ||
                    (player.vel.x < -SKID_THRESHOLD && (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D)))) {
                    skidTimer += dt;
                    if (skidTimer > 0.1f && player.isGrounded) {
                        SpawnParticle((Vector2){player.pos.x + 15, player.pos.y + 40}, LIGHTGRAY, 0.5f, 5);
                        skidTimer = 0;
                    }
                }

                if (player.vel.x > PLAYER_MAX_SPEED) player.vel.x = PLAYER_MAX_SPEED;
                if (player.vel.x < -PLAYER_MAX_SPEED) player.vel.x = -PLAYER_MAX_SPEED;
                
                if ((IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_SPACE)) && player.isGrounded) {
                    PlaySound(sfxJump);
                    player.vel.y = JUMP_SPEED;
                    for (int i=0; i<5; i++) SpawnParticle((Vector2){player.pos.x+15, player.pos.y+40}, LIGHTGRAY, 0.5f, 4);
                }

                UpdatePhysics(dt);
                UpdateEnemies(dt);

                for (int i = 0; i < MAX_PARTICLES; i++) {
                    if (particles[i].active) {
                        particles[i].pos.x += particles[i].vel.x * dt;
                        particles[i].pos.y += particles[i].vel.y * dt;
                        particles[i].rotation += 10.0f; 
                        particles[i].vel.y += GRAVITY * dt; 
                        particles[i].life -= dt * 1.0f;
                        if (particles[i].life <= 0) particles[i].active = false;
                    }
                }

                float desiredY = player.pos.y < 200 ? player.pos.y + 100 : 350.0f;
                targetCamY = Lerp(targetCamY, desiredY, 0.1f);
                camera.target.x = player.pos.x;
                camera.target.y = targetCamY;
                
                if (camera.target.x < 400.0f) camera.target.x = 400.0f;
                if (camera.target.x > (MAP_WIDTH * TILE_SIZE) - 400.0f) camera.target.x = (MAP_WIDTH * TILE_SIZE) - 400.0f;

                player_scores[current_player_idx] = player.score;

                if (player.isDead) { 
                    PlaySound(sfxDeath);
                    player_lives[current_player_idx]--; 
                    state = STATE_DEAD; 
                }
                break;

            case STATE_PAUSED:
                if (currentMusic != NULL) UpdateMusicStream(*currentMusic);
                if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S)) pauseSelection = (pauseSelection + 1) % 3;
                if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W)) pauseSelection = (pauseSelection - 1 + 3) % 3;
                if (IsKeyPressed(KEY_ENTER)) {
                    if (pauseSelection == 0) state = STATE_PLAYING; 
                    else if (pauseSelection == 1) SaveGame();       
                    else if (pauseSelection == 2) { 
                        StopGameMusic();
                        state = STATE_MENU; 
                    }
                }
                if (IsKeyPressed(KEY_P) || IsKeyPressed(KEY_ESCAPE)) state = STATE_PLAYING;
                if (saveMessageTimer > 0) saveMessageTimer -= dt;
                break;

            case STATE_DEAD:
                if (IsKeyPressed(KEY_ENTER)) {
                    int other_player = (current_player_idx + 1) % 2;
                    if (num_players == 2 && player_lives[other_player] > 0) {
                        current_player_idx = other_player;
                        state = STATE_READY;
                    } 
                    else if (player_lives[current_player_idx] > 0) {
                        state = STATE_READY; 
                    } 
                    else {
                        SubmitHighscore(player_scores[0], player_levels[0]);
                        if (num_players == 2) SubmitHighscore(player_scores[1], player_levels[1]);
                        StopGameMusic();
                        state = STATE_GAME_OVER;
                    }
                }
                break;

            case STATE_GAME_OVER:
                if (IsKeyPressed(KEY_ENTER)) state = STATE_MENU;
                break;
            
            case STATE_SECRET_CLEAR:
                if (currentMusic != NULL) UpdateMusicStream(*currentMusic);
                if (IsKeyPressed(KEY_ENTER)) {
                    state = STATE_READY;
                }
                break;

            case STATE_LEVEL_CLEAR:
                if (currentMusic != NULL) UpdateMusicStream(*currentMusic);
                if (IsKeyPressed(KEY_ENTER)) {
                    
                    if (current_level >= NUM_MAIN_LEVELS) {
                        current_level = player_return_levels[current_player_idx]; 
                        player_return_levels[current_player_idx] = -1; 
                    } else {
                        current_level++; 
                    }
                    
                    player_levels[current_player_idx] = current_level;
                    
                    if (current_level == NUM_MAIN_LEVELS) { 
                        SubmitHighscore(player_scores[current_player_idx], NUM_MAIN_LEVELS);
                        StopGameMusic();
                        state = STATE_VICTORY;
                    } else { 
                        state = STATE_READY; 
                    }
                }
                break;

            case STATE_VICTORY:
                if (IsKeyPressed(KEY_ENTER)) {
                    int other = (current_player_idx + 1) % 2;
                    if (num_players == 2 && player_lives[other] > 0 && player_levels[other] < NUM_MAIN_LEVELS) {
                        current_player_idx = other;
                        state = STATE_READY;
                    } else {
                        StopGameMusic();
                        state = STATE_MENU;
                    }
                }
                break;
        }

        BeginDrawing();
        if (state == STATE_PLAYING || state == STATE_PAUSED) {
            DrawGame();
            if (state == STATE_PAUSED) {
                DrawRectangle(0, 0, 800, 600, (Color){ 0, 0, 0, 200 });
                DrawText("PAUSED", 300, 150, 50, WHITE);
                DrawText(pauseSelection == 0 ? "> RESUME <" : "RESUME", 335, 250, 20, (pauseSelection == 0) ? GOLD : GRAY);
                DrawText(pauseSelection == 1 ? "> SAVE GAME <" : "SAVE GAME", 325, 300, 20, (pauseSelection == 1) ? GOLD : GRAY);
                DrawText(pauseSelection == 2 ? "> QUIT TO MENU <" : "QUIT TO MENU", 310, 350, 20, (pauseSelection == 2) ? GOLD : GRAY);
                if (saveMessageTimer > 0) DrawText("GAME SAVED!", 330, 420, 20, GREEN);
            }
        }
        else if (state == STATE_MENU) {
            ClearBackground((Color){20, 20, 50, 255});
            DrawText("QUEVEDO BROS", 170, 150, 60, GOLD);
            DrawText("THE GOLDEN MICROPHONES", 230, 220, 25, LIGHTGRAY);
            DrawText(menuSelection == 0 ? "> 1 PLAYER GAME <" : "1 PLAYER GAME", 280, 320, 20, (menuSelection == 0) ? GOLD : WHITE);
            DrawText(menuSelection == 1 ? "> 2 PLAYERS GAME <" : "2 PLAYERS GAME", 270, 360, 20, (menuSelection == 1) ? GOLD : WHITE);
            DrawText(menuSelection == 2 ? "> LOAD GAME <" : "LOAD GAME", 310, 400, 20, (menuSelection == 2) ? GREEN : DARKGREEN);
            DrawText(menuSelection == 3 ? "> HIGHSCORES <" : "HIGHSCORES", 305, 440, 20, (menuSelection == 3) ? ORANGE : MAROON);
            DrawText("Use Arrows to Select, Enter to Confirm", 180, 530, 20, GRAY);
        }
        else if (state == STATE_HIGHSCORES) {
            ClearBackground((Color){20, 20, 50, 255});
            DrawText("TOP 5 HIGHSCORES", 220, 100, 40, GOLD);
            for(int i=0; i<MAX_HIGHSCORES; i++) {
                DrawText(TextFormat("%d. SCORE: %06d   (LEVEL %d)", i+1, highscores[i].score, highscores[i].level+1), 220, 200 + (i*40), 20, WHITE);
            }
            DrawText("Press ENTER to return", 260, 480, 20, GRAY);
        }
        else if (state == STATE_READY) {
            ClearBackground(BLACK);
            Color pCol = (current_player_idx == 0) ? RED : GREEN;
            DrawText(TextFormat("PLAYER %d GET READY!", current_player_idx + 1), 200, 250, 40, pCol);
            DrawText(TextFormat("LIVES: %d", player.lives), 350, 320, 20, WHITE);
            
            if (current_level >= NUM_MAIN_LEVELS) DrawText("SECRET LEVEL", 330, 360, 20, LIME);
            else DrawText(TextFormat("LEVEL: %d", current_level + 1), 350, 360, 20, GRAY);
            
            DrawText("Press ENTER to Begin", 280, 450, 20, LIGHTGRAY);
        }
        else if (state == STATE_DEAD) {
            ClearBackground(BLACK);
            DrawText(TextFormat("PLAYER %d DIED", current_player_idx + 1), 250, 250, 40, RED);
            DrawText("Press ENTER", 330, 350, 20, LIGHTGRAY);
        }
        else if (state == STATE_GAME_OVER) {
            ClearBackground(BLACK);
            DrawText("GAME OVER", 280, 250, 40, RED);
            DrawText("Press ENTER for Main Menu", 250, 350, 20, LIGHTGRAY);
        }
        else if (state == STATE_LEVEL_CLEAR || state == STATE_SECRET_CLEAR) {
            DrawGame(); 
            DrawRectangle(0, 0, 800, 600, (Color){ 0, 0, 0, 180 });
            if (state == STATE_SECRET_CLEAR) {
                DrawText("WARP ZONE FOUND!", 200, 200, 40, LIME);
                DrawText("+5000 POINTS", 280, 250, 30, GOLD);
            } else { DrawText("LEVEL CLEARED!", 250, 200, 40, GOLD); }
            DrawText(LORE_TEXT[current_level], 150, 300, 20, WHITE);
            DrawText("Press ENTER to Continue", 280, 380, 20, LIGHTGRAY);
        }
        else if (state == STATE_VICTORY) {
            ClearBackground(GOLD);
            DrawText(TextFormat("PLAYER %d WINS!", current_player_idx + 1), 220, 200, 40, BLACK);
            DrawText(TextFormat("FINAL SCORE: %d", player.score), 280, 300, 30, RED);
            DrawText("Press ENTER to Continue", 250, 400, 20, DARKGRAY);
        }
        EndDrawing();
    }
    
    for (int i = 0; i < 4; i++) {
        UnloadMusicStream(themeMusic[i]);
    }
    UnloadSound(sfxJump); 
    UnloadSound(sfxCoin); 
    UnloadSound(sfxBreak);
    UnloadSound(sfxDeath); 
    UnloadSound(sfxClear);
    CloseAudioDevice();
    
    CloseWindow();
    return 0;
}