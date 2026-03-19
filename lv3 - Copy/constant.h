#ifndef CONSTANT_H
#define CONSTANT_H

/* -------------------- CONSTANTS -------------------- */
#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 800
#define NUM_BACKGROUNDS 4
#define NUM_BUTTONS 4
#define WINNER_WIDTH 500
#define WINNER_HEIGHT 320
// Player constants
#define PLAYER_WIDTH 120
#define PLAYER_HEIGHT 220
#define BG41_WIDTH 2400
#define BG41_HEIGHT 800

#define LV2_BG_WIDTH  3006   
#define LV2_BG_HEIGHT  800      

#define LV2_NUM_BRIDGES 9


#define BG41_GROUND_Y 120

#define PLAYER_START_X_BG41 150
#define PLAYER_START_Y_BG41 BG41_GROUND_Y

// Jump
const float GRAVITY = 1.2f;
const float JUMP_POWER = 19.0f;

// Charged punch
const int MAX_CHARGED_USES = 4;
const int CHARGED_TIME_LIMIT = 5000;

// ===== LIVES SYSTEM =====
#define MAX_LIVES 8
#define DEATH_DELAY_TIME 15

// Signpost constants
#define SIGNPOST_WIDTH 200
#define SIGNPOST_HEIGHT 170

// Win flag constants
#define WINFLAG_WIDTH 100
#define WINFLAG_HEIGHT 170
#define WINFLAG_X_OFFSET 10
#define WINFLAG_Y_OFFSET -25

// ---------------- ENEMY1 CONSTANTS ----------------
#define ENEMY1_HEIGHT PLAYER_HEIGHT
#define ENEMY1_WIDTH PLAYER_WIDTH
#define ENEMY1_BRIDGE_INDEX 0

const int ENEMY1_HITS_TO_KILL = 3;

// ---------------- ENEMY2 CONSTANTS ----------------
#define ENEMY2_HEIGHT PLAYER_HEIGHT
#define ENEMY2_WIDTH PLAYER_WIDTH
#define ENEMY2_FLOAT_HEIGHT 75
#define ENEMY2_FLOAT_SPEED 2.0f
#define ENEMY2_FLOAT_RANGE 300
#define ENEMY2_SCALE 0.8f

const int ENEMY2_HITS_TO_KILL = 3;
const int ENEMY2_MAX_BODY_COLLISIONS = 3;

// ---------------- ENEMY3 CONSTANTS ----------------
#define ENEMY3_BRIDGE_INDEX 8
#define ENEMY3_WIDTH PLAYER_WIDTH
#define ENEMY3_HEIGHT PLAYER_HEIGHT
#define ENEMY3_ATTACK_RANGE 55
#define ENEMY3_PUNCH_INTERVAL 12

const int ENEMY3_NORMAL_HITS_TO_KILL = 8;
const int ENEMY3_CHARGED_HITS_TO_KILL = 2;
const int ENEMY3_MAX_PUNCHES = 3;

// ---------------- ENEMY4 CONSTANTS ----------------
#define ENEMY4_HEIGHT PLAYER_HEIGHT
#define ENEMY4_WIDTH (int)(PLAYER_WIDTH * 1.4f)
#define ENEMY4_BRIDGE_INDEX 1
#define ENEMY4_HEIGHT_OFFSET 25

const int ENEMY4_HITS_TO_KILL = 4;

// ---------------- DRAGON CONSTANTS ----------------
#define DRAGON_FRAME_COUNT 12
#define DRAGON_BRIDGE_INDEX 5
#define DRAGON_HEIGHT PLAYER_HEIGHT
#define DRAGON_WIDTH (PLAYER_HEIGHT * 1.3)
#define DRAGON_X_OFFSET 130

const int DRAGON_HITS_TO_KILL = 6;

#define MAX_TOKENS 13
#define MAX_TREASURES 2

#endif