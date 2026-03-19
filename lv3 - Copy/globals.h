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

/* -------------------- ENUMS -------------------- */
enum Enemy3State { E3_WALKING, E3_ATTACKING };

/* -------------------- GLOBAL VARIABLES -------------------- */
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

#endif