#ifndef GLOBALS_H
#define GLOBALS_H

#include "constant.h"

/* -------------------- STRUCTS -------------------- */
struct Bridge {
  int minX, maxX;
  int minY, maxY;
  int snapY;
};

struct Collectible {
  int x, y;
  bool collected;
  bool isTreasure;
};

struct SpikeObstacle {
	float x;
	float y;
	bool playerTouching;
	bool kneeling;
	int  kneelTimer;
};

struct PowerUpToken {
	float x, y;
	bool collected;
};

struct TreeObstacle {
	float x;
	float y;
	bool  playerTouching;
};

struct Lv3Coin {
	float x, y;
	bool  collected;
	bool  showText;
	int   textTimer;
};

// Hitbox definitions (screen coords) — tweak freely
struct LvlHitbox { int x, y, w, h; };

/* -------------------- ENUMS -------------------- */
enum Enemy3State { E3_WALKING, E3_ATTACKING };

enum Lv3BossState {
	LV3BOSS_WALKING,   // idle walk loop on bridge 10
	LV3BOSS_FIRE,      // fire attack (loops 3×)
	LV3BOSS_JUMP,      // jump animation (once)
	LV3BOSS_SLASH      // slash animation (once)
};

// ── State enum ───────────────────────────────────────────────
enum Lv3En1State {
	LV3EN1_IDLE,
	LV3EN1_WALKING,
	LV3EN1_ATTACKING,
	LV3EN1_HIT_REACT
};

enum Lv3En2State {
	LV3EN2_IDLE,
	LV3EN2_RUNNING,
	LV3EN2_ATTACKING,
	LV3EN2_HIT_REACT
};

// ── State enum ───────────────────────────────────────────────
enum Lv3En3State {
	LV3EN3_IDLE_STATE,
	LV3EN3_WALKING,
	LV3EN3_ATTACKING,
	LV3EN3_HIT_REACT
};

enum Lv3En4State {
	LV3EN4_IDLE,
	LV3EN4_RUNNING,
	LV3EN4_ATTACKING,
	LV3EN4_HIT_REACT
};

enum Lv3En5State {
	LV3EN5_WAITING,
	LV3EN5_APPEARING,
	LV3EN5_IDLE,
	LV3EN5_RUNNING,
	LV3EN5_PUNCHING,
	LV3EN5_HIT_REACT,
	LV3EN5_REVIVING,
	LV3EN5_DYING,
	LV3EN5_TRULY_DEAD
};

enum Lv3En6State {
	LV3EN6_WAITING,
	LV3EN6_APPEARING,
	LV3EN6_IDLE,
	LV3EN6_RUNNING,
	LV3EN6_PUNCHING,
	LV3EN6_HIT_REACT,
	LV3EN6_REVIVING,
	LV3EN6_DYING,
	LV3EN6_TRULY_DEAD
};


/* -------------------- GLOBAL VARIABLES -------------------- */
//-----------------------------LEVEL 1--------------------------
// Background images
extern int bg1, bg2, bg41;
extern int bg5, bg6, bg7;
extern int currentBg;
// Story sequence
extern int storyImgs[7];
extern int storyIndex;
extern bool inStorySequence;
// Menu buttons
extern int btnNormal[NUM_BUTTONS];
extern int btnHover[NUM_BUTTONS];
extern bool btnIsHover[NUM_BUTTONS];

extern int backBtn, backBtnHover;
extern bool backHover;

extern int backSize;
extern int backMargin;
extern int backX;
extern int backY;
extern int backW;
extern int backH;

extern int btnX;
extern int btnW;
extern int btnH;
extern int btnStartY;
extern int btnGap;

extern int signpostImg;
extern int winFlagImg;
extern int winnerImg;
extern bool playerWon;

// Bridges
extern Bridge bridges[];
extern int numBridges;

// Player images
extern int playerSide, playerSideLeft;
extern int playerSideM, playerSideLeftM;
extern int punchFistImg, punchHitImg;
extern int punchFistImgM, punchHitImgM;

extern int chargedPunchImg, chargedFistImg, chargedPunchImgSeq;
extern int chargedPunchImgM, chargedFistImgM, chargedPunchImgSeqM;
extern int chargeScrollImg;

// Player state
extern int playerX;
extern int playerY;
extern bool movingFront;
extern int animFrame;
extern int punchState;
extern int chargedHitWindow;

// Punch hit flag
extern bool punchJustLanded;

// Jump
extern bool isJumping;
extern float jumpVelocity;

// Game state
extern bool gameStarted;

// Charged punch state
extern bool chargingPunch;
extern bool showChargeScroll;
extern bool chargeScrollShown;
extern bool chargeScrollDismissed;
extern bool chargedSequenceActive;
extern int chargedPunchState;
extern int chargedUseCount;
extern int chargedTimeCounter;
extern bool chargedTimerRunning;

// Player death
extern bool playerAlive;
extern int playerDeadImg;
extern int playerDeathY;
extern bool killedByEnemy1;
extern bool killedByDragon;
extern bool killedByEnemy3;
extern bool killedByEnemy4;
extern bool killedByEnemy2;

// Lives system
extern int playerLives;
extern bool gameOverTriggered;
extern bool gameOverScreen;
extern int gameOverImg;
extern int gameOverDelayCounter;
extern int deathDelayCounter;
extern bool deathWaiting;

// Camera
extern int cameraX;

// Enemy1 variables
extern int enemy1Img;
extern float enemy1FloatOffset;
extern float enemy1FloatSpeed;
extern float enemy1FloatRange;
extern bool enemy1Alive;
extern int enemy1HitCount;
extern int enemy1DeadImg;
extern float enemy1DeadY;
extern bool enemy1Falling;
extern float enemy1DeadVelocity;

// Enemy2 variables
extern int enemy2LeftImg, enemy2RightImg;
extern bool enemy2MovingLeft;
extern float enemy2X;
extern float enemy2Y;
extern float enemy2FloatOffset;
extern int enemy2HitCount;
extern bool enemy2Dead;
extern float enemy2DeadY;
extern bool enemy2Falling;
extern int enemy2DeadImg;
extern int enemy2BodyCollisionCount;
extern bool enemy2WasColliding;

// Enemy3 variables
extern int enemy3IdleImg;
extern int enemy3WalkImg;
extern int enemy3FistImg;
extern int enemy3PunchImg;
extern bool enemy3Alive;
extern int enemy3IdleImgM;
extern int enemy3WalkImgM;
extern int enemy3FistImgM;
extern int enemy3PunchImgM;
extern float enemy3X;
extern float enemy3Speed;
extern bool enemy3MovingLeft;
extern Enemy3State enemy3State;
extern float enemy3WalkOffset;
extern float enemy3WalkAmplitude;
extern float enemy3WalkSpeed;
extern int enemy3AnimFrame;
extern int enemy3StateTimer;
extern int enemy3PunchFrame;
extern bool enemy3PunchToggle;
extern int enemy3HitCount;
extern bool enemy3Falling;
extern float enemy3DeadY;
extern float enemy3DeadVelocity;
extern int enemy3DeadImg;
extern int enemy3PunchCount;

// Enemy4 variables
extern int enemy4Img;
extern int enemy4DeadImg;
extern bool enemy4Alive;
extern int enemy4HitCount;
extern float enemy4FloatOffset;
extern float enemy4FloatSpeed;
extern float enemy4FloatRange;
extern float enemy4DeadY;

// Dragon variables
extern int dragonFrames[DRAGON_FRAME_COUNT];
extern int dragonFrameIndex;
extern int dragonDeadImg;
extern bool dragonFireHitChecked;
extern bool dragonAlive;
extern int dragonHitCount;
extern bool dragonFireActive;

// Collectibles
extern int hudLifeImg, hudPunchImg;
extern int tokenImg, treasureImg;
extern int tokenCount, treasureCount;
extern Collectible collectibles[];
extern int numCollectibles;

extern bool swapEnemySprites;

//------------LEVEL 3 GLOBALS---------------------
extern int lastGameplayBg = 4;   // remembers which level was active before Game Over

extern int kneelImg;
extern bool showKneel;
extern int kneelTimer;
extern bool kneelHolding;
extern int kneelHoldCounter;

extern int instruction3Img;

#endif