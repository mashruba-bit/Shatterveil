#include "constant.h"
#include "globals.h"
#include "iGraphics.h"
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <mmsystem.h>
#include <windows.h>
#pragma comment(lib, "winmm.lib")

int lastGameplayBg = 4;   // remembers which level was active before Game Over

// ── Level Select Screen ───────────────────────────────────────
int lvlSelectImg;          // lvl.png  (base)
int lvlSelectImg1;         // lvl1.png (hover on box 1)
int lvlSelectImg2;         // lvl2.png (hover on box 2)
int lvlSelectImg3;         // lvl3.jpg (hover on box 3)

// Which hitbox is hovered: 0=none, 1=lv1, 2=lv2, 3=lv3
int lvlHoverIndex = 0;

// Hitbox definitions (screen coords) — tweak freely
struct LvlHitbox { int x, y, w, h; };
LvlHitbox lvlHB[3] = {
	{ 120, 10, 230, 600 },   // hitbox 1 — Level 1
	{ 495, 10, 230, 600 },   // hitbox 2 — Level 2
	{ 860, 10, 230, 600 },   // hitbox 3 — Level 3
};
#define LVL_SELECT_DRAW_DEBUG  false// ← set false to hide debug boxes

// ═══════════════════════════════════════════════════════════════
//  LEVEL 3  PLAYER  PER-POSE  HITBOX  CONSTANTS
//  All values are fractions (0.0–1.0) of PLAYER_WIDTH / PLAYER_HEIGHT
//  or CROUCH_W / CROUCH_H — tweak freely.
// ═══════════════════════════════════════════════════════════════

// ── Pose 1: Idle / Walk  (side, sidem, sideleft, sideleftm) ──────
#define LV3_HB_IDLE_LEFT    0.20f
#define LV3_HB_IDLE_RIGHT   0.75f
#define LV3_HB_IDLE_TOP     0.08f
#define LV3_HB_IDLE_BOTTOM  0.95f

// ── Pose 2: Punch / Charge  (punchf) ────
#define LV3_HB_PUNCH_LEFT   0.15f
#define LV3_HB_PUNCH_RIGHT  0.55f
#define LV3_HB_PUNCH_TOP    0.08f
#define LV3_HB_PUNCH_BOTTOM 0.95f

// ── Pose 3: Crouch  (crouch.png / crouchm.png) ───────────────────
//  Uses CROUCH_W / CROUCH_H, not PLAYER_WIDTH/HEIGHT
#define LV3_HB_CROUCH_LEFT    0.35f
#define LV3_HB_CROUCH_RIGHT   0.65f
#define LV3_HB_CROUCH_TOP     0.60f
#define LV3_HB_CROUCH_BOTTOM  0.15f //decrease to push lower

// ── Pose 4a: Fist wind-up facing RIGHT (fist.png / chargedfist.png) ──
#define LV3_HB_FIST_R_LEFT    0.20f
#define LV3_HB_FIST_R_RIGHT   0.47f
#define LV3_HB_FIST_R_TOP     0.08f
#define LV3_HB_FIST_R_BOTTOM  0.95f

// ── Pose 4b: Fist wind-up facing LEFT (fistm.png / chargedfistm.png) ──
#define LV3_HB_FIST_L_LEFT    0.53f
#define LV3_HB_FIST_L_RIGHT   0.80f
#define LV3_HB_FIST_L_TOP     0.08f
#define LV3_HB_FIST_L_BOTTOM  0.95f

// ── Pose 5: Charged Punch release (chargedpunch.png / chargedpunchm.png) ──
#define LV3_HB_CHPUNCH_LEFT   -0.28f
#define LV3_HB_CHPUNCH_RIGHT   1.28f
#define LV3_HB_CHPUNCH_TOP     0.08f
#define LV3_HB_CHPUNCH_BOTTOM  0.95f

// ── Live hitbox struct + instance ────────────────────────────────
struct Lv3PlayerHitbox {
	int x1, y1, x2, y2;
};
Lv3PlayerHitbox lv3PHB = { 0, 0, 0, 0 };


/* -------------------- GLOBAL VARIABLE DEFINITIONS -------------------- */

int bg1, bg2, bg41;
int bg5, bg6, bg7;
int currentBg = 1;
int instruction3Img;
#define LV3_BG_WIDTH  5100
#define LV3_BG_HEIGHT 800
// ── Level 3 Shield Power ──────────────────────────────────────────
bool lv3ShieldActive = false;
int shieldLoadFrames[8];   // shl1.png … shl8.png  (under player's feet)
int shieldAuraFrames[11];   // shield1.png … shield7.png  (right of player)
int shieldLoadFrameIndex = 0;
int shieldAuraFrameIndex = 0;
int shieldAuraFramesM[11];  // shield1m.png … shield11m.png (left-facing)
int shieldAuraFrameIndexM = 0;
int chargedUseCountLv3 = 0;  // separate charged punch counter for level 3
// Size tweaks — adjust freely
#define SHLD_LOAD_W  140    // width of foot glow
#define SHLD_LOAD_H  100     // height of foot glow
#define SHLD_LOAD_Y_OFF  -30  // tweak this — positive moves UP, negative moves DOWN
#define SHLD_AURA_W  90    // width of side shield
#define SHLD_AURA_H  240    // height of side shield
#define SHLD_AURA_X_DIST -50 // how far RIGHT of player the aura sits (tweak freely)
#define SHLD_AURA_Y_OFF  320  // vertical offset of aura relative to player Y (tweak freely)
// ── Boss Lv3 sprite counts ───────────────────────────────────
#define LV3BOSS_WALK_FRAMES   26
#define LV3BOSS_JUMP_FRAMES   32
#define LV3BOSS_FIRE_FRAMES   64
#define LV3BOSS_SLASH_FRAMES  53
int lv3BossDeadImg = -1;
// ── Boss Lv3 display size — tweak freely ────────────────────
#define LV3BOSS_W   310
#define LV3BOSS_H   440
#define LV3BOSS_YOFF  -10      // vertical offset above bridge snap-Y
#define LV3BOSS_LIVES_TO_KILL 5   // display lives (20 hits / 4 per life)
// ── Boss Lv3 fire-hazard hitbox (fraction of sprite) ─────────
// The "fire" region is the horizontal band the player must shield against.
// Tweak these 0-1 ratios to match the actual flame pixels in the sprite.
#define LV3BOSS_FIRE_HB_LEFT   0.15f
#define LV3BOSS_FIRE_HB_RIGHT  0.80f
#define LV3BOSS_FIRE_HB_TOP    0.30f
#define LV3BOSS_FIRE_HB_BOTTOM 0.70f

// ── Boss Lv3 slash-hazard: only frames 26-53 are lethal ──────
#define LV3BOSS_SLASH_LETHAL_START 25   // 0-based index of slash frame 26
#define LV3BOSS_SLASH_LETHAL_END   52   // 0-based index of slash frame 53

// ── Hits required to kill boss ───────────────────────────────
#define LV3BOSS_HITS_TO_KILL  20   // 10 charged-punch / chp hits

// ── Player damage cap before death ───────────────────────────
#define LV3BOSS_MAX_PLAYER_HITS 5  // 5 touches → game over

// ── Proximity to trigger fire (pixels from boss left edge) ───
#define LV3BOSS_TRIGGER_DIST  10

// ── Image arrays ─────────────────────────────────────────────
int lv3BossWalkFrames[LV3BOSS_WALK_FRAMES];
int lv3BossJumpFrames[LV3BOSS_JUMP_FRAMES];
int lv3BossFireFrames[LV3BOSS_FIRE_FRAMES];
int lv3BossSlashFrames[LV3BOSS_SLASH_FRAMES];

enum Lv3BossState {
	LV3BOSS_WALKING,   // idle walk loop on bridge 10
	LV3BOSS_FIRE,      // fire attack (loops 3×)
	LV3BOSS_JUMP,      // jump animation (once)
	LV3BOSS_SLASH      // slash animation (once)
};

Lv3BossState lv3BossState = LV3BOSS_WALKING;
int  lv3BossFrameIndex = 0;
int  lv3BossFireLoopCount = 0;   // counts completed fire loops (0-2)
bool lv3BossActive = false; // true once player enters bridge 10
bool lv3BossDead = false;
int  lv3BossHitCount = 0;   // charged-punch hits received
int  lv3BossPlayerHitCount = 0;   // times player was hurt by boss
float lv3BossX = 0.f; // world X — set in initBackground
int  lv3BossSlashCycleCount = 0;   // how many jump+slash cycles done

// Cooldown so one continuous overlap doesn't drain multiple lives at once
static int lv3BossHitCooldown = 0;

// ── Level 3 Charged Punch Power-up Overlay ────────────────────────
int chpFrames[7];    // chp1.gif … chp7.gif  (plays when chargedpunchm active)
int chpFramesM[7];   // chp1m.gif … chp7m.gif (plays when chargedpunch active)
int chpFrameIndex = 0;
bool chpAnimPlaying = false;
bool chpWasFrontActive = false;   // tracks previous chargedpunchm state
bool chpWasMActive = false;       // tracks previous chargedpunch state

#define CHP_W  150   // width of overlay — tweak freely
#define CHP_H  160   // height of overlay — tweak freely
#define CHP_X_OFF  -30  // horizontal offset from player — tweak freely
#define CHP_Y_OFF  75  // vertical offset from player Y — tweak freely
#define CHP_THROW_DIST  200  // total distance thrown — tweak freely

// ── Level 3 Crouch & Kick ─────────────────────────────────────
bool lv3Crouching = false;
int  crouchImg = -1;   // facing right
int  crouchImgM = -1;   // facing left

#define CROUCH_W    290    // ← tweak freely
#define CROUCH_H    290    // ← tweak freely
#define CROUCH_Y_OFF  -40  // ← negative = move DOWN toward bridge

bool playerWonLv3 = false;
int winnerLv3Img;
// ── Level 3 Spike Obstacle ─────────────────────────────────────
#define SPIKE_FRAMECOUNT 34
#define SPIKE_W  70    // display width  — tweak freely
#define SPIKE_H  190     // display height — tweak freely
#define SPIKE_Y_OFF  0  // vertical offset above bridge snapY — tweak freely
#define SPIKE0_X_OFF  -20    // Bridge 4 left edge  — tweak freely
#define SPIKE1_X_OFF  +20    // Bridge 4 right edge — tweak freely
#define SPIKE2_X_OFF  -20    // Bridge 8 left edge  — tweak freely
#define SPIKE3_X_OFF  +20   // Bridge 8 right edge — tweak freely
// Hitbox crop ratios
#define SPIKE_HB_LEFT   0.40f
#define SPIKE_HB_RIGHT  0.70f
#define SPIKE_HB_TOP    0.10f
#define SPIKE_HB_BOTTOM 0.90f

#define SPIKE_KNEEL_DURATION 40  // ticks before kneel releases

int spikeFrames[SPIKE_FRAMECOUNT];
int spikeAnimIndex = 0;

struct SpikeObstacle {
	float x;
	float y;
	bool playerTouching;
	bool kneeling;
	int  kneelTimer;
};

#define NUM_SPIKES_LV3 4
SpikeObstacle spikesLv3[NUM_SPIKES_LV3];
// ── Level 3 Power-up Tokens ──────────────────────────────────────
#define PUPTK_FRAMECOUNT  8
#define PUPTK_W  90     // display width  — tweak freely
#define PUPTK_H  120     // display height — tweak freely
#define PUPTK_Y_OFF  50 // height above bridge snapY — tweak freely

// ── Level 3 Bird Enemy (Bridge 2) ─────────────────────────────────────
#define LV3BIRD_FRAMECOUNT   8
#define LV3BIRD_BASEW        90    // ← tweak freely
#define LV3BIRD_BASEH        80     // ← tweak freely
#define LV3BIRD_SCALE        1.8f   // ← tweak freely
#define LV3BIRD_W            (int)(LV3BIRD_BASEW * LV3BIRD_SCALE)
#define LV3BIRD_H            (int)(LV3BIRD_BASEH * LV3BIRD_SCALE)
#define LV3BIRD_Y_OFF        75     // ← height above bridge snapY — tweak freely
#define LV3BIRD_SPEED        7.0f   // ← pixels per tick — tweak freely
#define LV3BIRD_HITS_TO_KILL 2      // normal punches to kill

// Hitbox crop ratios — tweak freely
#define LV3BIRD_HB_LEFT   0.30f
#define LV3BIRD_HB_RIGHT  0.72f
#define LV3BIRD_HB_TOP    0.25f
#define LV3BIRD_HB_BOTTOM 0.75f

// ── Level 3 Bird Enemy 2 (second bird, delayed spawn) ─────────────────────
// ▸ To change the delay before bird2 starts: edit LV3BIRD2_SPAWN_DELAY (in ticks at 40ms each)
//   e.g. 75 ticks × 40ms = 3 seconds
#define LV3BIRD2_SPAWN_DELAY  45    // ← TWEAK: ticks after bird1 activates before bird2 spawns

#define LV3BIRD2_W   LV3BIRD_W     // ← TWEAK: width  (currently mirrors bird1)
#define LV3BIRD2_H   LV3BIRD_H     // ← TWEAK: height (currently mirrors bird1)

#define LV3BIRD2_SPEED  LV3BIRD_SPEED   // ← TWEAK: pixels per tick

#define LV3BIRD2_Y_OFF  LV3BIRD_Y_OFF   // ← TWEAK: height above bridge snapY

// ▸ Hitbox crop ratios for bird2 — tweak independently if needed
#define LV3BIRD2_HB_LEFT    LV3BIRD_HB_LEFT
#define LV3BIRD2_HB_RIGHT   LV3BIRD_HB_RIGHT
#define LV3BIRD2_HB_TOP     LV3BIRD_HB_TOP
#define LV3BIRD2_HB_BOTTOM  LV3BIRD_HB_BOTTOM

bool  lv3Bird2Alive = true;
bool  lv3Bird2Active = false;
int   lv3Bird2HitCount = 0;
bool  lv3Bird2Falling = false;
float lv3Bird2X = 0.f;
float lv3Bird2DeadY = 0.f;
float lv3Bird2DeadVel = 0.f;
int   lv3Bird2FrameIndex = 0;
static bool lv3Bird2WasColliding = false;
static int  lv3Bird2SpawnTimer = 0;   // counts up after bird1 activates

int puptkFrames[PUPTK_FRAMECOUNT];
int puptkAnimIndex = 0;

struct PowerUpToken {
	float x, y;
	bool collected;
};

#define NUM_PUPTK_LV3 2
PowerUpToken puptkLv3[NUM_PUPTK_LV3];

// ── Power-up token pickup text ──
bool puptkShowText[NUM_PUPTK_LV3] = { false, false };
int  puptkTextTimer[NUM_PUPTK_LV3] = { 0, 0 };
#define PUPTK_TEXT_DURATION 60  // ~1 second at 16ms ticks

// ── Level 3 Bird Enemy globals ────────────────────────────────────────
int   lv3BirdFrames[LV3BIRD_FRAMECOUNT];
int   lv3BirdDeadImg = -1;
int   lv3BirdFrameIndex = 0;
bool  lv3BirdAlive = true;
bool  lv3BirdActive = false;   // activates when player steps on Bridge 2
int   lv3BirdHitCount = 0;
bool  lv3BirdFalling = false;
float lv3BirdX = 0.f;    // world X — set in initBackground
float lv3BirdDeadY = 0.f;
float lv3BirdDeadVel = 0.f;
static bool lv3BirdWasColliding = false;

// ═══════════════════════════════════════════════════════════════
//  LEVEL 3  ENEMY 1  (Bridge 2, right edge)
// ═══════════════════════════════════════════════════════════════

// ── Frame counts ─────────────────────────────────────────────
#define LV3EN1_IDLE_FRAMES   42
#define LV3EN1_WALK_FRAMES   50
#define LV3EN1_HIT_FRAMES    22
#define LV3EN1_DATK_FRAMES   39

// ── Display size — tweak freely ──────────────────────────────
#define LV3EN1_W             140    // ← display width
#define LV3EN1_H             160    // ← display height
#define LV3EN1_Y_OFF         -15      // ← vertical offset above bridge snapY

// ── Behaviour tweaks ─────────────────────────────────────────
#define LV3EN1_WALK_SPEED    2.5f   // ← pixels per tick toward player
#define LV3EN1_ATTACK_DIST   90     // ← world-pixel distance to start attacking
#define LV3EN1_ACTIVATE_OFFSET  (-80)   // ← negative = left of midpoint; adjust freely 

// ── Kill requirements ────────────────────────────────────────
#define LV3EN1_NORMAL_HITS   6      // normal punches to kill
#define LV3EN1_CHARGED_HITS  2      // charged punches to kill

// ── Animation speeds (ticks per frame advance) ───────────────
#define LV3EN1_IDLE_SPEED    1      // ← lower = faster
#define LV3EN1_WALK_SPEED_A  1      // ← animation speed for walk
#define LV3EN1_HIT_SPEED     1      // ← animation speed for hit reaction
#define LV3EN1_DATK_SPEED    1      // ← animation speed for attack

// Body hitbox — currently BROKEN (left > right, bottom too small)
#define LV3EN1_HB_LEFT       0.55f  // was 0.90f — start 65% from left
#define LV3EN1_HB_RIGHT      1.25f  // was 0.130f — end 95% from left
#define LV3EN1_HB_TOP        0.10f  // was 0.20f
#define LV3EN1_HB_BOTTOM     0.90f  // this one was fine

// Attack hitbox — BOTTOM was 0.120f = only 12% down (tiny!)
#define LV3EN1_ATK_HB_LEFT   0.15f  // fine
#define LV3EN1_ATK_HB_RIGHT  0.95f  // fine
#define LV3EN1_ATK_HB_TOP    0.10f
#define LV3EN1_ATK_HB_BOTTOM 0.90f  // was 0.120f — needs to be > TOP

#define LV3EN1_LETHAL_FRAME  13     

// ── State enum ───────────────────────────────────────────────
enum Lv3En1State {
	LV3EN1_IDLE,
	LV3EN1_WALKING,
	LV3EN1_ATTACKING,
	LV3EN1_HIT_REACT
};

// ── Image arrays ─────────────────────────────────────────────
int lv3En1IdleFrames[LV3EN1_IDLE_FRAMES];
int lv3En1WalkFrames[LV3EN1_WALK_FRAMES];
int lv3En1HitFrames[LV3EN1_HIT_FRAMES];
int lv3En1DatkFrames[LV3EN1_DATK_FRAMES];
int lv3En1DeadImg = -1;

// ── Runtime state ────────────────────────────────────────────
Lv3En1State lv3En1State = LV3EN1_IDLE;
float       lv3En1X = 0.f;     // world X — set in initBackground
bool        lv3En1Alive = true;
bool        lv3En1Active = false;   // true once player hits bridge2 midpoint
int         lv3En1NormalHits = 0;
int         lv3En1ChargedHits = 0;
bool        lv3En1Dead = false;
float       lv3En1DeadY = 0.f;
float       lv3En1DeadVel = 0.f;
bool        lv3En1Falling = false;

int  lv3En1FrameIndex = 0;
int  lv3En1AnimTick = 0;
bool lv3En1WasColliding = false;    // rising-edge guard for player damage
bool lv3En1WasPunching = false;    // rising-edge guard for hit-react
static int  lv3En1HitReactDone = 0; // counts completed hit-react cycles

// ── Per-state display sizes — tweak each independently ───────────
#define LV3EN1_IDLE_W     200    // ← idle sprite display width
#define LV3EN1_IDLE_H     220    // ← idle sprite display height
#define LV3EN1_WALK_W     200    // ← walk sprite display width
#define LV3EN1_WALK_H     220    // ← walk sprite display height
#define LV3EN1_HIT_W      218    // ← hit-react sprite display width
#define LV3EN1_HIT_H      175    // ← hit-react sprite display height
#define LV3EN1_DATK_W     320    // ← attack sprite display width
#define LV3EN1_DATK_H     340    // ← attack sprite display height

#define LV3EN1_HIT_X_OFF   80    // ← tweak this freely (positive = right, negative = left)
#define LV3EN1_HIT_Y_OFF   25    // ← tweak freely
#define LV3EN1_DEAD_W     160    // ← tweak freely
#define LV3EN1_DEAD_H     140    // ← tweak freely
#define LV3EN1_DATK_X_OFF -53     // ← tweak freely: shifts attack sprite left/right
#define LV3EN1_DATK_Y_OFF   0     // ← tweak freely: shifts attack sprite up/down

// ═══════════════════════════════════════════════════════════════
//  LEVEL 3  ENEMY 2  (Bridge 2, spawns from LEFT after En1 dies)
//  Mirrors En1 but faces RIGHT (running left→right toward player)
// ═══════════════════════════════════════════════════════════════

#define LV3EN2_IDLE_FRAMES   42
#define LV3EN2_RUN_FRAMES    22   // lvl3en1run1right … lvl3en1run22right
#define LV3EN2_HIT_FRAMES    22
#define LV3EN2_DATK_FRAMES   39

// ── Display sizes — tweak freely ─────────────────────────────
#define LV3EN2_IDLE_W     200
#define LV3EN2_IDLE_H     220
#define LV3EN2_RUN_W      200
#define LV3EN2_RUN_H      200
#define LV3EN2_HIT_W      218
#define LV3EN2_HIT_H      175
#define LV3EN2_DATK_W     317
#define LV3EN2_DATK_H     327
#define LV3EN2_DEAD_W     160
#define LV3EN2_DEAD_H     140

#define LV3EN2_Y_OFF         -15    // ← vertical offset above bridge snapY

// ── Behaviour tweaks ─────────────────────────────────────────
#define LV3EN2_RUN_SPEED     3.5f   // ← faster than En1's walk (2.5f)
#define LV3EN2_ATTACK_DIST   90     // ← slightly less than En1 (90)
#define LV3EN2_KILL_NORMAL   6      // normal punches to kill
#define LV3EN2_KILL_CHARGED  2      // charged punches to kill

// ── Animation speeds (ticks per frame advance) ───────────────
#define LV3EN2_IDLE_SPEED    1
#define LV3EN2_RUN_SPEED_A   1      // ← lower = faster run anim
#define LV3EN2_HIT_SPEED     1
#define LV3EN2_DATK_SPEED    1

// ── Body hitbox (fraction of sprite W/H) ─────────────────────
// Enemy faces RIGHT so the visible body is on the LEFT side of sprite
#define LV3EN2_HB_LEFT       0.05f  // ← tweak freely
#define LV3EN2_HB_RIGHT      0.47f  // ← tweak freely
#define LV3EN2_HB_TOP        0.10f
#define LV3EN2_HB_BOTTOM     0.90f

// ── Attack hitbox (lethal frame only) ────────────────────────
#define LV3EN2_ATK_HB_LEFT   0.05f
#define LV3EN2_ATK_HB_RIGHT  0.75f
#define LV3EN2_ATK_HB_TOP    0.10f
#define LV3EN2_ATK_HB_BOTTOM 0.90f
#define LV3EN2_LETHAL_FRAME  13     // 0-based index of datk14

// ── Hit sprite offsets ────────────────────────────────────────
#define LV3EN2_HIT_X_OFF   -80     // ← shift left (mirrored from En1's +80)
#define LV3EN2_HIT_Y_OFF    25
#define LV3EN2_RUN_Y_OFF    20
#define LV3EN2_DATK_X_OFF   -150    // ← tweak freely: shifts attack sprite left/right independently

enum Lv3En2State {
	LV3EN2_IDLE,
	LV3EN2_RUNNING,
	LV3EN2_ATTACKING,
	LV3EN2_HIT_REACT
};

// ── Image arrays ─────────────────────────────────────────────
int lv3En2IdleFrames[LV3EN2_IDLE_FRAMES];
int lv3En2RunFrames[LV3EN2_RUN_FRAMES];
int lv3En2HitFrames[LV3EN2_HIT_FRAMES];
int lv3En2DatkFrames[LV3EN2_DATK_FRAMES];
int lv3En2DeadImg = -1;

// ── Runtime state ────────────────────────────────────────────
Lv3En2State lv3En2State = LV3EN2_IDLE;
float       lv3En2X = 0.f;
bool        lv3En2Alive = true;
bool        lv3En2Active = false;   // true once En1 is dead
int         lv3En2NormalHits = 0;
int         lv3En2ChargedHits = 0;
bool        lv3En2Dead = false;
float       lv3En2DeadY = 0.f;
float       lv3En2DeadVel = 0.f;
bool        lv3En2Falling = false;

int  lv3En2FrameIndex = 0;
int  lv3En2AnimTick = 0;
bool lv3En2WasColliding = false;
bool lv3En2WasPunching = false;

// ═══════════════════════════════════════════════════════════════
//  LEVEL 3  ENEMY 3  (Bridge 6, right edge)
//  Mirrors En1/En2 pattern but on Bridge 6 (bridges3[5])
// ═══════════════════════════════════════════════════════════════

// ── Frame counts ──────────────────────────────────────────────
#define LV3EN3_IDLE_FRAMES   42
#define LV3EN3_WALK_FRAMES   50
#define LV3EN3_HIT_FRAMES    22
#define LV3EN3_DATK_FRAMES   39

// ── Per-state display sizes — tweak each independently ────────
#define LV3EN3_IDLE_W        200    // ← idle sprite display width
#define LV3EN3_IDLE_H        220    // ← idle sprite display height
#define LV3EN3_WALK_W        200    // ← walk sprite display width
#define LV3EN3_WALK_H        220    // ← walk sprite display height
#define LV3EN3_HIT_W         218    // ← hit-react sprite display width
#define LV3EN3_HIT_H         175    // ← hit-react sprite display height
#define LV3EN3_DATK_W        320    // ← attack sprite display width
#define LV3EN3_DATK_H        340    // ← attack sprite display height
#define LV3EN3_DEAD_W        160    // ← dead body display width
#define LV3EN3_DEAD_H        140    // ← dead body display height

// ── Vertical offset above bridge snapY ────────────────────────
#define LV3EN3_Y_OFF         -25    // ← tweak freely

// ── Hit/dead sprite offsets ───────────────────────────────────
#define LV3EN3_HIT_X_OFF      80    // ← shift hit sprite right (tweak freely)
#define LV3EN3_HIT_Y_OFF      25    // ← shift hit sprite up    (tweak freely)

// ── Behaviour tweaks ──────────────────────────────────────────
#define LV3EN3_WALK_SPEED     2.5f  // ← pixels per tick toward player
#define LV3EN3_ATTACK_DIST    90    // ← world-pixel distance to start attacking

// ── Activation: player must reach this X offset from bridge6 left edge ──
// Negative = left of bridge midpoint (e.g. -80 means 80px left of center)
#define LV3EN3_ACTIVATE_OFFSET  (-80)   // ← tweak freely

// ── Kill requirements ─────────────────────────────────────────
#define LV3EN3_NORMAL_HITS    6     // ← normal punches to kill
#define LV3EN3_CHARGED_HITS   2     // ← charged punches to kill

// ── Animation speeds (ticks per frame advance) — lower = faster ──
#define LV3EN3_IDLE_SPEED     1     // ← idle anim speed
#define LV3EN3_WALK_SPEED_A   1     // ← walk anim speed
#define LV3EN3_HIT_SPEED      1     // ← hit-react anim speed
#define LV3EN3_DATK_SPEED     1     // ← attack anim speed

// ── Body hitbox (fraction of sprite W/H) ──────────────────────
#define LV3EN3_HB_LEFT        0.25f
#define LV3EN3_HB_RIGHT       0.95f
#define LV3EN3_HB_TOP         0.10f
#define LV3EN3_HB_BOTTOM      0.90f

// ── Attack hitbox — active ONLY on datk frame 14 (0-based: 13) ──
#define LV3EN3_ATK_HB_LEFT    0.05f
#define LV3EN3_ATK_HB_RIGHT   0.95f
#define LV3EN3_ATK_HB_TOP     0.10f
#define LV3EN3_ATK_HB_BOTTOM  0.90f
#define LV3EN3_LETHAL_FRAME   13    // ← 0-based index of datk14 (tweak if needed)

#define LV3EN3_DATK_X_OFF   -80    // ← tweak freely: shifts attack sprite left/right
#define LV3EN3_DATK_Y_OFF    0     // ← tweak freely: shifts attack sprite up/down

// ── Debug hitbox draw toggle ──────────────────────────────────
#define LV3EN3_DRAW_DEBUG     true  // ← set false to hide hitboxes

// ── State enum ───────────────────────────────────────────────
enum Lv3En3State {
	LV3EN3_IDLE_STATE,
	LV3EN3_WALKING,
	LV3EN3_ATTACKING,
	LV3EN3_HIT_REACT
};

// ── Image arrays ─────────────────────────────────────────────
int lv3En3IdleFrames[LV3EN3_IDLE_FRAMES];
int lv3En3WalkFrames[LV3EN3_WALK_FRAMES];
int lv3En3HitFrames[LV3EN3_HIT_FRAMES];
int lv3En3DatkFrames[LV3EN3_DATK_FRAMES];
int lv3En3DeadImg = -1;

// ── Runtime state ────────────────────────────────────────────
Lv3En3State lv3En3State = LV3EN3_IDLE_STATE;
float       lv3En3X = 0.f;   // world X — set in initBackground
bool        lv3En3Alive = true;
bool        lv3En3Active = false;  // true once player hits bridge6 midpoint
int         lv3En3NormalHits = 0;
int         lv3En3ChargedHits = 0;
bool        lv3En3Dead = false;
float       lv3En3DeadY = 0.f;
float       lv3En3DeadVel = 0.f;
bool        lv3En3Falling = false;

int  lv3En3FrameIndex = 0;
int  lv3En3AnimTick = 0;
bool lv3En3WasColliding = false;
bool lv3En3WasPunching = false;

// ═══════════════════════════════════════════════════════════════
//  LEVEL 3  ENEMY 4  (Bridge 6, spawns from LEFT after En3 dies)
// ═══════════════════════════════════════════════════════════════

#define LV3EN4_IDLE_FRAMES   42
#define LV3EN4_RUN_FRAMES    22
#define LV3EN4_HIT_FRAMES    22
#define LV3EN4_DATK_FRAMES   39

#define LV3EN4_IDLE_W     200
#define LV3EN4_IDLE_H     220
#define LV3EN4_RUN_W      200
#define LV3EN4_RUN_H      200
#define LV3EN4_HIT_W      218
#define LV3EN4_HIT_H      175
#define LV3EN4_DATK_W     317
#define LV3EN4_DATK_H     337
#define LV3EN4_DEAD_W     160
#define LV3EN4_DEAD_H     140

#define LV3EN4_Y_OFF         -5
#define LV3EN4_HIT_X_OFF   -105
#define LV3EN4_HIT_Y_OFF    5
#define LV3EN4_DATK_X_OFF  -110
#define LV3EN4_DATK_Y_OFF  -17

#define LV3EN4_RUN_SPEED     3.5f
#define LV3EN4_ATTACK_DIST   17
#define LV3EN4_KILL_NORMAL   6
#define LV3EN4_KILL_CHARGED  2

#define LV3EN4_IDLE_SPEED    1
#define LV3EN4_RUN_SPEED_A   1
#define LV3EN4_HIT_SPEED     1
#define LV3EN4_DATK_SPEED    1

#define LV3EN4_HB_LEFT       -0.10f
#define LV3EN4_HB_RIGHT      0.30f
#define LV3EN4_HB_TOP        0.10f
#define LV3EN4_HB_BOTTOM     0.90f

#define LV3EN4_ATK_HB_LEFT   -0.05f
#define LV3EN4_ATK_HB_RIGHT  0.38f
#define LV3EN4_ATK_HB_TOP    0.10f
#define LV3EN4_ATK_HB_BOTTOM 0.90f
#define LV3EN4_LETHAL_FRAME  13

enum Lv3En4State {
	LV3EN4_IDLE,
	LV3EN4_RUNNING,
	LV3EN4_ATTACKING,
	LV3EN4_HIT_REACT
};

int lv3En4IdleFrames[LV3EN4_IDLE_FRAMES];
int lv3En4RunFrames[LV3EN4_RUN_FRAMES];
int lv3En4HitFrames[LV3EN4_HIT_FRAMES];
int lv3En4DatkFrames[LV3EN4_DATK_FRAMES];
int lv3En4DeadImg = -1;

Lv3En4State lv3En4State = LV3EN4_IDLE;
float       lv3En4X = 0.f;
bool        lv3En4Alive = true;
bool        lv3En4Active = false;
int         lv3En4NormalHits = 0;
int         lv3En4ChargedHits = 0;
bool        lv3En4Dead = false;
float       lv3En4DeadY = 0.f;
float       lv3En4DeadVel = 0.f;
bool        lv3En4Falling = false;
int         lv3En4FrameIndex = 0;
int         lv3En4AnimTick = 0;
bool        lv3En4WasColliding = false;
bool        lv3En4WasPunching = false;

// ═══════════════════════════════════════════════════════════════
//  LEVEL 3  WK ENEMIES  (Bridge 6 left edge, stationary pair)
//  Reuses lvl2wkenemy sprites (wken1.png … wken8.png)
// ═══════════════════════════════════════════════════════════════
#define LV3WK_FRAMECOUNT     8
#define LV3WK_BASEW          150    // ← same as LV2WK_BASEW
#define LV3WK_BASEH          90     // ← same as LV2WK_BASEH
#define LV3WK_SCALE          1.60f  // ← same as LV2WK_SCALE
#define LV3WK_WIDTH          (int)(LV3WK_BASEW * LV3WK_SCALE)
#define LV3WK_HEIGHT         (int)(LV3WK_BASEH * LV3WK_SCALE)

// Placement on Bridge 6 (bridges3[5]) — tweak freely
#define LV3WK0_X_OFF         0     // ← offset from bridge left edge — tweak freely
#define LV3WK_Y_OFF          -10    // ← vertical offset above bridge snapY

// Hitbox crop ratios — tweak freely
#define LV3WK_HB_LEFT        0.49f
#define LV3WK_HB_RIGHT       0.60f
#define LV3WK_HB_TOP         0.20f
#define LV3WK_HB_BOTTOM      0.85f

// Kneel duration after contact (ticks)
#define LV3WK_KNEEL_DURATION 40     // ← tweak freely

// Debug hitbox draw toggle
#define LV3WK_DRAW_DEBUG     false   // ← set false to hide debug boxes

// ── Level 3 WK Enemy pair (Bridge 6 left edge) ───────────────────────────────
int   lv3WkFrames[LV3WK_FRAMECOUNT];
int   lv3WkFrameIndex = 0;
bool  lv3WkAlive = true;
float lv3WkX = 0.f;   // set in initBackground
float lv3WkY = 0.f;   // set in initBackground
bool  lv3WkPlayerTouching = false;
bool  lv3WkKneeling = false;
int   lv3WkKneelTimer = 0;

// ── Dead body fade-out (Bridge 6) ─────────────────────────────────────────
#define LV3EN3_FADE_DURATION  150    // ← ticks before En3 body disappears — tweak freely
#define LV3EN4_FADE_DURATION  150    // ← ticks before En4 body disappears — tweak freely

int  lv3En3FadeTick = 0;          // counts up after death
bool lv3En3FadeDone = false;      // true = stop drawing body
int  lv3En4FadeTick = 0;
bool lv3En4FadeDone = false;

// ═══════════════════════════════════════════════════════════════
//  LEVEL 3 ENEMY 5 (Bridge 6 right edge)
// ═══════════════════════════════════════════════════════════════

#define LV3EN5_APR_FRAMES     5
#define LV3EN5_IDLE_FRAMES   11
#define LV3EN5_RUN_FRAMES    12
#define LV3EN5_PUNCH_FRAMES  15
#define LV3EN5_HIT_FRAMES     7
#define LV3EN5_REVIVE_FRAMES  8
#define LV3EN5_DEAD_FRAMES    8

// Landing position on Bridge 6 (bridges3[5])
#define LV3EN5_LAND_X_OFF    -200   // world X = bridges3[5].maxX + this
#define LV3EN5_LAND_Y_OFF      -20    // world Y = bridges3[5].snapY + this
#define LV3EN5_DROP_SPEED     18.f  // pixels per tick falling down

// Attack distance: how close before switching from run to punch
#define LV3EN5_ATTACK_DIST   100
#define LV3EN5_RUN_SPEED     3.0f

// Kill requirements per cycle
#define LV3EN5_NORMAL_PER_CYCLE   6
#define LV3EN5_CHARGED_PER_CYCLE  3

// Per-state display sizes
#define LV3EN5_APR_W         245
#define LV3EN5_APR_H         355
#define LV3EN5_APR_X_OFF       0
#define LV3EN5_APR_Y_OFF     -20

#define LV3EN5_IDLE_W        235
#define LV3EN5_IDLE_H        295
#define LV3EN5_IDLE_X_OFF      0
#define LV3EN5_IDLE_Y_OFF    -10

#define LV3EN5_RUN_W         210
#define LV3EN5_RUN_H         265
#define LV3EN5_RUN_X_OFF       0
#define LV3EN5_RUN_Y_OFF     -20

#define LV3EN5_PUNCH_W       285
#define LV3EN5_PUNCH_H       305
#define LV3EN5_PUNCH_X_OFF   -30
#define LV3EN5_PUNCH_Y_OFF   -20

#define LV3EN5_HIT_W         205
#define LV3EN5_HIT_H         265
#define LV3EN5_HIT_X_OFF      80
#define LV3EN5_HIT_Y_OFF     -30

#define LV3EN5_REVIVE_W      203
#define LV3EN5_REVIVE_H      257
#define LV3EN5_REVIVE_X_OFF    80
#define LV3EN5_REVIVE_Y_OFF  -30

#define LV3EN5_DEAD_W        280
#define LV3EN5_DEAD_H        285
#define LV3EN5_DEAD_X_OFF     35
#define LV3EN5_DEAD_Y_OFF      0

// Body hitbox (fraction of IDLE sprite)
#define LV3EN5_HB_LEFT    0.43f
#define LV3EN5_HB_RIGHT   0.80f
#define LV3EN5_HB_TOP     0.08f
#define LV3EN5_HB_BOTTOM  0.92f

// Attack hitbox (fraction of PUNCH sprite) — active only on frames 12 and 13
#define LV3EN5_ATK_HB_LEFT    0.17f
#define LV3EN5_ATK_HB_RIGHT   0.70f
#define LV3EN5_ATK_HB_TOP     0.25f
#define LV3EN5_ATK_HB_BOTTOM  0.75f
#define LV3EN5_LETHAL_FRAME_A 11    // 0-based index of punch12
#define LV3EN5_LETHAL_FRAME_B 12    // 0-based index of punch13

// Animation speeds (ticks per frame) — lower = faster
#define LV3EN5_APR_SPEED      12    // was 2 — apr drop animation
#define LV3EN5_IDLE_SPEED     6    // was 2 — idle loop
#define LV3EN5_RUN_SPEED_A    4    // was 1 — running
#define LV3EN5_PUNCH_SPEED    4    // was 1 — punch
#define LV3EN5_HIT_SPEED      5    // was 1 — hit react
#define LV3EN5_REVIVE_SPEED   4    // was 1 — revive
#define LV3EN5_DEAD_SPEED     6    // was 2 — death animation

#define LV3EN5_DRAW_DEBUG     false

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

int lv3En5AprFrames[LV3EN5_APR_FRAMES];
int lv3En5IdleFrames[LV3EN5_IDLE_FRAMES];
int lv3En5RunFrames[LV3EN5_RUN_FRAMES];
int lv3En5PunchFrames[LV3EN5_PUNCH_FRAMES];
int lv3En5HitFrames[LV3EN5_HIT_FRAMES];
int lv3En5ReviveFrames[LV3EN5_REVIVE_FRAMES];
int lv3En5DeadFrames[LV3EN5_DEAD_FRAMES];

Lv3En5State lv3En5State = LV3EN5_WAITING;
float       lv3En5X = 0.f;
float       lv3En5Y = 0.f;
int         lv3En5FrameIndex = 0;
int         lv3En5AnimTick = 0;
int         lv3En5NormalHits = 0;
int         lv3En5ChargedHits = 0;
int         lv3En5ReviveCount = 0;
bool        lv3En5WasColliding = false;
bool        lv3En5WasPunching = false;
bool        lv3En5En4HitTriggered = false;

bool lv3En5HitFreeze = false;   // true while frozen after hit animation
int  lv3En5HitFreezeTimer = 0;       // counts ticks during freeze

// ═══════════════════════════════════════════════════════════════
//  LEVEL 3  ENEMY 6  (Bridge 6 right edge, faces LEFT/right-sprites)
//  Mirror of En5 — same logic, different sprite folder
// ═══════════════════════════════════════════════════════════════

#define LV3EN6_APR_FRAMES     5
#define LV3EN6_IDLE_FRAMES   11
#define LV3EN6_RUN_FRAMES    12
#define LV3EN6_PUNCH_FRAMES  15
#define LV3EN6_HIT_FRAMES     7
#define LV3EN6_REVIVE_FRAMES  8
#define LV3EN6_DEAD_FRAMES    8

// Landing position on Bridge 6 (bridges3[5])
#define LV3EN6_LAND_X_OFF    +5   // offset from bridge LEFT edge — tweak freely
#define LV3EN6_LAND_Y_OFF      -20
#define LV3EN6_DROP_SPEED     18.f

#define LV3EN6_ATTACK_DIST   100
#define LV3EN6_RUN_SPEED     3.0f

// Kill requirements per cycle
#define LV3EN6_NORMAL_PER_CYCLE   6
#define LV3EN6_CHARGED_PER_CYCLE  3

// Per-state display sizes — tweak each independently
#define LV3EN6_APR_W         245
#define LV3EN6_APR_H         355
#define LV3EN6_APR_X_OFF       0
#define LV3EN6_APR_Y_OFF     -20

#define LV3EN6_IDLE_W        235
#define LV3EN6_IDLE_H        295
#define LV3EN6_IDLE_X_OFF      0
#define LV3EN6_IDLE_Y_OFF    -10

#define LV3EN6_RUN_W         210
#define LV3EN6_RUN_H         265
#define LV3EN6_RUN_X_OFF       0
#define LV3EN6_RUN_Y_OFF     -20

#define LV3EN6_PUNCH_W       285
#define LV3EN6_PUNCH_H       305
#define LV3EN6_PUNCH_X_OFF   -30
#define LV3EN6_PUNCH_Y_OFF   -20

#define LV3EN6_HIT_W         205
#define LV3EN6_HIT_H         265
#define LV3EN6_HIT_X_OFF      25
#define LV3EN6_HIT_Y_OFF     -30

#define LV3EN6_REVIVE_W      203
#define LV3EN6_REVIVE_H      257
#define LV3EN6_REVIVE_X_OFF    25
#define LV3EN6_REVIVE_Y_OFF  -30

#define LV3EN6_DEAD_W        280
#define LV3EN6_DEAD_H        285
#define LV3EN6_DEAD_X_OFF     35
#define LV3EN6_DEAD_Y_OFF      0

// Body hitbox (fraction of IDLE sprite)
#define LV3EN6_HB_LEFT    0.10f
#define LV3EN6_HB_RIGHT   0.43f
#define LV3EN6_HB_TOP     0.08f
#define LV3EN6_HB_BOTTOM  0.92f

// Attack hitbox (fraction of PUNCH sprite) — active on frames 11 and 12
#define LV3EN6_ATK_HB_LEFT    0.17f
#define LV3EN6_ATK_HB_RIGHT   0.70f
#define LV3EN6_ATK_HB_TOP     0.25f
#define LV3EN6_ATK_HB_BOTTOM  0.75f
#define LV3EN6_LETHAL_FRAME_A 11
#define LV3EN6_LETHAL_FRAME_B 12

// Animation speeds (ticks per frame) — lower = faster
#define LV3EN6_APR_SPEED      12
#define LV3EN6_IDLE_SPEED      6
#define LV3EN6_RUN_SPEED_A     4
#define LV3EN6_PUNCH_SPEED     4
#define LV3EN6_HIT_SPEED       5
#define LV3EN6_REVIVE_SPEED    4
#define LV3EN6_DEAD_SPEED      6

#define LV3EN6_DRAW_DEBUG     false

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

int lv3En6AprFrames[LV3EN6_APR_FRAMES];
int lv3En6IdleFrames[LV3EN6_IDLE_FRAMES];
int lv3En6RunFrames[LV3EN6_RUN_FRAMES];
int lv3En6PunchFrames[LV3EN6_PUNCH_FRAMES];
int lv3En6HitFrames[LV3EN6_HIT_FRAMES];
int lv3En6ReviveFrames[LV3EN6_REVIVE_FRAMES];
int lv3En6DeadFrames[LV3EN6_DEAD_FRAMES];

Lv3En6State lv3En6State = LV3EN6_WAITING;
float       lv3En6X = 0.f;
float       lv3En6Y = 0.f;
int         lv3En6FrameIndex = 0;
int         lv3En6AnimTick = 0;
int         lv3En6NormalHits = 0;
int         lv3En6ChargedHits = 0;
int         lv3En6ReviveCount = 0;
bool        lv3En6WasColliding = false;
bool        lv3En6WasPunching = false;
bool        lv3En6En4HitTriggered = false;
bool        lv3En6HitFreeze = false;
int         lv3En6HitFreezeTimer = 0;

// ── Level 3 Tree Obstacles ─────────────────────────────────────
#define TREE_FRAMECOUNT   69

// Display size — tweak freely
#define TREE_W            245
#define TREE_H            270

// Vertical Y position (world Y) — tweak freely per tree
#define TREE0_Y           -75    // between Bridge 1 and Bridge 2
#define TREE1_Y           -75    // between Bridge 2 and Bridge 3
#define TREE2_Y           -75    // between Bridge 5 and Bridge 6
#define TREE3_Y           -75    // between Bridge 6 and Bridge 7
#define TREE4_Y           -75    // between Bridge 9 and Bridge 10
#define TREE5_Y           -75    // between Bridge 10 and Bridge 11

// Horizontal X offset from midpoint between each bridge pair — tweak freely
#define TREE0_X_OFF       -17
#define TREE1_X_OFF       13
#define TREE2_X_OFF       -17
#define TREE3_X_OFF       13
#define TREE4_X_OFF       -17
#define TREE5_X_OFF       13

// Hitbox crop ratios — tweak freely
#define TREE_HB_LEFT      0.30f
#define TREE_HB_RIGHT     0.70f
#define TREE_HB_TOP       0.10f
#define TREE_HB_BOTTOM    0.75f

#define TREE_DRAW_DEBUG   false
#define NUM_TREES_LV3     6

// ── Level 3 Tree globals ──────────────────────────────────────
int treeFrames[TREE_FRAMECOUNT];
int treeAnimIndex = 0;

struct TreeObstacle {
	float x;
	float y;
	bool  playerTouching;
};

TreeObstacle treesLv3[NUM_TREES_LV3];

// ── Level 3 PBird Enemy (Bridge 3) ─────────────────────────────────────
#define LV3PBIRD_FRAMECOUNT   8
#define LV3PBIRD_W            235    // ← tweak freely
#define LV3PBIRD_H            195    // ← tweak freely
#define LV3PBIRD_Y_OFF        130     // ← height above bridge snapY — tweak freely
#define LV3PBIRD_SPEED        4.0f   // ← pixels per tick — tweak freely

// Hitbox crop ratios — tweak freely
#define LV3PBIRD_HB_LEFT   0.40f
#define LV3PBIRD_HB_RIGHT  0.60f
#define LV3PBIRD_HB_TOP    0.20f
#define LV3PBIRD_HB_BOTTOM 0.80f

#define LV3PBIRD_DRAW_DEBUG  false

// ── Level 3 PBird globals ─────────────────────────────────────────────
int   lv3PBirdFramesLeft[LV3PBIRD_FRAMECOUNT];   // pb1.png  … pb8.png  (right→left)
int   lv3PBirdFramesRight[LV3PBIRD_FRAMECOUNT];  // pb1m.png … pb8m.png (left→right)
int   lv3PBirdFrameIndex = 0;
bool  lv3PBirdAlive = true;
bool  lv3PBirdActive = false;   // activates when player steps on Bridge 3
float lv3PBirdX = 0.f;
bool  lv3PBirdMovingRight = true;  // starts flying left→right
static bool lv3PBirdWasColliding = false;

// ── Level 3 PBird2 Enemy (Bridge 7) ─────────────────────────────────────
#define LV3PBIRD2_FRAMECOUNT   8
#define LV3PBIRD2_W            235    // ← tweak freely
#define LV3PBIRD2_H            195    // ← tweak freely
#define LV3PBIRD2_Y_OFF        130    // ← height above bridge snapY — tweak freely
#define LV3PBIRD2_SPEED        4.0f   // ← pixels per tick — tweak freely

#define LV3PBIRD2_HB_LEFT   0.40f
#define LV3PBIRD2_HB_RIGHT  0.60f
#define LV3PBIRD2_HB_TOP    0.20f
#define LV3PBIRD2_HB_BOTTOM 0.80f

#define LV3PBIRD2_LEFT_OVERHANG   80  // ← tweak freely
#define LV3PBIRD2_RIGHT_OVERHANG  80  // ← tweak freely

#define LV3PBIRD2_DRAW_DEBUG  false

// ── Level 3 PBird2 globals (Bridge 7) ────────────────────────────────────
int   lv3PBird2FramesLeft[LV3PBIRD2_FRAMECOUNT];
int   lv3PBird2FramesRight[LV3PBIRD2_FRAMECOUNT];
int   lv3PBird2FrameIndex = 0;
bool  lv3PBird2Alive = true;
bool  lv3PBird2Active = true;
float lv3PBird2X = 0.f;
bool  lv3PBird2MovingRight = true;
static bool lv3PBird2WasColliding = false;

// ═══════════════════════════════════════════════════════════════
//  LEVEL 3  BARREL  (Bridge 5, bridges3[4])
// ═══════════════════════════════════════════════════════════════
// ── Barrel position and size — tweak freely ──────────────────
#define BARREL_X_OFF     -430   // offset from bridges3[4].maxX (right edge of B5)
#define BARREL_Y_OFF     -105    // offset from bridges3[4].snapY
#define BARREL_W         600    // display width
#define BARREL_H         600    // display height

// ── Barrel hitbox crop ratios — tweak freely ─────────────────
#define BARREL_HB_LEFT   0.50f
#define BARREL_HB_RIGHT  0.85f
#define BARREL_HB_TOP    0.05f
#define BARREL_HB_BOTTOM 0.95f

// ── Energy animation position and size — tweak freely ────────
#define ENERGY_X_OFF     -130    // offset from bridges3[4].maxX
#define ENERGY_Y_OFF      20    // offset from bridges3[4].snapY
#define ENERGY_W         130    // display width
#define ENERGY_H         150   // display height

// ── Energy hitbox crop ratios — tweak freely ─────────────────
#define ENERGY_HB_LEFT   0.25f
#define ENERGY_HB_RIGHT  0.75f
#define ENERGY_HB_TOP    0.10f
#define ENERGY_HB_BOTTOM 0.90f

// ── Animation speed (ticks per frame) ────────────────────────
#define BARREL_ENERGY_SPEED  3   // lower = faster energy anim
#define ENERGY_FRAMECOUNT   19

// ── Globals ───────────────────────────────────────────────────
int barrelFrames[4];        // bar1.png … bar4.png
int energyFrames[ENERGY_FRAMECOUNT];  // b1.png … b19.png

int  barrelState = 0;       // 0=bar1, 1=bar2, 2=bar3, 3=bar4, 4=energy, 5=done
int  energyFrameIndex = 0;
int  energyAnimTick = 0;
bool barrelWasPunching = false;   // rising-edge guard for punch
float barrelWorldX = 0.f;         // set in initBackground
float barrelWorldY = 0.f;         // set in initBackground

// ═══════════════════════════════════════════════════════════════
//  LEVEL 3  BARREL 2  (Bridge 9, bridges3[8])
// ═══════════════════════════════════════════════════════════════
#define BARREL2_X_OFF     -430
#define BARREL2_Y_OFF     -105
#define BARREL2_W         600
#define BARREL2_H         600

#define BARREL2_HB_LEFT   0.50f
#define BARREL2_HB_RIGHT  0.85f
#define BARREL2_HB_TOP    0.05f
#define BARREL2_HB_BOTTOM 0.95f

#define ENERGY2_X_OFF     -130
#define ENERGY2_Y_OFF      20
#define ENERGY2_W         130
#define ENERGY2_H         150

#define ENERGY2_HB_LEFT   0.25f
#define ENERGY2_HB_RIGHT  0.75f
#define ENERGY2_HB_TOP    0.10f
#define ENERGY2_HB_BOTTOM 0.90f

#define BARREL2_ENERGY_SPEED  3

// ── Level 3 Barrel 2 globals (Bridge 9) ──────────────────────────
int  barrel2State = 0;
int  energy2FrameIndex = 0;
int  energy2AnimTick = 0;
bool barrel2WasPunching = false;
float barrel2WorldX = 0.f;
float barrel2WorldY = 0.f;

int finalWinImg;
bool showFinalWin = false;

// Level 2 variables
int instruction2Img;
bool showInstruction2 = false;
int currentLevel = 1;
int bg51; // level 2 background images
int lv2CameraX = 0;
int lv2CharX = 100; // player screen X (stays on screen always)
int lv2CharY = 80;  // player screen Y - ADJUST to match ground
bool lv2IsJumping = false;
float lv2JumpVelocity = 0;
int lv2GroundY = 0; // ground level
// boss1//
int boss1HitCount = 0; // how many times boss hit player

int lv3CameraX = 0;
int lv3CharX = 100;
int lv3CharY = 80;
bool lv3IsJumping = false;
float lv3JumpVelocity = 0;
int lv3GroundY = 0;
int lv3Img;

int kneelImg; // kneel.png
bool showKneel = false;
int kneelTimer = 0;
bool kneelHolding = false;
int kneelHoldCounter = 0;
#define KNEEL_HOLD_TICKS 35 // ← tweak: how long kneel stays visible

bool boss1Dead = false;
int boss1ChargedHits = 0; // charged punch hits on boss
bool killedByBoss1 = false;
bool boss1Moving = true; // true = floating toward player, false = attacking
float boss1X = 0.f;      // boss world X position (set in initBackground)
float boss1FloatOffset = 0.0f;
int boss1DeadImg;
int btnNormal[NUM_BUTTONS];
int btnHover[NUM_BUTTONS];
bool btnIsHover[NUM_BUTTONS] = { false };

int backBtn, backBtnHover;
bool backHover = true;

int backSize = 60;
int backMargin = 20;
int backX = backMargin;
int backY = SCREEN_HEIGHT - backSize - backMargin;
int backW = backSize;
int backH = backSize;

int btnX = 30;
int btnW = 320;
int btnH = 100;
int btnStartY = 445;
int btnGap = 25;

int signpostImg;
int winFlagImg;
int winnerImg;
int hudLifeImg, hudPunchImg;
int tokenImg, treasureImg, tokenImgLv2, tokenCount = 0, treasureCount = 0,
tokenCountLv2 = 0;
bool playerWon = false;
// Story sequence - ADD THESE THREE LINES
int storyImgs[7];
int storyIndex = 0;
bool inStorySequence = false;
Bridge bridges[] = { { 290, 600, 160, 175, 165 }, { 890, 1090, 160, 180, 160 },
{ 1085, 1212, 185, 195, 185 }, { 1200, 1250, 290, 300, 290 },
{ 1170, 1290, 380, 390, 380 }, { 1320, 1650, 240, 250, 240 },
{ 1684, 1770, 290, 300, 295 }, { 1684, 1790, 400, 420, 405 },
{ 1760, 1900, 190, 200, 190 }, { 2290, 2400, 190, 200, 190 } };
int numBridges = sizeof(bridges) / sizeof(bridges[0]);

Bridge bridges2[] = {
	{ 00, 180, 190, 210, 190 },    // Bridge 1
	{ 70, 350, 80, 100, 80 },      // Bridge 2
	{ 410, 1084, 80, 100, 80 },    // Bridge 3
	{ 1150, 1430, 80, 100, 80 },   // Bridge 4
	{ 1320, 1690, 190, 210, 190 }, // Bridge 5
	{ 1570, 1850, 80, 100, 80 },   // Bridge 6
	{ 1925, 2580, 80, 100, 80 },   // Bridge 7
	{ 2650, 3000, 80, 100, 80 },   // Bridge 8
	{ 2830, 3000, 190, 210, 190 }, // Bridge 9
};
int numBridges2 = sizeof(bridges2) / sizeof(bridges2[0]);
Bridge bridges3[] = {
	{ 80, 380, 170, 190, 170 },    // Bridge 1  ← tweak these numbers to match your lv3.jpg
	{ 500, 1185, 70, 95, 70 },     // Bridge 2
	{ 1320, 1605, 170, 190, 170 },   // Bridge 3
	{ 1605, 1790, 80, 100, 80 },    // Bridge 4
	{ 1790, 2080, 170, 190, 170 },    // Bridge 5
	{ 2200, 2900, 80, 100, 80 },    // Bridge 6
	{ 3020, 3310, 170, 190, 170 },     // Bridge 7
	{ 3310, 3500, 80, 100, 80 },   // Bridge 8
	{ 3500, 3790, 170, 190, 170 },     // Bridge 9
	{ 3900, 4600, 80, 100, 80 },    // Bridge 10
	{ 4730, 5010, 170, 190, 170 }    // Bridge 11
};
int numBridges3 = sizeof(bridges3) / sizeof(bridges3[0]);
// Boss1 variables
int boss1Frames[41];
int boss1FrameIndex = 0;
bool boss1Active = true;

int shieldFrames[14];
int shieldFrameIndex = 0;
int winnerLv2Img;
bool playerWonLv2 = false;

//================= LEVEL 2 CONSTANTS ====================================

/* ─────────── LVL2 ENEMY 1 (Bridge 3) ─────────── */
#define LV2E1YOFFSET 75 // ← raise/lower enemy on the bridge (tweak freely)
#define LV2E1BASEW 90
#define LV2E1BASEH 90
#define LV2E1SCALE 2.5f // <--- tweak this to make bigger/smaller
#define LV2E1WIDTH (int)(LV2E1BASEW * LV2E1SCALE)
#define LV2E1HEIGHT (int)(LV2E1BASEH * LV2E1SCALE)
#define LV2E1_SPEED 2 // world-pixels per timer tick
#define LV2E1_HITS_TO_KILL 3
#define LV2E1FRAMECOUNT 9
/* ─────────── LVL2 WALK ENEMY (Bridge 3, left edge, stationary) ─────────── */
#define LV2WK_XOFFSET 5   // ← shift left/right on bridge (tweak freely)
#define LV2WK_YOFFSET -10 // ← shift up/down on bridge    (tweak freely)
#define LV2WK_BASEW 150
#define LV2WK_BASEH 90
#define LV2WK_SCALE 1.60f // ← make bigger/smaller
#define LV2WK_WIDTH (int)(LV2WK_BASEW * LV2WK_SCALE)
#define LV2WK_HEIGHT (int)(LV2WK_BASEH * LV2WK_SCALE)
#define LV2WK_FRAMECOUNT 8
#define LV2WK_KNEEL_DURATION 40 // ← how many ticks kneel stays (tweak freely)
// Hitbox crop ratios — tweak these to shrink/expand the hitbox
#define LV2WK_HB_LEFT 0.55f   // ← increase to shrink from left
#define LV2WK_HB_RIGHT 0.60f  // ← decrease to shrink from right
#define LV2WK_HB_TOP 0.20f    // ← increase to shrink from top
#define LV2WK_HB_BOTTOM 0.85f // ← decrease to shrink from bottom

/* ─────────── LVL2 WALK ENEMY 3 (Bridge 5, right edge, stationary) ───────────
*/
#define LV2WK3_XOFFSET 0
#define LV2WK3_YOFFSET -10
#define LV2WK3_BASEW 150
#define LV2WK3_BASEH 90
#define LV2WK3_SCALE 1.60f
#define LV2WK3_WIDTH (int)(LV2WK3_BASEW * LV2WK3_SCALE)
#define LV2WK3_HEIGHT (int)(LV2WK3_BASEH * LV2WK3_SCALE)
#define LV2WK3_FRAMECOUNT 8
#define LV2WK3_KNEEL_DURATION 40
#define LV2WK3_HB_LEFT 0.55f
#define LV2WK3_HB_RIGHT 0.60f
#define LV2WK3_HB_TOP 0.20f
#define LV2WK3_HB_BOTTOM 0.85f

/* ─────────── LVL2 WALK ENEMY 2 (Bridge 3, right edge, stationary) ───────────
*/
#define LV2WK2_XOFFSET 0   // ← shift left/right on bridge (tweak freely)
#define LV2WK2_YOFFSET -20 // ← shift up/down on bridge    (tweak freely)
#define LV2WK2_BASEW 150
#define LV2WK2_BASEH 90
#define LV2WK2_SCALE 1.60f // ← make bigger/smaller
#define LV2WK2_WIDTH (int)(LV2WK2_BASEW * LV2WK2_SCALE)
#define LV2WK2_HEIGHT (int)(LV2WK2_BASEH * LV2WK2_SCALE)
#define LV2WK2_FRAMECOUNT 37
#define LV2WK2_KNEEL_DURATION 40 // ← how many ticks kneel stays (tweak freely)
// Hitbox crop ratios — tweak these to shrink/expand the hitbox
#define LV2WK2_HB_LEFT 0.45f   // ← increase to shrink from left
#define LV2WK2_HB_RIGHT 0.50f  // ← decrease to shrink from right
#define LV2WK2_HB_TOP 0.20f    // ← increase to shrink from top
#define LV2WK2_HB_BOTTOM 0.85f // ← decrease to shrink from bottom

// LVL2 ENEMY 2  (flies over Bridge 5, index 4)
#define LV2E2BRIDGEINDEX 4 // bridge5 in bridges2[]
#define LV2E2YOFFSET 90    // tweak up/down above bridge

#define LV2E2BASEW 105 // base sprite size before scaling
#define LV2E2BASEH 95
#define LV2E2SCALE 2.2f // slightly bigger than enemy1; tweak freely
#define LV2E2WIDTH (int)(LV2E2BASEW * LV2E2SCALE)
#define LV2E2HEIGHT (int)(LV2E2BASEH * LV2E2SCALE)
#define LV2E2SPEED 3.0f // faster than LV2E1SPEED (2.0f)
#define LV2E2HITSTOKILL 4
#define LV2E2FRAMECOUNT 49

// Hitbox crop ratios for debug box (0–1, like your tightened e1 box)
#define LV2E2HBLEFT 0.20f
#define LV2E2HBRIGHT 0.80f
#define LV2E2HBTOP 0.15f
#define LV2E2HBBOTTOM 0.75f

/* ─────────── LV2 FIRE OBSTACLES ─────────── */
#define FIRE_FRAMECOUNT 16

// Hitbox crop ratios — tweak freely
#define NUM_FIRES 5
#define FIRE_HB_LEFT 0.20f
#define FIRE_HB_RIGHT 0.80f
#define FIRE_HB_TOP 0.15f
#define FIRE_HB_BOTTOM 0.65f

#define FIRE_KNEEL_DURATION 40 // ← how long kneel stays after fire contact

// Per-fire position and size tweaks — adjust freely
#define FIRE0_X_OFFSET -30 // ← shift left/right in gap 1 (bridge2-bridge3)
#define FIRE0_Y_OFFSET -15 // ← shift up/down
#define FIRE0_W 135        // ← display width
#define FIRE0_H 180        // ← display height

#define FIRE1_X_OFFSET -30 // ← shift left/right in gap 2 (bridge3-bridge4)
#define FIRE1_Y_OFFSET -15
#define FIRE1_W 135
#define FIRE1_H 180

#define FIRE2_X_OFFSET -30 // ← shift left/right in gap 3 (bridge6-bridge7)
#define FIRE2_Y_OFFSET -15
#define FIRE2_W 135
#define FIRE2_H 180

#define FIRE3_X_OFFSET 0 // ← shift left/right in gap (bridge4-bridge6)
#define FIRE3_Y_OFFSET -15
#define FIRE3_W 135
#define FIRE3_H 180

#define FIRE4_X_OFFSET -70 // ← shift left/right in gap (bridge7-bridge8)
#define FIRE4_Y_OFFSET -15
#define FIRE4_W 135
#define FIRE4_H 180

//================= LEVEL 2 GLOBALS ====================================
int lv2e2FramesRight[LV2E2FRAMECOUNT];
int lv2e2FramesLeft[LV2E2FRAMECOUNT];
int lv2e2FrameIndex = 0;
bool lv2e2Alive = true;
bool lv2e2MovingRight = true; // start going right, tweak if you want
int lv2e2HitCount = 0;
bool lv2e2Falling = false;
float lv2e2DeadY = 0.f;
float lv2e2DeadVel = 0.f;
float lv2e2X = 0.f; // set in initBackground

int lv2e1FramesRight[LV2E1FRAMECOUNT];
int lv2e1FramesLeft[LV2E1FRAMECOUNT];
int lv2e1FrameIndex = 0;
bool lv2e1Alive = true;
bool lv2e1Active = false;      // activates when player walks onto bridge 3
bool lv2e1MovingRight = false; // starts from right → left
int lv2e1HitCount = 0;
bool lv2e1Falling = false;
float lv2e1DeadY = 0.f;
float lv2e1DeadVel = 0.f;
float lv2e1X = 0.f; // set in initBackground()
/* ─── lvl2wkenemy globals ─── */
int lv2wkFrames[LV2WK_FRAMECOUNT];
int lv2wkFrameIndex = 0;
bool lv2wkAlive = true;
float lv2wkX = 0.f; // set in initBackground()
float lv2wkY = 0.f; // set in initBackground()
bool lv2wkPlayerTouching = false;
int lv2wkKneelTimer = 0;
bool lv2wkKneeling = false;

/* ─── lvl2wkenemy2 globals ─── */
int lv2wk2Frames[LV2WK2_FRAMECOUNT];
int lv2wk2FrameIndex = 0;
bool lv2wk2Alive = true;
float lv2wk2X = 0.f; // set in initBackground()
float lv2wk2Y = 0.f; // set in initBackground()
bool lv2wk2PlayerTouching = false;
int lv2wk2KneelTimer = 0;
bool lv2wk2Kneeling = false;

/* ─── lvl2wkenemy3 globals ─── */
int lv2wk3Frames[LV2WK3_FRAMECOUNT];
int lv2wk3FrameIndex = 0;
bool lv2wk3Alive = true;
float lv2wk3X = 0.f;
float lv2wk3Y = 0.f;
bool lv2wk3PlayerTouching = false;
int lv2wk3KneelTimer = 0;
bool lv2wk3Kneeling = false;

/* ─── Fire obstacle globals ─── */
int fireFrames[FIRE_FRAMECOUNT];
int fireAnimIndex = 0;

// Fire instance struct
struct FireObstacle {
	float x; // world X
	float y; // world Y (ground level)
	bool playerTouching;
	bool kneeling;
	int kneelTimer;
};
FireObstacle fires[NUM_FIRES];
int numFires = NUM_FIRES;

// ── Level 2 kill tracking ──────────────────────────────────────────
bool killedByLv2e1 = false;
bool killedByLv2e2 = false;
bool killedByLv2wk = false;
bool killedByLv2wk2 = false;
bool killedByLv2wk3 = false;
bool killedByFire = false;
float fireDeathWorldX = 0.f; // world X of fire that killed player
int pendingLivesLoss = 1;    // lives to deduct on next death tick

int lv2e1DeadImg;
int lv2e2DeadImg;
/* ─── Dead body position tweaks for Level 2 (adjust freely) ─── */
#define LV2_DEAD_BRIDGE_W (PLAYER_WIDTH + 40)
#define LV2_DEAD_BRIDGE_H (PLAYER_HEIGHT + 40)
#define LV2_DEAD_BRIDGE_YOFFSET -90 // ← shift up/down when lying on bridge
#define LV2_DEAD_BRIDGE_XOFFSET 0   // ← shift left/right

#define LV2_DEAD_FIRE_W (PLAYER_WIDTH + 40)
#define LV2_DEAD_FIRE_H (PLAYER_HEIGHT + 40)
#define LV2_DEAD_FIRE_YOFFSET -60 // ← shift up/down when lying on ground
#define LV2_DEAD_FIRE_XOFFSET 0   // ← shift left/right

Collectible collectibles[15];
int numCollectibles = 15;
Collectible collectiblesLv2[6];
int numCollectiblesLv2 = 6;
bool swapEnemySprites = false;

int playerSide, playerSideLeft;
int playerSideM, playerSideLeftM;
int punchFistImg, punchHitImg;
int punchFistImgM, punchHitImgM;

int chargedPunchImg, chargedFistImg, chargedPunchImgSeq;
int chargedPunchImgM, chargedFistImgM, chargedPunchImgSeqM;
int chargeScrollImg;

int playerX = PLAYER_START_X_BG41;
int playerY = PLAYER_START_Y_BG41;

bool movingFront = true;
int animFrame = 0;
int punchState = 0;
int chargedHitWindow = 0;

bool punchJustLanded = false;

bool isJumping = false;
float jumpVelocity = 0;

bool gameStarted = true;
int punchVolume =
700; // Range: 0 (silent) to 1000 (full). Adjust this to balance with music.
bool chargingPunch = false;
bool showChargeScroll = false;
bool chargeScrollShown = false;
bool chargeScrollDismissed = false;
bool chargedSequenceActive = false;
int chargedPunchState = 0;
int chargedUseCount = 0;
int chargedTimeCounter = 0;
bool chargedTimerRunning = false;

bool playerAlive = true;
int playerDeadImg;
int playerDeathY = 0;
bool killedByEnemy1 = false;
bool killedByDragon = false;
bool killedByEnemy3 = false;
bool killedByEnemy4 = false;
bool killedByEnemy2 = false;

int playerLives = MAX_LIVES;
bool gameOverTriggered = false;
bool gameOverScreen = false;
int gameOverImg;
int gameOverDelayCounter = 0;
int deathDelayCounter = 0;
bool deathWaiting = false;

int cameraX = 0;

int enemy1Img;
float enemy1FloatOffset = 0.0f;
float enemy1FloatSpeed = 0.08f;
float enemy1FloatRange = 6.0f;
bool enemy1Alive = true;
int enemy1HitCount = 0;
int enemy1DeadImg;
float enemy1DeadY = 0;
bool enemy1Falling = false;
float enemy1DeadVelocity = 0;

int enemy2LeftImg, enemy2RightImg;
bool enemy2MovingLeft = true;
float enemy2X = 600;
float enemy2Y = BG41_GROUND_Y + ENEMY2_FLOAT_HEIGHT;
float enemy2FloatOffset = 0.0f;
int enemy2HitCount = 0;
bool enemy2Dead = false;
float enemy2DeadY = 0;
bool enemy2Falling = false;
int enemy2DeadImg;
int enemy2BodyCollisionCount = 0;
bool enemy2WasColliding = false;

int enemy3IdleImg;
int enemy3WalkImg;
int enemy3FistImg;
int enemy3PunchImg;
bool enemy3Alive = true;
int enemy3IdleImgM;
int enemy3WalkImgM;
int enemy3FistImgM;
int enemy3PunchImgM;
float enemy3X = 0;
float enemy3Speed = 2.5f;
bool enemy3MovingLeft = true;
Enemy3State enemy3State = E3_WALKING;
float enemy3WalkOffset = 0.0f;
float enemy3WalkAmplitude = 2.0f;
float enemy3WalkSpeed = 0.1f;
int enemy3AnimFrame = 0;
int enemy3StateTimer = 0;
int enemy3PunchFrame = 0;
bool enemy3PunchToggle = false;
int enemy3HitCount = 0;
bool enemy3Falling = false;
float enemy3DeadY = 0;
float enemy3DeadVelocity = 0;
int enemy3DeadImg;
int enemy3PunchCount = 0;

int enemy4Img;
int enemy4DeadImg;
bool enemy4Alive = true;
int enemy4HitCount = 0;
float enemy4FloatOffset = 0.0f;
float enemy4FloatSpeed = 0.08f;
float enemy4FloatRange = 6.0f;
float enemy4DeadY = 0;

int dragonFrames[DRAGON_FRAME_COUNT];
int dragonFrameIndex = 0;
int dragonDeadImg;
bool dragonFireHitChecked = false;
bool dragonAlive = true;
int dragonHitCount = 0;
bool dragonFireActive = false;

void initCollectibles() {
	int tSize = 35;
	int trW = 80, trH = 80;
	int index = 0;

	// Bridge 1 (index 0) - More towards the left
	int leftX0 = bridges[0].minX + 60;
	collectibles[index++] = { leftX0, bridges[0].snapY + 45, false, false };
	collectibles[index++] = { leftX0 - tSize / 2 - 10, bridges[0].snapY + 10,
		false, false };
	collectibles[index++] = { leftX0 + tSize / 2 + 10, bridges[0].snapY + 10,
		false, false };

	// Bridge 2 (index 1)
	int midX1 = (bridges[1].minX + bridges[1].maxX) / 2;
	collectibles[index++] = { midX1 - tSize / 2, bridges[1].snapY + 45, false,
		false };
	collectibles[index++] = { midX1 - tSize - 10, bridges[1].snapY + 10, false,
		false };
	collectibles[index++] = { midX1 + 10, bridges[1].snapY + 10, false, false };

	// Bridge 4 (index 3)
	int midX3 = (bridges[3].minX + bridges[3].maxX) / 2;
	collectibles[index++] = { midX3 - tSize - 5, bridges[3].snapY + 10, false,
		false };
	collectibles[index++] = { midX3 + 5, bridges[3].snapY + 10, false, false };

	// Bridge 7 (index 6)
	int midX6 = (bridges[6].minX + bridges[6].maxX) / 2;
	collectibles[index++] = { midX6 - tSize - 5, bridges[6].snapY + 10, false,
		false };
	collectibles[index++] = { midX6 + 5, bridges[6].snapY + 10, false, false };

	// Bridge 9 (index 8)
	int midX8 = (bridges[8].minX + bridges[8].maxX) / 2;
	collectibles[index++] = { midX8 - tSize / 2, bridges[8].snapY + 45, false,
		false };
	collectibles[index++] = { midX8 - tSize - 10, bridges[8].snapY + 10, false,
		false };
	collectibles[index++] = { midX8 + 10, bridges[8].snapY + 10, false, false };

	// Bridge 5 (index 4)
	int midX4 = (bridges[4].minX + bridges[4].maxX) / 2;
	collectibles[index++] = { midX4 - trH / 2, bridges[4].snapY, false, true };

	// Bridge 8 (index 7)
	int midX7 = (bridges[7].minX + bridges[7].maxX) / 2;
	collectibles[index++] = { midX7 - trH / 2, bridges[7].snapY, false, true };
}

void checkCollectibleCollisions() {
	if (currentBg != 4 || !playerAlive)
		return;

	int tSize = 35;
	int trW = 80, trH = 80;

	for (int i = 0; i < numCollectibles; i++) {
		if (collectibles[i].collected)
			continue;

		int cW = collectibles[i].isTreasure ? trW : tSize;
		int cH = collectibles[i].isTreasure ? trH : tSize;

		if (playerX < collectibles[i].x + cW &&
			playerX + PLAYER_WIDTH > collectibles[i].x &&
			playerY < collectibles[i].y + cH &&
			playerY + PLAYER_HEIGHT > collectibles[i].y) {
			collectibles[i].collected = true;
			if (collectibles[i].isTreasure) {
				treasureCount++;
			}
			else {
				tokenCount++;
			}
		}
	}
}

void initCollectiblesLv2() {
	int tSize = 35; // Reduced size as requested
	int index = 0;

	// Bridge 2 (index 1) - Triangle pattern
	int midX2 = (bridges2[1].minX + bridges2[1].maxX) / 2;
	int baseLineY2 = bridges2[1].snapY + 15;
	collectiblesLv2[index++] = { midX2 - tSize / 2, baseLineY2 + 50, false,
		false }; // Top
	collectiblesLv2[index++] = { midX2 - tSize - 10, baseLineY2, false,
		false }; // Bottom Left
	collectiblesLv2[index++] = { midX2 + 10, baseLineY2, false,
		false }; // Bottom Right

	// Bridge 6 (index 5) - Triangle pattern
	int midX6 = (bridges2[5].minX + bridges2[5].maxX) / 2;
	int baseLineY6 = bridges2[5].snapY + 15;
	collectiblesLv2[index++] = { midX6 - tSize / 2, baseLineY6 + 50, false,
		false }; // Top
	collectiblesLv2[index++] = { midX6 - tSize - 10, baseLineY6, false,
		false }; // Bottom Left
	collectiblesLv2[index++] = { midX6 + 10, baseLineY6, false,
		false }; // Bottom Right
}

void checkCollectibleCollisionsLv2() {
	if (currentBg != 10 || !playerAlive)
		return;

	int tSize = 35; // Visual size for draw
	int cSize = 20; // Collision size (tighter box)

	for (int i = 0; i < numCollectiblesLv2; i++) {
		if (collectiblesLv2[i].collected)
			continue;

		// Using a tighter hitbox (playerX, playerY are already big, so shrinking
		// token box helps)
		if (lv2CharX < collectiblesLv2[i].x + cSize &&
			lv2CharX + PLAYER_WIDTH > collectiblesLv2[i].x &&
			lv2CharY < collectiblesLv2[i].y + cSize &&
			lv2CharY + PLAYER_HEIGHT > collectiblesLv2[i].y) {
			collectiblesLv2[i].collected = true;
			tokenCountLv2++;
		}
	}
}

// ---------------- FORWARD DECLARATIONS ----------------

void updateEnemy1Hit();
void updateEnemy2Hit();
void updateEnemy4Hit();
void checkPlayerEnemy2Collision();
void checkPlayerEnemy4Collision();
void updateEnemy3();
void updateEnemy3Hit();
void checkEnemy3BodyKill();
void handlePlayerDeath();
void updateGameOverDelay();
void shuffleEnemySprites();
void checkBoss1PlayerCollision();
void checkBoss1ChargedHit();
void checkShieldCollision();
void updateLv2Enemy1();
void checkLv2Enemy1Hits();
void updateLv2Enemy2();
void checkLv2Enemy2Hits();
void updateLv2WkEnemy();
void updateLv2WkEnemy2();
void initFires();
void lv2HitPlayer(int livesLost, bool byFire = false, float fireWorldX = 0.f);
void updateLv3Boss();
void checkLv3BossHits();
void updateLv3Bird();
void updateLv3En2();
void updateLv3En4();
void updateLv3WkEnemies();
void updateLv3Trees();
void updateLv3PBird();
void updateLv3PBird2();
void updateLv3Barrel();
void updateLv3Barrel2();

void updateDragonHit() {
	if (currentBg != 4)
		return;

	Bridge &b = bridges[DRAGON_BRIDGE_INDEX];

	int dragonX = b.minX + DRAGON_X_OFFSET;
	int dragonY = b.snapY - 5; // slightly above ground

	// Dragon hitbox
	int dX1 = dragonX;
	int dX2 = dragonX + DRAGON_WIDTH;
	int dY1 = dragonY;
	int dY2 = dragonY + DRAGON_HEIGHT;

	bool invincible = (!dragonAlive) || (chargingPunch && chargeScrollDismissed);

	// -------- Charged Punch (longer reach) --------
	if (chargedSequenceActive) {
		int reachExtra = 80; // extend punch reach
		int pX1 = playerX + (movingFront ? PLAYER_WIDTH : -10 - reachExtra);
		int pX2 = playerX + (movingFront ? PLAYER_WIDTH + 80 + reachExtra : 10);
		int pY1 = playerY + 40;
		int pY2 = playerY + PLAYER_HEIGHT;

		bool overlapX = !(pX2 < dX1 || pX1 > dX2);
		bool overlapY = !(pY2 < dY1 || pY1 > dY2);

		if (overlapX && overlapY) {
			dragonHitCount++;
		}
	}

	if (dragonHitCount >= DRAGON_HITS_TO_KILL + 1) {
		dragonAlive = false;
	}
}

void updateDragon() {
	dragonFrameIndex = (dragonFrameIndex + 1) % DRAGON_FRAME_COUNT;

	if (dragonFrameIndex >= 5 && dragonFrameIndex <= 10)
		dragonFireActive = true;
	else
		dragonFireActive = false;

	if (currentBg == 4 && playerAlive && dragonFireActive) {
		Bridge &b = bridges[DRAGON_BRIDGE_INDEX];

		int dragonX = b.minX + DRAGON_X_OFFSET;
		int dragonY = b.snapY - 5;

		int fireX1 = dragonX + 80;
		int fireX2 = dragonX + DRAGON_WIDTH + 40;
		int fireY1 = dragonY + 80;
		int fireY2 = dragonY + DRAGON_HEIGHT - 40;

		int pX1 = playerX;
		int pX2 = playerX + PLAYER_WIDTH;
		int pY1 = playerY;
		int pY2 = playerY + PLAYER_HEIGHT;

		bool overlapX = !(fireX2 < pX1 || fireX1 > pX2);
		bool overlapY = !(fireY2 < pY1 || fireY1 > pY2);
		bool invincible =
			(!dragonAlive) || (chargingPunch && chargeScrollDismissed);

		if (overlapX && overlapY && !invincible) {
			playerAlive = false;
			gameStarted = false;

			killedByDragon = true;
			killedByEnemy1 = false;

			// Snap dead body to dragon ground
			playerDeathY = b.snapY;
		}
	}
	updateDragonHit();
	updateEnemy1Hit();
	handlePlayerDeath();
}

void checkPlayerEnemy1Collision() {
	if (!enemy1Alive)
		return; // enemy must be alive
	if (!playerAlive)
		return;
	if (currentBg != 4)
		return;

	Bridge &b = bridges[ENEMY1_BRIDGE_INDEX];

	// Enemy1 position exactly as drawn
	int enemyX = b.maxX - ENEMY1_WIDTH - 60;
	int enemyY;
	if (enemy1Alive) {
		enemyY = b.snapY + (int)(sin(enemy1FloatOffset) * enemy1FloatRange);
	}
	else if (enemy1Falling) {
		enemyY = (int)enemy1DeadY;
	}
	else {
		enemyY = b.snapY;
	}

	// Shrink hitbox to match visible body
	int eX1 = enemyX + 90; // a bit inside horizontally
	int eX2 = enemyX + ENEMY1_WIDTH - 10;
	int eY1 = enemyY + 10;                 // a bit above the bottom
	int eY2 = enemyY + ENEMY1_HEIGHT - 10; // a bit below the top

	// Player body hitbox
	int pX1 = playerX;
	int pX2 = playerX + PLAYER_WIDTH;
	int pY1 = playerY;
	int pY2 = playerY + PLAYER_HEIGHT;

	// Check overlap
	bool overlapX = !(pX2 < eX1 || pX1 > eX2);
	bool overlapY = !(pY2 < eY1 || pY1 > eY2);

	if (overlapX && overlapY) {
		playerAlive = false;
		gameStarted = false;

		killedByEnemy1 = true;
		killedByDragon = false;

		// Snap dead body to enemy1's Y
		playerDeathY = enemyY;
	}
}


void updateEnemy1() {
	enemy1FloatOffset += enemy1FloatSpeed;
	if (enemy1FloatOffset > 2 * 3.1416f)
		enemy1FloatOffset = 0;

	checkPlayerEnemy1Collision();
	handlePlayerDeath();
}


void updateEnemy3() {
	if (currentBg != 4)
		return;

	Bridge &b = bridges[ENEMY3_BRIDGE_INDEX];

	// -------- DEAD FALL --------
	if (!enemy3Alive && enemy3Falling) {
		enemy3DeadVelocity -= 1.4f;
		enemy3DeadY += enemy3DeadVelocity;

		if (enemy3DeadY <= BG41_GROUND_Y) {
			enemy3DeadY = BG41_GROUND_Y;
			enemy3Falling = false;
		}
		return;
	}

	if (!enemy3Alive)
		return;

	// -------- PLAYER DEAD: FLOAT ONLY --------
	if (!playerAlive) {
		// float up and down slightly on bridge6
		enemy3WalkOffset += enemy3WalkSpeed;
		if (enemy3WalkOffset > 2 * 3.1416f)
			enemy3WalkOffset = 0;
		return; // skip attack logic
	}

	// -------- ATTACK LOGIC --------
	if (enemy3State == E3_ATTACKING) {
		enemy3StateTimer++;

		if (enemy3StateTimer >= ENEMY3_PUNCH_INTERVAL) {
			enemy3StateTimer = 0;
			enemy3PunchToggle ^= 1;

			if (!chargingPunch && !chargedSequenceActive) {
				enemy3PunchCount++;
				if (enemy3PunchCount >= ENEMY3_MAX_PUNCHES) {
					playerAlive = false;
					gameStarted = false;

					killedByEnemy3 = true;
					killedByEnemy1 = false;
					killedByDragon = false;

					playerDeathY = BG41_GROUND_Y;
				}
			}
		}
	}

	// -------- CHECK PLAYER PROXIMITY FOR ATTACK --------
	if (playerAlive) {
		int playerCenterX = playerX + PLAYER_WIDTH / 2;
		int enemyCenterX = enemy3X + ENEMY3_WIDTH / 2;
		int distX = abs(playerCenterX - enemyCenterX);
		bool sameBridge = abs(playerY - BG41_GROUND_Y) <= 6;

		if (sameBridge && distX <= ENEMY3_ATTACK_RANGE) {
			enemy3State = E3_ATTACKING;

			// Face the player before attacking
			if (playerCenterX < enemyCenterX)
				enemy3MovingLeft = true; // Player is on the left, face left
			else
				enemy3MovingLeft = false; // Player is on the right, face right
		}
		else
			enemy3State = E3_WALKING;
	}

	// -------- WALKING LOGIC --------
	if (enemy3State == E3_WALKING) {
		if (enemy3MovingLeft)
			enemy3X -= enemy3Speed;
		else
			enemy3X += enemy3Speed;

		enemy3WalkOffset += enemy3WalkSpeed;
		if (enemy3WalkOffset > 2 * 3.1416f)
			enemy3WalkOffset = 0;

		// In updateEnemy3(), replace the walking bounds:
		if (enemy3X <= bridges[8].maxX + 20) {
			enemy3X = bridges[8].maxX + 20;
			enemy3MovingLeft = false;
		}
		else if (enemy3X + ENEMY3_WIDTH >= bridges[9].minX - 20) {
			enemy3X = bridges[9].minX - 20 - ENEMY3_WIDTH;
			enemy3MovingLeft = true;
		}

		enemy3StateTimer++;
		if (enemy3StateTimer > 20) {
			enemy3StateTimer = 0;
			enemy3AnimFrame ^= 1;
		}
	}
	updateEnemy3Hit();
	checkEnemy3BodyKill();
	handlePlayerDeath();
}

void checkEnemy3BodyKill() {
	if (!enemy3Alive)
		return;
	if (!playerAlive)
		return;
	if (currentBg != 4)
		return;

	// Enemy3 can only kill while ATTACKING and punch frame is active
	if (enemy3State != E3_ATTACKING || !enemy3PunchToggle)
		return;

	// Player immunity during charged actions
	if ((chargingPunch && chargeScrollDismissed) || chargedSequenceActive)
		return;

	Bridge &b = bridges[ENEMY3_BRIDGE_INDEX];

	// Enemy3 body hitbox
	int eX1 = enemy3X + 30;
	int eX2 = enemy3X + ENEMY3_WIDTH - 30;
	int eY1 = BG41_GROUND_Y + 20;
	int eY2 = BG41_GROUND_Y + ENEMY3_HEIGHT - 20;

	// Player body hitbox
	int pX1 = playerX + 20;
	int pX2 = playerX + PLAYER_WIDTH - 20;
	int pY1 = playerY;
	int pY2 = playerY + PLAYER_HEIGHT;

	bool overlapX = !(pX2 < eX1 || pX1 > eX2);
	bool overlapY = !(pY2 < eY1 || pY1 > eY2);

	if (overlapX && overlapY) {
		playerAlive = false;
		gameStarted = false;

		killedByEnemy3 = true;
		killedByEnemy1 = false;
		killedByDragon = false;

		playerDeathY = b.snapY;
	}
}

void updateEnemy3Hit() {
	if (!enemy3Alive)
		return;
	if (currentBg != 4)
		return;

	Bridge &b = bridges[ENEMY3_BRIDGE_INDEX];

	// Enemy3 hitbox
	int eX1 = enemy3X;
	int eX2 = enemy3X + ENEMY3_WIDTH;
	int eY1 = b.snapY;
	int eY2 = b.snapY + ENEMY3_HEIGHT;

	// -------- NORMAL PUNCH --------
	if (punchJustLanded) {
		int pX1 = playerX + (movingFront ? PLAYER_WIDTH : -60);
		int pX2 = playerX + (movingFront ? PLAYER_WIDTH + 60 : 10);
		int pY1 = playerY + 60;
		int pY2 = playerY + 140;

		bool overlapX = !(pX2 < eX1 || pX1 > eX2);
		bool overlapY = !(pY2 < eY1 || pY1 > eY2);

		if (overlapX && overlapY) {
			enemy3HitCount++;
			punchJustLanded = false;
			// If normal punch count reaches 8, enemy3 dies
			if (enemy3HitCount >= ENEMY3_NORMAL_HITS_TO_KILL) {
				enemy3Alive = false;
				enemy3Falling = true;
				enemy3DeadVelocity = 0;
				enemy3DeadY = BG41_GROUND_Y + 250; // start ABOVE, for falling animation
			}
		}
	}

	// -------- CHARGED PUNCH --------
	if (chargedSequenceActive) {
		int pX1 = playerX + (movingFront ? PLAYER_WIDTH : -120);
		int pX2 = playerX + (movingFront ? PLAYER_WIDTH + 120 : 10);
		int pY1 = playerY + 40;
		int pY2 = playerY + PLAYER_HEIGHT;

		bool overlapX = !(pX2 < eX1 || pX1 > eX2);
		bool overlapY = !(pY2 < eY1 || pY1 > eY2);

		if (overlapX && overlapY) {
			enemy3Alive = false;
			enemy3Falling = true;
			enemy3DeadVelocity = 0;
			enemy3DeadY = b.snapY + 250; // start ABOVE, for falling animation
			return;
		}
	}

	// -------- DEATH CHECK --------
	if (enemy3HitCount >= ENEMY3_NORMAL_HITS_TO_KILL) {
		enemy3Alive = false;
		enemy3Falling = true;
		enemy3DeadVelocity = 0;
		enemy3DeadY = b.snapY + 250; // start ABOVE, for falling animation
	}
}

void updateKneelHold() {
	if (kneelHolding) {
		kneelHoldCounter++;
		if (kneelHoldCounter >= KNEEL_HOLD_TICKS) {
			kneelHolding = false;
			kneelHoldCounter = 0;
			showKneel = false;
		}
	}
}

void updateChargedHitWindow() {
	if (chargedHitWindow > 0) {
		chargedHitWindow--;
		if (chargedHitWindow == 0)
			chargedSequenceActive = false;
	}
}

void updateEnemy1Hit() {
	if (!enemy1Alive)
		return;
	if (currentBg != 4)
		return;
	if (!punchJustLanded)
		return; // only count completed punches

	Bridge &b = bridges[ENEMY1_BRIDGE_INDEX];

	// Enemy1 position
	int enemyX = b.maxX - ENEMY1_WIDTH - 60;
	int enemyY = b.snapY + (int)(sin(enemy1FloatOffset) * enemy1FloatRange);

	// Enemy hitbox
	int eX1 = enemyX;
	int eX2 = enemyX + ENEMY1_WIDTH;
	int eY1 = enemyY;
	int eY2 = enemyY + ENEMY1_HEIGHT;

	// Player punch hitbox (NORMAL punch)
	int punchReach = 60; // extend reach to the left
	int pHeight = 80;    // vertical hitbox height
	int pY1 = playerY + 60;
	int pY2 = playerY + 60 + pHeight;

	int pX1, pX2;
	if (movingFront) {
		// Facing right: start hitbox slightly LEFT of player's right edge
		pX1 = playerX + PLAYER_WIDTH - 20;
		pX2 = playerX + PLAYER_WIDTH + punchReach;
	}
	else {
		// Facing left: hitbox slightly wider to left
		pX1 = playerX - punchReach;
		pX2 = playerX + 70;
	}

	// Check overlap
	bool overlapX = !(pX2 < eX1 || pX1 > eX2);
	bool overlapY = !(pY2 < eY1 || pY1 > eY2);

	if (overlapX && overlapY) {
		enemy1HitCount++;
		punchJustLanded = false; // consume hit

		if (enemy1HitCount >= ENEMY1_HITS_TO_KILL) {
			enemy1Alive = false;

			// Immediately place dead body on bridge1 at player position
			enemy1DeadY = bridges[ENEMY1_BRIDGE_INDEX].snapY; // exactly on bridge
		}
	}
}


void updateEnemy2() {
	if (currentBg != 4)
		return;

	// Fall if dead
	if (enemy2Dead && enemy2Falling) {
		enemy2DeadY -= 2; // Fall downward (negative because falling)
		if (enemy2DeadY <= BG41_GROUND_Y) {
			enemy2DeadY = BG41_GROUND_Y; // Make sure it touches the ground
			enemy2Falling = false;       // Stop falling when it reaches the ground
		}
		return; // EXIT early - don't run movement code below
	}

	// Only move if alive
	if (!enemy2Dead) {
		// Horizontal movement
		if (enemy2MovingLeft) {
			enemy2X -= ENEMY2_FLOAT_SPEED;    // Move left
			if (enemy2X <= bridges[0].maxX) { // If Enemy2 reaches Bridge 1
				enemy2MovingLeft = false;       // Change direction to right
			}
		}
		else {
			enemy2X += ENEMY2_FLOAT_SPEED; // Move right
			if (enemy2X >=
				bridges[1].minX - ENEMY2_WIDTH) { // If Enemy2 reaches Bridge 2
				enemy2MovingLeft = true;            // Change direction to left
			}
		}

		// Ensure enemy2 doesn't go off the screen (boundary check)
		if (enemy2X < bridges[0].maxX) {
			enemy2X =
				bridges[0].maxX; // Keep enemy2 within the left side of the screen
		}
		if (enemy2X > bridges[1].minX - ENEMY2_WIDTH) {
			enemy2X = bridges[1].minX -
				ENEMY2_WIDTH; // Keep enemy2 within the right side of the screen
		}

		// Floating effect (up and down)
		enemy2FloatOffset += 0.1f; // Adjust floating speed
		if (enemy2FloatOffset > 2 * 3.1416f) {
			enemy2FloatOffset = 0.0f; // Reset after a full cycle
		}
		enemy2Y = BG41_GROUND_Y + ENEMY2_FLOAT_HEIGHT +
			sin(enemy2FloatOffset) * 10; // Small floating bounce
	}

	updateEnemy2Hit();
	checkPlayerEnemy2Collision();
	handlePlayerDeath();
}

void updateEnemy4() {
	if (currentBg != 4)
		return;

	enemy4FloatOffset += enemy4FloatSpeed;
	if (enemy4FloatOffset > 2 * 3.1416f)
		enemy4FloatOffset = 0;

	// Update the position for Enemy 4 to the very right edge of Bridge 2
	Bridge &b = bridges[ENEMY4_BRIDGE_INDEX];
	int enemyX = b.maxX - ENEMY4_WIDTH; // Move to the right edge of Bridge 2

	updateEnemy4Hit();
	checkPlayerEnemy4Collision();
	handlePlayerDeath();
}

void updateEnemy4Hit() {
	if (!enemy4Alive)
		return;
	if (currentBg != 4)
		return;
	if (!punchJustLanded)
		return;

	Bridge &b = bridges[ENEMY4_BRIDGE_INDEX];

	// Enemy4 position (right edge of bridge)
	int enemy4X = b.minX + 100; // EXACT left edge of bridge
	int enemyY = b.snapY + ENEMY4_HEIGHT_OFFSET +
		(int)(sin(enemy4FloatOffset) * enemy4FloatRange);

	// Enemy hitbox
	int eX1 = enemy4X;
	int eX2 = enemy4X + ENEMY4_WIDTH;
	int eY1 = enemyY;
	int eY2 = enemyY + ENEMY4_HEIGHT;

	// Player punch hitbox (NORMAL punch)
	int punchReach = 60;
	int pHeight = 80;
	int pY1 = playerY + 60;
	int pY2 = playerY + 60 + pHeight;

	int pX1, pX2;
	if (movingFront) {
		pX1 = playerX + PLAYER_WIDTH - 20;
		pX2 = playerX + PLAYER_WIDTH + punchReach;
	}
	else {
		pX1 = playerX - punchReach;
		pX2 = playerX + 70;
	}

	// Check overlap
	bool overlapX = !(pX2 < eX1 || pX1 > eX2);
	bool overlapY = !(pY2 < eY1 || pY1 > eY2);

	if (overlapX && overlapY) {
		enemy4HitCount++;
		punchJustLanded = false; // consume hit

		if (enemy4HitCount >= ENEMY4_HITS_TO_KILL) {
			enemy4Alive = false;
			enemy4DeadY = bridges[ENEMY4_BRIDGE_INDEX].snapY;
		}
	}
}
void updateEnemy2Hit() {
	if (enemy2Dead)
		return;
	if (currentBg != 4)
		return;
	if (!punchJustLanded)
		return; // only check when punch just landed

	// Enemy2's hitbox (same as draw)
	int eX1 = enemy2X;
	int eX2 = enemy2X + ENEMY2_WIDTH;
	int eY1 = enemy2Y;
	int eY2 = enemy2Y + ENEMY2_HEIGHT;

	// Player punch hitbox (NORMAL punch)
	int punchReach = 60; // extend reach to the left
	int pHeight = 80;    // vertical hitbox height
	int pY1 = playerY + 60;
	int pY2 = playerY + 60 + pHeight;

	int pX1, pX2;
	if (movingFront) {
		// Facing right: start hitbox slightly LEFT of player's right edge
		pX1 = playerX + PLAYER_WIDTH - 20;
		pX2 = playerX + PLAYER_WIDTH + punchReach;
	}
	else {
		// Facing left: hitbox slightly wider to left
		pX1 = playerX - punchReach;
		pX2 = playerX + 70;
	}

	// Check overlap
	bool overlapX = !(pX2 < eX1 || pX1 > eX2);
	bool overlapY = !(pY2 < eY1 || pY1 > eY2);

	if (overlapX && overlapY) {
		enemy2HitCount++;
		punchJustLanded = false; // CONSUME the punch flag here

		if (enemy2HitCount >= ENEMY2_HITS_TO_KILL) {
			enemy2Dead = true;     // Mark Enemy2 as dead
			enemy2Falling = true;  // Start falling animation
			enemy2DeadY = enemy2Y; // Set the death position at current Y
		}
	}
}

void checkPlayerEnemy2Collision() {
	if (enemy2Dead)
		return;
	if (!playerAlive)
		return;
	if (currentBg != 4)
		return;
	if ((chargingPunch && chargeScrollDismissed) || chargedSequenceActive)
		return;

	int drawW = (int)(ENEMY2_WIDTH * ENEMY2_SCALE);
	int drawH = (int)(ENEMY2_HEIGHT * ENEMY2_SCALE);

	int eX1 = enemy2X + (ENEMY2_WIDTH - drawW) / 2;
	int eY1 = enemy2Y + (ENEMY2_HEIGHT - drawH) / 2;

	int eX2 = eX1 + drawW;
	int eY2 = eY1 + drawH;

	int shrinkX = 20;
	int shrinkY = 20;

	eX1 += shrinkX;
	eX2 -= shrinkX;
	eY1 += shrinkY;
	eY2 -= shrinkY;

	int pX1 = playerX + 20;
	int pX2 = playerX + PLAYER_WIDTH - 20;
	int pY1 = playerY + 20;
	int pY2 = playerY + PLAYER_HEIGHT - 20;

	bool overlapX = !(pX2 < eX1 || pX1 > eX2);
	bool overlapY = !(pY2 < eY1 || pY1 > eY2);

	bool isColliding = overlapX && overlapY;

	if (isColliding && !enemy2WasColliding) {
		enemy2BodyCollisionCount++;

		if (enemy2BodyCollisionCount >= ENEMY2_MAX_BODY_COLLISIONS) {
			playerAlive = false;
			gameStarted = false;

			killedByEnemy2 = true;
			killedByEnemy1 = false;
			killedByDragon = false;
			killedByEnemy3 = false;

			playerDeathY = BG41_GROUND_Y;
		}
	}
	enemy2WasColliding = isColliding;
}

void checkPlayerEnemy4Collision() {
	if (!enemy4Alive)
		return; // enemy must be alive
	if (!playerAlive)
		return;
	if (currentBg != 4)
		return;

	// Player immunity during charged actions
	if ((chargingPunch && chargeScrollDismissed) || chargedSequenceActive)
		return;

	Bridge &b = bridges[ENEMY4_BRIDGE_INDEX];

	// Enemy4 position exactly as drawn
	int enemy4X = b.minX + 100;
	int enemyY = b.snapY + ENEMY4_HEIGHT_OFFSET +
		(int)(sin(enemy4FloatOffset) * enemy4FloatRange);

	// Shrink hitbox to ensure REAL touching
	int eX1 = enemy4X + 90;
	int eX2 = enemy4X + ENEMY4_WIDTH - 30;
	int eY1 = enemyY + 20;
	int eY2 = enemyY + ENEMY4_HEIGHT - 20;

	// Player body hitbox (slightly reduced)
	int pX1 = playerX + 20;
	int pX2 = playerX + PLAYER_WIDTH - 20;
	int pY1 = playerY + 20;
	int pY2 = playerY + PLAYER_HEIGHT - 20;

	// Check overlap
	bool overlapX = !(pX2 < eX1 || pX1 > eX2);
	bool overlapY = !(pY2 < eY1 || pY1 > eY2);

	if (overlapX && overlapY) {
		playerAlive = false;
		gameStarted = false;

		killedByEnemy4 = true;
		killedByEnemy1 = false;
		killedByDragon = false;
		killedByEnemy3 = false;
		killedByEnemy2 = false;

		// Snap dead body to bridge2
		playerDeathY = b.snapY;
	}
}
void checkWinFlagCollision() {
	if (!playerAlive)
		return;
	if (currentBg != 4)
		return;
	if (playerWon)
		return; // Already won

	Bridge &bridge10 = bridges[9]; // Bridge 10
	int flagX = bridge10.maxX - WINFLAG_WIDTH + WINFLAG_X_OFFSET;
	int flagY = bridge10.snapY + WINFLAG_Y_OFFSET;

	// Win flag hitbox - SHRINK IT to require closer contact
	int shrinkX = 40; // Make hitbox smaller horizontally
	int shrinkY = 40; // Make hitbox smaller vertically

	int fX1 = flagX + shrinkX;
	int fX2 = flagX + WINFLAG_WIDTH - shrinkX;
	int fY1 = flagY + shrinkY;
	int fY2 = flagY + WINFLAG_HEIGHT - shrinkY;

	// Player hitbox - also shrink slightly
	int pX1 = playerX + 20;
	int pX2 = playerX + PLAYER_WIDTH - 20;
	int pY1 = playerY + 20;
	int pY2 = playerY + PLAYER_HEIGHT - 20;

	// Check overlap
	bool overlapX = !(pX2 < fX1 || pX1 > fX2);
	bool overlapY = !(pY2 < fY1 || pY1 > fY2);

	if (overlapX && overlapY) {
		playerWon = true;
		gameStarted = false;
	}
}
void checkWinFlagCollisionLv3() {
	if (!playerAlive) return;
	if (currentBg != 12) return;
	if (playerWonLv3) return;

	Bridge &bridge11 = bridges3[10];
	int flagX = bridge11.maxX - WINFLAG_WIDTH + WINFLAG_X_OFFSET - 60;
	int flagY = bridge11.snapY + WINFLAG_Y_OFFSET + 10;

	int shrinkX = 40;
	int shrinkY = 40;

	int fX1 = flagX + shrinkX;
	int fX2 = flagX + WINFLAG_WIDTH - shrinkX;
	int fY1 = flagY + shrinkY;
	int fY2 = flagY + WINFLAG_HEIGHT + 50 - shrinkY;

	int pX1 = lv3CharX + 20;
	int pX2 = lv3CharX + PLAYER_WIDTH - 20;
	int pY1 = lv3CharY + 20;
	int pY2 = lv3CharY + PLAYER_HEIGHT - 20;

	bool overlapX = !(pX2 < fX1 || pX1 > fX2);
	bool overlapY = !(pY2 < fY1 || pY1 > fY2);

	if (overlapX && overlapY) {
		playerWonLv3 = true;
		gameStarted = false;
	}
}
void updateBoss1Movement();
void updateLevel2() {
	if (currentBg != 10)
		return;
	checkBoss1PlayerCollision();
	checkBoss1ChargedHit();
	checkShieldCollision();
}

/* -------------------- HELPER -------------------- */
bool isInsideCircle(int mx, int my) {
	double cx = backX + backW / 2.0;
	double cy = backY + backH / 2.0;
	double r = backW / 2.0;
	double d = sqrt((mx - cx) * (mx - cx) + (my - cy) * (my - cy));
	return d <= r;
}

/* -------------------- CAMERA -------------------- */
void updateCamera() {
	if (currentBg != 4)
		return;
	if (!playerAlive)
		return; // CAMERA FREEZE ON DEATH

	cameraX = playerX - SCREEN_WIDTH / 2;
	if (cameraX < 0)
		cameraX = 0;
	if (cameraX > BG41_WIDTH - SCREEN_WIDTH)
		cameraX = BG41_WIDTH - SCREEN_WIDTH;
}

/* -------------------- BRIDGE HELPERS -------------------- */
bool isOnBridge();
bool isBelowBridge();

bool isBelowBridge() {
	if (currentBg != 4)
		return false;
	if (isJumping)
		return false; // Allow movement while jumping
	if (isOnBridge())
		return false;

	int cx = playerX + PLAYER_WIDTH / 2;
	int nearestBridgeY = -1;
	int nearestBridgeIndex = -1;

	// Find the LOWEST bridge above the player
	for (int i = 0; i < numBridges; i++) {
		if (cx >= bridges[i].minX && cx <= bridges[i].maxX &&
			bridges[i].snapY > playerY) {
			if (nearestBridgeY == -1 || bridges[i].snapY < nearestBridgeY) {
				nearestBridgeY = bridges[i].snapY;
				nearestBridgeIndex = i;
			}
		}
	}

	// If no bridge above, don't block
	if (nearestBridgeY == -1)
		return false;
	if (nearestBridgeIndex == 3 ||
		nearestBridgeIndex == 4) // Bridge 4 or Bridge 5
	{
		// Check if player is horizontally between bridge 3 and bridge 6
		int bridge3End = bridges[2].maxX;   // End of bridge 3
		int bridge6Start = bridges[5].minX; // Start of bridge 6

		if (cx > bridge3End && cx < bridge6Start) {
			// Player is in the gap - allow movement
			return false;
		}
	}

	if (nearestBridgeIndex == 6 ||
		nearestBridgeIndex == 7) // Bridge 7 or Bridge 8
	{
		// Check if player is horizontally between bridge 6 and bridge 9
		int bridge6End = bridges[5].maxX;   // End of bridge 6
		int bridge9Start = bridges[8].minX; // Start of bridge 9

		if (cx > bridge6End && cx < bridge9Start) {
			// Player is in the gap - allow movement
			return false;
		}
	}

	// Block movement ONLY if player is clearly below the bridge
	if (playerY < nearestBridgeY - 20)
		return true;

	return false;
}

bool isOnBridge() {
	if (currentBg != 4)
		return false;

	int cx = playerX + PLAYER_WIDTH / 2;

	for (int i = 0; i < numBridges; i++) {
		if (cx >= bridges[i].minX && cx <= bridges[i].maxX &&
			abs(playerY - bridges[i].snapY) <= 6 &&
			playerY >= bridges[i].snapY - 6) {
			return true;
		}
	}
	return false;
}

void updateJump() {

	// ---------------- LEVEL 1 ----------------
	if (currentBg == 4) {

		if (!playerAlive)
			return;

		int groundY = BG41_GROUND_Y;
		bool onBridge = false;
		float prevY = playerY;

		playerY += jumpVelocity;
		jumpVelocity -= GRAVITY;

		// -------- BRIDGE LANDING --------
		if (jumpVelocity < 0) {
			for (int i = 0; i < numBridges; i++) {
				int cx = playerX + PLAYER_WIDTH / 2;

				if (cx >= bridges[i].minX && cx <= bridges[i].maxX &&
					prevY >= bridges[i].snapY && playerY <= bridges[i].snapY) {

					playerY = bridges[i].snapY;
					isJumping = false;
					jumpVelocity = 0;
					onBridge = true;
					break;
				}
			}
		}

		// -------- LOCK PLAYER TO BRIDGE --------
		if (!isJumping) {
			int cx = playerX + PLAYER_WIDTH / 2;

			for (int i = 0; i < numBridges; i++) {
				if (cx >= bridges[i].minX && cx <= bridges[i].maxX &&
					abs(playerY - bridges[i].snapY) <= 6) {

					playerY = bridges[i].snapY;
					jumpVelocity = 0;
					break;
				}
			}
		}

		// -------- GROUND COLLISION --------
		if (playerY <= groundY) {
			playerY = groundY;
			isJumping = false;
			jumpVelocity = 0;
		}

		updateCamera();
		checkCollectibleCollisions();
	}
	else if (currentBg == 10) {
		// -------- CAMERA --------
		lv2CameraX = lv2CharX - SCREEN_WIDTH / 2;
		if (lv2CameraX < 0)
			lv2CameraX = 0;
		if (lv2CameraX > LV2_BG_WIDTH - SCREEN_WIDTH)
			lv2CameraX = LV2_BG_WIDTH - SCREEN_WIDTH;

		// -------- JUMP PHYSICS --------
		if (lv2IsJumping) {
			float prevY = lv2CharY;
			lv2CharY += lv2JumpVelocity;
			lv2JumpVelocity -= 0.6f;

			// Bridge landing — only when falling
			if (lv2JumpVelocity < 0) {
				for (int i = 0; i < numBridges2; i++) {
					int cx = lv2CharX + PLAYER_WIDTH / 2;
					if (cx >= bridges2[i].minX && cx <= bridges2[i].maxX &&
						prevY >= bridges2[i].snapY && lv2CharY <= bridges2[i].snapY) {
						lv2CharY = bridges2[i].snapY;
						lv2IsJumping = false;
						lv2JumpVelocity = 0;
						break;
					}
				}
			}

			// Fall to y=0 (no ground — pit death)
			if (lv2CharY <= 0) {
				lv2CharY = 0;
				lv2IsJumping = false;
				lv2JumpVelocity = 0;
				// TODO: trigger death/respawn here if desired
			}
		}
		// -------- LOCK ON BRIDGE WHEN NOT JUMPING --------
		if (!lv2IsJumping) {
			int cx = lv2CharX + PLAYER_WIDTH / 2;
			bool onBridge2 = false;
			for (int i = 0; i < numBridges2; i++) {
				if (cx >= bridges2[i].minX && cx <= bridges2[i].maxX &&
					abs(lv2CharY - bridges2[i].snapY) <= 8) {
					lv2CharY = bridges2[i].snapY;
					onBridge2 = true;
					break;
				}
			}

			// NOT on any bridge and NOT at y=0 — start falling
			if (!onBridge2 && lv2CharY > 0) {
				lv2IsJumping = true;
				lv2JumpVelocity = 0; // fall with no initial velocity
			}
		}
		checkCollectibleCollisionsLv2();
	}
	else if (currentBg == 12) {
		// -------- CAMERA --------
		lv3CameraX = lv3CharX - SCREEN_WIDTH / 2;
		if (lv3CameraX < 0)
			lv3CameraX = 0;
		if (lv3CameraX > LV3_BG_WIDTH - SCREEN_WIDTH)
			lv3CameraX = LV3_BG_WIDTH - SCREEN_WIDTH;

		// -------- JUMP PHYSICS --------
		if (lv3IsJumping) {
			float prevY = lv3CharY;
			lv3CharY += lv3JumpVelocity;
			lv3JumpVelocity -= 0.6f;

			// Bridge landing — only when falling
			if (lv3JumpVelocity < 0) {
				for (int i = 0; i < numBridges3; i++) {
					int cx = lv3CharX + PLAYER_WIDTH / 2;
					if (cx >= bridges3[i].minX && cx <= bridges3[i].maxX &&
						prevY >= bridges3[i].snapY && lv3CharY <= bridges3[i].snapY) {
						lv3CharY = bridges3[i].snapY;
						lv3IsJumping = false;
						lv3JumpVelocity = 0;
						break;
					}
				}
			}

			// Fall to ground
			if (lv3CharY <= lv3GroundY) {
				lv3CharY = lv3GroundY;
				lv3IsJumping = false;
				lv3JumpVelocity = 0;
			}
		}

		// -------- LOCK ON BRIDGE WHEN NOT JUMPING --------
		if (!lv3IsJumping) {
			int cx = lv3CharX + PLAYER_WIDTH / 2;
			bool onBridge3 = false;
			for (int i = 0; i < numBridges3; i++) {
				if (cx >= bridges3[i].minX && cx <= bridges3[i].maxX &&
					abs(lv3CharY - bridges3[i].snapY) <= 8) {
					lv3CharY = bridges3[i].snapY;
					onBridge3 = true;
					break;
				}
			}
			// Not on any bridge and above ground — start falling
			if (!onBridge3 && lv3CharY > lv3GroundY) {
				lv3IsJumping = true;
				lv3JumpVelocity = 0;
			}
		}
	}
}

/* -------------------- CHARGED TIMER -------------------- */
void updateChargedTimer() {
	if (!chargingPunch)
		return;
	chargedTimeCounter += 100;
	if (chargedTimeCounter >= CHARGED_TIME_LIMIT) {
		chargingPunch = false;
		chargedSequenceActive = false;
		chargedPunchState = 0;
		chargedTimeCounter = 0;
	}
}

/* -------------------- MUSIC -------------------- */
void playMusic() {
	mciSendString("stop music", NULL, 0, NULL);
	mciSendString("close music", NULL, 0, NULL);
	mciSendString("open \"Audios/music.mp3\" type mpegvideo alias music", NULL, 0,
		NULL);
	mciSendString("play music repeat", NULL, 0, NULL);
}

void restartMusic() {
	mciSendString("stop music", NULL, 0, NULL);
	mciSendString("seek music to start", NULL, 0, NULL);
	mciSendString("play music repeat", NULL, 0, NULL);
}
void initPunchSound() {
	mciSendString("close punch", NULL, 0, NULL);
	mciSendString("open \"Audios/punch.mp3\" type mpegvideo alias punch", NULL, 0,
		NULL);
}

void playPunchSound() {
	// Stop any currently playing punch sound, seek to start, play
	mciSendString("stop punch", NULL, 0, NULL);
	mciSendString("seek punch to start", NULL, 0, NULL);

	// Set volume (0-1000 scale)
	char volCmd[64];
	sprintf_s(volCmd, "setaudio punch volume to %d", punchVolume);
	mciSendString(volCmd, NULL, 0, NULL);

	mciSendString("play punch", NULL, 0,
		NULL); // no "repeat" - punch sound plays once
}

void drawHUD() {
	if ((currentBg != 4 && currentBg != 10) || playerWon || playerWonLv2)
		return;

	int hudW = 300; // Longer
	int hudH = 25;  // Thicker
	int margin = 20;
	int gap = 20;      // Increased gap for larger icons
	int iconSize = 45; // Bigger icons
	int startX = SCREEN_WIDTH - hudW - margin;
	int startY = SCREEN_HEIGHT - margin - hudH;

	// Icons (Heart/Life)
	int lifeIconX =
		(currentBg == 10) ? (startX - iconSize + 10) : (startX - iconSize - 2);
	iShowImage(lifeIconX, startY - (iconSize - hudH) / 2, iconSize, iconSize,
		hudLifeImg);

	// Background for Life Bar
	iSetColor(40, 40, 40);
	iFilledRectangle(startX, startY, hudW, hudH);

	// Life Bar (Deep Red)
	float lifeRatio = (float)playerLives / MAX_LIVES;
	if (lifeRatio > 1.0f)
		lifeRatio = 1.0f;
	if (lifeRatio > 0) {
		iSetColor(180, 0, 0); // Deep Red
		iFilledRectangle(startX, startY, (int)(hudW * lifeRatio), hudH);
	}

	// Border for Life bar (Black)
	iSetColor(0, 0, 0);
	iRectangle(startX, startY, hudW, hudH);

	// Background for Charged Punch Bar
	int punchY = startY - hudH - gap;

	// Icons (Punch)
	int punchIconX =
		(currentBg == 10) ? (startX - iconSize + 10) : (startX - iconSize - 2);
	iShowImage(punchIconX, punchY - (iconSize - hudH) / 2, iconSize, iconSize,
		hudPunchImg);

	iSetColor(40, 40, 40);
	iFilledRectangle(startX, punchY, hudW, hudH);

	// Charged Punch Bar (Deeper Blue)
	int remainingPunches = MAX_CHARGED_USES - chargedUseCount;
	if (remainingPunches < 0)
		remainingPunches = 0;
	float punchRatio = (float)remainingPunches / MAX_CHARGED_USES;
	if (punchRatio > 0) {
		iSetColor(0, 0, 120); // Deeper Blue
		iFilledRectangle(startX, punchY, (int)(hudW * punchRatio), hudH);
	}

	// Border for Punch bar (Black)
	iSetColor(0, 0, 0);
	iRectangle(startX, punchY, hudW, hudH);

	// ---------------- TOKEN BAR (LEVEL 1: TOP-LEFT, LEVEL 2: TOP-RIGHT BELOW
	// PUNCH) ----------------
	int tokenStartX = (currentBg == 10) ? startX : margin;
	int tokenStartY = (currentBg == 10) ? (punchY - hudH - gap)
		: (SCREEN_HEIGHT - margin - hudH);
	int tokenIconSize = 45;

	int currentTokenCount = (currentBg == 10) ? tokenCountLv2 : tokenCount;
	int currentMaxTokens = (currentBg == 10) ? 6 : MAX_TOKENS;
	int currentTokenImg = (currentBg == 10) ? tokenImgLv2 : tokenImg;

	// Background for Token Bar
	iSetColor(40, 40, 40);
	iFilledRectangle(tokenStartX, tokenStartY, hudW, hudH);

	float tokenRatio = (float)currentTokenCount / currentMaxTokens;
	if (tokenRatio > 1.0f)
		tokenRatio = 1.0f;
	if (tokenRatio > 0) {
		if (currentBg == 10)
			iSetColor(150, 0, 255); // Purple for Level 2 tokens
		else
			iSetColor(0, 0, 0); // Black Bar for Level 1
		iFilledRectangle(tokenStartX, tokenStartY, (int)(hudW * tokenRatio), hudH);
	}

	// Token Count Text (Centered in bar)
	char tokenStr[20];
	sprintf_s(tokenStr, "%d/%d", currentTokenCount, currentMaxTokens);
	iSetColor(255, 255, 255);
	int textX = tokenStartX + (hudW - (int)strlen(tokenStr) * 10) / 2;
	int textY = tokenStartY + (hudH - 18) / 2;
	iText(textX, textY, tokenStr, GLUT_BITMAP_HELVETICA_18);

	// Border for Token bar (Black)
	iSetColor(0, 0, 0);
	iRectangle(tokenStartX, tokenStartY, hudW, hudH);

	// Icons (Token) - Slightly overlap in Level 2
	int tokenIconX;
	if (currentBg == 10) {
		tokenIconX = tokenStartX - tokenIconSize + 10; // Overlap on the left
	}
	else {
		tokenIconX = tokenStartX + hudW + 7; // Level 1 is on the right
	}
	iShowImage(tokenIconX, tokenStartY - (tokenIconSize - hudH) / 2,
		tokenIconSize, tokenIconSize, currentTokenImg);

	// ---------------- TREASURE BAR (BELOW TOKEN BAR - ONLY LEVEL 1)
	// ----------------
	if (currentBg == 4) {
		int treasureStartY = tokenStartY - hudH - gap;

		// Background for Treasure Bar
		iSetColor(40, 40, 40);
		iFilledRectangle(tokenStartX, treasureStartY, hudW, hudH);

		// Treasure Bar Fill (Deep Purple/Electric Purple)
		float treasureRatio = (float)treasureCount / MAX_TREASURES;
		if (treasureRatio > 1.0f)
			treasureRatio = 1.0f;
		if (treasureRatio > 0) {
			iSetColor(120, 0, 120); // Electric Purple
			iFilledRectangle(tokenStartX, treasureStartY, (int)(hudW * treasureRatio),
				hudH);
		}

		// Treasure Count Text (Centered in bar)
		char treasureStr[20];
		sprintf_s(treasureStr, "%d/%d", treasureCount, MAX_TREASURES);
		iSetColor(255, 255, 255);
		int tTextX = tokenStartX + (hudW - (int)strlen(treasureStr) * 10) / 2;
		int tTextY = treasureStartY + (hudH - 18) / 2;
		iText(tTextX, tTextY, treasureStr, GLUT_BITMAP_HELVETICA_18);

		// Border for Treasure bar (Black)
		iSetColor(0, 0, 0);
		iRectangle(tokenStartX, treasureStartY, hudW, hudH);

		// Icons (Treasure) - Mirroring Token Bar style
		int treasureIconSize = 65;
		iShowImage(tokenStartX + hudW + 2,
			treasureStartY - (treasureIconSize - hudH) / 2, treasureIconSize,
			treasureIconSize, treasureImg);
	}
}

// In checkBoss1PlayerCollision(), fix positions to match draw:
void checkBoss1PlayerCollision() {
	if (currentBg != 10)
		return;
	if (!boss1Active || boss1Dead)
		return;
	if (!playerAlive)
		return;
	if (boss1Moving)
		return; // can't hurt player while still approaching

	bool attackFrameActive = (boss1FrameIndex >= 20 && boss1FrameIndex <= 26);
	if (!attackFrameActive)
		return;

	int bossW = PLAYER_WIDTH + 250;
	int bossH = PLAYER_HEIGHT + 150;
	int bossX = (int)boss1X;
	int bossY = bridges2[6].snapY;

	int eX1 = bossX - 20;
	int eX2 = bossX + bossW - 80;
	int eY1 = bossY + 20;
	int eY2 = bossY + bossH - 20;

	int pX1 = lv2CharX;
	int pX2 = lv2CharX + PLAYER_WIDTH;
	int pY1 = lv2CharY;
	int pY2 = lv2CharY + PLAYER_HEIGHT;

	bool overlapX = !(pX2 < eX1 || pX1 > eX2);
	bool overlapY = !(pY2 < eY1 || pY1 > eY2);

	bool invincible =
		(chargingPunch && chargeScrollDismissed) || chargedSequenceActive;

	if (overlapX && overlapY && !invincible) {
		static int hitCooldown = 0;
		if (hitCooldown > 0) {
			hitCooldown--;
			return;
		}
		hitCooldown = 40;

		boss1HitCount++;
		killedByBoss1 = true;
		playerDeathY = bridges2[6].snapY;
		lv2HitPlayer(2);
	}
}
// In checkBoss1ChargedHit(), same bossX fix:
void checkBoss1ChargedHit() {
	if (currentBg != 10)
		return;
	if (!boss1Active || boss1Dead)
		return;

	static int cooldown = 0;
	if (cooldown > 0) {
		cooldown--;
		return;
	}

	bool punchActive = (punchState == 2) || chargedSequenceActive;
	if (!punchActive)
		return;

	bool bossAttacking =
		(!boss1Moving) && (boss1FrameIndex >= 20 && boss1FrameIndex <= 26);
	if (bossAttacking)
		return;

	int bossW = PLAYER_WIDTH + 250;
	int bossX = (int)boss1X;

	int eX1 = bossX - 50;
	int eX2 = bossX + bossW;
	int pX1 = lv2CharX;
	int pX2 = lv2CharX + PLAYER_WIDTH + 200;

	bool overlapX = !(pX2 < eX1 || pX1 > eX2);

	if (overlapX) {
		cooldown = 30;
		punchState = 0;
		punchJustLanded = false;
		boss1ChargedHits++;
		if (boss1ChargedHits >= 5) {
			boss1Dead = true;
			boss1Active = false;
		}
	}
}
void checkShieldCollision() {
	if (currentBg != 10)
		return;
	if (!playerAlive)
		return;
	if (playerWonLv2)
		return;

	Bridge &b9 = bridges2[8];
	int shieldX = (b9.minX + b9.maxX) / 2 + 10;
	int shieldY = b9.snapY + 30;
	int shieldW = 80, shieldH = 80;

	int pX1 = lv2CharX;
	int pX2 = lv2CharX + PLAYER_WIDTH;
	int pY1 = lv2CharY;
	int pY2 = lv2CharY + PLAYER_HEIGHT;

	bool overlapX = !(pX2 < shieldX || pX1 > shieldX + shieldW);
	bool overlapY = !(pY2 < shieldY || pY1 > shieldY + shieldH);

	if (overlapX && overlapY) {
		playerWonLv2 = true;
		gameStarted = false;
	}
}

/* =====================================================
LVL2 ENEMY 1  —  flies over Bridge 3 (bridges2[2])
===================================================== */

void updateLv2Enemy1() {
	if (currentBg != 10)
		return;

	// Advance animation
	static int animTick = 0;
	if (++animTick >= 3) {
		animTick = 0;
		lv2e1FrameIndex = (lv2e1FrameIndex + 1) % LV2E1FRAMECOUNT;
	}

	// Dead-fall physics
	if (!lv2e1Alive && lv2e1Falling) {
		lv2e1DeadVel -= 1.2f;
		lv2e1DeadY += lv2e1DeadVel;
		if (lv2e1DeadY <= 0) {
			lv2e1DeadY = 0;
			lv2e1Falling = false;
		}
		return;
	}
	if (!lv2e1Alive)
		return;

	// Activate once player steps onto bridge 3
	int cx = lv2CharX + PLAYER_WIDTH / 2;
	if (cx >= bridges2[2].minX && cx <= bridges2[2].maxX)
		lv2e1Active = true;

	if (!lv2e1Active || !playerAlive)
		return;

	// Bounce between left / right edge of bridge 3
	float leftBound = (float)bridges2[2].minX;
	float rightBound = (float)(bridges2[2].maxX - LV2E1WIDTH);

	if (lv2e1MovingRight) {
		lv2e1X += LV2E1_SPEED;
		if (lv2e1X >= rightBound) {
			lv2e1X = rightBound;
			lv2e1MovingRight = false;
		}
	}
	else {
		lv2e1X -= LV2E1_SPEED;
		if (lv2e1X <= leftBound) {
			lv2e1X = leftBound;
			lv2e1MovingRight = true;
		}
	}
}

void checkLv2Enemy1Hits() {
	if (currentBg != 10)
		return;
	if (!lv2e1Alive || !lv2e1Active)
		return;

	static bool lv2e1WasColliding = false;

	int cx = lv2CharX + PLAYER_WIDTH / 2;
	bool playerOnBridge3 = (cx >= bridges2[2].minX && cx <= bridges2[2].maxX);

	if (!playerOnBridge3) {
		lv2e1WasColliding = false;
		return;
	}

	int enemyWorldX = (int)lv2e1X;
	int enemyY = bridges2[2].snapY + LV2E1YOFFSET;

	// ── TIGHTENED enemy hitbox: crop out transparent edges ──
	// From debug: enemy image is 180px wide, 180px tall
	// Visible body is roughly center 60% horizontally, bottom 50% vertically
	int eX1 = enemyWorldX + (int)(LV2E1WIDTH * 0.20f); // cut 20% from left
	int eX2 = enemyWorldX + (int)(LV2E1WIDTH * 0.80f); // cut 20% from right
	int eY1 = enemyY + (int)(LV2E1HEIGHT * 0.50f); // cut top 50% (transparent)
	int eY2 = enemyY + (int)(LV2E1HEIGHT * 0.90f); // cut bottom 10%

	// ── TIGHTENED player hitbox: visible body only ──────────
	int pX1 = lv2CharX + (int)(PLAYER_WIDTH * 0.05f);
	int pX2 = lv2CharX + (int)(PLAYER_WIDTH * 0.25f);
	int pY1 = lv2CharY + (int)(PLAYER_HEIGHT * 0.40f); // waist up
	int pY2 = lv2CharY + (int)(PLAYER_HEIGHT * 0.85f); // feet

	bool overlapX = !(pX2 < eX1 || pX1 > eX2);
	bool overlapY = !(pY2 < eY1 || pY1 > eY2);
	bool isColliding = overlapX && overlapY;

	// ── Body contact / release ───────────────────────────────
	if (isColliding && !lv2e1WasColliding) {
		killedByLv2e1 = true;
		playerDeathY = bridges2[2].snapY;
		lv2HitPlayer(1); // 1 life
	}
	if (!isColliding && lv2e1WasColliding) {
	}
	lv2e1WasColliding = isColliding;

	// Consume any queued punch while kneeling
	if (showKneel) {
		punchJustLanded = false;
		return; // block ALL punch logic while in contact
	}

	// ── Normal punch — only 3 hits, no charged punch ─────────
	if (punchJustLanded && playerAlive) {
		int punchReach = 70;
		int ppX1, ppX2;
		if (movingFront) {
			ppX1 = lv2CharX + PLAYER_WIDTH - 20;
			ppX2 = lv2CharX + PLAYER_WIDTH + punchReach;
		}
		else {
			ppX1 = lv2CharX - punchReach;
			ppX2 = lv2CharX + 70;
		}
		int ppY1 = lv2CharY + 30;
		int ppY2 = lv2CharY + PLAYER_HEIGHT;

		// Punch hitbox vs same tightened enemy box
		bool pOverlapX = !(ppX2 < eX1 || ppX1 > eX2);
		bool pOverlapY = !(ppY2 < eY1 || ppY1 > eY2);

		if (pOverlapX && pOverlapY) {
			punchJustLanded = false;
			lv2e1HitCount++;
			if (lv2e1HitCount >= 3) {
				lv2e1Alive = false;
				lv2e1Falling = true;
				lv2e1DeadVel = 0.f;
				lv2e1DeadY = (float)enemyY;
				showKneel = false;    // ← ADD
				kneelHolding = false; // ← ADD
				kneelHoldCounter = 0; // ← ADD
				lv2e1WasColliding = false;
			}
		}
	}
}

// LVL2 ENEMY 2: flies over Bridge 5 (bridges2[4])
void updateLv2Enemy2() {
	if (currentBg != 10)
		return;

	// Advance animation (independent of movement)
	static int animTick2 = 0;
	if (++animTick2 >= 3) { // tweak 3 to speed/slow animation
		animTick2 = 0;
		lv2e2FrameIndex++;
		if (lv2e2FrameIndex >= LV2E2FRAMECOUNT)
			lv2e2FrameIndex = 0;
	}

	// Dead‑fall physics
	if (!lv2e2Alive) {
		if (lv2e2Falling) {
			lv2e2DeadVel -= 1.2f;
			lv2e2DeadY += lv2e2DeadVel;
			if (lv2e2DeadY <= 0) {
				lv2e2DeadY = 0;
				lv2e2Falling = false;
			}
		}
		return;
	}

	if (!playerAlive)
		return; // keep only this

	// Bounce between left/right edge of bridge 5
	float leftBound = (float)bridges2[LV2E2BRIDGEINDEX].minX;
	float rightBound = (float)bridges2[LV2E2BRIDGEINDEX].maxX - LV2E2WIDTH;

	if (lv2e2MovingRight) {
		lv2e2X += LV2E2SPEED;
		if (lv2e2X >= rightBound) {
			lv2e2X = rightBound;
			lv2e2MovingRight = false;
		}
	}
	else {
		lv2e2X -= LV2E2SPEED;
		if (lv2e2X <= leftBound) {
			lv2e2X = leftBound;
			lv2e2MovingRight = true;
		}
	}
}

void checkLv2Enemy2Hits() {
	if (currentBg != 10)
		return;
	if (!lv2e2Alive)
		return;
	if (!playerAlive)
		return;

	static bool lv2e2WasColliding = false;

	int cx = lv2CharX + PLAYER_WIDTH / 2;
	bool playerOnBridge5 = (cx >= bridges2[LV2E2BRIDGEINDEX].minX &&
		cx <= bridges2[LV2E2BRIDGEINDEX].maxX);

	if (!playerOnBridge5) {
		lv2e2WasColliding = false;
		return;
	}

	int enemyWorldX = (int)lv2e2X;
	int enemyY = bridges2[LV2E2BRIDGEINDEX].snapY + LV2E2YOFFSET;

	int eX1 = enemyWorldX + (int)(LV2E2WIDTH * LV2E2HBLEFT);
	int eX2 = enemyWorldX + (int)(LV2E2WIDTH * LV2E2HBRIGHT);
	int eY1 = enemyY + (int)(LV2E2HEIGHT * 0.15f);
	int eY2 = enemyY + (int)(LV2E2HEIGHT * 0.75f);

	// ── BODY CONTACT / RELEASE (mirrors lv2e1 exactly) ──────────────
	int pX1 = lv2CharX + (int)(PLAYER_WIDTH * 0.05f);
	int pX2 = lv2CharX + (int)(PLAYER_WIDTH * 0.25f);
	int pY1 = lv2CharY + (int)(PLAYER_HEIGHT * 0.40f);
	int pY2 = lv2CharY + (int)(PLAYER_HEIGHT * 0.85f);

	bool overlapX = !(pX2 < eX1 || pX1 > eX2);
	bool overlapY = !(pY2 < eY1 || pY1 > eY2);
	bool isColliding = overlapX && overlapY;

	if (isColliding && !lv2e2WasColliding) {
		killedByLv2e2 = true;
		playerDeathY = bridges2[LV2E2BRIDGEINDEX].snapY;
		lv2HitPlayer(2); // 2 lives
	}
	if (!isColliding && lv2e2WasColliding) {
	}
	lv2e2WasColliding = isColliding;

	// Block ALL punch logic while in contact — same as enemy1
	if (showKneel) {
		punchJustLanded = false;
		return;
	}

	// ── NORMAL PUNCH (4 hits to kill) ────────────────────────────────
	if (punchJustLanded && playerAlive) {
		int punchReach = 70;
		int ppX1, ppX2;
		if (movingFront) {
			ppX1 = lv2CharX + PLAYER_WIDTH - 20;
			ppX2 = lv2CharX + PLAYER_WIDTH + punchReach;
		}
		else {
			ppX1 = lv2CharX - punchReach;
			ppX2 = lv2CharX + 70;
		}
		int ppY1 = lv2CharY + 30;
		int ppY2 = lv2CharY + PLAYER_HEIGHT;

		bool pOverlapX = !(ppX2 < eX1 || ppX1 > eX2);
		bool pOverlapY = !(ppY2 < eY1 || ppY1 > eY2);

		if (pOverlapX && pOverlapY) {
			punchJustLanded = false;
			lv2e2HitCount++;
			if (lv2e2HitCount >= LV2E2HITSTOKILL) {
				lv2e2Alive = false;
				lv2e2Falling = true;
				lv2e2DeadVel = 0.f;
				lv2e2DeadY = (float)enemyY;
				showKneel = false;    // ← ADD
				kneelHolding = false; // ← ADD
				kneelHoldCounter = 0; // ← ADD
				lv2e2WasColliding = false;
			}
		}
	}

	// ── CHARGED PUNCH (instant kill) ─────────────────────────────────
	if (chargedSequenceActive && playerAlive) {
		int ppX1 = lv2CharX - 120;
		int ppX2 = lv2CharX + PLAYER_WIDTH + 120;
		int ppY1 = lv2CharY + 20;
		int ppY2 = lv2CharY + PLAYER_HEIGHT;

		bool pOverlapX = !(ppX2 < eX1 || ppX1 > eX2);
		bool pOverlapY = !(ppY2 < eY1 || ppY1 > eY2);

		if (pOverlapX && pOverlapY) {
			lv2e2Alive = false;
			lv2e2Falling = true;
			lv2e2DeadVel = 0.f;
			lv2e2DeadY = (float)enemyY;
			showKneel = false;    // ← ADD
			kneelHolding = false; // ← ADD
			kneelHoldCounter = 0; // ← ADD
			lv2e2WasColliding = false;
			return;
		}
	}
}
/* =====================================================
LVL2 WALK ENEMY  —  stationary on Bridge 3 left edge
===================================================== */
void updateLv2WkEnemy() {
	if (currentBg != 10)
		return;
	if (!lv2wkAlive)
		return;

	// ── Animate frames ──────────────────────────────────────
	static int wkAnimTick = 0;
	if (++wkAnimTick >= 4) { // ← change 4 to speed up/slow down animation
		wkAnimTick = 0;
		lv2wkFrameIndex = (lv2wkFrameIndex + 1) % LV2WK_FRAMECOUNT;
	}

	// ── Kneel timer: count down after contact, then release ──
	if (lv2wkKneeling) {
		lv2wkKneelTimer++;
		if (lv2wkKneelTimer >= LV2WK_KNEEL_DURATION) {
			lv2wkKneeling = false;
			lv2wkKneelTimer = 0;
			kneelHolding = false; // ← ADD
			kneelHoldCounter = 0; // ← ADD
			showKneel = false;    // ← ADD
		}
		return; // skip collision check while already kneeling
	}

	if (!playerAlive)
		return;

	// ── Enemy hitbox (tightened to visible body) ─────────────
	int eX1 = (int)lv2wkX + (int)(LV2WK_WIDTH * LV2WK_HB_LEFT);
	int eX2 = (int)lv2wkX + (int)(LV2WK_WIDTH * LV2WK_HB_RIGHT);
	int eY1 = (int)lv2wkY + (int)(LV2WK_HEIGHT * LV2WK_HB_TOP);
	int eY2 = (int)lv2wkY + (int)(LV2WK_HEIGHT * LV2WK_HB_BOTTOM);

	// ── Player hitbox ────────────────────────────────────────
	int pX1 = lv2CharX + (int)(PLAYER_WIDTH * 0.10f);
	int pX2 = lv2CharX + (int)(PLAYER_WIDTH * 0.90f);
	int pY1 = lv2CharY + (int)(PLAYER_HEIGHT * 0.10f);
	int pY2 = lv2CharY + (int)(PLAYER_HEIGHT * 0.90f);

	bool overlapX = !(pX2 < eX1 || pX1 > eX2);
	bool overlapY = !(pY2 < eY1 || pY1 > eY2);
	bool isColliding = overlapX && overlapY;

	if (isColliding && !lv2wkPlayerTouching) {
		lv2wkPlayerTouching = true;
		lv2wkKneeling = true;
		lv2wkKneelTimer = 0;
		punchJustLanded = false;
		killedByLv2wk = true;
		playerDeathY = (int)lv2wkY;
		lv2HitPlayer(1); // 1 life
	}

	if (!isColliding) {
		lv2wkPlayerTouching = false;
	}
}

/* =====================================================
LVL2 WALK ENEMY 2  —  stationary on Bridge 3 right edge
===================================================== */
void updateLv2WkEnemy2() {
	if (currentBg != 10)
		return;
	if (!lv2wk2Alive)
		return;

	// ── Animate frames ──────────────────────────────────────
	static int wk2AnimTick = 0;
	if (++wk2AnimTick >= 4) { // ← change 4 to speed up/slow down animation
		wk2AnimTick = 0;
		lv2wk2FrameIndex = (lv2wk2FrameIndex + 1) % LV2WK2_FRAMECOUNT;
	}

	// ── Kneel timer: count down after contact, then release ──
	if (lv2wk2Kneeling) {
		lv2wk2KneelTimer++;
		if (lv2wk2KneelTimer >= LV2WK2_KNEEL_DURATION) {
			lv2wk2Kneeling = false;
			lv2wk2KneelTimer = 0;
		}
		return; // skip collision check while already kneeling
	}

	if (!playerAlive)
		return;

	// ── Enemy hitbox (tightened to visible body) ─────────────
	int eX1 = (int)lv2wk2X + (int)(LV2WK2_WIDTH * LV2WK2_HB_LEFT);
	int eX2 = (int)lv2wk2X + (int)(LV2WK2_WIDTH * LV2WK2_HB_RIGHT);
	int eY1 = (int)lv2wk2Y + (int)(LV2WK2_HEIGHT * LV2WK2_HB_TOP);
	int eY2 = (int)lv2wk2Y + (int)(LV2WK2_HEIGHT * LV2WK2_HB_BOTTOM);

	// ── Player hitbox ────────────────────────────────────────
	int pX1 = lv2CharX + (int)(PLAYER_WIDTH * 0.10f);
	int pX2 = lv2CharX + (int)(PLAYER_WIDTH * 0.90f);
	int pY1 = lv2CharY + (int)(PLAYER_HEIGHT * 0.10f);
	int pY2 = lv2CharY + (int)(PLAYER_HEIGHT * 0.90f);

	bool overlapX = !(pX2 < eX1 || pX1 > eX2);
	bool overlapY = !(pY2 < eY1 || pY1 > eY2);
	bool isColliding = overlapX && overlapY;

	if (isColliding && !lv2wk2PlayerTouching) {
		lv2wk2PlayerTouching = true;
		lv2wk2Kneeling = true;
		lv2wk2KneelTimer = 0;
		punchJustLanded = false;
		killedByLv2wk2 = true;
		playerDeathY = (int)lv2wk2Y;
		lv2HitPlayer(1); // 1 life
	}

	if (!isColliding) {
		lv2wk2PlayerTouching = false;
	}
}

void updateLv2WkEnemy3() {
	if (currentBg != 10)
		return;
	if (!lv2wk3Alive)
		return;

	static int wk3AnimTick = 0;
	if (++wk3AnimTick >= 4) {
		wk3AnimTick = 0;
		lv2wk3FrameIndex = (lv2wk3FrameIndex + 1) % LV2WK3_FRAMECOUNT;
	}

	if (lv2wk3Kneeling) {
		lv2wk3KneelTimer++;
		if (lv2wk3KneelTimer >= LV2WK3_KNEEL_DURATION) {
			lv2wk3Kneeling = false;
			lv2wk3KneelTimer = 0;
		}
		return;
	}

	if (!playerAlive)
		return;

	int eX1 = (int)lv2wk3X + (int)(LV2WK3_WIDTH * LV2WK3_HB_LEFT);
	int eX2 = (int)lv2wk3X + (int)(LV2WK3_WIDTH * LV2WK3_HB_RIGHT);
	int eY1 = (int)lv2wk3Y + (int)(LV2WK3_HEIGHT * LV2WK3_HB_TOP);
	int eY2 = (int)lv2wk3Y + (int)(LV2WK3_HEIGHT * LV2WK3_HB_BOTTOM);

	int pX1 = lv2CharX + (int)(PLAYER_WIDTH * 0.10f);
	int pX2 = lv2CharX + (int)(PLAYER_WIDTH * 0.90f);
	int pY1 = lv2CharY + (int)(PLAYER_HEIGHT * 0.10f);
	int pY2 = lv2CharY + (int)(PLAYER_HEIGHT * 0.90f);

	bool overlapX = !(pX2 < eX1 || pX1 > eX2);
	bool overlapY = !(pY2 < eY1 || pY1 > eY2);
	bool isColliding = overlapX && overlapY;

	if (isColliding && !lv2wk3PlayerTouching) {
		lv2wk3PlayerTouching = true;
		lv2wk3Kneeling = true;
		lv2wk3KneelTimer = 0;
		punchJustLanded = false;
		killedByLv2wk3 = true;
		playerDeathY = (int)lv2wk3Y;
		lv2HitPlayer(1); // 1 life
	}

	if (!isColliding) {
		lv2wk3PlayerTouching = false;
	}
}

void updateLv2Fires() {
	if (currentBg != 10)
		return;

	static int fireAnimTick = 0;
	if (++fireAnimTick >= 4) {
		fireAnimTick = 0;
		fireAnimIndex = (fireAnimIndex + 1) % FIRE_FRAMECOUNT;
	}

	if (!playerAlive)
		return;

	int fireW[NUM_FIRES] = { FIRE0_W, FIRE1_W, FIRE2_W, FIRE3_W, FIRE4_W };
	int fireH[NUM_FIRES] = { FIRE0_H, FIRE1_H, FIRE2_H, FIRE3_H, FIRE4_H };

	for (int i = 0; i < numFires; i++) {
		FireObstacle &f = fires[i];

		if (f.kneeling) {
			f.kneelTimer++;
			if (f.kneelTimer >= FIRE_KNEEL_DURATION) {
				f.kneeling = false;
				f.kneelTimer = 0;
				f.playerTouching = false;
				kneelHolding = false;
				kneelHoldCounter = 0;
				showKneel = false;
			}
			continue;
		}

		// Hitbox using per-fire size
		int eX1 = (int)f.x + (int)(fireW[i] * FIRE_HB_LEFT);
		int eX2 = (int)f.x + (int)(fireW[i] * FIRE_HB_RIGHT);
		int eY1 = (int)f.y + (int)(fireH[i] * FIRE_HB_TOP);
		int eY2 = (int)f.y + (int)(fireH[i] * FIRE_HB_BOTTOM);

		int pX1 = lv2CharX + (int)(PLAYER_WIDTH * 0.10f);
		int pX2 = lv2CharX + (int)(PLAYER_WIDTH * 0.90f);
		int pY1 = lv2CharY + (int)(PLAYER_HEIGHT * 0.10f);
		int pY2 = lv2CharY + (int)(PLAYER_HEIGHT * 0.90f);

		bool overlapX = !(pX2 < eX1 || pX1 > eX2);
		bool overlapY = !(pY2 < eY1 || pY1 > eY2);
		bool isColliding = overlapX && overlapY;

		if (isColliding && !f.playerTouching) {
			f.playerTouching = true;
			f.kneeling = true;
			f.kneelTimer = 0;
			punchJustLanded = false;
			// Fire always kills instantly — pass world X so body draws at fire
			// location
			lv2HitPlayer(playerLives, true, f.x); // drain ALL lives → instant death
		}

		if (!isColliding) {
			f.playerTouching = false;
		}
	}
}
void updateLv3Spikes() {
	if (currentBg != 12) return;

	// Advance animation
	static int spikeTick = 0;
	if (++spikeTick >= 3) {   // tweak 3 to speed up / slow down
		spikeTick = 0;
		spikeAnimIndex = (spikeAnimIndex + 1) % SPIKE_FRAMECOUNT;
	}

	if (!playerAlive) return;

	for (int i = 0; i < NUM_SPIKES_LV3; i++) {
		SpikeObstacle &s = spikesLv3[i];

		// Count down kneel timer
		if (s.kneeling) {
			s.kneelTimer++;
			if (s.kneelTimer >= SPIKE_KNEEL_DURATION) {
				s.kneeling = false;
				s.kneelTimer = 0;
				s.playerTouching = false;
				kneelHolding = false;
				kneelHoldCounter = 0;
				showKneel = false;
			}
			continue;   // skip collision while kneeling
		}
		// Only lethal during frames sp6–sp33 (0-based index 5–32)
		bool spikeLethal = (spikeAnimIndex >= 5 && spikeAnimIndex <= 32);
		if (!spikeLethal) {
			s.playerTouching = false;
			continue;
		}
		// Hitbox
		int eX1 = (int)s.x + (int)(SPIKE_W * SPIKE_HB_LEFT);
		int eX2 = (int)s.x + (int)(SPIKE_W * SPIKE_HB_RIGHT);
		int eY1 = (int)s.y + (int)(SPIKE_H * SPIKE_HB_TOP);
		int eY2 = (int)s.y + (int)(SPIKE_H * SPIKE_HB_BOTTOM);

		// Player hitbox (world coords)
		int pX1 = lv3PHB.x1;
		int pX2 = lv3PHB.x2;
		int pY1 = lv3PHB.y1;
		int pY2 = lv3PHB.y2;

		bool overlapX = !(pX2 < eX1 || pX1 > eX2);
		bool overlapY = !(pY2 < eY1 || pY1 > eY2);
		bool isColliding = overlapX && overlapY;

		if (isColliding && !s.playerTouching) {
			s.playerTouching = true;
			s.kneeling = true;
			s.kneelTimer = 0;
			punchJustLanded = false;

			playerLives--;
			if (playerLives < 0) playerLives = 0;

			if (playerLives > 0) {
				showKneel = true;
				kneelHolding = true;
				kneelHoldCounter = 0;
			}
			else {
				showKneel = false;
				kneelHolding = false;
				playerAlive = false;
				gameStarted = false;
				playerDeathY = bridges3[3].snapY;   // or the bridge containing the spike
				handlePlayerDeath();                // triggers game over / respawn
			}
		}
	}
}

void updateBoss1Movement() {
	if (currentBg != 10)
		return;
	if (!boss1Active || boss1Dead)
		return;
	if (!boss1Moving)
		return;

	Bridge &b7 = bridges2[6];
	int cx = lv2CharX + PLAYER_WIDTH / 2;
	bool playerOnBridge7 = (cx >= b7.minX && cx <= b7.maxX);
	if (!playerOnBridge7)
		return;

	// Move left toward player
	boss1X -= 2.0f;

	// Stop when boss right edge is within 150px of player left edge
	float bossRight = boss1X + (PLAYER_WIDTH + 10);
	float playerLeft = (float)lv2CharX;

	if (bossRight <= playerLeft + 30.0f) {
		boss1Moving = false;
		boss1FrameIndex = 0;
	}

	// Hard clamp — never go past bridge left boundary
	if (boss1X < (float)(b7.minX + 50)) {
		boss1X = (float)(b7.minX + 50);
		boss1Moving = false;
		boss1FrameIndex = 0;
	}
}
void updateBoss1Anim() {
	if (currentBg != 10)
		return;
	if (!boss1Active || boss1Dead)
		return;

	Bridge &b7 = bridges2[6];
	int cx = lv2CharX + PLAYER_WIDTH / 2;
	bool playerOnBridge7 = (cx >= b7.minX && cx <= b7.maxX);
	if (!playerOnBridge7)
		return;

	boss1FloatOffset += 0.05f;
	if (boss1FloatOffset > 2 * 3.1416f)
		boss1FloatOffset = 0.0f;

	if (boss1Moving) {
		// Only frames 0-5 while floating toward player
		boss1FrameIndex = (boss1FrameIndex + 1) % 6;
	}
	else {
		// All 41 frames during attack
		boss1FrameIndex = (boss1FrameIndex + 1) % 41;
	}
}
void updateLv3Shield() {
	if (currentBg != 12 || !lv3ShieldActive) return;

	static int shldTick = 0;
	if (++shldTick >= 2) {          // ← change 4 to speed up / slow down
		shldTick = 0;
		shieldLoadFrameIndex = (shieldLoadFrameIndex + 1) % 8;
		shieldAuraFrameIndex = (shieldAuraFrameIndex + 1) % 11;
		shieldAuraFrameIndexM = (shieldAuraFrameIndexM + 1) % 11;
	}
}

void updateLv3Boss() {
	if (currentBg != 12) return;
	if (lv3BossDead) return;

	// ── Activate when player walks onto bridge 10 (bridges3[10]) ──
	if (!lv3BossActive) {
		int cx = lv3CharX + PLAYER_WIDTH / 2;
		if (cx >= bridges3[9].minX && cx <= bridges3[9].maxX)
			lv3BossActive = true;
		else
			return;  // not yet active — nothing to do
	}

	// ── Count-down hit cooldown ────────────────────────────────────
	if (lv3BossHitCooldown > 0) lv3BossHitCooldown--;

	// ════════════════════════════════════════════════════════════════
	//  STATE:  WALKING  (idle until player comes close)
	// ════════════════════════════════════════════════════════════════
	if (lv3BossState == LV3BOSS_WALKING) {

		// Advance walk animation
		static int walkTick = 0;
		if (++walkTick >= 2) {
			walkTick = 0;
			lv3BossFrameIndex = (lv3BossFrameIndex + 1) % LV3BOSS_WALK_FRAMES;
		}

		// Check proximity: player within TRIGGER_DIST of boss left edge
		int bossLeft = (int)lv3BossX;
		int playerRight = lv3CharX + PLAYER_WIDTH;

		if (playerRight >= bossLeft - LV3BOSS_TRIGGER_DIST) {
			// Transition → FIRE
			lv3BossState = LV3BOSS_FIRE;
			lv3BossFrameIndex = 13;
			lv3BossFireLoopCount = 0;
		}
		return;
	}

	// ════════════════════════════════════════════════════════════════
	//  STATE:  FIRE  (loops 2×)
	// ════════════════════════════════════════════════════════════════
	if (lv3BossState == LV3BOSS_FIRE) {

		static int fireTick = 0;
		if (++fireTick >= -5) {      // advance every tick (40 ms × 1)
			fireTick = 0;
			lv3BossFrameIndex++;

			if (lv3BossFrameIndex >= LV3BOSS_FIRE_FRAMES) {
				// One full fire loop completed
				lv3BossFrameIndex = 13;
				lv3BossFireLoopCount++;

				if (lv3BossFireLoopCount >= 2) {
					// 3 loops done → transition to JUMP
					lv3BossState = LV3BOSS_JUMP;
					lv3BossFrameIndex = 0;
					lv3BossFireLoopCount = 0;
				}
			}
		}
		return;
	}

	// ════════════════════════════════════════════════════════════════
	//  STATE:  JUMP  (plays once)
	// ════════════════════════════════════════════════════════════════
	if (lv3BossState == LV3BOSS_JUMP) {

		static int jumpTick = 0;
		if (++jumpTick >= 1) {
			jumpTick = 0;
			lv3BossFrameIndex++;

			if (lv3BossFrameIndex >= LV3BOSS_JUMP_FRAMES) {
				// Jump done → immediately transition to SLASH
				lv3BossState = LV3BOSS_SLASH;
				lv3BossFrameIndex = 0;
			}
		}
		return;
	}

	// ════════════════════════════════════════════════════════════════
	//  STATE:  SLASH  (plays once, then loops back to FIRE)
	// ════════════════════════════════════════════════════════════════
	if (lv3BossState == LV3BOSS_SLASH) {

		static int slashTick = 0;
		if (++slashTick >= -3) {
			slashTick = 0;
			lv3BossFrameIndex++;

			if (lv3BossFrameIndex >= LV3BOSS_SLASH_FRAMES) {
				// Slash done → go back to FIRE (pattern: fire×3 → jump → slash → repeat)
				lv3BossState = LV3BOSS_FIRE;
				lv3BossFrameIndex = 13;
				lv3BossFireLoopCount = 0;
			}
		}
		return;
	}
}
void checkLv3BossHits() {
	if (currentBg != 12) return;
	if (!lv3BossActive || lv3BossDead) return;
	if (!playerAlive) return;

	// ── Shared geometry ───────────────────────────────────────────
	int bossX = (int)lv3BossX;
	int bossY = bridges3[9].snapY + LV3BOSS_YOFF;

	// ─────────────────────────────────────────────────────────────
	//  A)  BOSS → PLAYER DAMAGE
	// ─────────────────────────────────────────────────────────────

	// ── FIRE DAMAGE (active during LV3BOSS_FIRE state) ───────────

	if (lv3BossState == LV3BOSS_FIRE &&
		(lv3BossFrameIndex == 36 || lv3BossFrameIndex == 49)) {

		// Fire hitbox in WORLD coordinates — lv3PHB is world space, no camera offset
		int fX1 = bossX + (int)(LV3BOSS_W * LV3BOSS_FIRE_HB_LEFT);
		int fX2 = bossX + (int)(LV3BOSS_W * LV3BOSS_FIRE_HB_RIGHT);
		int fY1 = bossY + (int)(LV3BOSS_H * LV3BOSS_FIRE_HB_TOP);
		int fY2 = bossY + (int)(LV3BOSS_H * LV3BOSS_FIRE_HB_BOTTOM);

		// Player hitbox (world coords)
		int pX1 = lv3PHB.x1;
		int pX2 = lv3PHB.x2;
		int pY1 = lv3PHB.y1;
		int pY2 = lv3PHB.y2;

		bool overlapX = !(pX2 < fX1 || pX1 > fX2);
		bool overlapY = !(pY2 < fY1 || pY1 > fY2);

		// Shield active → immune to fire damage
		bool shielded = lv3ShieldActive;

		if (overlapX && overlapY && !shielded && lv3BossHitCooldown == 0) {
			lv3BossHitCooldown = 50; // ~800 ms immunity window

			lv3BossPlayerHitCount++;
			playerLives -= 3;
			if (playerLives < 0) playerLives = 0;

			if (playerLives > 0) {
				// Show kneel (reuse existing system)
				showKneel = true;
				kneelHolding = true;
				kneelHoldCounter = 0;
			}
			else {
				// Out of lives → death
				showKneel = false;
				kneelHolding = false;
				playerAlive = false;
				gameStarted = false;
				playerDeathY = bridges3[9].snapY;
				handlePlayerDeath();
			}
		}
	}

	// ── SLASH DAMAGE (only lethal frames: slash26-53, i.e. index 25-52) ──
	if (lv3BossState == LV3BOSS_SLASH) {

		bool lethalFrame = (lv3BossFrameIndex >= LV3BOSS_SLASH_LETHAL_START &&
			lv3BossFrameIndex <= LV3BOSS_SLASH_LETHAL_END);

		if (lethalFrame) {
			// Player must NOT be on bridge 10 (must have jumped off)
			int cx = lv3CharX + PLAYER_WIDTH / 2;
			bool playerOnBridge10 = (cx >= bridges3[9].minX &&
				cx <= bridges3[9].maxX &&
				abs(lv3CharY - bridges3[9].snapY) <= 20);

			if (playerOnBridge10 && lv3BossHitCooldown == 0) {
				lv3BossHitCooldown = 50;

				lv3BossPlayerHitCount++;
				playerLives -= 3;
				if (playerLives < 0) playerLives = 0;

				if (playerLives > 0) {
					showKneel = true;
					kneelHolding = true;
					kneelHoldCounter = 0;
				}
				else {
					showKneel = false;
					kneelHolding = false;
					playerAlive = false;
					gameStarted = false;
					playerDeathY = bridges3[9].snapY;
					handlePlayerDeath();
				}
			}
		}
	}

	// ─────────────────────────────────────────────────────────────────
	//  B)  PLAYER → BOSS DAMAGE
	//  1 hit per F press (per chp throw). Uses chargedPunchState
	//  toggle count so it's completely independent of animation timing.
	// ─────────────────────────────────────────────────────────────────
	static int lastFPressCount = 0;  // tracks which F press we already counted

	// Count total F presses that produced a chp throw
	// chargedPunchState flips 0→1→0→1 each F press
	// We track how many times it has been in state=1 cumulatively
	static int chpThrowCount = 0;
	static bool prevState1 = false;
	bool currState1 = (chargedSequenceActive && chargedPunchState == 1);

	// Rising edge of chargedPunchState==1 → new throw started
	if (currState1 && !prevState1) {
		chpThrowCount++;  // one new chp throw happened
	}
	prevState1 = currState1;

	// Boss hitbox in world coordinates
	int bX1 = bossX - 10;
	int bX2 = bossX + LV3BOSS_W + 10;
	int bY1 = bossY;
	int bY2 = bossY + LV3BOSS_H;

	// Only register hit if this throw hasn't been counted yet
	// AND chp projectile is visually overlapping boss
	if (chpAnimPlaying && chpFrameIndex >= 1 && chpThrowCount > lastFPressCount) {

		float progress = (float)chpFrameIndex / 6.0f;
		int throwOffset = (int)(progress * CHP_THROW_DIST);

		int projWorldX;
		if (movingFront)
			projWorldX = lv3CharX + CHP_X_OFF - throwOffset;
		else
			projWorldX = lv3CharX - CHP_X_OFF + throwOffset;

		int projX1 = projWorldX - 50;
		int projX2 = projWorldX + CHP_W + 50;
		int projY1 = lv3CharY + CHP_Y_OFF - 30;
		int projY2 = projY1 + CHP_H + 60;

		bool oX = !(projX2 < bX1 || projX1 > bX2);
		bool oY = !(projY2 < bY1 || projY1 > bY2);

		if (oX && oY) {
			lastFPressCount = chpThrowCount;  // mark this throw as counted
			lv3BossHitCount++;
			printf("CHP HIT! throw#%d  boss_hp=%d\n", chpThrowCount, lv3BossHitCount);
			if (lv3BossHitCount >= LV3BOSS_HITS_TO_KILL) {
				lv3BossDead = true;
				lv3BossActive = false;
			}
		}
	}

	// Reset throw counter when charged sequence ends (P expires)
	if (!chargedSequenceActive) {
		chpThrowCount = 0;
		lastFPressCount = 0;
	}
}
void updateLv3ChpAnim() {
	if (currentBg != 12) return;

	static int prevChargedPunchState = 0;
	static bool throwingFront = false;  // direction locked at throw moment

	// Trigger ONLY when transitioning into chargedPunchState == 1
	if (chargedSequenceActive && chargedPunchState == 1 && prevChargedPunchState != 1) {
		chpAnimPlaying = true;
		chpFrameIndex = 0;
		throwingFront = movingFront;  // lock direction at moment of throw
	}

	prevChargedPunchState = chargedSequenceActive ? chargedPunchState : 0;

	if (!chargedSequenceActive) {
		chpAnimPlaying = false;
		chpFrameIndex = 0;
		return;
	}

	if (!chpAnimPlaying) return;

	static int chpTick = 0;
	if (++chpTick >= 4) {  // ← tweak speed here
		chpTick = 0;
		if (chpFrameIndex < 6) {
			chpFrameIndex++;
		}
		else {
			chpAnimPlaying = false;
			chpFrameIndex = 6;
		}
	}
}
void drawLv3Debug() {
	if (currentBg != 12) return;

	for (int i = 0; i < numBridges3; i++) {
		int drawX1 = bridges3[i].minX - lv3CameraX;
		int drawX2 = bridges3[i].maxX - lv3CameraX;
		int drawY = bridges3[i].snapY;

		// Top line
		iSetColor(0, 255, 0);
		iLine(drawX1, drawY, drawX2, drawY);

		// Left and right edges
		iLine(drawX1, drawY, drawX1, drawY + 20);
		iLine(drawX2, drawY, drawX2, drawY + 20);

		// Bridge number label
		char label[16];
		sprintf_s(label, "B%d", i + 1);
		iSetColor(255, 255, 0);
		iText(drawX1 + 5, drawY + 5, label, GLUT_BITMAP_HELVETICA_12);
	}
}

// ── Draw player debug hitbox in Level 3 (world-space, camera-adjusted) ──
/*void drawLv3PlayerDebug() {
if (currentBg != 12) return;
if (!playerAlive) return;

// ── Normal hitbox — cyan ──────────────────────────────────────
int pX1 = lv3PHB.x1 - lv3CameraX;
int pY1 = lv3PHB.y1;
int pX2 = lv3PHB.x2 - lv3CameraX;
int pY2 = lv3PHB.y2;

iSetColor(0, 255, 255);
iRectangle(pX1, pY1, pX2 - pX1, pY2 - pY1);

int cx = (pX1 + pX2) / 2;
int cy = (pY1 + pY2) / 2;
iLine(cx - 5, cy, cx + 5, cy);
iLine(cx, cy - 5, cx, cy + 5);

// ── Charged punch hitbox — bright yellow, only when active ───
if (chargedSequenceActive && chargedPunchState == 1) {
int cpX1 = lv3CharX + (int)(PLAYER_WIDTH  * LV3_HB_CHPUNCH_LEFT) - lv3CameraX;
int cpX2 = lv3CharX + (int)(PLAYER_WIDTH  * LV3_HB_CHPUNCH_RIGHT) - lv3CameraX;
int cpY1 = lv3CharY + (int)(PLAYER_HEIGHT * LV3_HB_CHPUNCH_TOP);
int cpY2 = lv3CharY + (int)(PLAYER_HEIGHT * LV3_HB_CHPUNCH_BOTTOM);

iSetColor(255, 255, 0);   // bright yellow
iRectangle(cpX1, cpY1, cpX2 - cpX1, cpY2 - cpY1);

// crosshair at center
int ccx = (cpX1 + cpX2) / 2;
int ccy = (cpY1 + cpY2) / 2;
iLine(ccx - 6, ccy, ccx + 6, ccy);
iLine(ccx, ccy - 6, ccx, ccy + 6);
}
}*/

/* -------------------- DRAW -------------------- */
void iDraw() {

	iClear();
	iSetColor(255, 255, 255);

	// ================= BACKGROUNDS (ADDED) =================
	if (currentBg == 1)
		iShowImage(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, bg1);
	else if (currentBg == 3) {
		iShowImage(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, storyImgs[storyIndex]);
	}
	else if (currentBg == 2) {
		iShowImage(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, bg2);
		for (int i = 0; i < NUM_BUTTONS; i++) {
			int y = btnStartY - i * (btnH + btnGap);
			iShowImage(btnX, y, btnW, btnH,
				btnIsHover[i] ? btnHover[i] : btnNormal[i]);
		}
	}

	else if (currentBg == 4)
		iShowImage(-cameraX, 0, BG41_WIDTH, BG41_HEIGHT, bg41);

	else if (currentBg == 5)
		iShowImage(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, bg5);

	else if (currentBg == 6)
		iShowImage(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, bg6);

	else if (currentBg == 7)
		iShowImage(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, bg7);
	else if (currentBg == 8) {
		iShowImage(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, gameOverImg);
	}
	else if (currentBg == 9) {
		iShowImage(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, instruction2Img);
	}
	else if (currentBg == 11) {
		iShowImage(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, instruction3Img);
	}
	else if (currentBg == 13) {
		// Base image
		int dispImg = lvlSelectImg;
		if (lvlHoverIndex == 1) dispImg = lvlSelectImg1;
		else if (lvlHoverIndex == 2) dispImg = lvlSelectImg2;
		else if (lvlHoverIndex == 3) dispImg = lvlSelectImg3;

		iShowImage(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, dispImg);

		// Debug hitboxes
		if (LVL_SELECT_DRAW_DEBUG) {
			for (int i = 0; i < 3; i++) {
				if (lvlHoverIndex == i + 1)
					iSetColor(0, 255, 0);    // green when hovered
				else
					iSetColor(255, 0, 0);    // red when idle
				iRectangle(lvlHB[i].x, lvlHB[i].y, lvlHB[i].w, lvlHB[i].h);

				// Label
				char label[8];
				sprintf_s(label, "LV%d", i + 1);
				iSetColor(255, 255, 0);
				iText(lvlHB[i].x + 4, lvlHB[i].y + 4, label, GLUT_BITMAP_HELVETICA_12);
			}
		}
	}
	else if (currentBg == 12) {
		iShowImage(-lv3CameraX, 0, LV3_BG_WIDTH, LV3_BG_HEIGHT, lv3Img);

		// Draw player
		if (!playerAlive) {
			// Dead body
			int drawX = lv3CharX - lv3CameraX;
			int deadWidth = PLAYER_WIDTH + 40;
			int deadHeight = PLAYER_HEIGHT + 40;
			drawX -= (deadWidth - PLAYER_WIDTH) / 2;
			int drawY = lv3GroundY - 90;
			iShowImage(drawX, drawY, deadWidth, deadHeight, playerDeadImg);
		}
		else {
			int drawX = lv3CharX - lv3CameraX;

			if (!showKneel) {
				// ── Level 3 exclusive: crouch overrides everything ──────────
				if (lv3Crouching) {
					int cX = drawX - (CROUCH_W - PLAYER_WIDTH) / 2;
					int cY = lv3CharY + CROUCH_Y_OFF;
					iShowImage(cX, cY, CROUCH_W, CROUCH_H,
						movingFront ? crouchImgM : crouchImg);
				}
				// ── Level 3 exclusive: kick sprite ──────────────────────────

				else if (chargingPunch && chargeScrollDismissed) {
					if (chargedSequenceActive) {
						if (chargedPunchState == 0)
							iShowImage(drawX, lv3CharY, PLAYER_WIDTH, PLAYER_HEIGHT,
							movingFront ? chargedFistImg : chargedFistImgM);
						else
							iShowImage(drawX, lv3CharY, PLAYER_WIDTH, PLAYER_HEIGHT,
							movingFront ? chargedPunchImgSeq : chargedPunchImgSeqM);
					}
					else {
						iShowImage(drawX, lv3CharY, PLAYER_WIDTH, PLAYER_HEIGHT,
							movingFront ? chargedPunchImg : chargedPunchImgM);
					}
				}
				else {
					int img;
					if (punchState == 0)
						img = movingFront ? (animFrame ? playerSideLeft : playerSide)
						: (animFrame ? playerSideLeftM : playerSideM);
					else if (punchState == 1)
						img = movingFront ? punchFistImg : punchFistImgM;
					else
						img = movingFront ? punchHitImg : punchHitImgM;

					iShowImage(drawX, lv3CharY, PLAYER_WIDTH, PLAYER_HEIGHT, img);
				}
			}

			// Kneel sprite
			if (showKneel && playerAlive) {
				iShowImage(lv3CharX - lv3CameraX, lv3CharY,
					PLAYER_WIDTH, PLAYER_HEIGHT, kneelImg);
			}

			// Kneel sprite
			if (showKneel && playerAlive) {
				iShowImage(lv3CharX - lv3CameraX, lv3CharY,
					PLAYER_WIDTH, PLAYER_HEIGHT, kneelImg);
			}
		}
		// ── Level 3 shield overlay ──────────────────────────────────────
		if (lv3ShieldActive && playerAlive) {
			int drawX = lv3CharX - lv3CameraX;

			// Foot glow — always shown regardless of direction
			int loadX = drawX + (PLAYER_WIDTH - SHLD_LOAD_W) / 2;
			int loadY = lv3CharY + SHLD_LOAD_Y_OFF;
			iShowImage(loadX, loadY, SHLD_LOAD_W, SHLD_LOAD_H,
				shieldLoadFrames[shieldLoadFrameIndex]);

			if (!movingFront) {
				// Facing right (side, sideleft, fist, punchf) — aura on LEFT
				int auraX = drawX + PLAYER_WIDTH + SHLD_AURA_X_DIST;
				int auraY = lv3CharY - SHLD_AURA_H + SHLD_AURA_Y_OFF;
				iShowImage(auraX, auraY, SHLD_AURA_W, SHLD_AURA_H,
					shieldAuraFrames[shieldAuraFrameIndex]);
			}
			else {
				// Facing left (sidem, sideleftm, fistm, punchff) — aura on RIGHT
				int auraX = drawX - SHLD_AURA_W - SHLD_AURA_X_DIST;
				int auraY = lv3CharY - SHLD_AURA_H + SHLD_AURA_Y_OFF;
				iShowImage(auraX, auraY, SHLD_AURA_W, SHLD_AURA_H,
					shieldAuraFramesM[shieldAuraFrameIndexM]);
			}
		}


		// ── Level 3 Spike Obstacles ──────────────────────────────────────
		for (int i = 0; i < NUM_SPIKES_LV3; i++) {
			int drawX = (int)spikesLv3[i].x - lv3CameraX;
			int drawY = (int)spikesLv3[i].y;
			iShowImage(drawX, drawY, SPIKE_W, SPIKE_H, spikeFrames[spikeAnimIndex]);
		}

		// ── Level 3 Bird Enemy ───────────────────────────────────────────
		if (lv3BirdAlive && lv3BirdActive) {
			int drawX = (int)lv3BirdX - lv3CameraX;
			int drawY = bridges3[1].snapY + LV3BIRD_Y_OFF;
			iShowImage(drawX, drawY, LV3BIRD_W, LV3BIRD_H,
				lv3BirdFrames[lv3BirdFrameIndex]);
		}
		else if (!lv3BirdAlive) {
			int drawX = (int)lv3BirdX - lv3CameraX;
			int drawY = (int)lv3BirdDeadY + 20;
			iShowImage(drawX, drawY, LV3BIRD_W, LV3BIRD_H, lv3BirdDeadImg);
		}

		// ── Level 3 Bird Enemy 2 ────────────────────────────────────────────
		if (lv3Bird2Alive && lv3Bird2Active) {
			int drawX = (int)lv3Bird2X - lv3CameraX;
			int drawY = bridges3[1].snapY + LV3BIRD2_Y_OFF;
			iShowImage(drawX, drawY, LV3BIRD2_W, LV3BIRD2_H,
				lv3BirdFrames[lv3Bird2FrameIndex]);   // reuses same sprite sheet
		}
		else if (!lv3Bird2Alive) {
			int drawX = (int)lv3Bird2X - lv3CameraX;
			int drawY = (int)lv3Bird2DeadY + 20;
			iShowImage(drawX, drawY, LV3BIRD2_W, LV3BIRD2_H, lv3BirdDeadImg);
		}

		// ── Level 3 PBird Enemy (Bridge 3) ──────────────────────────────────────
		if (lv3PBirdAlive && lv3PBirdActive) {
			int drawX = (int)lv3PBirdX - lv3CameraX;
			int drawY = bridges3[2].snapY + LV3PBIRD_Y_OFF;
			int frameImg = lv3PBirdMovingRight
				? lv3PBirdFramesRight[lv3PBirdFrameIndex]
				: lv3PBirdFramesLeft[lv3PBirdFrameIndex];
			iShowImage(drawX, drawY, LV3PBIRD_W, LV3PBIRD_H, frameImg);

			if (LV3PBIRD_DRAW_DEBUG) {
				int hX1 = (int)lv3PBirdX + (int)(LV3PBIRD_W * LV3PBIRD_HB_LEFT) - lv3CameraX;
				int hX2 = (int)lv3PBirdX + (int)(LV3PBIRD_W * LV3PBIRD_HB_RIGHT) - lv3CameraX;
				int hY1 = bridges3[2].snapY + LV3PBIRD_Y_OFF + (int)(LV3PBIRD_H * LV3PBIRD_HB_TOP);
				int hY2 = bridges3[2].snapY + LV3PBIRD_Y_OFF + (int)(LV3PBIRD_H * LV3PBIRD_HB_BOTTOM);
				iSetColor(0, 200, 255);   // light blue — pbird hitbox
				iRectangle(hX1, hY1, hX2 - hX1, hY2 - hY1);
			}
		}

		// ── Level 3 PBird2 Enemy (Bridge 7) ─────────────────────────────────────
		if (lv3PBird2Alive && lv3PBird2Active) {
			int drawX = (int)lv3PBird2X - lv3CameraX;
			int drawY = bridges3[6].snapY + LV3PBIRD2_Y_OFF;
			int frameImg = lv3PBird2MovingRight
				? lv3PBird2FramesRight[lv3PBird2FrameIndex]
				: lv3PBird2FramesLeft[lv3PBird2FrameIndex];
			iShowImage(drawX, drawY, LV3PBIRD2_W, LV3PBIRD2_H, frameImg);

			if (LV3PBIRD2_DRAW_DEBUG) {
				int hX1 = (int)lv3PBird2X + (int)(LV3PBIRD2_W * LV3PBIRD2_HB_LEFT) - lv3CameraX;
				int hX2 = (int)lv3PBird2X + (int)(LV3PBIRD2_W * LV3PBIRD2_HB_RIGHT) - lv3CameraX;
				int hY1 = bridges3[6].snapY + LV3PBIRD2_Y_OFF + (int)(LV3PBIRD2_H * LV3PBIRD2_HB_TOP);
				int hY2 = bridges3[6].snapY + LV3PBIRD2_Y_OFF + (int)(LV3PBIRD2_H * LV3PBIRD2_HB_BOTTOM);
				iSetColor(0, 200, 255);
				iRectangle(hX1, hY1, hX2 - hX1, hY2 - hY1);
			}
		}

		// ── Level 3 Enemy 1 ──────────────────────────────────────────────
		if (lv3En1Alive) {
			int drawX = (int)lv3En1X - lv3CameraX;
			int drawY = bridges3[1].snapY + LV3EN1_Y_OFF;
			int img = -1;
			int drawW = LV3EN1_W, drawH = LV3EN1_H;

			switch (lv3En1State) {
			case LV3EN1_IDLE:
				img = lv3En1IdleFrames[lv3En1FrameIndex];
				drawW = LV3EN1_IDLE_W; drawH = LV3EN1_IDLE_H;
				break;
			case LV3EN1_WALKING:
				img = lv3En1WalkFrames[lv3En1FrameIndex];
				drawW = LV3EN1_WALK_W; drawH = LV3EN1_WALK_H;
				break;
			case LV3EN1_ATTACKING:
				img = lv3En1DatkFrames[lv3En1FrameIndex];
				drawW = LV3EN1_DATK_W; drawH = LV3EN1_DATK_H;
				drawX += LV3EN1_DATK_X_OFF;
				drawY += LV3EN1_DATK_Y_OFF;
				break;
			case LV3EN1_HIT_REACT:
				img = lv3En1HitFrames[lv3En1FrameIndex];
				drawW = LV3EN1_HIT_W; drawH = LV3EN1_HIT_H;
				drawX += LV3EN1_HIT_X_OFF;
				drawY += LV3EN1_HIT_Y_OFF;    // ← ADD THIS LINE
				break;
			}
			if (img != -1)
				iShowImage(drawX, drawY, drawW, drawH, img);

			/*// ── En1 debug hitbox ─────────────────────────────────────────
			int en1BaseDrawX = (int)lv3En1X - lv3CameraX;
			int en1BaseDrawY = bridges3[1].snapY + LV3EN1_Y_OFF;
			int hX1 = (int)lv3En1X + (int)(LV3EN1_W * LV3EN1_HB_LEFT) - lv3CameraX;
			int hX2 = (int)lv3En1X + (int)(LV3EN1_W * LV3EN1_HB_RIGHT) - lv3CameraX;
			int hY1 = en1BaseDrawY + (int)(LV3EN1_H * LV3EN1_HB_TOP);
			int hY2 = en1BaseDrawY + (int)(LV3EN1_H * LV3EN1_HB_BOTTOM);
			iSetColor(255, 165, 0);   // orange — body hitbox
			iRectangle(hX1, hY1, hX2 - hX1, hY2 - hY1);

			// Attack hitbox — red, only on lethal frame
			if (lv3En1State == LV3EN1_ATTACKING &&
			lv3En1FrameIndex == LV3EN1_LETHAL_FRAME) {
			int aX1 = (int)lv3En1X + (int)(LV3EN1_W * LV3EN1_ATK_HB_LEFT) - lv3CameraX;
			int aX2 = (int)lv3En1X + (int)(LV3EN1_W * LV3EN1_ATK_HB_RIGHT) - lv3CameraX;
			int aY1 = en1BaseDrawY + (int)(LV3EN1_H * LV3EN1_ATK_HB_TOP);
			int aY2 = en1BaseDrawY + (int)(LV3EN1_H * LV3EN1_ATK_HB_BOTTOM);
			iSetColor(255, 0, 0);
			iRectangle(aX1, aY1, aX2 - aX1, aY2 - aY1);
			}*/

		}
		else if (!lv3En1Alive) {
			int drawX = (int)lv3En1X - lv3CameraX + 100;
			int drawY = (int)lv3En1DeadY + 40;
			iShowImage(drawX, drawY, LV3EN1_DEAD_W, LV3EN1_DEAD_H, lv3En1DeadImg);
		}

		// ── Level 3 Enemy 2 ──────────────────────────────────────────────
		if (lv3En2Alive && lv3En2Active) {
			int drawX = (int)lv3En2X - lv3CameraX;
			int drawY = bridges3[1].snapY + LV3EN2_Y_OFF;
			int img = -1;
			int drawW, drawH;

			switch (lv3En2State) {
			case LV3EN2_IDLE:
				img = lv3En2IdleFrames[lv3En2FrameIndex];
				drawW = LV3EN2_IDLE_W; drawH = LV3EN2_IDLE_H;
				break;
			case LV3EN2_RUNNING:
				img = lv3En2RunFrames[lv3En2FrameIndex];
				drawW = LV3EN2_RUN_W; drawH = LV3EN2_RUN_H;
				drawY += LV3EN2_RUN_Y_OFF;
				break;
			case LV3EN2_ATTACKING:
				img = lv3En2DatkFrames[lv3En2FrameIndex];
				drawW = LV3EN2_DATK_W; drawH = LV3EN2_DATK_H;
				break;
			case LV3EN2_HIT_REACT:
				img = lv3En2HitFrames[lv3En2FrameIndex];
				drawW = LV3EN2_HIT_W; drawH = LV3EN2_HIT_H;
				drawX += LV3EN2_HIT_X_OFF;
				drawY += LV3EN2_HIT_Y_OFF;
				break;
			default:
				drawW = LV3EN2_RUN_W; drawH = LV3EN2_RUN_H;
				break;
			}
			if (img != -1)
				iShowImage(drawX, drawY, drawW, drawH, img);
		}
		else if (!lv3En2Alive && lv3En2Active) {
			int drawX = (int)lv3En2X - lv3CameraX - 30;   // small left nudge
			int drawY = (int)lv3En2DeadY + 40;
			iShowImage(drawX, drawY, LV3EN2_DEAD_W, LV3EN2_DEAD_H, lv3En2DeadImg);
		}

		// ── Level 3 Enemy 3 (Bridge 6) ──────────────────────────────────────
		if (lv3En3Alive) {
			int en3DrawX = (int)lv3En3X - lv3CameraX;
			int en3DrawY = bridges3[5].snapY + LV3EN3_Y_OFF;
			int en3Img = -1;
			int en3W, en3H;

			switch (lv3En3State) {
			case LV3EN3_IDLE_STATE:
				en3Img = lv3En3IdleFrames[lv3En3FrameIndex];
				en3W = LV3EN3_IDLE_W; en3H = LV3EN3_IDLE_H;
				break;
			case LV3EN3_WALKING:
				en3Img = lv3En3WalkFrames[lv3En3FrameIndex];
				en3W = LV3EN3_WALK_W; en3H = LV3EN3_WALK_H;
				break;
			case LV3EN3_ATTACKING:
				en3Img = lv3En3DatkFrames[lv3En3FrameIndex];
				en3W = LV3EN3_DATK_W; en3H = LV3EN3_DATK_H;
				en3DrawX += LV3EN3_DATK_X_OFF;   // ← ADD
				en3DrawY += LV3EN3_DATK_Y_OFF;   // ← ADD
				break;
			case LV3EN3_HIT_REACT:
				en3Img = lv3En3HitFrames[lv3En3FrameIndex];
				en3W = LV3EN3_HIT_W;  en3H = LV3EN3_HIT_H;
				en3DrawX += LV3EN3_HIT_X_OFF;
				en3DrawY += LV3EN3_HIT_Y_OFF;
				break;
			default:
				en3W = LV3EN3_WALK_W; en3H = LV3EN3_WALK_H;
				break;
			}
			if (en3Img != -1)
				iShowImage(en3DrawX, en3DrawY, en3W, en3H, en3Img);
		}
		else if (!lv3En3Alive && !lv3En3FadeDone) {
			float fade = 1.0f - (float)lv3En3FadeTick / (float)LV3EN3_FADE_DURATION;
			int drawW = (int)(LV3EN3_DEAD_W * fade);
			int drawH = (int)(LV3EN3_DEAD_H * fade);
			if (drawW > 0 && drawH > 0) {
				int en3DrawX = (int)lv3En3X - lv3CameraX + 100 + (LV3EN3_DEAD_W - drawW) / 2;
				int en3DrawY = (int)lv3En3DeadY + 40 + (LV3EN3_DEAD_H - drawH) / 2;
				iShowImage(en3DrawX, en3DrawY, drawW, drawH, lv3En3DeadImg);
			}
		}

		// ── Level 3 Enemy 4 (Bridge 6) ───────────────────────────────────────
		if (lv3En4Alive && lv3En4Active) {
			int en4DrawX = (int)lv3En4X - lv3CameraX;
			int en4DrawY = bridges3[5].snapY + LV3EN4_Y_OFF;
			int en4Img = -1;
			int en4W, en4H;

			switch (lv3En4State) {
			case LV3EN4_IDLE:
				en4Img = lv3En4IdleFrames[lv3En4FrameIndex];
				en4W = LV3EN4_IDLE_W; en4H = LV3EN4_IDLE_H;
				break;
			case LV3EN4_RUNNING:
				en4Img = lv3En4RunFrames[lv3En4FrameIndex];
				en4W = LV3EN4_RUN_W;  en4H = LV3EN4_RUN_H;
				break;
			case LV3EN4_ATTACKING:
				en4Img = lv3En4DatkFrames[lv3En4FrameIndex];
				en4W = LV3EN4_DATK_W; en4H = LV3EN4_DATK_H;
				en4DrawX += LV3EN4_DATK_X_OFF;
				en4DrawY += LV3EN4_DATK_Y_OFF;
				break;
			case LV3EN4_HIT_REACT:
				en4Img = lv3En4HitFrames[lv3En4FrameIndex];
				en4W = LV3EN4_HIT_W;  en4H = LV3EN4_HIT_H;
				en4DrawX += LV3EN4_HIT_X_OFF;
				en4DrawY += LV3EN4_HIT_Y_OFF;
				break;
			default:
				en4W = LV3EN4_RUN_W; en4H = LV3EN4_RUN_H;
				break;
			}
			if (en4Img != -1)
				iShowImage(en4DrawX, en4DrawY, en4W, en4H, en4Img);
		}
		else if (!lv3En4Alive && lv3En4Active && !lv3En4FadeDone) {
			float fade = 1.0f - (float)lv3En4FadeTick / (float)LV3EN4_FADE_DURATION;
			int drawW = (int)(LV3EN4_DEAD_W * fade);
			int drawH = (int)(LV3EN4_DEAD_H * fade);
			if (drawW > 0 && drawH > 0) {
				int en4DrawX = (int)lv3En4X - lv3CameraX - 30 + (LV3EN4_DEAD_W - drawW) / 2;
				int en4DrawY = (int)lv3En4DeadY + 40 + (LV3EN4_DEAD_H - drawH) / 2;
				iShowImage(en4DrawX, en4DrawY, drawW, drawH, lv3En4DeadImg);
			}
		}

		// ── Level 3 WK Enemy (Bridge 6 left edge) ───────────────────────────
		if (lv3WkAlive) {
			int drawX = (int)lv3WkX - lv3CameraX;
			int drawY = (int)lv3WkY;
			iShowImage(drawX, drawY, LV3WK_WIDTH, LV3WK_HEIGHT,
				lv3WkFrames[lv3WkFrameIndex]);

			if (LV3WK_DRAW_DEBUG) {
				int hX1 = (int)lv3WkX + (int)(LV3WK_WIDTH  * LV3WK_HB_LEFT) - lv3CameraX;
				int hX2 = (int)lv3WkX + (int)(LV3WK_WIDTH  * LV3WK_HB_RIGHT) - lv3CameraX;
				int hY1 = (int)lv3WkY + (int)(LV3WK_HEIGHT * LV3WK_HB_TOP);
				int hY2 = (int)lv3WkY + (int)(LV3WK_HEIGHT * LV3WK_HB_BOTTOM);
				iSetColor(255, 165, 0);
				iRectangle(hX1, hY1, hX2 - hX1, hY2 - hY1);
			}
		}

		// ── Level 3 Enemy 5 ──────────────────────────────────────────────
		if (lv3En5State != LV3EN5_WAITING) {
			int en5BaseY = bridges3[5].snapY + LV3EN5_LAND_Y_OFF;
			int en5DrawX = (int)lv3En5X - lv3CameraX;
			int en5DrawY = en5BaseY;
			int en5W = LV3EN5_IDLE_W;
			int en5H = LV3EN5_IDLE_H;
			int en5Img = -1;

			switch (lv3En5State) {
			case LV3EN5_APPEARING:
				en5Img = lv3En5AprFrames[lv3En5FrameIndex];
				en5W = LV3EN5_APR_W;    en5H = LV3EN5_APR_H;
				en5DrawX += LV3EN5_APR_X_OFF;
				en5DrawY = (int)lv3En5Y + LV3EN5_APR_Y_OFF;
				break;
			case LV3EN5_IDLE:
				en5Img = lv3En5IdleFrames[lv3En5FrameIndex];
				en5W = LV3EN5_IDLE_W;   en5H = LV3EN5_IDLE_H;
				en5DrawX += LV3EN5_IDLE_X_OFF;
				en5DrawY += LV3EN5_IDLE_Y_OFF;
				break;
			case LV3EN5_RUNNING:
				en5Img = lv3En5RunFrames[lv3En5FrameIndex];
				en5W = LV3EN5_RUN_W;    en5H = LV3EN5_RUN_H;
				en5DrawX += LV3EN5_RUN_X_OFF;
				en5DrawY += LV3EN5_RUN_Y_OFF;
				break;
			case LV3EN5_PUNCHING:
				en5Img = lv3En5PunchFrames[lv3En5FrameIndex];
				en5W = LV3EN5_PUNCH_W;  en5H = LV3EN5_PUNCH_H;
				en5DrawX += LV3EN5_PUNCH_X_OFF;
				en5DrawY += LV3EN5_PUNCH_Y_OFF;
				break;
			case LV3EN5_HIT_REACT:
				en5Img = lv3En5HitFrames[lv3En5FrameIndex];
				en5W = LV3EN5_HIT_W;    en5H = LV3EN5_HIT_H;
				en5DrawX += LV3EN5_HIT_X_OFF;
				en5DrawY += LV3EN5_HIT_Y_OFF;
				break;
			case LV3EN5_REVIVING:
				en5Img = lv3En5ReviveFrames[lv3En5FrameIndex];
				en5W = LV3EN5_REVIVE_W; en5H = LV3EN5_REVIVE_H;
				en5DrawX += LV3EN5_REVIVE_X_OFF;
				en5DrawY += LV3EN5_REVIVE_Y_OFF;
				break;
			case LV3EN5_DYING:
			case LV3EN5_TRULY_DEAD:
				en5Img = lv3En5DeadFrames[lv3En5FrameIndex];
				en5W = LV3EN5_DEAD_W;   en5H = LV3EN5_DEAD_H;
				en5DrawX += LV3EN5_DEAD_X_OFF;
				en5DrawY += LV3EN5_DEAD_Y_OFF;
				break;
			default: break;
			}

			if (en5Img != -1)
				iShowImage(en5DrawX, en5DrawY, en5W, en5H, en5Img);

			// ── Debug hitboxes ───────────────────────────────────────
			if (LV3EN5_DRAW_DEBUG &&
				lv3En5State != LV3EN5_DYING &&
				lv3En5State != LV3EN5_TRULY_DEAD) {

				// Body hitbox — cyan
				int hX1 = (int)lv3En5X + (int)(LV3EN5_IDLE_W * LV3EN5_HB_LEFT) - lv3CameraX;
				int hX2 = (int)lv3En5X + (int)(LV3EN5_IDLE_W * LV3EN5_HB_RIGHT) - lv3CameraX;
				int hY1 = en5BaseY + (int)(LV3EN5_IDLE_H * LV3EN5_HB_TOP);
				int hY2 = en5BaseY + (int)(LV3EN5_IDLE_H * LV3EN5_HB_BOTTOM);
				iSetColor(0, 255, 255);
				iRectangle(hX1, hY1, hX2 - hX1, hY2 - hY1);

				// Attack hitbox — red, only on lethal punch frames
				if (lv3En5State == LV3EN5_PUNCHING &&
					(lv3En5FrameIndex == LV3EN5_LETHAL_FRAME_A ||
					lv3En5FrameIndex == LV3EN5_LETHAL_FRAME_B)) {
					int aX1 = (int)lv3En5X + LV3EN5_PUNCH_X_OFF +
						(int)(LV3EN5_PUNCH_W * LV3EN5_ATK_HB_LEFT) - lv3CameraX;
					int aX2 = (int)lv3En5X + LV3EN5_PUNCH_X_OFF +
						(int)(LV3EN5_PUNCH_W * LV3EN5_ATK_HB_RIGHT) - lv3CameraX;
					int aY1 = en5BaseY + LV3EN5_PUNCH_Y_OFF +
						(int)(LV3EN5_PUNCH_H * LV3EN5_ATK_HB_TOP);
					int aY2 = en5BaseY + LV3EN5_PUNCH_Y_OFF +
						(int)(LV3EN5_PUNCH_H * LV3EN5_ATK_HB_BOTTOM);
					iSetColor(255, 0, 0);
					iRectangle(aX1, aY1, aX2 - aX1, aY2 - aY1);
				}
			}
		}

		// ── Level 3 Enemy 6 ──────────────────────────────────────────────
		if (lv3En6State != LV3EN6_WAITING) {
			int en6BaseY = bridges3[5].snapY + LV3EN6_LAND_Y_OFF;
			int en6DrawX = (int)lv3En6X - lv3CameraX;
			int en6DrawY = en6BaseY;
			int en6W = LV3EN6_IDLE_W;
			int en6H = LV3EN6_IDLE_H;
			int en6Img = -1;

			switch (lv3En6State) {
			case LV3EN6_APPEARING:
				en6Img = lv3En6AprFrames[lv3En6FrameIndex];
				en6W = LV3EN6_APR_W;    en6H = LV3EN6_APR_H;
				en6DrawX += LV3EN6_APR_X_OFF;
				en6DrawY = (int)lv3En6Y + LV3EN6_APR_Y_OFF;
				break;
			case LV3EN6_IDLE:
				en6Img = lv3En6IdleFrames[lv3En6FrameIndex];
				en6W = LV3EN6_IDLE_W;   en6H = LV3EN6_IDLE_H;
				en6DrawX += LV3EN6_IDLE_X_OFF;
				en6DrawY += LV3EN6_IDLE_Y_OFF;
				break;
			case LV3EN6_RUNNING:
				en6Img = lv3En6RunFrames[lv3En6FrameIndex];
				en6W = LV3EN6_RUN_W;    en6H = LV3EN6_RUN_H;
				en6DrawX += LV3EN6_RUN_X_OFF;
				en6DrawY += LV3EN6_RUN_Y_OFF;
				break;
			case LV3EN6_PUNCHING:
				en6Img = lv3En6PunchFrames[lv3En6FrameIndex];
				en6W = LV3EN6_PUNCH_W;  en6H = LV3EN6_PUNCH_H;
				en6DrawX += LV3EN6_PUNCH_X_OFF;
				en6DrawY += LV3EN6_PUNCH_Y_OFF;
				break;
			case LV3EN6_HIT_REACT:
				en6Img = lv3En6HitFrames[lv3En6FrameIndex];
				en6W = LV3EN6_HIT_W;    en6H = LV3EN6_HIT_H;
				en6DrawX += LV3EN6_HIT_X_OFF;
				en6DrawY += LV3EN6_HIT_Y_OFF;
				break;
			case LV3EN6_REVIVING:
				en6Img = lv3En6ReviveFrames[lv3En6FrameIndex];
				en6W = LV3EN6_REVIVE_W; en6H = LV3EN6_REVIVE_H;
				en6DrawX += LV3EN6_REVIVE_X_OFF;
				en6DrawY += LV3EN6_REVIVE_Y_OFF;
				break;
			case LV3EN6_DYING:
			case LV3EN6_TRULY_DEAD:
				en6Img = lv3En6DeadFrames[lv3En6FrameIndex];
				en6W = LV3EN6_DEAD_W;   en6H = LV3EN6_DEAD_H;
				en6DrawX += LV3EN6_DEAD_X_OFF;
				en6DrawY += LV3EN6_DEAD_Y_OFF;
				break;
			default: break;
			}

			if (en6Img != -1)
				iShowImage(en6DrawX, en6DrawY, en6W, en6H, en6Img);

			// ── Debug hitboxes ───────────────────────────────────────────
			if (LV3EN6_DRAW_DEBUG &&
				lv3En6State != LV3EN6_DYING &&
				lv3En6State != LV3EN6_TRULY_DEAD) {

				// Body hitbox — cyan
				int hX1 = (int)lv3En6X + (int)(LV3EN6_IDLE_W * LV3EN6_HB_LEFT) - lv3CameraX;
				int hX2 = (int)lv3En6X + (int)(LV3EN6_IDLE_W * LV3EN6_HB_RIGHT) - lv3CameraX;
				int hY1 = en6BaseY + (int)(LV3EN6_IDLE_H * LV3EN6_HB_TOP);
				int hY2 = en6BaseY + (int)(LV3EN6_IDLE_H * LV3EN6_HB_BOTTOM);
				iSetColor(0, 255, 255);
				iRectangle(hX1, hY1, hX2 - hX1, hY2 - hY1);

				// Attack hitbox — red, only on lethal punch frames
				if (lv3En6State == LV3EN6_PUNCHING &&
					(lv3En6FrameIndex == LV3EN6_LETHAL_FRAME_A ||
					lv3En6FrameIndex == LV3EN6_LETHAL_FRAME_B)) {
					int aX1 = (int)lv3En6X + LV3EN6_PUNCH_X_OFF +
						(int)(LV3EN6_PUNCH_W * LV3EN6_ATK_HB_LEFT) - lv3CameraX;
					int aX2 = (int)lv3En6X + LV3EN6_PUNCH_X_OFF +
						(int)(LV3EN6_PUNCH_W * LV3EN6_ATK_HB_RIGHT) - lv3CameraX;
					int aY1 = en6BaseY + LV3EN6_PUNCH_Y_OFF +
						(int)(LV3EN6_PUNCH_H * LV3EN6_ATK_HB_TOP);
					int aY2 = en6BaseY + LV3EN6_PUNCH_Y_OFF +
						(int)(LV3EN6_PUNCH_H * LV3EN6_ATK_HB_BOTTOM);
					iSetColor(255, 0, 0);
					iRectangle(aX1, aY1, aX2 - aX1, aY2 - aY1);
				}
			}
		}

		// ── Level 3 Barrel (Bridge 5 right edge) ────────────────────────
		if (barrelState >= 0 && barrelState <= 2) {
			// Draw barrel (bar1, bar2, bar3 only — bar4 is transitional, auto-skipped)
			int drawX = (int)(bridges3[4].maxX + BARREL_X_OFF) - lv3CameraX;
			int drawY = (int)(bridges3[4].snapY + BARREL_Y_OFF);
			iShowImage(drawX, drawY, BARREL_W, BARREL_H, barrelFrames[barrelState]);

			/*// ── Debug hitbox — magenta ─────────────────────────────────
			int hX1 = (int)barrelWorldX + (int)(BARREL_W * BARREL_HB_LEFT) - lv3CameraX;
			int hX2 = (int)barrelWorldX + (int)(BARREL_W * BARREL_HB_RIGHT) - lv3CameraX;
			int hY1 = (int)barrelWorldY + (int)(BARREL_H * BARREL_HB_TOP);
			int hY2 = (int)barrelWorldY + (int)(BARREL_H * BARREL_HB_BOTTOM);
			iSetColor(255, 0, 255);
			iRectangle(hX1, hY1, hX2 - hX1, hY2 - hY1);*/
		}
		else if (barrelState == 4) {
			// Draw energy animation (loops)
			int drawX = (int)(bridges3[4].maxX + ENERGY_X_OFF) - lv3CameraX;
			int drawY = (int)(bridges3[4].snapY + ENERGY_Y_OFF);
			iShowImage(drawX, drawY, ENERGY_W, ENERGY_H, energyFrames[energyFrameIndex]);

			/*// ── Debug hitbox — bright yellow ───────────────────────────
			float energyWX = (float)(bridges3[4].maxX + ENERGY_X_OFF);
			float energyWY = (float)(bridges3[4].snapY + ENERGY_Y_OFF);
			int hX1 = (int)energyWX + (int)(ENERGY_W * ENERGY_HB_LEFT) - lv3CameraX;
			int hX2 = (int)energyWX + (int)(ENERGY_W * ENERGY_HB_RIGHT) - lv3CameraX;
			int hY1 = (int)energyWY + (int)(ENERGY_H * ENERGY_HB_TOP);
			int hY2 = (int)energyWY + (int)(ENERGY_H * ENERGY_HB_BOTTOM);
			iSetColor(255, 255, 0);
			iRectangle(hX1, hY1, hX2 - hX1, hY2 - hY1);*/
		}

		// ── Level 3 Barrel 2 (Bridge 9 right edge) ──────────────────────
		if (barrel2State >= 0 && barrel2State <= 2) {
			int drawX = (int)(bridges3[8].maxX + BARREL2_X_OFF) - lv3CameraX;
			int drawY = (int)(bridges3[8].snapY + BARREL2_Y_OFF);
			iShowImage(drawX, drawY, BARREL2_W, BARREL2_H, barrelFrames[barrel2State]);

			/*int hX1 = (int)barrel2WorldX + (int)(BARREL2_W * BARREL2_HB_LEFT) - lv3CameraX;
			int hX2 = (int)barrel2WorldX + (int)(BARREL2_W * BARREL2_HB_RIGHT) - lv3CameraX;
			int hY1 = (int)barrel2WorldY + (int)(BARREL2_H * BARREL2_HB_TOP);
			int hY2 = (int)barrel2WorldY + (int)(BARREL2_H * BARREL2_HB_BOTTOM);
			iSetColor(255, 0, 255);
			iRectangle(hX1, hY1, hX2 - hX1, hY2 - hY1);*/
		}
		else if (barrel2State == 4) {
			int drawX = (int)(bridges3[8].maxX + ENERGY2_X_OFF) - lv3CameraX;
			int drawY = (int)(bridges3[8].snapY + ENERGY2_Y_OFF);
			iShowImage(drawX, drawY, ENERGY2_W, ENERGY2_H, energyFrames[energy2FrameIndex]);

			/*float e2WX = (float)(bridges3[8].maxX + ENERGY2_X_OFF);
			float e2WY = (float)(bridges3[8].snapY + ENERGY2_Y_OFF);
			int hX1 = (int)e2WX + (int)(ENERGY2_W * ENERGY2_HB_LEFT) - lv3CameraX;
			int hX2 = (int)e2WX + (int)(ENERGY2_W * ENERGY2_HB_RIGHT) - lv3CameraX;
			int hY1 = (int)e2WY + (int)(ENERGY2_H * ENERGY2_HB_TOP);
			int hY2 = (int)e2WY + (int)(ENERGY2_H * ENERGY2_HB_BOTTOM);
			iSetColor(255, 255, 0);
			iRectangle(hX1, hY1, hX2 - hX1, hY2 - hY1);*/
		}

		// ── Level 3 Tree Obstacles ───────────────────────────────────────
		for (int i = 0; i < NUM_TREES_LV3; i++) {
			int drawX = (int)treesLv3[i].x - lv3CameraX;
			int drawY = (int)treesLv3[i].y;
			iShowImage(drawX, drawY, TREE_W, TREE_H, treeFrames[treeAnimIndex]);

			if (TREE_DRAW_DEBUG) {
				int hX1 = (int)treesLv3[i].x + (int)(TREE_W * TREE_HB_LEFT) - lv3CameraX;
				int hX2 = (int)treesLv3[i].x + (int)(TREE_W * TREE_HB_RIGHT) - lv3CameraX;
				int hY1 = (int)treesLv3[i].y + (int)(TREE_H * TREE_HB_TOP);
				int hY2 = (int)treesLv3[i].y + (int)(TREE_H * TREE_HB_BOTTOM);
				iSetColor(255, 0, 255);   // magenta — tree hitbox
				iRectangle(hX1, hY1, hX2 - hX1, hY2 - hY1);
			}
		}

		// ── Level 3 Power-Up Tokens ──────────────────────────────────────
		for (int i = 0; i < NUM_PUPTK_LV3; i++) {
			if (!puptkLv3[i].collected) {
				int drawX = (int)puptkLv3[i].x - lv3CameraX;
				int drawY = (int)puptkLv3[i].y;
				iShowImage(drawX, drawY, PUPTK_W, PUPTK_H, puptkFrames[puptkAnimIndex]);
			}
		}
		// ── Power-up token pickup text ──
		for (int i = 0; i < NUM_PUPTK_LV3; i++) {
			if (puptkShowText[i]) {
				int textX = (int)puptkLv3[i].x - lv3CameraX - 30;
				int textY = (int)puptkLv3[i].y + PUPTK_H + 40;
				iSetColor(0, 255, 255);
				iText(textX, textY, "+1 charged power gained!", GLUT_BITMAP_HELVETICA_18);
			}
		}
		drawLv3Debug();      // existing bridge outlines
		//drawLv3PlayerDebug();  // ← ADD: cyan player hitbox

		// ── Level 3 Win Flag on Bridge 11 ──
		if (!playerWonLv3) {
			Bridge &bridge11 = bridges3[10];
			int flagX = bridge11.maxX - WINFLAG_WIDTH + WINFLAG_X_OFFSET - 60;
			int flagY = bridge11.snapY + WINFLAG_Y_OFFSET + 10;
			iShowImage(flagX - lv3CameraX, flagY, WINFLAG_WIDTH, WINFLAG_HEIGHT + 50, winFlagImg);
		}
		if (playerWonLv3) {
			int scrollX = (SCREEN_WIDTH - WINNER_WIDTH) / 2;
			int scrollY = (SCREEN_HEIGHT - WINNER_HEIGHT) / 2;
			iShowImage(scrollX, scrollY, WINNER_WIDTH, WINNER_HEIGHT, winnerLv3Img);
		}
		if (showFinalWin) {
			iShowImage(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, finalWinImg);
		}
		// ── Level 3 Boss ──────────────────────────────────────────────
		if (lv3BossActive && !lv3BossDead) {
			int bossDrawX = (int)lv3BossX - lv3CameraX;
			int bossDrawY = bridges3[9].snapY + LV3BOSS_YOFF;
			int bossImg = -1;

			switch (lv3BossState) {
			case LV3BOSS_WALKING:
				bossImg = lv3BossWalkFrames[lv3BossFrameIndex];
				break;
			case LV3BOSS_FIRE:
				bossImg = lv3BossFireFrames[lv3BossFrameIndex];
				break;
			case LV3BOSS_JUMP:
				bossImg = lv3BossJumpFrames[lv3BossFrameIndex];
				break;
			case LV3BOSS_SLASH:
				bossImg = lv3BossSlashFrames[lv3BossFrameIndex];
				break;
			}

			if (bossImg != -1)
				iShowImage(bossDrawX, bossDrawY, LV3BOSS_W, LV3BOSS_H, bossImg);
			// ── Boss Health HUD (above boss head) ──────────────────────
			int hudW = 110;
			int hudH = 15;
			int hudX = bossDrawX + (LV3BOSS_W - hudW) / 2 + 10;
			int hudY = bossDrawY + LV3BOSS_H - 160;  // tweak +20 up/down above head

			// How many "lives" remain: each life = 4 hits
			int bossLivesRemaining = LV3BOSS_LIVES_TO_KILL - (lv3BossHitCount / 4);
			if (bossLivesRemaining < 0) bossLivesRemaining = 0;
			float bossHpRatio = (float)bossLivesRemaining / (float)LV3BOSS_LIVES_TO_KILL;

			// Dark background
			iSetColor(20, 20, 20);
			iFilledRectangle(hudX, hudY, hudW, hudH);

			// Dark red fill
			if (bossHpRatio > 0) {
				iSetColor(160, 0, 0);
				iFilledRectangle(hudX, hudY, (int)(hudW * bossHpRatio), hudH);
			}

			// Bold black outline (draw 3 times offset for thickness)
			iSetColor(0, 0, 0);
			iRectangle(hudX - 1, hudY - 1, hudW + 2, hudH + 2);
			iRectangle(hudX - 2, hudY - 2, hudW + 4, hudH + 4);
			iRectangle(hudX - 3, hudY - 3, hudW + 6, hudH + 6);
			iRectangle(hudX - 4, hudY - 4, hudW + 8, hudH + 8);
			iRectangle(hudX, hudY, hudW, hudH);

		}
		// ── Dead boss image flat on bridge 10 ──────────────────────
		if (lv3BossDead && lv3BossDeadImg != -1 && !showFinalWin) {
			int bossDrawX = (int)lv3BossX - lv3CameraX;
			int bossDrawY = bridges3[9].snapY - 20;
			iShowImage(bossDrawX, bossDrawY, LV3BOSS_W, LV3BOSS_H, lv3BossDeadImg);
		}
		// ── Level 3 charged punch power-up overlay ──────────────────────
		if (currentBg == 12 && playerAlive && chpAnimPlaying) {
			int drawX = lv3CharX - lv3CameraX;

			// Calculate throw offset: starts at 0, travels to CHP_THROW_DIST over 7 frames
			float progress = (float)chpFrameIndex / 6.0f;  // 0.0 to 1.0
			int throwOffset = (int)(progress * CHP_THROW_DIST);

			int overlayX, overlayY;
			overlayY = lv3CharY + CHP_Y_OFF;

			if (movingFront) {

				overlayX = drawX + CHP_X_OFF - throwOffset;
				iShowImage(overlayX, overlayY, CHP_W, CHP_H, chpFramesM[chpFrameIndex]);

			}
			else {

				overlayX = drawX - CHP_X_OFF + throwOffset;
				iShowImage(overlayX, overlayY, CHP_W, CHP_H, chpFrames[chpFrameIndex]);
			}
		}
		// Charge scroll overlay
		if (showChargeScroll && !chargeScrollDismissed)
			iShowImage((SCREEN_WIDTH - 400) / 2, (SCREEN_HEIGHT - 400) / 2,
			400, 300, chargeScrollImg);
	}
	else if (currentBg == 10) {
		// Draw bg5.1 with camera offset — exactly like bg41
		iShowImage(-lv2CameraX, 0, LV2_BG_WIDTH, LV2_BG_HEIGHT, bg51);

		//-------boss1 draw-----//
		if (!boss1Dead) {
			Bridge &b7 = bridges2[6];
			int cx = lv2CharX + PLAYER_WIDTH / 2;
			bool playerOnBridge7 = (cx >= b7.minX && cx <= b7.maxX);

			if (playerOnBridge7 && boss1Active) {
				int bossW = PLAYER_WIDTH + 250;
				int bossH = PLAYER_HEIGHT + 150;
				int bossX = (int)boss1X;
				int bossY =
					b7.snapY + (boss1Moving ? (int)(sin(boss1FloatOffset) * 10.0f) : 0);
				iShowImage(bossX - lv2CameraX, bossY, bossW, bossH,
					boss1Frames[boss1FrameIndex]);
			}
		}
		else {
			// Boss dead — show dead image flat on bridge 7
			Bridge &b7 = bridges2[6];
			int bossW = PLAYER_WIDTH + 70;
			int bossH = PLAYER_HEIGHT + 140;
			int bossX = (int)boss1X;
			int bossY = b7.snapY - 30; // tweak -30 up/down to sit flush on bridge
			iShowImage(bossX - lv2CameraX, bossY, bossW, bossH, boss1DeadImg);
		}

		// Shield animation on Bridge 9 (bridges2[8])
		if (currentBg == 10 && !playerWonLv2) {
			static int shieldCounter = 0;
			shieldCounter++;
			if (shieldCounter >= 18) {
				shieldCounter = 0;
				shieldFrameIndex = (shieldFrameIndex + 1) % 14;
			}
			Bridge &b9 = bridges2[8];
			int shieldX = (b9.minX + b9.maxX) / 2 - 40;
			int shieldY = b9.snapY + 30;
			iShowImage(shieldX - lv2CameraX, shieldY, 90, 120,
				shieldFrames[shieldFrameIndex]);
			// ── "Infinity Shield" label ──────────────────────────────
			iSetColor(180, 100, 255); // light purple
			iText(shieldX - lv2CameraX - 10, shieldY + 105, "Infinity Shield",
				GLUT_BITMAP_HELVETICA_18);
		}

		// ---- Draw lvl2enemy1 ----
		if (lv2e1Alive && lv2e1Active) {
			int drawX = (int)lv2e1X - lv2CameraX;
			int drawY = bridges2[2].snapY + LV2E1YOFFSET;

			int frameImg;
			if (lv2e1MovingRight) {
				frameImg = lv2e1FramesRight[lv2e1FrameIndex]; // en1.1.gif .. en1.9.gif
			}
			else {
				frameImg = lv2e1FramesLeft[lv2e1FrameIndex]; // en1.1o.gif .. en1.9o.gif
			}

			iShowImage(drawX, drawY, LV2E1WIDTH, LV2E1HEIGHT, frameImg);
		}
		else if (!lv2e1Alive) {
			int drawX = (int)lv2e1X - lv2CameraX;
			int drawY = (int)lv2e1DeadY + 25;
			iShowImage(drawX, drawY, LV2E1WIDTH, LV2E1HEIGHT, lv2e1DeadImg);
		}

		// ---- Draw lvl2enemy2 over Bridge 5 ----
		if (lv2e2Alive) {
			int drawX = (int)lv2e2X - lv2CameraX;
			int drawY = bridges2[LV2E2BRIDGEINDEX].snapY + LV2E2YOFFSET;

			int frameImg;
			if (lv2e2MovingRight)
				frameImg = lv2e2FramesRight[lv2e2FrameIndex]; // en2.*o.png
			else
				frameImg = lv2e2FramesLeft[lv2e2FrameIndex]; // en2.*.png

			iShowImage(drawX, drawY, LV2E2WIDTH, LV2E2HEIGHT, frameImg);

		}
		else if (!lv2e2Alive) {
			int drawX = (int)lv2e2X - lv2CameraX;
			int drawY = (int)lv2e2DeadY + 130;
			iShowImage(drawX, drawY, LV2E2WIDTH, LV2E2HEIGHT, lv2e2DeadImg);
		}

		// ---- Draw lvl2wkenemy (stationary, always visible on bridge 3) ----
		if (lv2wkAlive) {
			int drawX = (int)lv2wkX - lv2CameraX;
			int drawY = (int)lv2wkY;
			iShowImage(drawX, drawY, LV2WK_WIDTH, LV2WK_HEIGHT,
				lv2wkFrames[lv2wkFrameIndex]);
		}

		// ---- Draw lvl2wkenemy2 (stationary, always visible on bridge 3 right)
		// ----
		if (lv2wk2Alive) {
			int drawX = (int)lv2wk2X - lv2CameraX;
			int drawY = (int)lv2wk2Y;
			iShowImage(drawX, drawY, LV2WK2_WIDTH, LV2WK2_HEIGHT,
				lv2wk2Frames[lv2wk2FrameIndex]);
		}

		// ---- Draw lvl2wkenemy3 (stationary, right edge of bridge 5) ----
		if (lv2wk3Alive) {
			int drawX = (int)lv2wk3X - lv2CameraX;
			int drawY = (int)lv2wk3Y;
			iShowImage(drawX, drawY, LV2WK3_WIDTH, LV2WK3_HEIGHT,
				lv2wkFrames[lv2wk3FrameIndex]);
		}

		// ---- Draw fires ----
		if (currentBg == 10) {
			int fireW[NUM_FIRES] = { FIRE0_W, FIRE1_W, FIRE2_W, FIRE3_W, FIRE4_W };
			int fireH[NUM_FIRES] = { FIRE0_H, FIRE1_H, FIRE2_H, FIRE3_H, FIRE4_H };

			for (int i = 0; i < numFires; i++) {
				int drawX = (int)fires[i].x - lv2CameraX;
				int drawY = (int)fires[i].y;
				iShowImage(drawX, drawY, fireW[i], fireH[i], fireFrames[fireAnimIndex]);
			}
		}

		// ---- Draw lvl2collectibles ----
		if (currentBg == 10) {
			int tSize = 35;
			for (int i = 0; i < numCollectiblesLv2; i++) {
				if (!collectiblesLv2[i].collected) {
					int drawX = collectiblesLv2[i].x - lv2CameraX;
					int drawY = collectiblesLv2[i].y;
					iShowImage(drawX, drawY, tSize, tSize, tokenImgLv2);
				}
			}
		}

		// Dead player draw
		if (!playerAlive) {
			if (killedByFire) {
				int dW = LV2_DEAD_FIRE_W;
				int dH = LV2_DEAD_FIRE_H;
				int drawX = (int)fireDeathWorldX - lv2CameraX + LV2_DEAD_FIRE_XOFFSET -
					(dW - PLAYER_WIDTH) / 2;
				int drawY = playerDeathY + LV2_DEAD_FIRE_YOFFSET;
				iShowImage(drawX, drawY, dW, dH, playerDeadImg);
			}
			else {
				int dW = LV2_DEAD_BRIDGE_W;
				int dH = LV2_DEAD_BRIDGE_H;
				int drawX = lv2CharX - lv2CameraX + LV2_DEAD_BRIDGE_XOFFSET -
					(dW - PLAYER_WIDTH) / 2;
				int drawY = playerDeathY + LV2_DEAD_BRIDGE_YOFFSET;
				iShowImage(drawX, drawY, dW, dH, playerDeadImg);
			}
		}
		else {
			int drawX = lv2CharX - lv2CameraX;

			if (!showKneel) { // ← ADD THIS
				if (chargingPunch && chargeScrollDismissed) {
					if (chargedSequenceActive) {
						if (chargedPunchState == 0)
							iShowImage(drawX, lv2CharY, PLAYER_WIDTH, PLAYER_HEIGHT,
							movingFront ? chargedFistImg : chargedFistImgM);
						else
							iShowImage(drawX, lv2CharY, PLAYER_WIDTH, PLAYER_HEIGHT,
							movingFront ? chargedPunchImgSeq
							: chargedPunchImgSeqM);
					}
					else {
						iShowImage(drawX, lv2CharY, PLAYER_WIDTH, PLAYER_HEIGHT,
							movingFront ? chargedPunchImg : chargedPunchImgM);
					}
				}
				else {
					int img;
					if (punchState == 0)
						img = movingFront ? (animFrame ? playerSideLeft : playerSide)
						: (animFrame ? playerSideLeftM : playerSideM);
					else if (punchState == 1)
						img = movingFront ? punchFistImg : punchFistImgM;
					else
						img = movingFront ? punchHitImg : punchHitImgM;

					iShowImage(drawX, lv2CharY, PLAYER_WIDTH, PLAYER_HEIGHT, img);
				}
			} // ← CLOSE !showKneel
		}

		// Kneel — standalone block
		if (showKneel && playerAlive) {
			iShowImage(lv2CharX - lv2CameraX, lv2CharY, PLAYER_WIDTH, PLAYER_HEIGHT,
				kneelImg);
		}

		if (showChargeScroll && !chargeScrollDismissed)
			iShowImage((SCREEN_WIDTH - 400) / 2, (SCREEN_HEIGHT - 400) / 2, 400, 300,
			chargeScrollImg);
	}
	// ---------------- LEVEL COLLECTIBLES ----------------
	if (currentBg == 4) {
		int tSize = 35;
		int trW = 80, trH = 80;

		for (int i = 0; i < numCollectibles; i++) {
			if (!collectibles[i].collected) {
				int drawX = collectibles[i].x - cameraX;
				int drawY = collectibles[i].y;
				int img = collectibles[i].isTreasure ? treasureImg : tokenImg;
				int w = collectibles[i].isTreasure ? trW : tSize;
				int h = collectibles[i].isTreasure ? trH : tSize;

				iShowImage(drawX, drawY, w, h, img);
			}
		}
	}

	// ============ ADD SIGNPOST HERE (BEFORE ENEMIES) ============
	if (currentBg == 4) {
		Bridge &bridge6 = bridges[5];       // Bridge 6 is index 5 (0-based)
		int signpostX = bridge6.minX - 60;  // Left edge of bridge 6
		int signpostY = bridge6.snapY - 15; // On the bridge surface

		iShowImage(signpostX - cameraX, signpostY, SIGNPOST_WIDTH, SIGNPOST_HEIGHT,
			signpostImg);
	}
	// ============ DRAW WIN FLAG ON BRIDGE 10 ============
	if (currentBg == 4) {
		Bridge &bridge10 = bridges[9]; // Bridge 10 is index 9 (0-based)
		int flagX = bridge10.maxX - WINFLAG_WIDTH +
			WINFLAG_X_OFFSET;                  // Right edge of bridge 10
		int flagY = bridge10.snapY + WINFLAG_Y_OFFSET; // On the bridge surface

		iShowImage(flagX - cameraX, flagY, WINFLAG_WIDTH, WINFLAG_HEIGHT,
			winFlagImg);
	}

	// ---------------- ENEMY1 DRAW ----------------
	if (currentBg == 4) {
		Bridge &b = bridges[ENEMY1_BRIDGE_INDEX];
		int enemy1X = b.maxX - ENEMY1_WIDTH - 60;

		if (enemy1Alive) {
			// Float up and down using sin()
			int drawY = b.snapY + (int)(sin(enemy1FloatOffset) * enemy1FloatRange);
			iShowImage(enemy1X - cameraX, drawY, ENEMY1_WIDTH, ENEMY1_HEIGHT,
				swapEnemySprites ? enemy4Img : enemy1Img);
		}
		else {
			// Dead enemy
			int drawY = enemy1DeadY - ENEMY1_HEIGHT + 140; // already set on bridge
			iShowImage(enemy1X - cameraX, drawY, ENEMY1_WIDTH, ENEMY1_HEIGHT,
				swapEnemySprites ? enemy4DeadImg : enemy1DeadImg);
		}
	}


	// ---------------- ENEMY4 DRAW ----------------
	if (currentBg == 4) {
		Bridge &b = bridges[ENEMY4_BRIDGE_INDEX];
		int enemy4X = b.minX + 100; // EXACT left edge of bridge

		if (enemy4Alive) {
			// Float up and down using sin()
			int drawY = b.snapY + ENEMY4_HEIGHT_OFFSET +
				(int)(sin(enemy4FloatOffset) * enemy4FloatRange);
			iShowImage(enemy4X - cameraX, drawY, ENEMY4_WIDTH, ENEMY4_HEIGHT,
				swapEnemySprites ? enemy1Img : enemy4Img);
		}
		else {
			// Dead enemy - sits FLUSH on bridge
			int drawY = b.snapY - ENEMY4_HEIGHT +
				130; // Adjust so bottom of image touches bridge
			iShowImage(enemy4X - cameraX, drawY, ENEMY4_WIDTH, ENEMY4_HEIGHT,
				swapEnemySprites ? enemy1DeadImg : enemy4DeadImg);
		}
	}

	// ---------------- ENEMY3 DRAW ----------------
	if (currentBg == 4) {
		Bridge &b = bridges[ENEMY3_BRIDGE_INDEX];

		if (enemy3Alive) {
			int drawY =
				BG41_GROUND_Y + (int)(sin(enemy3WalkOffset) * enemy3WalkAmplitude);

			int img;

			if (enemy3State == E3_ATTACKING) {
				if (enemy3MovingLeft)
					img = enemy3PunchToggle ? enemy3PunchImg : enemy3FistImg;
				else
					img = enemy3PunchToggle ? enemy3PunchImgM : enemy3FistImgM;
			}
			else {
				if (enemy3MovingLeft)
					img = enemy3AnimFrame ? enemy3WalkImg : enemy3IdleImg;
				else
					img = enemy3AnimFrame ? enemy3WalkImgM : enemy3IdleImgM;
			}

			iShowImage(enemy3X - cameraX, drawY, ENEMY3_WIDTH, ENEMY3_HEIGHT, img);
		}
		else {
			// ---------------- ENEMY3 DEAD DRAW (FIXED SIZE) ----------------
			float widthScale = 2.7f;

			int deadW = (int)(ENEMY3_WIDTH * widthScale);
			int deadH = ENEMY3_HEIGHT + 5;

			int drawX = enemy3X - cameraX - (deadW - ENEMY3_WIDTH) / 2;
			int drawY = enemy3DeadY - 90; //  TOUCHES BRIDGE

			iShowImage(drawX, drawY, deadW, deadH, enemy3DeadImg);
		}
	}

	// ---------------- ENEMY2 DRAW ----------------
	if (currentBg == 4) {
		int drawW = (int)(ENEMY2_WIDTH * ENEMY2_SCALE);
		int drawH = (int)(ENEMY2_HEIGHT * ENEMY2_SCALE);

		// center scaling so it doesn't shift sideways
		int drawX = enemy2X - cameraX + (ENEMY2_WIDTH - drawW) / 2;
		int drawY = enemy2Y + (ENEMY2_HEIGHT - drawH) / 2;

		// Draw Enemy2 while alive
		if (!enemy2Dead) {
			iShowImage(drawX, drawY, drawW, drawH,
				enemy2MovingLeft ? enemy2LeftImg : enemy2RightImg);
		}
		else {
			// Draw dead Enemy2 BIGGER
			float deadScale = 1.2f; // 120% size (increase this for even bigger)
			int deadW = (int)(ENEMY2_WIDTH * deadScale);
			int deadH = (int)(ENEMY2_HEIGHT * deadScale);

			// Center the dead body horizontally
			int deadDrawX = enemy2X - cameraX - (deadW - ENEMY2_WIDTH) / 2;

			// Position dead body lower on the ground
			int deadDrawY = enemy2DeadY - 90;

			iShowImage(deadDrawX, deadDrawY, deadW, deadH, enemy2DeadImg);
		}
	}
	// ---------------- DRAGON DRAW (GROUND BETWEEN 9th and 10th BRIDGE)
	if (currentBg == 4) {
		Bridge &b = bridges[DRAGON_BRIDGE_INDEX];

		int dragonX = b.minX + DRAGON_X_OFFSET;
		int dragonY = b.snapY - 5;

		if (dragonAlive) {
			iShowImage(dragonX - cameraX, dragonY, DRAGON_WIDTH, DRAGON_HEIGHT,
				dragonFrames[dragonFrameIndex]);
		}
		else {
			int deadOffsetY = -60;
			iShowImage(dragonX - cameraX, dragonY + deadOffsetY, DRAGON_WIDTH,
				DRAGON_HEIGHT, dragonDeadImg);
		}
	}

	// ---------------- PLAYER DRAW ----------------

	if (!playerAlive && currentBg == 4) {
		int drawX = playerX - cameraX;

		int deadWidth = PLAYER_WIDTH + 40;
		int deadHeight = PLAYER_HEIGHT + 40;

		int drawY;
		if (killedByEnemy1)
			drawY = playerDeathY - 110;
		else if (killedByDragon)
			drawY = playerDeathY - 90;
		else if (killedByEnemy3)
			drawY = playerDeathY - 150; // Places bottom at ground level
		else if (killedByEnemy2)
			drawY = playerDeathY - 90;
		else if (killedByEnemy4)
			drawY = playerDeathY - 100;
		else
			drawY = BG41_GROUND_Y - 90;

		drawX -= (deadWidth - PLAYER_WIDTH) / 2;

		iShowImage(drawX, drawY, deadWidth, deadHeight, playerDeadImg);
	}

	else if (currentBg == 4) {
		int drawX = (currentBg == 4) ? playerX - cameraX : playerX;

		// Charged attack
		if (chargingPunch && chargeScrollDismissed) {
			if (chargedSequenceActive) {
				if (chargedPunchState == 0)
					iShowImage(drawX, playerY, PLAYER_WIDTH, PLAYER_HEIGHT,
					movingFront ? chargedFistImg : chargedFistImgM);
				else
					iShowImage(drawX, playerY, PLAYER_WIDTH, PLAYER_HEIGHT,
					movingFront ? chargedPunchImgSeq : chargedPunchImgSeqM);
			}
			else {
				iShowImage(drawX, playerY, PLAYER_WIDTH, PLAYER_HEIGHT,
					movingFront ? chargedPunchImg : chargedPunchImgM);
			}
		}
		// Normal attack
		else {
			int img;
			if (punchState == 0)
				img = movingFront ? (animFrame ? playerSideLeft : playerSide)
				: (animFrame ? playerSideLeftM : playerSideM);
			else if (punchState == 1)
				img = movingFront ? punchFistImg : punchFistImgM;
			else
				img = movingFront ? punchHitImg : punchHitImgM;

			iShowImage(drawX, playerY, PLAYER_WIDTH, PLAYER_HEIGHT, img);
		}
	}

	// ---------------- CHARGE SCROLL ----------------
	if (showChargeScroll && !chargeScrollDismissed) {
		iShowImage((SCREEN_WIDTH - 400) / 2, (SCREEN_HEIGHT - 400) / 2, 400, 300,
			chargeScrollImg);
	}

	// ---------------- BACK BUTTON (ADDED) ----------------
	if (currentBg == 5 || currentBg == 6 || currentBg == 7) {
		iShowImage(backX, backY, backW, backH, backHover ? backBtnHover : backBtn);
	}
	// ============ DRAW WINNER SCREEN ============
	if (playerWon) {
		int scrollX = (SCREEN_WIDTH - WINNER_WIDTH) / 2;
		int scrollY = (SCREEN_HEIGHT - WINNER_HEIGHT) / 2;
		iShowImage(scrollX, scrollY, WINNER_WIDTH, WINNER_HEIGHT, winnerImg);
	}
	if (playerWonLv2) {
		int scrollX = (SCREEN_WIDTH - WINNER_WIDTH) / 2;
		int scrollY = (SCREEN_HEIGHT - WINNER_HEIGHT) / 2;
		iShowImage(scrollX, scrollY, WINNER_WIDTH, WINNER_HEIGHT, winnerLv2Img);
	}
	drawHUD();
}

void iKeyboard(unsigned char key) {
	if (playerWon) {
		if (key == ' ') {
			playerWon = false;
			showInstruction2 = true;
			currentBg = 9; // new screen state for instruction2
		}
		return;
	}
	if (playerWonLv2) {
		if (key == ' ') {
			playerWonLv2 = false;
			currentBg = 11;   // go to instruction3
		}
		return;
	}
	if (playerWonLv3) {
		if (key == ' ') {
			if (!showFinalWin) {
				showFinalWin = true;
			}
		}
		return;
	}
	if (currentBg == 9 && key == ' ') {
		showInstruction2 = false;
		currentLevel = 2;
		currentBg = 10;
		lv2CameraX = 0;
		lv2CharX = 100; // start position in world
		lv2CharX = 100;
		lv2CharY = bridges2[0].snapY;
		lv2IsJumping = false;
		lv2JumpVelocity = 0;
		chargeScrollShown = false;
		chargeScrollDismissed = false;
		chargingPunch = false;
		chargedSequenceActive = false;
		chargedUseCount = 0;
		chargedPunchState = 0;
		chargedTimeCounter = 0;
		punchState = 0;
		punchJustLanded = false;
		// In the currentBg==9 && key==' ' block, ADD these resets:
		boss1HitCount = 0;
		boss1ChargedHits = 0;
		boss1Dead = false;
		boss1Active = true;
		killedByBoss1 = false;
		showKneel = false;
		kneelTimer = 0;
		boss1Moving = true;
		boss1FrameIndex = 0;
		boss1X = (float)(bridges2[6].maxX - (PLAYER_WIDTH + 70) - 50);
		// Reset lvl2enemy1
		lv2e1Alive = true;
		lv2e1Active = false;
		lv2e1HitCount = 0;
		lv2e1Falling = false;
		lv2e1MovingRight = false;
		lv2e1X = (float)(bridges2[2].maxX - LV2E1WIDTH);
		lv2e1DeadY = (float)(bridges2[2].snapY + LV2E1YOFFSET);
		lv2e1FrameIndex = 0;
		initCollectiblesLv2();
		tokenCountLv2 = 0;
		// Reset lvl2enemy2
		lv2e2Alive = true;
		lv2e2HitCount = 0;
		lv2e2Falling = false;
		lv2e2MovingRight = true;
		lv2e2X = (float)(bridges2[LV2E2BRIDGEINDEX].minX);
		lv2e2DeadY = (float)(bridges2[LV2E2BRIDGEINDEX].snapY + LV2E2YOFFSET);
		lv2e2FrameIndex = 0;
		// Reset lvl2wkenemy
		lv2wkAlive = true;
		lv2wkFrameIndex = 0;
		lv2wkKneeling = false;
		lv2wkKneelTimer = 0;
		lv2wkPlayerTouching = false;
		lv2wkX = (float)(bridges2[2].minX + LV2WK_XOFFSET);
		lv2wkY = (float)(bridges2[2].snapY + LV2WK_YOFFSET);
		// Reset lvl2wkenemy2
		lv2wk2Alive = true;
		lv2wk2FrameIndex = 0;
		lv2wk2Kneeling = false;
		lv2wk2KneelTimer = 0;
		lv2wk2PlayerTouching = false;
		lv2wk2X = (float)(bridges2[2].maxX - LV2WK2_WIDTH + LV2WK2_XOFFSET);
		lv2wk2Y = (float)(bridges2[2].snapY + LV2WK2_YOFFSET);
		// Reset lvl2wkenemy3
		lv2wk3Alive = true;
		lv2wk3FrameIndex = 0;
		lv2wk3Kneeling = false;
		lv2wk3KneelTimer = 0;
		lv2wk3PlayerTouching = false;
		lv2wk3X = (float)(bridges2[4].maxX - LV2WK3_WIDTH + LV2WK3_XOFFSET);
		lv2wk3Y = (float)(bridges2[4].snapY + LV2WK3_YOFFSET);
		// Reset fires
		initFires();
		fireAnimIndex = 0;
		killedByLv2e1 = false;
		killedByLv2e2 = false;
		killedByLv2wk = false;
		killedByLv2wk2 = false;
		killedByLv2wk3 = false;
		killedByFire = false;
		pendingLivesLoss = 1;
		// Reset Player
		playerAlive = true;      // make sure player is alive
		playerLives = MAX_LIVES; // reset lives for level 2
		deathWaiting = false;
		gameOverTriggered = false;
		gameOverScreen = false;
		return;
	}
	if (currentBg == 11 && key == ' ') {
		currentBg = 12;
		lv3CameraX = 0;
		lv3CharX = 100;
		lv3CharY = bridges3[0].snapY;
		lv3IsJumping = false;
		lv3JumpVelocity = 0;
		playerAlive = true;
		gameStarted = true;
		chargedUseCountLv3 = 0;      // ← ADD THIS
		chargeScrollShown = false;   // ← ADD THIS
		chargeScrollDismissed = false; // ← ADD THIS
		chargingPunch = false;         // ← ADD THIS
		chargedSequenceActive = false; // ← ADD THIS
		chargedPunchState = 0;         // ← ADD THIS
		chargedTimeCounter = 0;        // ← ADD THIS
		punchState = 0;                // ← ADD THIS
		punchJustLanded = false;       // ← ADD THIS
		lv3BossState = LV3BOSS_WALKING;
		lv3BossFrameIndex = 0;
		lv3BossFireLoopCount = 0;
		lv3BossActive = false;
		lv3BossDead = false;
		lv3BossHitCount = 0;
		lv3BossPlayerHitCount = 0;
		lv3BossSlashCycleCount = 0;
		lv3BossHitCooldown = 0;
		lv3BirdAlive = true;
		lv3BirdActive = false;
		lv3BirdHitCount = 0;
		lv3BirdFalling = false;
		lv3BirdFrameIndex = 0;
		lv3BirdWasColliding = false;
		lv3Bird2Alive = true;
		lv3Bird2Active = false;
		lv3Bird2HitCount = 0;
		lv3Bird2Falling = false;
		lv3Bird2FrameIndex = 0;
		lv3Bird2WasColliding = false;
		lv3Bird2SpawnTimer = 0;
		lv3Bird2X = (float)((bridges3[1].minX + bridges3[1].maxX) / 2);
		lv3BirdX = (float)((bridges3[1].minX + bridges3[1].maxX) / 2);
		lv3PBirdAlive = true;
		lv3PBirdActive = true;
		lv3PBirdFrameIndex = 0;
		lv3PBirdMovingRight = true;
		lv3PBirdWasColliding = false;
		lv3PBirdX = (float)bridges3[2].minX;
		lv3PBird2Alive = true;
		lv3PBird2Active = true;
		lv3PBird2FrameIndex = 0;
		lv3PBird2MovingRight = true;
		lv3PBird2WasColliding = false;
		lv3PBird2X = (float)bridges3[6].minX;
		lv3En1Alive = true;
		lv3En1Active = false;
		lv3En1State = LV3EN1_IDLE;
		lv3En1FrameIndex = 0;
		lv3En1AnimTick = 0;
		lv3En1NormalHits = 0;
		lv3En1ChargedHits = 0;
		lv3En1Dead = false;
		lv3En1Falling = false;
		lv3En1DeadY = 0.f;
		lv3En1DeadVel = 0.f;
		lv3En1WasColliding = false;
		lv3En1WasPunching = false;
		lv3En1X = (float)(bridges3[1].maxX - LV3EN1_W);
		lv3En2Alive = true;
		lv3En2Active = false;
		lv3En2State = LV3EN2_IDLE;
		lv3En2FrameIndex = 0;
		lv3En2AnimTick = 0;
		lv3En2NormalHits = 0;
		lv3En2ChargedHits = 0;
		lv3En2Dead = false;
		lv3En2Falling = false;
		lv3En2DeadY = 0.f;
		lv3En2DeadVel = 0.f;
		lv3En2WasColliding = false;
		lv3En2WasPunching = false;
		lv3En2X = (float)(bridges3[1].minX);
		lv3En3Alive = true;
		lv3En3Active = false;
		lv3En3State = LV3EN3_IDLE_STATE;
		lv3En3FrameIndex = 0;
		lv3En3AnimTick = 0;
		lv3En3NormalHits = 0;
		lv3En3ChargedHits = 0;
		lv3En3Dead = false;
		lv3En3Falling = false;
		lv3En3DeadY = 0.f;
		lv3En3DeadVel = 0.f;
		lv3En3WasColliding = false;
		lv3En3WasPunching = false;
		lv3En3X = (float)(bridges3[5].maxX - LV3EN3_IDLE_W);
		lv3En3FadeTick = 0;   lv3En3FadeDone = false;
		lv3En4FadeTick = 0;   lv3En4FadeDone = false;
		lv3En4Alive = true;
		lv3En4Active = false;
		lv3En4State = LV3EN4_IDLE;
		lv3En4FrameIndex = 0;
		lv3En4AnimTick = 0;
		lv3En4NormalHits = 0;
		lv3En4ChargedHits = 0;
		lv3En4Dead = false;
		lv3En4Falling = false;
		lv3En4DeadY = 0.f;
		lv3En4DeadVel = 0.f;
		lv3En4WasColliding = false;
		lv3En4WasPunching = false;
		lv3En4X = (float)(bridges3[5].minX);

		lv3WkAlive = true;
		lv3WkFrameIndex = 0;
		lv3WkKneeling = false;
		lv3WkKneelTimer = 0;
		lv3WkPlayerTouching = false;
		lv3WkX = (float)(bridges3[5].minX + LV3WK0_X_OFF);
		lv3WkY = (float)(bridges3[5].snapY + LV3WK_Y_OFF);

		lv3En5State = LV3EN5_WAITING;
		lv3En5FrameIndex = 0;
		lv3En5AnimTick = 0;
		lv3En5NormalHits = 0;
		lv3En5ChargedHits = 0;
		lv3En5ReviveCount = 0;
		lv3En5WasColliding = false;
		lv3En5WasPunching = false;
		lv3En5En4HitTriggered = false;
		lv3En5X = (float)(bridges3[5].maxX + LV3EN5_LAND_X_OFF);
		lv3En5Y = (float)(800);
		lv3En5HitFreeze = false;
		lv3En5HitFreezeTimer = 0;
		lv3En6State = LV3EN6_WAITING;
		lv3En6FrameIndex = 0;
		lv3En6AnimTick = 0;
		lv3En6NormalHits = 0;
		lv3En6ChargedHits = 0;
		lv3En6ReviveCount = 0;
		lv3En6WasColliding = false;
		lv3En6WasPunching = false;
		lv3En6En4HitTriggered = false;
		lv3En6X = (float)(bridges3[5].maxX + LV3EN6_LAND_X_OFF);
		lv3En6Y = (float)(800);
		lv3En6HitFreeze = false;
		lv3En6HitFreezeTimer = 0;

		// Reset trees
		for (int i = 0; i < NUM_TREES_LV3; i++)
			treesLv3[i].playerTouching = false;
		treeAnimIndex = 0;

		lv3BossX = (float)(bridges3[9].maxX - LV3BOSS_W - 10);
		// Reset Level 3 spikes
		for (int i = 0; i < NUM_SPIKES_LV3; i++) {
			spikesLv3[i].playerTouching = false;
			spikesLv3[i].kneeling = false;
			spikesLv3[i].kneelTimer = 0;
		}
		spikeAnimIndex = 0;
		// Reset power-up tokens
		for (int i = 0; i < NUM_PUPTK_LV3; i++)
			puptkLv3[i].collected = false;
		puptkAnimIndex = 0;
		for (int i = 0; i < NUM_PUPTK_LV3; i++) {
			puptkShowText[i] = false;
			puptkTextTimer[i] = 0;
		}
		lv3Crouching = false;
		playerLives = 12;
		barrelState = 0;
		energyFrameIndex = 0;
		energyAnimTick = 0;
		barrelWasPunching = false;
		barrel2State = 0;
		energy2FrameIndex = 0;
		energy2AnimTick = 0;
		barrel2WasPunching = false;
		return;
	}
	// --------------------- GAME OVER SCREEN CONTROLS ---------------------
	// right before: currentBg = 8;
	static int currentBgBeforeDeath = 4;  // declare globally near top

	if (gameOverTriggered && !gameOverScreen) {
		currentBgBeforeDeath = currentBg;  // remember where death came from
		gameOverDelayCounter++;
		if (gameOverDelayCounter >= 15) {
			gameOverScreen = true;
			currentBg = 8;
		}
	}

	if (currentBg == 8) // Game Over screen
	{
		if (key == 27) // ESC key - return to menu
		{
			currentBg = 2;
			gameStarted = false;
			gameOverScreen = false;
			gameOverTriggered = false;
			return;
		}

		if (key == 'y' || key == 'Y')
		{
			gameOverScreen = false;
			gameOverTriggered = false;
			deathWaiting = false;
			playerAlive = true;
			gameStarted = true;
			playerLives = MAX_LIVES;
			showKneel = false;
			kneelHolding = false;
			kneelHoldCounter = 0;
			chargingPunch = false;
			chargedSequenceActive = false;
			chargedUseCount = chargedUseCountLv3 = 0;
			chargedTimeCounter = 0;
			punchState = 0;
			punchJustLanded = false;

			// restart based on last gameplay background
			if (lastGameplayBg == 10) {     // Level 2 death
				currentBg = 10;
				// minimal Level‑2 reset
				lv2CameraX = 0;  lv2CharX = 100;  lv2CharY = bridges2[0].snapY;
				lv2IsJumping = false;  lv2JumpVelocity = 0;
				boss1Dead = false;  boss1Active = true;  boss1Moving = true;
				boss1X = (float)(bridges2[6].maxX - (PLAYER_WIDTH + 70) - 50);
				tokenCountLv2 = 0;  initCollectiblesLv2();  initFires();
			}
			else if (lastGameplayBg == 12) { // Level 3 death
				currentBg = 12;
				lv3CameraX = 0;  lv3CharX = 100;  lv3CharY = bridges3[0].snapY;
				lv3IsJumping = false;  lv3JumpVelocity = 0;
				// reset key Level 3 entities
				lv3BossDead = false;  lv3BossActive = false;  lv3BossHitCount = 0;
				lv3BossPlayerHitCount = 0;  lv3BossX = (float)(bridges3[10].maxX - LV3BOSS_W - 60);
				lv3Crouching = false;  lv3ShieldActive = false;
				chargedUseCountLv3 = 0;
				for (int i = 0; i < NUM_PUPTK_LV3; i++) puptkLv3[i].collected = false;
			}
			else {                       // default → Level 1
				currentBg = 4;
				cameraX = 0;  playerX = PLAYER_START_X_BG41;  playerY = PLAYER_START_Y_BG41;
				enemy1Alive = true;  enemy2Dead = false;  enemy3Alive = true;  enemy4Alive = true;
				dragonAlive = true;  tokenCount = 0; treasureCount = 0;
				initCollectibles();  shuffleEnemySprites();
			}
		}

	}

	if (!playerAlive)
		return;

	// --------------------- MENU SHORTCUTS ---------------------
	if (currentBg == 1 && key == ' ') {
		currentBg = 2;
		return; // prevent falling into BG3/4 movement logic
	}
	// --------------------- STORY SEQUENCE ADVANCE ---------------------
	if (currentBg == 3 && key == ' ') {
		if (storyIndex < 6) {
			storyIndex++;
		}
		else {

			// Last slide (instruction) done — actually start the game
			inStorySequence = false;
			storyIndex = 0;
			currentBg = 4;
			gameStarted = true;
			shuffleEnemySprites();
			restartMusic(); // ← restart from beginning when game starts

		}
		return;
	}
	if (key == 27) // ESC key
	{
		currentBg = 2;
		gameStarted = false;
		return;
	}

	// Do NOT have a generic space key change after this

	if (currentBg == 4 || currentBg == 10 || currentBg == 12) {
		if (key == 'h' || key == 'H') {
			if (currentBg == 12)
				lv3ShieldActive = true;
		}

		// ── Level 3: Crouch (S held) ─────────────────────────────────
		if (key == 's' || key == 'S') {
			if (currentBg == 12) {
				lv3Crouching = true;
				return;   // don't fall into movement logic below
			}
		}
		// --------------------- NORMAL AND CHARGED PUNCH ---------------------
		if (key == 'f' || key == 'F') {
			if (chargingPunch && chargeScrollDismissed) {
				if (!chargedSequenceActive) {
					// First F press: go to FIST (state 0), start sequence
					chargedSequenceActive = true;
					chargedPunchState = 0; // 0 = fist
					chargedHitWindow = 6;
					playPunchSound();
				}
				else {
					// Toggle between fist (0) and punch (1)
					chargedPunchState ^= 1;
					if (chargedPunchState == 1) {
						chargedHitWindow = 6;
						playPunchSound();
					}
				}
			}
			else {
				if (punchState == 0) {
					punchState = 1;
					punchJustLanded = false;
				}
				else if (punchState == 1) {
					punchState = 2;         // SECOND PUNCH
					punchJustLanded = true; // REGISTER HIT ATTEMPT
					playPunchSound();
				}
				else if (punchState == 2) {
					punchState = 1;
					punchJustLanded = false;
				}
			}
		}

		int &activeChargedCount = (currentBg == 12) ? chargedUseCountLv3 : chargedUseCount;

		if ((key == 'p' || key == 'P') && activeChargedCount < MAX_CHARGED_USES + NUM_PUPTK_LV3) {
			if (!chargeScrollShown) {
				showChargeScroll = true;
				chargeScrollShown = true;
				chargeScrollDismissed = false;
				chargingPunch = false;
				chargedSequenceActive = false;
			}
			else {
				showChargeScroll = false;
				chargeScrollDismissed = true;
				chargingPunch = true;
				chargedSequenceActive = false;
				chargedPunchState = 0;

				activeChargedCount++;
				chargedTimeCounter = 0;
				chargedTimerRunning = true;
			}
		}
		// --------------------- DISMISS CHARGE SCROLL ---------------------
		if ((key == 'e' || key == 'E') && showChargeScroll) {
			showChargeScroll = false;
			chargeScrollDismissed = true;
			chargingPunch = true;
			chargedUseCount++;
			chargedTimeCounter = 0;
			if (currentBg == 12) chargedUseCountLv3++;
			chargedTimerRunning = true;
		}

		// --------------------- PLAYER MOVEMENT ---------------------
		if (showKneel)
			return;
		if (key == 'a') {
			if (currentBg == 4 && !isBelowBridge()) {
				playerX -= 10;
				movingFront = true;
				punchState = 0;
				animFrame ^= 1;
			}
			else if (currentBg == 10) {
				lv2CharX -= 10;
				movingFront = true;
				punchState = 0;
				animFrame ^= 1;
			}
			else if (currentBg == 12) {

				lv3CharX -= 10;
				movingFront = true;
				punchState = 0;
				animFrame ^= 1;
			}
		}
		if (showKneel)
			return;
		if (key == 'd') {
			if (currentBg == 4 && !isBelowBridge()) {
				playerX += 10;
				movingFront = false;
				punchState = 0;
				animFrame ^= 1;
			}
			else if (currentBg == 10) {
				// Block rightward movement on bridge 7 until boss1 is dead
				bool onBridge7 = (lv2CharX + PLAYER_WIDTH / 2 >= bridges2[6].minX &&
					lv2CharX + PLAYER_WIDTH / 2 <= bridges2[6].maxX);

				// Only block if boss is alive AND player would collide with boss's left
				// side
				bool bossBlocking = onBridge7 && !boss1Dead &&
					(lv2CharX + PLAYER_WIDTH >= (int)boss1X - 30);

				if (!bossBlocking) {
					lv2CharX += 10;
					movingFront = false;
					punchState = 0;
					animFrame ^= 1;
				}
			}
			else if (currentBg == 12) {

				lv3CharX += 10;
				movingFront = false;
				punchState = 0;
				animFrame ^= 1;
			}
		}
		if (showKneel)
			return;
		if (key == 'w' || key == 'W') {
			if (currentBg == 4 && !isJumping) {
				isJumping = true;
				jumpVelocity = JUMP_POWER;
			}
			else if (currentBg == 10 && !lv2IsJumping) {
				lv2IsJumping = true;
				lv2JumpVelocity = 19.0f;
			}
			else if (currentBg == 12 && !lv3IsJumping) {
				lv3IsJumping = true;
				lv3JumpVelocity = 19.0f;
			}
		}

		// Clamp positions
		if (currentBg == 4) {
			if (playerX < 0)
				playerX = 0;
			if (playerX + PLAYER_WIDTH > BG41_WIDTH)
				playerX = BG41_WIDTH - PLAYER_WIDTH;
		}
		else if (currentBg == 10) {
			if (lv2CharX < 0)
				lv2CharX = 0;
			if (lv2CharX + PLAYER_WIDTH > LV2_BG_WIDTH)
				lv2CharX = LV2_BG_WIDTH - PLAYER_WIDTH;
		}
		else if (currentBg == 12) {
			if (lv3CharX < 0)
				lv3CharX = 0;
			if (lv3CharX + PLAYER_WIDTH > LV3_BG_WIDTH)
				lv3CharX = LV3_BG_WIDTH - PLAYER_WIDTH;
		}
	}
}
void keyboardUp(unsigned char key, int x, int y) {
	if (key == 'h' || key == 'H') {
		lv3ShieldActive = false;
		shieldLoadFrameIndex = 0;
		shieldAuraFrameIndex = 0;
		shieldAuraFrameIndexM = 0;
	}
	// ── Level 3: release crouch when S is lifted ─────────────────
	if (key == 's' || key == 'S') {
		if (currentBg == 12)
			lv3Crouching = false;
	}
}
/* -------------------- MOUSE -------------------- */
void iMouse(int button, int state, int mx, int my) {
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		// Check if btn1 is clicked
		// btn1 (index 0) → go to level select
		int y = btnStartY - 0 * (btnH + btnGap);
		if (mx >= btnX && mx <= btnX + btnW && my >= y && my <= y + btnH) {
			lvlHoverIndex = 0;
			currentBg = 13;
		}

		// Level select clicks
		if (currentBg == 13 && lvlHoverIndex > 0) {

			// LEVEL 1 → story sequence
			if (lvlHoverIndex == 1) {
				inStorySequence = true;
				storyIndex = 0;
				currentBg = 3;
				return;
			}

			// LEVEL 2 → instruction2.png
			if (lvlHoverIndex == 2) {
				currentBg = 9;      // instruction2 screen
				return;
			}

			// LEVEL 3 → instruction3.png
			if (lvlHoverIndex == 3) {
				currentBg = 11;     // instruction3 screen
				return;
			}
		}


		// -------- CHARGE SCROLL DISMISS --------
		if (showChargeScroll) {
			int scrollX = (SCREEN_WIDTH - 400) / 2;
			int scrollY = (SCREEN_HEIGHT - 400) / 2;
			int scrollW = 400;
			int scrollH = 400;

			if (mx >= scrollX && mx <= scrollX + scrollW && my >= scrollY &&
				my <= scrollY + scrollH) {
				showChargeScroll = false;
				return;
			}
		}

		// -------- MENU BUTTON CLICKS (BG2) --------
		if (currentBg == 2) {
			for (int i = 0; i < NUM_BUTTONS; i++) {
				int y = btnStartY - i * (btnH + btnGap);
				if (mx >= btnX && mx <= btnX + btnW && my >= y && my <= y + btnH) {
					if (i == 0)
						currentBg = 5;
					else if (i == 1)
						currentBg = 6;
					else if (i == 2)
						currentBg = 7;
					else
						exit(0);
					return;
				}
			}
		}

		// -------- BACK BUTTON CLICK --------
		if ((currentBg == 5 || currentBg == 6 || currentBg == 7) &&
			isInsideCircle(mx, my)) {
			currentBg = 2;
			return;
		}
	}
}

void iMouseMove(int mx, int my) {}
void iSpecialKeyboard(unsigned char key) {}
void iPassiveMouseMove(int mx, int my) {
	// Level select hover
	if (currentBg == 13) {
		lvlHoverIndex = 0;
		for (int i = 0; i < 3; i++) {
			if (mx >= lvlHB[i].x && mx <= lvlHB[i].x + lvlHB[i].w &&
				my >= lvlHB[i].y && my <= lvlHB[i].y + lvlHB[i].h) {
				lvlHoverIndex = i + 1;
				break;
			}
		}
	}
	if (currentBg == 2) {
		for (int i = 0; i < NUM_BUTTONS; i++) {
			int y = btnStartY - i * (btnH + btnGap);
			btnIsHover[i] =
				(mx >= btnX && mx <= btnX + btnW && my >= y && my <= y + btnH);
		}
	}

	if (currentBg == 5 || currentBg == 6 || currentBg == 7)
		backHover = isInsideCircle(mx, my);
}

/* -------------------- INIT -------------------- */
void loadButtons() {
	btnNormal[0] = iLoadImage("Images/btn1.png");
	btnNormal[1] = iLoadImage("Images/btn2.png");
	btnNormal[2] = iLoadImage("Images/btn3.png");
	btnNormal[3] = iLoadImage("Images/btn4.png");

	btnHover[0] = iLoadImage("Images/btn1_hover.png");
	btnHover[1] = iLoadImage("Images/btn2_hover.png");
	btnHover[2] = iLoadImage("Images/btn3_hover.png");
	btnHover[3] = iLoadImage("Images/btn4_hover.png");

	backBtn = iLoadImage("Images/back.png");
	backBtnHover = iLoadImage("Images/back_hover.png");
}

// ---- VIRTUAL TIMER SYSTEM — must be declared FIRST ----
struct VTimer {
	int interval;
	int counter;
	void(*callback)();
};

VTimer vTimers[50];
int vTimerCount = 0;

void addVTimer(int intervalMs, void(*fn)()) {
	vTimers[vTimerCount++] = { intervalMs, 0, fn };
}

void masterTimerTick() {
	for (int i = 0; i < vTimerCount; i++) {
		vTimers[i].counter += 16;
		if (vTimers[i].counter >= vTimers[i].interval) {
			vTimers[i].counter = 0;
			vTimers[i].callback();
		}
	}
}
// ---- END VIRTUAL TIMER SYSTEM ----

void initFires() {
	fires[0] = { (float)(bridges2[1].maxX + FIRE0_X_OFFSET),
		(float)(FIRE0_Y_OFFSET), false, false, 0 };
	fires[1] = { (float)(bridges2[2].maxX + FIRE1_X_OFFSET),
		(float)(FIRE1_Y_OFFSET), false, false, 0 };
	fires[2] = { (float)(bridges2[5].maxX + FIRE2_X_OFFSET),
		(float)(FIRE2_Y_OFFSET), false, false, 0 };
	fires[3] = { (float)(bridges2[3].maxX + FIRE3_X_OFFSET),
		(float)(FIRE3_Y_OFFSET), false, false, 0 };
	fires[4] = {
		(float)((bridges2[6].maxX + bridges2[7].minX) / 2 + FIRE4_X_OFFSET),
		(float)(FIRE4_Y_OFFSET), false, false, 0 };
}

// ─────────────────────────────────────────────────────────────────────────────
//  updateLv3PlayerHitbox()
//  Call once per frame (add to masterTimerTick via addVTimer).
//  Recomputes lv3PHB in WORLD coordinates based on current pose.
//  All other collision checks in Level 3 should use lv3PHB instead of
//  raw fraction math — so you only need to change numbers in constant.h.
// ─────────────────────────────────────────────────────────────────────────────
void updateLv3PlayerHitbox() {
	if (currentBg != 12) return;

	// Determine which pose is active
	if (lv3Crouching) {
		// ── Crouch pose — sprite is CROUCH_W × CROUCH_H, offset by CROUCH_Y_OFF ──
		int sprX = lv3CharX - (CROUCH_W - PLAYER_WIDTH) / 2;
		int sprY = lv3CharY + CROUCH_Y_OFF;

		lv3PHB.x1 = sprX + (int)(CROUCH_W * LV3_HB_CROUCH_LEFT);
		lv3PHB.x2 = sprX + (int)(CROUCH_W * LV3_HB_CROUCH_RIGHT);
		lv3PHB.y1 = sprY + (int)(CROUCH_H * LV3_HB_CROUCH_TOP);
		lv3PHB.y2 = sprY + (int)(CROUCH_H * LV3_HB_CROUCH_BOTTOM);
	}
	else if (punchState == 2) {
		// ── Active punch hit / kick — fist.png IS punchf pose with extended reach ──
		lv3PHB.x1 = lv3CharX + (int)(PLAYER_WIDTH  * LV3_HB_PUNCH_LEFT);
		lv3PHB.x2 = lv3CharX + (int)(PLAYER_WIDTH  * LV3_HB_PUNCH_RIGHT);
		lv3PHB.y1 = lv3CharY + (int)(PLAYER_HEIGHT * LV3_HB_PUNCH_TOP);
		lv3PHB.y2 = lv3CharY + (int)(PLAYER_HEIGHT * LV3_HB_PUNCH_BOTTOM);
	}
	else if (punchState == 1 ||
		(chargingPunch && chargeScrollDismissed && !chargedSequenceActive) ||
		(chargedSequenceActive && chargedPunchState == 0)) {
		if (movingFront) {
			// facing left — fistm.png
			lv3PHB.x1 = lv3CharX + (int)(PLAYER_WIDTH  * LV3_HB_FIST_L_LEFT);
			lv3PHB.x2 = lv3CharX + (int)(PLAYER_WIDTH  * LV3_HB_FIST_L_RIGHT);
			lv3PHB.y1 = lv3CharY + (int)(PLAYER_HEIGHT * LV3_HB_FIST_L_TOP);
			lv3PHB.y2 = lv3CharY + (int)(PLAYER_HEIGHT * LV3_HB_FIST_L_BOTTOM);
		}
		else {
			// facing right — fist.png
			lv3PHB.x1 = lv3CharX + (int)(PLAYER_WIDTH  * LV3_HB_FIST_R_LEFT);
			lv3PHB.x2 = lv3CharX + (int)(PLAYER_WIDTH  * LV3_HB_FIST_R_RIGHT);
			lv3PHB.y1 = lv3CharY + (int)(PLAYER_HEIGHT * LV3_HB_FIST_R_TOP);
			lv3PHB.y2 = lv3CharY + (int)(PLAYER_HEIGHT * LV3_HB_FIST_R_BOTTOM);
		}
	}
	else if (chargedSequenceActive && chargedPunchState == 1) {
		// ── Charged punch release: chargedpunch.png / chargedpunchm.png ──
		lv3PHB.x1 = lv3CharX + (int)(PLAYER_WIDTH  * LV3_HB_CHPUNCH_LEFT);
		lv3PHB.x2 = lv3CharX + (int)(PLAYER_WIDTH  * LV3_HB_CHPUNCH_RIGHT);
		lv3PHB.y1 = lv3CharY + (int)(PLAYER_HEIGHT * LV3_HB_CHPUNCH_TOP);
		lv3PHB.y2 = lv3CharY + (int)(PLAYER_HEIGHT * LV3_HB_CHPUNCH_BOTTOM);
	}
	else {
		// ── Idle / walk pose (default) ────────────────────────────────────────
		lv3PHB.x1 = lv3CharX + (int)(PLAYER_WIDTH  * LV3_HB_IDLE_LEFT);
		lv3PHB.x2 = lv3CharX + (int)(PLAYER_WIDTH  * LV3_HB_IDLE_RIGHT);
		lv3PHB.y1 = lv3CharY + (int)(PLAYER_HEIGHT * LV3_HB_IDLE_TOP);
		lv3PHB.y2 = lv3CharY + (int)(PLAYER_HEIGHT * LV3_HB_IDLE_BOTTOM);
	}
}

void updateLv3Camera() {
	if (currentBg != 12) return;

	lv3CameraX = lv3CharX - SCREEN_WIDTH / 2;
	if (lv3CameraX < 0)
		lv3CameraX = 0;
	if (lv3CameraX > LV3_BG_WIDTH - SCREEN_WIDTH)
		lv3CameraX = LV3_BG_WIDTH - SCREEN_WIDTH;
}
void updateLv3PowerUpTokens() {
	if (currentBg != 12) return;

	// Animate
	static int tkTick = 0;
	if (++tkTick >= 4) {   // tweak 4 to speed up/slow down
		tkTick = 0;
		puptkAnimIndex = (puptkAnimIndex + 1) % PUPTK_FRAMECOUNT;
	}
	// Count down pickup text timers
	for (int i = 0; i < NUM_PUPTK_LV3; i++) {
		if (puptkShowText[i]) {
			puptkTextTimer[i]++;
			if (puptkTextTimer[i] >= PUPTK_TEXT_DURATION) {
				puptkShowText[i] = false;
				puptkTextTimer[i] = 0;
			}
		}
	}
	if (!playerAlive) return;

	for (int i = 0; i < NUM_PUPTK_LV3; i++) {
		if (puptkLv3[i].collected) continue;

		// Hitbox (world coords)
		int tX1 = (int)puptkLv3[i].x;
		int tX2 = tX1 + PUPTK_W;
		int tY1 = (int)puptkLv3[i].y;
		int tY2 = tY1 + PUPTK_H;

		// Player hitbox
		int pX1 = lv3PHB.x1;
		int pX2 = lv3PHB.x2;
		int pY1 = lv3PHB.y1;
		int pY2 = lv3PHB.y2;

		bool overlapX = !(pX2 < tX1 || pX1 > tX2);
		bool overlapY = !(pY2 < tY1 || pY1 > tY2);

		if (overlapX && overlapY) {
			puptkLv3[i].collected = true;
			// Grant 1 extra charged punch — decrement even below 0
			// (negative count = bonus uses above MAX)
			chargedUseCountLv3--;
			puptkShowText[i] = true;
			puptkTextTimer[i] = 0;
		}
	}
}

void updateLv3Bird() {
	if (currentBg != 12) return;

	// ── Advance animation ──────────────────────────────────────────
	static int birdAnimTick = 0;
	if (++birdAnimTick >= 1) {          // tweak 3 to change anim speed
		birdAnimTick = 0;
		lv3BirdFrameIndex = (lv3BirdFrameIndex + 1) % LV3BIRD_FRAMECOUNT;
	}

	// ── Dead-fall physics ──────────────────────────────────────────
	if (!lv3BirdAlive) {
		if (lv3BirdFalling) {
			lv3BirdDeadVel -= 1.2f;
			lv3BirdDeadY += lv3BirdDeadVel;
			if (lv3BirdDeadY <= 0.f) {
				lv3BirdDeadY = 0.f;
				lv3BirdFalling = false;
			}
		}
		return;
	}

	// ── Activate when player steps onto Bridge 2 (bridges3[1]) ────
	int cx = lv3CharX + PLAYER_WIDTH / 2;
	if (!lv3BirdActive) {
		if (cx >= bridges3[1].minX && cx <= bridges3[1].maxX)
			lv3BirdActive = true;
		else
			return;
	}

	if (!playerAlive) return;

	// ── Fly left across Bridge 2 ───────────────────────────────────
	lv3BirdX -= LV3BIRD_SPEED;

	// Clamp at left edge of bridge so it doesn't fly off forever
	float leftBound = (float)(bridges3[1].minX - LV3BIRD_W);
	if (lv3BirdX < leftBound)
		lv3BirdX = (float)((bridges3[1].minX + bridges3[1].maxX) / 2);

	// ── Enemy hitbox (world coords) ────────────────────────────────
	int birdWorldY = bridges3[1].snapY + LV3BIRD_Y_OFF;
	int eX1 = (int)lv3BirdX + (int)(LV3BIRD_W * LV3BIRD_HB_LEFT);
	int eX2 = (int)lv3BirdX + (int)(LV3BIRD_W * LV3BIRD_HB_RIGHT);
	int eY1 = birdWorldY + (int)(LV3BIRD_H * LV3BIRD_HB_TOP);
	int eY2 = birdWorldY + (int)(LV3BIRD_H * LV3BIRD_HB_BOTTOM);

	// ── Player hitbox (world coords) ───────────────────────────────
	int pX1 = lv3PHB.x1;
	int pX2 = lv3PHB.x2;
	int pY1 = lv3PHB.y1;
	int pY2 = lv3PHB.y2;

	bool overlapX = !(pX2 < eX1 || pX1 > eX2);
	bool overlapY = !(pY2 < eY1 || pY1 > eY2);
	bool isColliding = overlapX && overlapY;

	// ── Body contact → lose 1 life (rising-edge only) ──────────────
	if (isColliding && !lv3BirdWasColliding) {
		// ← Don't hurt player if they're actively punching
		bool playerPunching = (punchState == 2) || chargedSequenceActive;
		if (!playerPunching) {
			playerLives--;
			if (playerLives < 0) playerLives = 0;

			if (playerLives > 0) {
				showKneel = true;
				kneelHolding = true;
				kneelHoldCounter = 0;
			}
			else {
				showKneel = false;
				kneelHolding = false;
				playerAlive = false;
				gameStarted = false;
				playerDeathY = bridges3[1].snapY;
				handlePlayerDeath();
			}
		}
	}
	lv3BirdWasColliding = isColliding;

	// ── Normal punch (3 hits to kill) ──────────────────────────────
	if (punchJustLanded && playerAlive) {
		int punchReach = 85;   // ← increased from 70
		int ppX1, ppX2;
		if (movingFront) {
			ppX1 = lv3CharX + PLAYER_WIDTH - 20;
			ppX2 = lv3CharX + PLAYER_WIDTH + punchReach;
		}
		else {
			ppX1 = lv3CharX - punchReach;
			ppX2 = lv3CharX + 70;
		}
		// ← wider vertical range to reach the bird above
		int ppY1 = lv3CharY + 10;
		int ppY2 = lv3CharY + PLAYER_HEIGHT - 10;

		bool pOX = !(ppX2 < eX1 || ppX1 > eX2);
		bool pOY = !(ppY2 < eY1 || ppY1 > eY2);

		if (pOX && pOY) {
			punchJustLanded = false;
			lv3BirdHitCount++;
			if (lv3BirdHitCount >= LV3BIRD_HITS_TO_KILL) {
				lv3BirdAlive = false;
				lv3BirdFalling = true;
				lv3BirdDeadVel = 0.f;
				lv3BirdDeadY = (float)birdWorldY;
				lv3BirdWasColliding = false;
				showKneel = false;
				kneelHolding = false;
				kneelHoldCounter = 0;
			}
		}
	}
	// ── Charged punch (2 hits to kill) ─────────────────────────────
	if (chargedSequenceActive && playerAlive) {
		int ppX1 = lv3CharX - 120;
		int ppX2 = lv3CharX + PLAYER_WIDTH + 120;
		int ppY1 = lv3CharY + 20;
		int ppY2 = lv3CharY + PLAYER_HEIGHT;

		bool pOX = !(ppX2 < eX1 || ppX1 > eX2);
		bool pOY = !(ppY2 < eY1 || ppY1 > eY2);

		if (pOX && pOY) {
			lv3BirdHitCount++;
			if (lv3BirdHitCount >= LV3BIRD_HITS_TO_KILL) {
				lv3BirdAlive = false;
				lv3BirdFalling = true;
				lv3BirdDeadVel = 0.f;
				lv3BirdDeadY = (float)birdWorldY;
				lv3BirdWasColliding = false;
				showKneel = false;
				kneelHolding = false;
				kneelHoldCounter = 0;
			}
		}
	}
}

void updateLv3Bird2() {
	if (currentBg != 12) return;

	// ── Advance animation (shares frames with bird1) ───────────────────
	static int bird2AnimTick = 0;
	if (++bird2AnimTick >= 1) {
		bird2AnimTick = 0;
		lv3Bird2FrameIndex = (lv3Bird2FrameIndex + 1) % LV3BIRD_FRAMECOUNT;
	}

	// ── Dead-fall physics ──────────────────────────────────────────────
	if (!lv3Bird2Alive) {
		if (lv3Bird2Falling) {
			lv3Bird2DeadVel -= 1.2f;
			lv3Bird2DeadY += lv3Bird2DeadVel;
			if (lv3Bird2DeadY <= 0.f) {
				lv3Bird2DeadY = 0.f;
				lv3Bird2Falling = false;
			}
		}
		return;
	}

	// ── Wait for bird1 to activate first, then count spawn delay ──────
	if (!lv3Bird2Active) {
		if (!lv3BirdActive) return;   // bird1 hasn't activated yet — do nothing

		// Bird1 is active — count down spawn delay
		lv3Bird2SpawnTimer++;
		if (lv3Bird2SpawnTimer < LV3BIRD2_SPAWN_DELAY)
			return;   // still waiting

		// Delay elapsed → activate bird2 from same start position as bird1
		lv3Bird2Active = true;
		lv3Bird2X = (float)((bridges3[1].minX + bridges3[1].maxX) / 2);
	}

	if (!playerAlive) return;

	// ── Fly left across Bridge 2 ───────────────────────────────────────
	lv3Bird2X -= LV3BIRD2_SPEED;

	float leftBound = (float)(bridges3[1].minX - LV3BIRD2_W);
	if (lv3Bird2X < leftBound)
		lv3Bird2X = (float)((bridges3[1].minX + bridges3[1].maxX) / 2);

	// ── Enemy hitbox (world coords) ────────────────────────────────────
	int birdWorldY = bridges3[1].snapY + LV3BIRD2_Y_OFF;
	int eX1 = (int)lv3Bird2X + (int)(LV3BIRD2_W * LV3BIRD2_HB_LEFT);
	int eX2 = (int)lv3Bird2X + (int)(LV3BIRD2_W * LV3BIRD2_HB_RIGHT);
	int eY1 = birdWorldY + (int)(LV3BIRD2_H * LV3BIRD2_HB_TOP);
	int eY2 = birdWorldY + (int)(LV3BIRD2_H * LV3BIRD2_HB_BOTTOM);

	// ── Player hitbox (world coords) ───────────────────────────────────
	int pX1 = lv3PHB.x1;
	int pX2 = lv3PHB.x2;
	int pY1 = lv3PHB.y1;
	int pY2 = lv3PHB.y2;

	bool overlapX = !(pX2 < eX1 || pX1 > eX2);
	bool overlapY = !(pY2 < eY1 || pY1 > eY2);
	bool isColliding = overlapX && overlapY;

	// ── Body contact → lose 1 life (rising-edge only) ──────────────────
	if (isColliding && !lv3Bird2WasColliding) {
		bool playerPunching = (punchState == 2) || chargedSequenceActive;
		if (!playerPunching) {
			playerLives--;
			if (playerLives < 0) playerLives = 0;

			if (playerLives > 0) {
				showKneel = true;
				kneelHolding = true;
				kneelHoldCounter = 0;
			}
			else {
				showKneel = false;
				kneelHolding = false;
				playerAlive = false;
				gameStarted = false;
				playerDeathY = bridges3[1].snapY;
				handlePlayerDeath();
			}
		}
	}
	lv3Bird2WasColliding = isColliding;

	// ── Normal punch ───────────────────────────────────────────────────
	if (punchJustLanded && playerAlive) {
		int punchReach = 85;
		int ppX1, ppX2;
		if (movingFront) {
			ppX1 = lv3CharX + PLAYER_WIDTH - 20;
			ppX2 = lv3CharX + PLAYER_WIDTH + punchReach;
		}
		else {
			ppX1 = lv3CharX - punchReach;
			ppX2 = lv3CharX + 70;
		}
		int ppY1 = lv3CharY + 10;
		int ppY2 = lv3CharY + PLAYER_HEIGHT - 10;

		bool pOX = !(ppX2 < eX1 || ppX1 > eX2);
		bool pOY = !(ppY2 < eY1 || ppY1 > eY2);

		if (pOX && pOY) {
			punchJustLanded = false;
			lv3Bird2HitCount++;
			if (lv3Bird2HitCount >= LV3BIRD_HITS_TO_KILL) {
				lv3Bird2Alive = false;
				lv3Bird2Falling = true;
				lv3Bird2DeadVel = 0.f;
				lv3Bird2DeadY = (float)birdWorldY;
				lv3Bird2WasColliding = false;
				showKneel = false;
				kneelHolding = false;
				kneelHoldCounter = 0;
			}
		}
	}

	// ── Charged punch ──────────────────────────────────────────────────
	if (chargedSequenceActive && playerAlive) {
		int ppX1 = lv3CharX - 120;
		int ppX2 = lv3CharX + PLAYER_WIDTH + 120;
		int ppY1 = lv3CharY + 20;
		int ppY2 = lv3CharY + PLAYER_HEIGHT;

		bool pOX = !(ppX2 < eX1 || ppX1 > eX2);
		bool pOY = !(ppY2 < eY1 || ppY1 > eY2);

		if (pOX && pOY) {
			lv3Bird2HitCount++;
			if (lv3Bird2HitCount >= LV3BIRD_HITS_TO_KILL) {
				lv3Bird2Alive = false;
				lv3Bird2Falling = true;
				lv3Bird2DeadVel = 0.f;
				lv3Bird2DeadY = (float)birdWorldY;
				lv3Bird2WasColliding = false;
				showKneel = false;
				kneelHolding = false;
				kneelHoldCounter = 0;
			}
		}
	}
}

void updateLv3PBird() {
	if (currentBg != 12) return;

	// ── Advance animation ──────────────────────────────────────────
	static int pBirdAnimTick = 0;
	if (++pBirdAnimTick >= 3) {
		pBirdAnimTick = 0;
		lv3PBirdFrameIndex = (lv3PBirdFrameIndex + 1) % LV3PBIRD_FRAMECOUNT;
	}

	// ── Bounce left↔right across Bridge 3 ─────────────────────────
#define LV3PBIRD_LEFT_OVERHANG   80
#define LV3PBIRD_RIGHT_OVERHANG  80

	float leftBound = (float)(bridges3[2].minX - LV3PBIRD_LEFT_OVERHANG);
	float rightBound = (float)(bridges3[2].maxX - LV3PBIRD_W + LV3PBIRD_RIGHT_OVERHANG);

	if (lv3PBirdMovingRight) {
		lv3PBirdX += LV3PBIRD_SPEED;
		if (lv3PBirdX >= rightBound) {
			lv3PBirdX = rightBound;
			lv3PBirdMovingRight = false;
		}
	}
	else {
		lv3PBirdX -= LV3PBIRD_SPEED;
		if (lv3PBirdX <= leftBound) {
			lv3PBirdX = leftBound;
			lv3PBirdMovingRight = true;
		}
	}

	if (!playerAlive) return;

	// ── Enemy hitbox (world coords) ────────────────────────────────
	int pBirdWorldY = bridges3[2].snapY + LV3PBIRD_Y_OFF;
	int eX1 = (int)lv3PBirdX + (int)(LV3PBIRD_W * LV3PBIRD_HB_LEFT);
	int eX2 = (int)lv3PBirdX + (int)(LV3PBIRD_W * LV3PBIRD_HB_RIGHT);
	int eY1 = pBirdWorldY + (int)(LV3PBIRD_H * LV3PBIRD_HB_TOP);
	int eY2 = pBirdWorldY + (int)(LV3PBIRD_H * LV3PBIRD_HB_BOTTOM);

	// ── Player hitbox (world coords) — covers ALL poses via lv3PHB ─
	int pX1 = lv3PHB.x1;
	int pX2 = lv3PHB.x2;
	int pY1 = lv3PHB.y1;
	int pY2 = lv3PHB.y2;

	bool overlapX = !(pX2 < eX1 || pX1 > eX2);
	bool overlapY = !(pY2 < eY1 || pY1 > eY2);
	bool isColliding = overlapX && overlapY;

	// ── Body contact → lose 1 life (rising-edge, no pose immunity) ─
	if (isColliding && !lv3PBirdWasColliding) {
		playerLives--;
		if (playerLives < 0) playerLives = 0;

		if (playerLives > 0) {
			showKneel = true;
			kneelHolding = true;
			kneelHoldCounter = 0;
		}
		else {
			showKneel = false;
			kneelHolding = false;
			playerAlive = false;
			gameStarted = false;
			playerDeathY = bridges3[2].snapY;
			handlePlayerDeath();
		}
	}
	lv3PBirdWasColliding = isColliding;
}

void updateLv3PBird2() {
	if (currentBg != 12) return;

	// ── Advance animation ──────────────────────────────────────────
	static int pBird2AnimTick = 0;
	if (++pBird2AnimTick >= 3) {
		pBird2AnimTick = 0;
		lv3PBird2FrameIndex = (lv3PBird2FrameIndex + 1) % LV3PBIRD2_FRAMECOUNT;
	}

	// ── Bounce left↔right across Bridge 7 ─────────────────────────
	float leftBound = (float)(bridges3[6].minX - LV3PBIRD2_LEFT_OVERHANG);
	float rightBound = (float)(bridges3[6].maxX - LV3PBIRD2_W + LV3PBIRD2_RIGHT_OVERHANG);

	if (lv3PBird2MovingRight) {
		lv3PBird2X += LV3PBIRD2_SPEED;
		if (lv3PBird2X >= rightBound) {
			lv3PBird2X = rightBound;
			lv3PBird2MovingRight = false;
		}
	}
	else {
		lv3PBird2X -= LV3PBIRD2_SPEED;
		if (lv3PBird2X <= leftBound) {
			lv3PBird2X = leftBound;
			lv3PBird2MovingRight = true;
		}
	}

	if (!playerAlive) return;

	// ── Enemy hitbox (world coords) ────────────────────────────────
	int pBird2WorldY = bridges3[6].snapY + LV3PBIRD2_Y_OFF;
	int eX1 = (int)lv3PBird2X + (int)(LV3PBIRD2_W * LV3PBIRD2_HB_LEFT);
	int eX2 = (int)lv3PBird2X + (int)(LV3PBIRD2_W * LV3PBIRD2_HB_RIGHT);
	int eY1 = pBird2WorldY + (int)(LV3PBIRD2_H * LV3PBIRD2_HB_TOP);
	int eY2 = pBird2WorldY + (int)(LV3PBIRD2_H * LV3PBIRD2_HB_BOTTOM);

	// ── Player hitbox (world coords) — covers ALL poses via lv3PHB ─
	int pX1 = lv3PHB.x1;
	int pX2 = lv3PHB.x2;
	int pY1 = lv3PHB.y1;
	int pY2 = lv3PHB.y2;

	bool overlapX = !(pX2 < eX1 || pX1 > eX2);
	bool overlapY = !(pY2 < eY1 || pY1 > eY2);
	bool isColliding = overlapX && overlapY;

	// ── Body contact → lose 1 life (rising-edge, no pose immunity) ─
	if (isColliding && !lv3PBird2WasColliding) {
		playerLives--;
		if (playerLives < 0) playerLives = 0;

		if (playerLives > 0) {
			showKneel = true;
			kneelHolding = true;
			kneelHoldCounter = 0;
		}
		else {
			showKneel = false;
			kneelHolding = false;
			playerAlive = false;
			gameStarted = false;
			playerDeathY = bridges3[6].snapY;
			handlePlayerDeath();
		}
	}
	lv3PBird2WasColliding = isColliding;
}

void updateLv3En1() {
	if (currentBg != 12) return;

	// ── Dead-fall physics ────────────────────────────────────────
	if (!lv3En1Alive) {
		if (lv3En1Falling) {
			lv3En1DeadVel -= 1.2f;
			lv3En1DeadY += lv3En1DeadVel;
			if (lv3En1DeadY <= 0.f) {
				lv3En1DeadY = 0.f;
				lv3En1Falling = false;
			}
		}
		return;
	}

	// ── Activation: player reaches midpoint of Bridge 2 ─────────
	if (!lv3En1Active) {
		int bridge2Mid = (bridges3[1].minX + bridges3[1].maxX) / 2;
		int cx = lv3CharX + PLAYER_WIDTH / 2;
		if (cx >= bridge2Mid + LV3EN1_ACTIVATE_OFFSET)
			lv3En1Active = true;
		else {
			lv3En1AnimTick++;
			if (lv3En1AnimTick >= LV3EN1_IDLE_SPEED) {
				lv3En1AnimTick = 0;
				lv3En1FrameIndex = (lv3En1FrameIndex + 1) % LV3EN1_IDLE_FRAMES;
			}
			return;
		}
	}

	if (!playerAlive) return;

	// ── Shared: enemy body hitbox (world coords) ─────────────────
	int enY = bridges3[1].snapY + LV3EN1_Y_OFF;
	int eX1 = (int)lv3En1X + (int)(LV3EN1_W * LV3EN1_HB_LEFT);
	int eX2 = (int)lv3En1X + (int)(LV3EN1_W * LV3EN1_HB_RIGHT);
	int eY1 = enY + (int)(LV3EN1_H * LV3EN1_HB_TOP);
	int eY2 = enY + (int)(LV3EN1_H * LV3EN1_HB_BOTTOM);

	// ════════════════════════════════════════════════════════════
	//  STATE: HIT REACT — plays once, then returns to ATTACKING
	// ════════════════════════════════════════════════════════════
	if (lv3En1State == LV3EN1_HIT_REACT) {
		lv3En1AnimTick++;
		if (lv3En1AnimTick >= LV3EN1_HIT_SPEED) {
			lv3En1AnimTick = 0;
			lv3En1FrameIndex++;
			if (lv3En1FrameIndex >= LV3EN1_HIT_FRAMES) {
				lv3En1FrameIndex = 0;
				lv3En1State = LV3EN1_ATTACKING;
			}
		}
		return;
	}

	// ════════════════════════════════════════════════════════════
	//  PUNCH LANDING CHECK
	//  Uses lv3PHB so the fist sprite must physically touch the enemy.
	// ════════════════════════════════════════════════════════════
	bool normalPunchActive = (punchState == 2);
	bool chargedPunchActive = (chargedSequenceActive && chargedPunchState == 1);
	bool anyPunchActive = normalPunchActive || chargedPunchActive;

	if (anyPunchActive && !lv3En1WasPunching && lv3En1State != LV3EN1_HIT_REACT) {
		int ppX1 = lv3PHB.x1;
		int ppX2 = lv3PHB.x2;
		int ppY1 = lv3PHB.y1;
		int ppY2 = lv3PHB.y2;

		bool oX = !(ppX2 < eX1 || ppX1 > eX2);
		bool oY = !(ppY2 < eY1 || ppY1 > eY2);

		if (oX && oY) {
			if (chargedPunchActive)
				lv3En1ChargedHits++;
			else
				lv3En1NormalHits++;

			bool killed = (lv3En1NormalHits >= LV3EN1_NORMAL_HITS) ||
				(lv3En1ChargedHits >= LV3EN1_CHARGED_HITS);

			if (killed) {
				lv3En1Alive = false;
				lv3En1Falling = true;
				lv3En1DeadVel = 2.f;
				lv3En1DeadY = (float)(bridges3[1].snapY + LV3EN1_Y_OFF);
				lv3En1WasColliding = false;
				showKneel = false;
				kneelHolding = false;
				kneelHoldCounter = 0;
				return;
			}
			else {
				lv3En1State = LV3EN1_HIT_REACT;
				lv3En1FrameIndex = 0;
				lv3En1AnimTick = 0;
			}
		}
	}
	lv3En1WasPunching = anyPunchActive;

	// ════════════════════════════════════════════════════════════
	//  STATE: IDLE → force to WALKING after activation
	// ════════════════════════════════════════════════════════════
	if (lv3En1State == LV3EN1_IDLE)
		lv3En1State = LV3EN1_WALKING;

	// ════════════════════════════════════════════════════════════
	//  STATE: WALKING
	// ════════════════════════════════════════════════════════════
	if (lv3En1State == LV3EN1_WALKING) {
		int playerCx = lv3CharX + PLAYER_WIDTH / 2;
		int enemyCx = (int)lv3En1X + LV3EN1_W / 2;
		float dist = (float)abs(playerCx - enemyCx);

		if (dist > LV3EN1_ATTACK_DIST) {
			if (playerCx < enemyCx)
				lv3En1X -= LV3EN1_WALK_SPEED;
			else
				lv3En1X += LV3EN1_WALK_SPEED;

			lv3En1AnimTick++;
			if (lv3En1AnimTick >= LV3EN1_WALK_SPEED_A) {
				lv3En1AnimTick = 0;
				lv3En1FrameIndex = (lv3En1FrameIndex + 1) % LV3EN1_WALK_FRAMES;
			}
		}
		else {
			lv3En1State = LV3EN1_ATTACKING;
			lv3En1FrameIndex = 0;
			lv3En1AnimTick = 0;
		}
		return;
	}

	// ════════════════════════════════════════════════════════════
	//  STATE: ATTACKING
	// ════════════════════════════════════════════════════════════
	if (lv3En1State == LV3EN1_ATTACKING) {
		int playerCx = lv3CharX + PLAYER_WIDTH / 2;
		int enemyCx = (int)lv3En1X + LV3EN1_W / 2;
		float dist = (float)abs(playerCx - enemyCx);

		if (dist > LV3EN1_ATTACK_DIST + 30) {
			lv3En1State = LV3EN1_WALKING;
			lv3En1FrameIndex = 0;
			lv3En1AnimTick = 0;
			return;
		}

		lv3En1AnimTick++;
		if (lv3En1AnimTick >= LV3EN1_DATK_SPEED) {
			lv3En1AnimTick = 0;
			lv3En1FrameIndex = (lv3En1FrameIndex + 1) % LV3EN1_DATK_FRAMES;
		}

		// ── Lethal frame → damage player ─────────────────────────
		if (lv3En1FrameIndex == LV3EN1_LETHAL_FRAME) {
			int aX1 = (int)lv3En1X + (int)(LV3EN1_W * LV3EN1_ATK_HB_LEFT);
			int aX2 = (int)lv3En1X + (int)(LV3EN1_W * LV3EN1_ATK_HB_RIGHT);
			int aY1 = enY + (int)(LV3EN1_H * LV3EN1_ATK_HB_TOP);
			int aY2 = enY + (int)(LV3EN1_H * LV3EN1_ATK_HB_BOTTOM);

			int pX1 = lv3PHB.x1;
			int pX2 = lv3PHB.x2;
			int pY1 = lv3PHB.y1;
			int pY2 = lv3PHB.y2;

			bool oX = !(pX2 < aX1 || pX1 > aX2);
			bool oY = !(pY2 < aY1 || pY1 > aY2);
			bool isColliding = oX && oY;

			bool playerInvincible = lv3ShieldActive || showKneel || kneelHolding;

			if (isColliding && !lv3En1WasColliding && !playerInvincible) {
				lv3En1WasColliding = true;

				playerLives--;
				if (playerLives < 0) playerLives = 0;

				if (playerLives > 0) {
					showKneel = true;
					kneelHolding = true;
					kneelHoldCounter = 0;
				}
				else {
					showKneel = false;
					kneelHolding = false;
					playerAlive = false;
					gameStarted = false;
					playerDeathY = bridges3[1].snapY;
					handlePlayerDeath();
				}
			}
			if (!isColliding)
				lv3En1WasColliding = false;
		}
		else {
			lv3En1WasColliding = false;
		}
		return;
	}
}

void updateLv3En2() {
	if (currentBg != 12) return;

	// ── Activate only after En1 is dead ──────────────────────────
	if (!lv3En2Active) {
		if (!lv3En1Alive) {   // En1 just died → spawn En2 at left edge
			lv3En2Active = true;
			lv3En2X = (float)(bridges3[1].minX);   // left edge of Bridge 2
			lv3En2State = LV3EN2_RUNNING;
			lv3En2FrameIndex = 0;
			lv3En2AnimTick = 0;
		}
		else {
			return;   // En1 still alive — do nothing
		}
	}

	// ── Dead-fall physics ─────────────────────────────────────────
	if (!lv3En2Alive) {
		if (lv3En2Falling) {
			lv3En2DeadVel -= 1.2f;
			lv3En2DeadY += lv3En2DeadVel;
			if (lv3En2DeadY <= 0.f) {
				lv3En2DeadY = 0.f;
				lv3En2Falling = false;
			}
		}
		return;
	}

	if (!playerAlive) return;

	// ── Shared: enemy body hitbox (world coords) ──────────────────
	int enY = bridges3[1].snapY + LV3EN2_Y_OFF;
	// Use DATK size for body box reference (widest sprite)
	int eX1 = (int)lv3En2X + (int)(LV3EN2_DATK_W * LV3EN2_HB_LEFT);
	int eX2 = (int)lv3En2X + (int)(LV3EN2_DATK_W * LV3EN2_HB_RIGHT);
	int eY1 = enY + (int)(LV3EN2_DATK_H * LV3EN2_HB_TOP);
	int eY2 = enY + (int)(LV3EN2_DATK_H * LV3EN2_HB_BOTTOM);

	// ════════════════════════════════════════════════════════════
	//  STATE: HIT REACT — plays once, returns to ATTACKING
	// ════════════════════════════════════════════════════════════
	if (lv3En2State == LV3EN2_HIT_REACT) {
		lv3En2AnimTick++;
		if (lv3En2AnimTick >= LV3EN2_HIT_SPEED) {
			lv3En2AnimTick = 0;
			lv3En2FrameIndex++;
			if (lv3En2FrameIndex >= LV3EN2_HIT_FRAMES) {
				lv3En2FrameIndex = 0;
				lv3En2State = LV3EN2_ATTACKING;
			}
		}
		return;
	}

	// ════════════════════════════════════════════════════════════
	//  PUNCH LANDING CHECK
	// ════════════════════════════════════════════════════════════
	bool normalPunchActive = (punchState == 2);
	bool chargedPunchActive = (chargedSequenceActive && chargedPunchState == 1);
	bool anyPunchActive = normalPunchActive || chargedPunchActive;

	if (anyPunchActive && !lv3En2WasPunching && lv3En2State != LV3EN2_HIT_REACT) {
		int ppX1 = lv3PHB.x1;
		int ppX2 = lv3PHB.x2;
		int ppY1 = lv3PHB.y1;
		int ppY2 = lv3PHB.y2;

		bool oX = !(ppX2 < eX1 || ppX1 > eX2);
		bool oY = !(ppY2 < eY1 || ppY1 > eY2);

		if (oX && oY) {
			if (chargedPunchActive)
				lv3En2ChargedHits++;
			else
				lv3En2NormalHits++;

			bool killed = (lv3En2NormalHits >= LV3EN2_KILL_NORMAL) ||
				(lv3En2ChargedHits >= LV3EN2_KILL_CHARGED);

			if (killed) {
				lv3En2Alive = false;
				lv3En2Falling = true;
				lv3En2DeadVel = 2.f;
				lv3En2DeadY = (float)(bridges3[1].snapY + LV3EN2_Y_OFF);
				lv3En2WasColliding = false;
				showKneel = false;
				kneelHolding = false;
				kneelHoldCounter = 0;
				return;
			}
			else {
				lv3En2State = LV3EN2_HIT_REACT;
				lv3En2FrameIndex = 0;
				lv3En2AnimTick = 0;
			}
		}
	}
	lv3En2WasPunching = anyPunchActive;

	// ════════════════════════════════════════════════════════════
	//  STATE: RUNNING — move right until within ATTACK_DIST
	// ════════════════════════════════════════════════════════════
	if (lv3En2State == LV3EN2_RUNNING) {
		int playerCx = lv3CharX + PLAYER_WIDTH / 2;
		int enemyCx = (int)lv3En2X + LV3EN2_RUN_W / 2;
		float dist = (float)(playerCx - enemyCx);   // positive = player is to the right

		if (dist > LV3EN2_ATTACK_DIST) {
			lv3En2X += LV3EN2_RUN_SPEED;   // move RIGHT toward player

			lv3En2AnimTick++;
			if (lv3En2AnimTick >= LV3EN2_RUN_SPEED_A) {
				lv3En2AnimTick = 0;
				lv3En2FrameIndex = (lv3En2FrameIndex + 1) % LV3EN2_RUN_FRAMES;
			}
		}
		else {
			lv3En2State = LV3EN2_ATTACKING;
			lv3En2FrameIndex = 0;
			lv3En2AnimTick = 0;
		}
		return;
	}

	// ════════════════════════════════════════════════════════════
	//  STATE: ATTACKING
	// ════════════════════════════════════════════════════════════
	if (lv3En2State == LV3EN2_ATTACKING) {
		int playerCx = lv3CharX + PLAYER_WIDTH / 2;
		int enemyCx = (int)lv3En2X + LV3EN2_DATK_W / 2;
		float dist = (float)(playerCx - enemyCx);

		// Player moved away — resume running
		if (dist > LV3EN2_ATTACK_DIST + 50) {
			lv3En2State = LV3EN2_RUNNING;
			lv3En2FrameIndex = 0;
			lv3En2AnimTick = 0;
			return;
		}

		lv3En2AnimTick++;
		if (lv3En2AnimTick >= LV3EN2_DATK_SPEED) {
			lv3En2AnimTick = 0;
			lv3En2FrameIndex = (lv3En2FrameIndex + 1) % LV3EN2_DATK_FRAMES;
		}

		// ── Lethal frame → damage player ──────────────────────
		if (lv3En2FrameIndex == LV3EN2_LETHAL_FRAME) {
			int aX1 = (int)lv3En2X + (int)(LV3EN2_DATK_W * LV3EN2_ATK_HB_LEFT);
			int aX2 = (int)lv3En2X + (int)(LV3EN2_DATK_W * LV3EN2_ATK_HB_RIGHT);
			int aY1 = enY + (int)(LV3EN2_DATK_H * LV3EN2_ATK_HB_TOP);
			int aY2 = enY + (int)(LV3EN2_DATK_H * LV3EN2_ATK_HB_BOTTOM);

			int pX1 = lv3PHB.x1;
			int pX2 = lv3PHB.x2;
			int pY1 = lv3PHB.y1;
			int pY2 = lv3PHB.y2;

			bool oX = !(pX2 < aX1 || pX1 > aX2);
			bool oY = !(pY2 < aY1 || pY1 > aY2);
			bool isColliding = oX && oY;

			bool playerInvincible = lv3ShieldActive || showKneel || kneelHolding;

			if (isColliding && !lv3En2WasColliding && !playerInvincible) {
				lv3En2WasColliding = true;

				playerLives--;
				if (playerLives < 0) playerLives = 0;

				if (playerLives > 0) {
					showKneel = true;
					kneelHolding = true;
					kneelHoldCounter = 0;
				}
				else {
					showKneel = false;
					kneelHolding = false;
					playerAlive = false;
					gameStarted = false;
					playerDeathY = bridges3[1].snapY;
					handlePlayerDeath();
				}
			}
			if (!isColliding)
				lv3En2WasColliding = false;
		}
		else {
			lv3En2WasColliding = false;
		}
		return;
	}
}

void updateLv3En3() {
	if (currentBg != 12) return;

	// ── Dead-fall physics ────────────────────────────────────────
	if (!lv3En3Alive) {
		if (lv3En3Falling) {
			lv3En3DeadVel -= 1.2f;
			lv3En3DeadY += lv3En3DeadVel;
			if (lv3En3DeadY <= 0.f) {
				lv3En3DeadY = 0.f;
				lv3En3Falling = false;
			}
		}
		if (!lv3En3FadeDone) {
			lv3En3FadeTick++;
			if (lv3En3FadeTick >= LV3EN3_FADE_DURATION)
				lv3En3FadeDone = true;
		}
		return;
	}

	// ── Activation: player reaches left-center of Bridge 6 ──────
	// Bridge 6 is bridges3[5]
	if (!lv3En3Active) {
		int bridge6Mid = (bridges3[5].minX + bridges3[5].maxX) / 2;
		int cx = lv3CharX + PLAYER_WIDTH / 2;
		if (cx >= bridge6Mid + LV3EN3_ACTIVATE_OFFSET) {
			lv3En3Active = true;
		}
		else {
			// Still idle — animate but don't move
			lv3En3AnimTick++;
			if (lv3En3AnimTick >= LV3EN3_IDLE_SPEED) {
				lv3En3AnimTick = 0;
				lv3En3FrameIndex = (lv3En3FrameIndex + 1) % LV3EN3_IDLE_FRAMES;
			}
			return;
		}
	}

	if (!playerAlive) return;

	// ── Shared: body hitbox — use IDLE/WALK size for body reference ──
	int enY = bridges3[5].snapY + LV3EN3_Y_OFF;
	int eX1 = (int)lv3En3X + (int)(LV3EN3_WALK_W * LV3EN3_HB_LEFT);
	int eX2 = (int)lv3En3X + (int)(LV3EN3_WALK_W * LV3EN3_HB_RIGHT);
	int eY1 = enY + (int)(LV3EN3_WALK_H * LV3EN3_HB_TOP);
	int eY2 = enY + (int)(LV3EN3_WALK_H * LV3EN3_HB_BOTTOM);

	// ════════════════════════════════════════════════════════════
	//  STATE: HIT REACT — plays full animation once, then ATTACKING
	// ════════════════════════════════════════════════════════════
	if (lv3En3State == LV3EN3_HIT_REACT) {
		lv3En3AnimTick++;
		if (lv3En3AnimTick >= LV3EN3_HIT_SPEED) {
			lv3En3AnimTick = 0;
			lv3En3FrameIndex++;
			if (lv3En3FrameIndex >= LV3EN3_HIT_FRAMES) {
				lv3En3FrameIndex = 0;
				lv3En3State = LV3EN3_ATTACKING;
			}
		}
		// No punch registration while hit-react is playing
		return;
	}

	// ════════════════════════════════════════════════════════════
	//  PUNCH LANDING CHECK
	//  Only punchState==2 (punchf/punchff) and chargedPunchState==1
	//  count as hits. Uses lv3PHB for accurate fist-sprite overlap.
	// ════════════════════════════════════════════════════════════
	bool normalPunchActive = (punchState == 2);
	bool chargedPunchActive = (chargedSequenceActive && chargedPunchState == 1);
	bool anyPunchActive = normalPunchActive || chargedPunchActive;

	if (anyPunchActive && !lv3En3WasPunching && lv3En3State != LV3EN3_HIT_REACT) {
		int ppX1 = lv3PHB.x1;
		int ppX2 = lv3PHB.x2;
		int ppY1 = lv3PHB.y1;
		int ppY2 = lv3PHB.y2;

		bool oX = !(ppX2 < eX1 || ppX1 > eX2);
		bool oY = !(ppY2 < eY1 || ppY1 > eY2);

		if (oX && oY) {
			if (chargedPunchActive)
				lv3En3ChargedHits++;
			else
				lv3En3NormalHits++;

			bool killed = (lv3En3NormalHits >= LV3EN3_NORMAL_HITS) ||
				(lv3En3ChargedHits >= LV3EN3_CHARGED_HITS);

			if (killed) {
				lv3En3Alive = false;
				lv3En3Falling = true;
				lv3En3DeadVel = 2.f;
				lv3En3DeadY = (float)(bridges3[5].snapY + LV3EN3_Y_OFF);
				lv3En3WasColliding = false;
				lv3En3FadeTick = 0;
				lv3En3FadeDone = false;
				showKneel = false;
				kneelHolding = false;
				kneelHoldCounter = 0;
				return;
			}
			else {
				// Hit react — interrupt current state
				lv3En3State = LV3EN3_HIT_REACT;
				lv3En3FrameIndex = 0;
				lv3En3AnimTick = 0;
			}
		}
	}
	lv3En3WasPunching = anyPunchActive;

	// ════════════════════════════════════════════════════════════
	//  STATE: IDLE → force to WALKING immediately after activation
	// ════════════════════════════════════════════════════════════
	if (lv3En3State == LV3EN3_IDLE_STATE)
		lv3En3State = LV3EN3_WALKING;

	// ════════════════════════════════════════════════════════════
	//  STATE: WALKING — move toward player
	// ════════════════════════════════════════════════════════════
	if (lv3En3State == LV3EN3_WALKING) {
		int playerCx = lv3CharX + PLAYER_WIDTH / 2;
		int enemyCx = (int)lv3En3X + LV3EN3_WALK_W / 2;
		float dist = (float)abs(playerCx - enemyCx);

		if (dist > LV3EN3_ATTACK_DIST - 10) {
			// Move left (toward player, who is to the left)
			if (playerCx < enemyCx)
				lv3En3X -= LV3EN3_WALK_SPEED;
			else
				lv3En3X += LV3EN3_WALK_SPEED;

			lv3En3AnimTick++;
			if (lv3En3AnimTick >= LV3EN3_WALK_SPEED_A) {
				lv3En3AnimTick = 0;
				lv3En3FrameIndex = (lv3En3FrameIndex + 1) % LV3EN3_WALK_FRAMES;
			}
		}
		else {
			lv3En3State = LV3EN3_ATTACKING;
			lv3En3FrameIndex = 0;
			lv3En3AnimTick = 0;
		}
		return;
	}

	// ════════════════════════════════════════════════════════════
	//  STATE: ATTACKING
	// ════════════════════════════════════════════════════════════
	if (lv3En3State == LV3EN3_ATTACKING) {
		int playerCx = lv3CharX + PLAYER_WIDTH / 2;
		// Use WALK_W for center calculation (same as walking state) to avoid position jump
		int enemyCx = (int)lv3En3X + LV3EN3_WALK_W / 2;
		float dist = (float)abs(playerCx - enemyCx);

		// Use a larger hysteresis gap to prevent flicker
		if (dist > LV3EN3_ATTACK_DIST + 60) {
			lv3En3State = LV3EN3_WALKING;
			lv3En3FrameIndex = 0;
			lv3En3AnimTick = 0;
			return;
		}

		lv3En3AnimTick++;
		if (lv3En3AnimTick >= LV3EN3_DATK_SPEED) {
			lv3En3AnimTick = 0;
			lv3En3FrameIndex = (lv3En3FrameIndex + 1) % LV3EN3_DATK_FRAMES;
		}

		// ── Lethal frame only: datk14 (0-based index 13) ─────────
		if (lv3En3FrameIndex == LV3EN3_LETHAL_FRAME) {
			// Attack hitbox uses DATK sprite size
			int aX1 = (int)lv3En3X + (int)(LV3EN3_DATK_W * LV3EN3_ATK_HB_LEFT);
			int aX2 = (int)lv3En3X + (int)(LV3EN3_DATK_W * LV3EN3_ATK_HB_RIGHT);
			int aY1 = enY + (int)(LV3EN3_DATK_H * LV3EN3_ATK_HB_TOP);
			int aY2 = enY + (int)(LV3EN3_DATK_H * LV3EN3_ATK_HB_BOTTOM);

			int pX1 = lv3PHB.x1;
			int pX2 = lv3PHB.x2;
			int pY1 = lv3PHB.y1;
			int pY2 = lv3PHB.y2;

			bool oX = !(pX2 < aX1 || pX1 > aX2);
			bool oY = !(pY2 < aY1 || pY1 > aY2);
			bool isColliding = oX && oY;

			// Shield or kneel = invincible
			bool playerInvincible = lv3ShieldActive || showKneel || kneelHolding;

			if (isColliding && !lv3En3WasColliding && !playerInvincible) {
				lv3En3WasColliding = true;

				playerLives--;
				if (playerLives < 0) playerLives = 0;

				if (playerLives > 0) {
					showKneel = true;
					kneelHolding = true;
					kneelHoldCounter = 0;
				}
				else {
					showKneel = false;
					kneelHolding = false;
					playerAlive = false;
					gameStarted = false;
					playerDeathY = bridges3[5].snapY;
					handlePlayerDeath();
				}
			}
			if (!isColliding)
				lv3En3WasColliding = false;
		}
		else {
			lv3En3WasColliding = false;
		}
		return;
	}
}

void updateLv3En4() {
	if (currentBg != 12) return;

	if (!lv3En4Active) {
		if (!lv3En3Alive) {
			lv3En4Active = true;
			lv3En4X = (float)(bridges3[5].minX);
			lv3En4State = LV3EN4_RUNNING;
			lv3En4FrameIndex = 0;
			lv3En4AnimTick = 0;
		}
		else return;
	}

	if (!lv3En4Alive) {
		if (lv3En4Falling) {
			lv3En4DeadVel -= 1.2f;
			lv3En4DeadY += lv3En4DeadVel;
			if (lv3En4DeadY <= 0.f) {
				lv3En4DeadY = 0.f;
				lv3En4Falling = false;
			}
		}
		if (!lv3En4FadeDone) {
			lv3En4FadeTick++;
			if (lv3En4FadeTick >= LV3EN4_FADE_DURATION)
				lv3En4FadeDone = true;
		}
		return;
	}

	if (!playerAlive) return;

	int enY = bridges3[5].snapY + LV3EN4_Y_OFF;
	int eX1 = (int)lv3En4X + (int)(LV3EN4_DATK_W * LV3EN4_HB_LEFT);
	int eX2 = (int)lv3En4X + (int)(LV3EN4_DATK_W * LV3EN4_HB_RIGHT);
	int eY1 = enY + (int)(LV3EN4_DATK_H * LV3EN4_HB_TOP);
	int eY2 = enY + (int)(LV3EN4_DATK_H * LV3EN4_HB_BOTTOM);

	if (lv3En4State == LV3EN4_HIT_REACT) {
		lv3En4AnimTick++;
		if (lv3En4AnimTick >= LV3EN4_HIT_SPEED) {
			lv3En4AnimTick = 0;
			lv3En4FrameIndex++;
			if (lv3En4FrameIndex >= LV3EN4_HIT_FRAMES) {
				lv3En4FrameIndex = 0;
				lv3En4State = LV3EN4_ATTACKING;
			}
		}
		return;
	}

	bool normalPunchActive = (punchState == 2);
	bool chargedPunchActive = (chargedSequenceActive && chargedPunchState == 1);
	bool anyPunchActive = normalPunchActive || chargedPunchActive;

	if (anyPunchActive && !lv3En4WasPunching && lv3En4State != LV3EN4_HIT_REACT) {
		int ppX1 = lv3PHB.x1, ppX2 = lv3PHB.x2;
		int ppY1 = lv3PHB.y1, ppY2 = lv3PHB.y2;

		bool oX = !(ppX2 < eX1 || ppX1 > eX2);
		bool oY = !(ppY2 < eY1 || ppY1 > eY2);

		if (oX && oY) {
			if (chargedPunchActive) lv3En4ChargedHits++;
			else                    lv3En4NormalHits++;

			bool killed = (lv3En4NormalHits >= LV3EN4_KILL_NORMAL) ||
				(lv3En4ChargedHits >= LV3EN4_KILL_CHARGED);

			if (killed) {
				lv3En4Alive = false;
				lv3En4Falling = true;
				lv3En4DeadVel = 2.f;
				lv3En4DeadY = (float)(bridges3[5].snapY + LV3EN4_Y_OFF);
				lv3En4WasColliding = false;
				lv3En4FadeTick = 0;
				lv3En4FadeDone = false;
				showKneel = false;
				kneelHolding = false;
				kneelHoldCounter = 0;
				return;
			}
			else {
				lv3En4State = LV3EN4_HIT_REACT;
				lv3En4FrameIndex = 0;
				lv3En4AnimTick = 0;
			}
		}
	}
	lv3En4WasPunching = anyPunchActive;

	if (lv3En4State == LV3EN4_RUNNING) {
		int playerCx = lv3CharX + PLAYER_WIDTH / 2;
		int enemyCx = (int)lv3En4X + LV3EN4_RUN_W / 2;
		float dist = (float)(playerCx - enemyCx);

		if (dist > LV3EN4_ATTACK_DIST) {
			lv3En4X += LV3EN4_RUN_SPEED;
			lv3En4AnimTick++;
			if (lv3En4AnimTick >= LV3EN4_RUN_SPEED_A) {
				lv3En4AnimTick = 0;
				lv3En4FrameIndex = (lv3En4FrameIndex + 1) % LV3EN4_RUN_FRAMES;
			}
		}
		else {
			lv3En4State = LV3EN4_ATTACKING;
			lv3En4FrameIndex = 0;
			lv3En4AnimTick = 0;
		}
		return;
	}

	if (lv3En4State == LV3EN4_ATTACKING) {
		int playerCx = lv3CharX + PLAYER_WIDTH / 2;
		int enemyCx = (int)lv3En4X + LV3EN4_DATK_W / 2;
		float dist = (float)(playerCx - enemyCx);

		if (dist > LV3EN4_ATTACK_DIST + 50) {
			lv3En4State = LV3EN4_RUNNING;
			lv3En4FrameIndex = 0;
			lv3En4AnimTick = 0;
			return;
		}

		lv3En4AnimTick++;
		if (lv3En4AnimTick >= LV3EN4_DATK_SPEED) {
			lv3En4AnimTick = 0;
			lv3En4FrameIndex = (lv3En4FrameIndex + 1) % LV3EN4_DATK_FRAMES;
		}

		if (lv3En4FrameIndex == LV3EN4_LETHAL_FRAME) {
			int aX1 = (int)lv3En4X + (int)(LV3EN4_DATK_W * LV3EN4_ATK_HB_LEFT);
			int aX2 = (int)lv3En4X + (int)(LV3EN4_DATK_W * LV3EN4_ATK_HB_RIGHT);
			int aY1 = enY + (int)(LV3EN4_DATK_H * LV3EN4_ATK_HB_TOP);
			int aY2 = enY + (int)(LV3EN4_DATK_H * LV3EN4_ATK_HB_BOTTOM);

			int pX1 = lv3PHB.x1, pX2 = lv3PHB.x2;
			int pY1 = lv3PHB.y1, pY2 = lv3PHB.y2;

			bool oX = !(pX2 < aX1 || pX1 > aX2);
			bool oY = !(pY2 < aY1 || pY1 > aY2);
			bool isColliding = oX && oY;
			bool playerInvincible = lv3ShieldActive || showKneel || kneelHolding;

			if (isColliding && !lv3En4WasColliding && !playerInvincible) {
				lv3En4WasColliding = true;
				playerLives--;
				if (playerLives < 0) playerLives = 0;

				if (playerLives > 0) {
					showKneel = true;
					kneelHolding = true;
					kneelHoldCounter = 0;
				}
				else {
					showKneel = false;
					kneelHolding = false;
					playerAlive = false;
					gameStarted = false;
					playerDeathY = bridges3[5].snapY;
					handlePlayerDeath();
				}
			}
			if (!isColliding) lv3En4WasColliding = false;
		}
		else {
			lv3En4WasColliding = false;
		}
		return;
	}
}

void updateLv3WkEnemies() {
	if (currentBg != 12) return;

	// ── Advance animation ───────────────────────────────────────
	static int lv3WkAnimTick = 0;
	if (++lv3WkAnimTick >= 4) {
		lv3WkAnimTick = 0;
		if (lv3WkAlive)
			lv3WkFrameIndex = (lv3WkFrameIndex + 1) % LV3WK_FRAMECOUNT;
	}

	if (!playerAlive) return;
	if (!lv3WkAlive)  return;

	// ── Kneel cooldown ──────────────────────────────────────────
	if (lv3WkKneeling) {
		lv3WkKneelTimer++;
		if (lv3WkKneelTimer >= LV3WK_KNEEL_DURATION) {
			lv3WkKneeling = false;
			lv3WkKneelTimer = 0;
			lv3WkPlayerTouching = false;
			kneelHolding = false;
			kneelHoldCounter = 0;
			showKneel = false;
		}
		return;
	}

	// ── Enemy hitbox (world coords) ────────────────────────────
	int eX1 = (int)lv3WkX + (int)(LV3WK_WIDTH  * LV3WK_HB_LEFT);
	int eX2 = (int)lv3WkX + (int)(LV3WK_WIDTH  * LV3WK_HB_RIGHT);
	int eY1 = (int)lv3WkY + (int)(LV3WK_HEIGHT * LV3WK_HB_TOP);
	int eY2 = (int)lv3WkY + (int)(LV3WK_HEIGHT * LV3WK_HB_BOTTOM);

	// ── Player hitbox (world coords via lv3PHB) ────────────────
	int pX1 = lv3PHB.x1;
	int pX2 = lv3PHB.x2;
	int pY1 = lv3PHB.y1;
	int pY2 = lv3PHB.y2;

	bool overlapX = !(pX2 < eX1 || pX1 > eX2);
	bool overlapY = !(pY2 < eY1 || pY1 > eY2);
	bool isColliding = overlapX && overlapY;

	if (isColliding && !lv3WkPlayerTouching) {
		lv3WkPlayerTouching = true;
		lv3WkKneeling = true;
		lv3WkKneelTimer = 0;
		punchJustLanded = false;

		playerLives--;
		if (playerLives < 0) playerLives = 0;

		if (playerLives > 0) {
			showKneel = true;
			kneelHolding = true;
			kneelHoldCounter = 0;
		}
		else {
			showKneel = false;
			kneelHolding = false;
			playerAlive = false;
			gameStarted = false;
			playerDeathY = bridges3[5].snapY;
			handlePlayerDeath();
		}
	}

	if (!isColliding)
		lv3WkPlayerTouching = false;
}

void updateLv3En5() {
	if (currentBg != 12) return;

	int bridge6SnapY = bridges3[5].snapY;
	int enBaseY = bridge6SnapY + LV3EN5_LAND_Y_OFF;

	// ── TRIGGER: En4 receives first normal punch hit ─────────────────
	if (lv3En5State == LV3EN5_WAITING) {
		if (!lv3En5En4HitTriggered && (lv3En4NormalHits >= 1 || lv3En4ChargedHits >= 1)) {
			lv3En5En4HitTriggered = true;
			lv3En5X = (float)(bridges3[5].maxX + LV3EN5_LAND_X_OFF);
			lv3En5Y = (float)(800);
			lv3En5State = LV3EN5_APPEARING;
			lv3En5FrameIndex = 0;
			lv3En5AnimTick = 0;
		}
		return;
	}

	if (lv3En5State == LV3EN5_TRULY_DEAD) return;

	// ── APPEARING: drop from top until landing Y ──────────────────────
	if (lv3En5State == LV3EN5_APPEARING) {
		lv3En5AnimTick++;
		if (lv3En5AnimTick >= LV3EN5_APR_SPEED) {
			lv3En5AnimTick = 0;
			lv3En5FrameIndex = (lv3En5FrameIndex + 1) % LV3EN5_APR_FRAMES;
		}
		lv3En5Y -= LV3EN5_DROP_SPEED;
		if (lv3En5Y <= (float)enBaseY) {
			lv3En5Y = (float)enBaseY;
			lv3En5State = LV3EN5_IDLE;
			lv3En5FrameIndex = 0;
			lv3En5AnimTick = 0;
		}
		return;
	}

	// ── Body hitbox (world coords, IDLE size as reference) ────────────
	int bX1 = (int)lv3En5X + (int)(LV3EN5_IDLE_W * LV3EN5_HB_LEFT);
	int bX2 = (int)lv3En5X + (int)(LV3EN5_IDLE_W * LV3EN5_HB_RIGHT);
	int bY1 = enBaseY + (int)(LV3EN5_IDLE_H * LV3EN5_HB_TOP);
	int bY2 = enBaseY + (int)(LV3EN5_IDLE_H * LV3EN5_HB_BOTTOM);

	// ── PUNCH LANDING CHECK ───────────────────────────────────────────
	// Blocked during HIT_REACT and REVIVING
	bool punchable = (lv3En5State == LV3EN5_IDLE ||
		lv3En5State == LV3EN5_RUNNING ||
		lv3En5State == LV3EN5_PUNCHING);

	bool normalPunchActive = (punchState == 2);
	static bool en5PrevChargedState = false;
	bool chargedRisingEdge = (chargedSequenceActive && chargedPunchState == 1) && !en5PrevChargedState;
	en5PrevChargedState = (chargedSequenceActive && chargedPunchState == 1);
	bool anyPunchActive = normalPunchActive || chargedRisingEdge;

	if (punchable && anyPunchActive && !lv3En5WasPunching) {
		int ppX1 = lv3PHB.x1, ppX2 = lv3PHB.x2;
		int ppY1 = lv3PHB.y1, ppY2 = lv3PHB.y2;

		bool oX = !(ppX2 < bX1 || ppX1 > bX2);
		bool oY = !(ppY2 < bY1 || ppY1 > bY2);

		if (oX && oY) {
			if (chargedRisingEdge) lv3En5ChargedHits++;
			else                   lv3En5NormalHits++;

			int c1N = LV3EN5_NORMAL_PER_CYCLE;
			int c1C = LV3EN5_CHARGED_PER_CYCLE;
			int c2N = LV3EN5_NORMAL_PER_CYCLE * 2;
			int c2C = LV3EN5_CHARGED_PER_CYCLE * 2;

			bool cycle1Done = (lv3En5NormalHits >= c1N ||
				lv3En5ChargedHits >= c1C);
			bool cycle2Done = (lv3En5NormalHits >= c2N ||
				lv3En5ChargedHits >= c2C);

			if (lv3En5ReviveCount >= 1 && cycle2Done) {
				lv3En5State = LV3EN5_DYING;
				lv3En5FrameIndex = 0;
				lv3En5AnimTick = 0;
			}
			else if (lv3En5ReviveCount == 0 && cycle1Done) {
				lv3En5State = LV3EN5_HIT_REACT;
				lv3En5FrameIndex = 0;
				lv3En5AnimTick = 0;
			}
		}
	}
	lv3En5WasPunching = normalPunchActive;  // only guard normal punch;
	//charged is already rising-edge gated by chargedPunchState toggle

	// ── IDLE ─────────────────────────────────────────────────────────
	if (lv3En5State == LV3EN5_IDLE) {
		lv3En5AnimTick++;
		if (lv3En5AnimTick >= LV3EN5_IDLE_SPEED) {
			lv3En5AnimTick = 0;
			lv3En5FrameIndex = (lv3En5FrameIndex + 1) % LV3EN5_IDLE_FRAMES;
		}
		if (lv3En4Alive) return; // stay idle until En4 dies

		int playerCx = lv3CharX + PLAYER_WIDTH / 2;
		int enemyCx = (int)lv3En5X + LV3EN5_IDLE_W / 2;
		float dist = (float)abs(playerCx - enemyCx);

		if (dist <= LV3EN5_ATTACK_DIST) {
			lv3En5State = LV3EN5_PUNCHING;
			lv3En5FrameIndex = 0;
			lv3En5AnimTick = 0;
		}
		else {
			lv3En5State = LV3EN5_RUNNING;
			lv3En5FrameIndex = 0;
			lv3En5AnimTick = 0;
		}
		return;
	}

	// ── RUNNING ───────────────────────────────────────────────────────
	if (lv3En5State == LV3EN5_RUNNING) {
		int playerCx = lv3CharX + PLAYER_WIDTH / 2;
		int enemyCx = (int)lv3En5X + LV3EN5_RUN_W / 2;
		float dist = (float)abs(playerCx - enemyCx);

		if (dist > LV3EN5_ATTACK_DIST) {
			if (playerCx < enemyCx) lv3En5X -= LV3EN5_RUN_SPEED;
			else                    lv3En5X += LV3EN5_RUN_SPEED;

			lv3En5AnimTick++;
			if (lv3En5AnimTick >= LV3EN5_RUN_SPEED_A) {
				lv3En5AnimTick = 0;
				lv3En5FrameIndex = (lv3En5FrameIndex + 1) % LV3EN5_RUN_FRAMES;
			}
		}
		else {
			lv3En5State = LV3EN5_PUNCHING;
			lv3En5FrameIndex = 0;
			lv3En5AnimTick = 0;
		}
		return;
	}

	// ── PUNCHING ──────────────────────────────────────────────────────
	if (lv3En5State == LV3EN5_PUNCHING) {
		lv3En5AnimTick++;
		if (lv3En5AnimTick >= LV3EN5_PUNCH_SPEED) {
			lv3En5AnimTick = 0;
			lv3En5FrameIndex = (lv3En5FrameIndex + 1) % LV3EN5_PUNCH_FRAMES;
		}

		bool lethalFrame = (lv3En5FrameIndex == LV3EN5_LETHAL_FRAME_A ||
			lv3En5FrameIndex == LV3EN5_LETHAL_FRAME_B);

		if (lethalFrame && playerAlive) {
			int aX1 = (int)lv3En5X + LV3EN5_PUNCH_X_OFF +
				(int)(LV3EN5_PUNCH_W * LV3EN5_ATK_HB_LEFT);
			int aX2 = (int)lv3En5X + LV3EN5_PUNCH_X_OFF +
				(int)(LV3EN5_PUNCH_W * LV3EN5_ATK_HB_RIGHT);
			int aY1 = enBaseY + LV3EN5_PUNCH_Y_OFF +
				(int)(LV3EN5_PUNCH_H * LV3EN5_ATK_HB_TOP);
			int aY2 = enBaseY + LV3EN5_PUNCH_Y_OFF +
				(int)(LV3EN5_PUNCH_H * LV3EN5_ATK_HB_BOTTOM);

			int pX1 = lv3PHB.x1, pX2 = lv3PHB.x2;
			int pY1 = lv3PHB.y1, pY2 = lv3PHB.y2;

			bool oX = !(pX2 < aX1 || pX1 > aX2);
			bool oY = !(pY2 < aY1 || pY1 > aY2);
			bool isColliding = oX && oY;
			bool playerInvincible = lv3ShieldActive || showKneel || kneelHolding;

			if (isColliding && !lv3En5WasColliding && !playerInvincible) {
				lv3En5WasColliding = true;
				playerLives -= 2;
				if (playerLives < 0) playerLives = 0;

				if (playerLives > 0) {
					showKneel = true;
					kneelHolding = true;
					kneelHoldCounter = 0;
				}
				else {
					showKneel = false;
					kneelHolding = false;
					playerAlive = false;
					gameStarted = false;
					playerDeathY = bridge6SnapY;
					handlePlayerDeath();
				}
			}
			if (!isColliding) lv3En5WasColliding = false;
		}
		else {
			lv3En5WasColliding = false;
		}

		// If player moves away, resume running
		int playerCx = lv3CharX + PLAYER_WIDTH / 2;
		int enemyCx = (int)lv3En5X + LV3EN5_PUNCH_W / 2;
		float dist = (float)abs(playerCx - enemyCx);
		if (dist > LV3EN5_ATTACK_DIST + 60) {
			lv3En5State = LV3EN5_RUNNING;
			lv3En5FrameIndex = 0;
			lv3En5AnimTick = 0;
		}
		return;
	}

	// ── HIT_REACT: plays once → freeze → REVIVING ────────────────────
#define LV3EN5_HIT_FREEZE_TICKS  27  // ← tweak freely (120 ticks × 16ms ≈ 2 seconds)

	if (lv3En5State == LV3EN5_HIT_REACT) {
		lv3En5AnimTick++;
		if (lv3En5AnimTick >= LV3EN5_HIT_SPEED) {
			lv3En5AnimTick = 0;
			// Only advance frame if animation not yet done
			if (!lv3En5HitFreeze) {
				lv3En5FrameIndex++;
				if (lv3En5FrameIndex >= LV3EN5_HIT_FRAMES) {
					// Animation done — freeze on last frame
					lv3En5FrameIndex = LV3EN5_HIT_FRAMES - 1;
					lv3En5HitFreeze = true;
					lv3En5HitFreezeTimer = 0;
				}
			}
		}
		// Count freeze ticks
		if (lv3En5HitFreeze) {
			lv3En5HitFreezeTimer++;
			if (lv3En5HitFreezeTimer >= LV3EN5_HIT_FREEZE_TICKS) {
				lv3En5HitFreeze = false;
				lv3En5HitFreezeTimer = 0;
				lv3En5State = LV3EN5_REVIVING;
				lv3En5FrameIndex = 0;
				lv3En5AnimTick = 0;
			}
		}
		return;
	}

	// ── REVIVING: plays once → IDLE (then IDLE decides run vs punch) ──
	if (lv3En5State == LV3EN5_REVIVING) {
		lv3En5AnimTick++;
		if (lv3En5AnimTick >= LV3EN5_REVIVE_SPEED) {
			lv3En5AnimTick = 0;
			lv3En5FrameIndex++;
			if (lv3En5FrameIndex >= LV3EN5_REVIVE_FRAMES) {
				lv3En5ReviveCount++;
				lv3En5State = LV3EN5_IDLE;
				lv3En5FrameIndex = 0;
				lv3En5AnimTick = 0;
			}
		}
		return;
	}

	// ── DYING: plays once → TRULY_DEAD (freeze on last frame) ────────
	if (lv3En5State == LV3EN5_DYING) {
		lv3En5AnimTick++;
		if (lv3En5AnimTick >= LV3EN5_DEAD_SPEED) {
			lv3En5AnimTick = 0;
			lv3En5FrameIndex++;
			if (lv3En5FrameIndex >= LV3EN5_DEAD_FRAMES) {
				lv3En5FrameIndex = LV3EN5_DEAD_FRAMES - 1;
				lv3En5State = LV3EN5_TRULY_DEAD;
			}
		}
		return;
	}
}

void updateLv3En6() {
	if (currentBg != 12) return;

	int bridge6SnapY = bridges3[5].snapY;
	int enBaseY = bridge6SnapY + LV3EN6_LAND_Y_OFF;

	// ── TRIGGER: appear after En5 is fully dead ──────────────────────
	if (lv3En6State == LV3EN6_WAITING) {
		if (lv3En5State == LV3EN5_TRULY_DEAD) {
			lv3En6X = (float)(bridges3[5].minX + LV3EN6_LAND_X_OFF);
			lv3En6Y = (float)(800);
			lv3En6State = LV3EN6_APPEARING;
			lv3En6FrameIndex = 0;
			lv3En6AnimTick = 0;
		}
		return;
	}

	if (lv3En6State == LV3EN6_TRULY_DEAD) return;

	// ── Body hitbox (world coords, IDLE size as reference) ────────────
	int bX1 = (int)lv3En6X + (int)(LV3EN6_IDLE_W * LV3EN6_HB_LEFT);
	int bX2 = (int)lv3En6X + (int)(LV3EN6_IDLE_W * LV3EN6_HB_RIGHT);
	int bY1 = enBaseY + (int)(LV3EN6_IDLE_H * LV3EN6_HB_TOP);
	int bY2 = enBaseY + (int)(LV3EN6_IDLE_H * LV3EN6_HB_BOTTOM);

	// ── PUNCH LANDING CHECK ───────────────────────────────────────────
	bool punchable = (lv3En6State == LV3EN6_IDLE ||
		lv3En6State == LV3EN6_RUNNING ||
		lv3En6State == LV3EN6_PUNCHING);

	bool normalPunchActive = (punchState == 2);
	static bool en6PrevChargedState = false;
	bool en6ChargedNow = (chargedSequenceActive && chargedPunchState == 1);
	bool chargedRisingEdge = en6ChargedNow && !en6PrevChargedState;
	en6PrevChargedState = en6ChargedNow;
	bool anyPunchActive = normalPunchActive || chargedRisingEdge;

	if (punchable && anyPunchActive && !lv3En6WasPunching) {
		int ppX1 = lv3PHB.x1, ppX2 = lv3PHB.x2;
		int ppY1 = lv3PHB.y1, ppY2 = lv3PHB.y2;

		bool oX = !(ppX2 < bX1 || ppX1 > bX2);
		bool oY = !(ppY2 < bY1 || ppY1 > bY2);

		if (oX && oY) {
			if (chargedRisingEdge) lv3En6ChargedHits++;
			else                   lv3En6NormalHits++;

			int c1N = LV3EN6_NORMAL_PER_CYCLE;
			int c1C = LV3EN6_CHARGED_PER_CYCLE;
			int c2N = LV3EN6_NORMAL_PER_CYCLE * 2;
			int c2C = LV3EN6_CHARGED_PER_CYCLE * 2;

			bool cycle1Done = (lv3En6NormalHits >= c1N ||
				lv3En6ChargedHits >= c1C);
			bool cycle2Done = (lv3En6NormalHits >= c2N ||
				lv3En6ChargedHits >= c2C);

			if (lv3En6ReviveCount == 0 && cycle1Done) {
				lv3En6State = LV3EN6_HIT_REACT;
				lv3En6FrameIndex = 0;
				lv3En6AnimTick = 0;
			}
			else if (lv3En6ReviveCount >= 1 && cycle2Done) {
				lv3En6State = LV3EN6_DYING;
				lv3En6FrameIndex = 0;
				lv3En6AnimTick = 0;
			}
		}
	}
	lv3En6WasPunching = normalPunchActive;

	// ── APPEARING ────────────────────────────────────────────────────
	if (lv3En6State == LV3EN6_APPEARING) {
		lv3En6AnimTick++;
		if (lv3En6AnimTick >= LV3EN6_APR_SPEED) {
			lv3En6AnimTick = 0;
			lv3En6FrameIndex = (lv3En6FrameIndex + 1) % LV3EN6_APR_FRAMES;
		}
		lv3En6Y -= LV3EN6_DROP_SPEED;
		if (lv3En6Y <= (float)enBaseY) {
			lv3En6Y = (float)enBaseY;
			lv3En6State = LV3EN6_IDLE;
			lv3En6FrameIndex = 0;
			lv3En6AnimTick = 0;
		}
		return;
	}

	// ── IDLE ──────────────────────────────────────────────────────────
	if (lv3En6State == LV3EN6_IDLE) {
		lv3En6AnimTick++;
		if (lv3En6AnimTick >= LV3EN6_IDLE_SPEED) {
			lv3En6AnimTick = 0;
			lv3En6FrameIndex = (lv3En6FrameIndex + 1) % LV3EN6_IDLE_FRAMES;
		}
		if (lv3En4Alive) return;

		int playerCx = lv3CharX + PLAYER_WIDTH / 2;
		int enemyCx = (int)lv3En6X + LV3EN6_IDLE_W / 2;
		float dist = (float)abs(playerCx - enemyCx);

		if (dist <= LV3EN6_ATTACK_DIST) {
			lv3En6State = LV3EN6_PUNCHING;
			lv3En6FrameIndex = 0;
			lv3En6AnimTick = 0;
		}
		else {
			lv3En6State = LV3EN6_RUNNING;
			lv3En6FrameIndex = 0;
			lv3En6AnimTick = 0;
		}
		return;
	}

	// ── RUNNING (moves LEFT toward player) ────────────────────────────
	if (lv3En6State == LV3EN6_RUNNING) {
		int playerCx = lv3CharX + PLAYER_WIDTH / 2;
		int enemyCx = (int)lv3En6X + LV3EN6_RUN_W / 2;
		float dist = (float)abs(playerCx - enemyCx);

		if (dist > LV3EN6_ATTACK_DIST) {
			// En6 faces left — player is always to the left
			if (playerCx < enemyCx) lv3En6X -= LV3EN6_RUN_SPEED;
			else                    lv3En6X += LV3EN6_RUN_SPEED;

			lv3En6AnimTick++;
			if (lv3En6AnimTick >= LV3EN6_RUN_SPEED_A) {
				lv3En6AnimTick = 0;
				lv3En6FrameIndex = (lv3En6FrameIndex + 1) % LV3EN6_RUN_FRAMES;
			}
		}
		else {
			lv3En6State = LV3EN6_PUNCHING;
			lv3En6FrameIndex = 0;
			lv3En6AnimTick = 0;
		}
		return;
	}

	// ── PUNCHING ──────────────────────────────────────────────────────
	if (lv3En6State == LV3EN6_PUNCHING) {
		lv3En6AnimTick++;
		if (lv3En6AnimTick >= LV3EN6_PUNCH_SPEED) {
			lv3En6AnimTick = 0;
			lv3En6FrameIndex = (lv3En6FrameIndex + 1) % LV3EN6_PUNCH_FRAMES;
		}

		bool lethalFrame = (lv3En6FrameIndex == LV3EN6_LETHAL_FRAME_A ||
			lv3En6FrameIndex == LV3EN6_LETHAL_FRAME_B);

		if (lethalFrame && playerAlive) {
			int aX1 = (int)lv3En6X + LV3EN6_PUNCH_X_OFF +
				(int)(LV3EN6_PUNCH_W * LV3EN6_ATK_HB_LEFT);
			int aX2 = (int)lv3En6X + LV3EN6_PUNCH_X_OFF +
				(int)(LV3EN6_PUNCH_W * LV3EN6_ATK_HB_RIGHT);
			int aY1 = enBaseY + LV3EN6_PUNCH_Y_OFF +
				(int)(LV3EN6_PUNCH_H * LV3EN6_ATK_HB_TOP);
			int aY2 = enBaseY + LV3EN6_PUNCH_Y_OFF +
				(int)(LV3EN6_PUNCH_H * LV3EN6_ATK_HB_BOTTOM);

			int pX1 = lv3PHB.x1, pX2 = lv3PHB.x2;
			int pY1 = lv3PHB.y1, pY2 = lv3PHB.y2;

			bool oX = !(pX2 < aX1 || pX1 > aX2);
			bool oY = !(pY2 < aY1 || pY1 > aY2);
			bool isColliding = oX && oY;
			bool playerInvincible = lv3ShieldActive || showKneel || kneelHolding;

			if (isColliding && !lv3En6WasColliding && !playerInvincible) {
				lv3En6WasColliding = true;
				playerLives -= 2;
				if (playerLives < 0) playerLives = 0;

				if (playerLives > 0) {
					showKneel = true;
					kneelHolding = true;
					kneelHoldCounter = 0;
				}
				else {
					showKneel = false;
					kneelHolding = false;
					playerAlive = false;
					gameStarted = false;
					playerDeathY = bridge6SnapY;
					handlePlayerDeath();
				}
			}
			if (!isColliding) lv3En6WasColliding = false;
		}
		else {
			lv3En6WasColliding = false;
		}

		int playerCx = lv3CharX + PLAYER_WIDTH / 2;
		int enemyCx = (int)lv3En6X + LV3EN6_PUNCH_W / 2;
		float dist = (float)abs(playerCx - enemyCx);
		if (dist > LV3EN6_ATTACK_DIST + 60) {
			lv3En6State = LV3EN6_RUNNING;
			lv3En6FrameIndex = 0;
			lv3En6AnimTick = 0;
		}
		return;
	}

	// ── HIT_REACT: plays once → freeze → REVIVING ────────────────────
#define LV3EN6_HIT_FREEZE_TICKS 27

	if (lv3En6State == LV3EN6_HIT_REACT) {
		lv3En6AnimTick++;
		if (lv3En6AnimTick >= LV3EN6_HIT_SPEED) {
			lv3En6AnimTick = 0;
			if (!lv3En6HitFreeze) {
				lv3En6FrameIndex++;
				if (lv3En6FrameIndex >= LV3EN6_HIT_FRAMES) {
					lv3En6FrameIndex = LV3EN6_HIT_FRAMES - 1;
					lv3En6HitFreeze = true;
					lv3En6HitFreezeTimer = 0;
				}
			}
		}
		if (lv3En6HitFreeze) {
			lv3En6HitFreezeTimer++;
			if (lv3En6HitFreezeTimer >= LV3EN6_HIT_FREEZE_TICKS) {
				lv3En6HitFreeze = false;
				lv3En6HitFreezeTimer = 0;
				lv3En6State = LV3EN6_REVIVING;
				lv3En6FrameIndex = 0;
				lv3En6AnimTick = 0;
			}
		}
		return;
	}

	// ── REVIVING ──────────────────────────────────────────────────────
	if (lv3En6State == LV3EN6_REVIVING) {
		lv3En6AnimTick++;
		if (lv3En6AnimTick >= LV3EN6_REVIVE_SPEED) {
			lv3En6AnimTick = 0;
			lv3En6FrameIndex++;
			if (lv3En6FrameIndex >= LV3EN6_REVIVE_FRAMES) {
				lv3En6ReviveCount++;
				lv3En6State = LV3EN6_IDLE;
				lv3En6FrameIndex = 0;
				lv3En6AnimTick = 0;
			}
		}
		return;
	}

	// ── DYING ─────────────────────────────────────────────────────────
	if (lv3En6State == LV3EN6_DYING) {
		lv3En6AnimTick++;
		if (lv3En6AnimTick >= LV3EN6_DEAD_SPEED) {
			lv3En6AnimTick = 0;
			lv3En6FrameIndex++;
			if (lv3En6FrameIndex >= LV3EN6_DEAD_FRAMES) {
				lv3En6FrameIndex = LV3EN6_DEAD_FRAMES - 1;
				lv3En6State = LV3EN6_TRULY_DEAD;
			}
		}
		return;
	}
}

void updateLv3Trees() {
	if (currentBg != 12) return;

	// Advance animation
	static int treeTick = 0;
	if (++treeTick >= 3) {   // tweak 3 to speed up / slow down
		treeTick = 0;
		treeAnimIndex = (treeAnimIndex + 1) % TREE_FRAMECOUNT;
	}

	if (!playerAlive) return;

	for (int i = 0; i < NUM_TREES_LV3; i++) {
		TreeObstacle &t = treesLv3[i];

		// Hitbox (world coords)
		int eX1 = (int)t.x + (int)(TREE_W * TREE_HB_LEFT);
		int eX2 = (int)t.x + (int)(TREE_W * TREE_HB_RIGHT);
		int eY1 = (int)t.y + (int)(TREE_H * TREE_HB_TOP);
		int eY2 = (int)t.y + (int)(TREE_H * TREE_HB_BOTTOM);

		// Player hitbox (world coords)
		int pX1 = lv3PHB.x1;
		int pX2 = lv3PHB.x2;
		int pY1 = lv3PHB.y1;
		int pY2 = lv3PHB.y2;

		bool overlapX = !(pX2 < eX1 || pX1 > eX2);
		bool overlapY = !(pY2 < eY1 || pY1 > eY2);
		bool isColliding = overlapX && overlapY;

		if (isColliding && !t.playerTouching) {
			t.playerTouching = true;

			playerLives -= 2;
			if (playerLives < 0) playerLives = 0;

			// Reset player to level 3 start position
			lv3CharX = 100;
			lv3CharY = bridges3[0].snapY;
			lv3IsJumping = false;
			lv3JumpVelocity = 0;
			showKneel = true;
			kneelHolding = true;
			kneelHoldCounter = 0;

			// Clear all tree touch flags so none re-trigger on respawn
			for (int j = 0; j < NUM_TREES_LV3; j++)
				treesLv3[j].playerTouching = false;

			if (playerLives <= 0) {
				playerLives = 0;
				showKneel = false;
				kneelHolding = false;
				playerAlive = false;
				gameStarted = false;
				playerDeathY = 0;
				handlePlayerDeath();
			}
			return;
		}

		if (!isColliding)
			t.playerTouching = false;
	}
}

void updateLv3Barrel() {
	if (currentBg != 12) return;
	if (barrelState >= 5) return;

	// ── ENERGY ANIMATION (state 4) ──────────────────────────────
	if (barrelState == 4) {
		// Advance energy animation — loops continuously
		if (++energyAnimTick >= BARREL_ENERGY_SPEED) {
			energyAnimTick = 0;
			energyFrameIndex = (energyFrameIndex + 1) % ENERGY_FRAMECOUNT;
		}

		// ── Crouch hitbox overlaps energy → restore full lives ──
		if (lv3Crouching && playerAlive) {
			float energyWX = (float)(bridges3[4].maxX + ENERGY_X_OFF);
			float energyWY = (float)(bridges3[4].snapY + ENERGY_Y_OFF);

			// Energy hitbox (world coords)
			int eX1 = (int)energyWX + (int)(ENERGY_W * ENERGY_HB_LEFT);
			int eX2 = (int)energyWX + (int)(ENERGY_W * ENERGY_HB_RIGHT);
			int eY1 = (int)energyWY;
			int eY2 = (int)energyWY + ENERGY_H;

			// Player hitbox — raw crouch sprite box, direction-independent
			int sprX = lv3CharX - (CROUCH_W - PLAYER_WIDTH) / 2;
			int sprY = lv3CharY + CROUCH_Y_OFF;
			int pX1 = sprX;
			int pX2 = sprX + CROUCH_W;
			int pY1 = sprY;
			int pY2 = sprY + CROUCH_H;

			bool oX = !(pX2 < eX1 || pX1 > eX2);
			bool oY = !(pY2 < eY1 || pY1 > eY2);

			if (oX && oY) {
				playerLives = 12;
				barrelState = 5;   // consumed — vanish
			}
		}
		return;
	}

	// ── States 0-2: wait for normal punch, advance state ─────────
	// State 3 (bar4): auto-convert to energy immediately, no punch needed
	if (barrelState == 3) {
		barrelState = 4;
		energyFrameIndex = 0;
		energyAnimTick = 0;
		return;
	}

	// ── States 0-2: check for normal punch ───────────────────────
	if (!playerAlive) return;

	int eX1 = (int)barrelWorldX + (int)(BARREL_W * BARREL_HB_LEFT);
	int eX2 = (int)barrelWorldX + (int)(BARREL_W * BARREL_HB_RIGHT);
	int eY1 = (int)barrelWorldY + (int)(BARREL_H * BARREL_HB_TOP);
	int eY2 = (int)barrelWorldY + (int)(BARREL_H * BARREL_HB_BOTTOM);

	bool normalPunchActive = (punchState == 2);

	if (normalPunchActive && !barrelWasPunching) {
		int ppX1 = lv3PHB.x1;
		int ppX2 = lv3PHB.x2;
		int ppY1 = lv3PHB.y1;
		int ppY2 = lv3PHB.y2;

		bool oX = !(ppX2 < eX1 || ppX1 > eX2);
		bool oY = !(ppY2 < eY1 || ppY1 > eY2);

		if (oX && oY) {
			barrelState++;   // 0→1, 1→2, 2→3
			// state 3 will auto-convert to energy on next tick (handled above)
		}
	}
	barrelWasPunching = normalPunchActive;
}

void updateLv3Barrel2() {
	if (currentBg != 12) return;
	if (barrel2State >= 5) return;

	// ── ENERGY ANIMATION (state 4) ──────────────────────────────
	if (barrel2State == 4) {
		if (++energy2AnimTick >= BARREL2_ENERGY_SPEED) {
			energy2AnimTick = 0;
			energy2FrameIndex = (energy2FrameIndex + 1) % ENERGY_FRAMECOUNT;
		}

		// ── Crouch hitbox overlaps energy → restore full lives ──
		if (lv3Crouching && playerAlive) {
			float e2WX = (float)(bridges3[8].maxX + ENERGY2_X_OFF);
			float e2WY = (float)(bridges3[8].snapY + ENERGY2_Y_OFF);

			int eX1 = (int)e2WX + (int)(ENERGY2_W * ENERGY2_HB_LEFT);
			int eX2 = (int)e2WX + (int)(ENERGY2_W * ENERGY2_HB_RIGHT);
			int eY1 = (int)e2WY;
			int eY2 = (int)e2WY + ENERGY2_H;

			int sprX = lv3CharX - (CROUCH_W - PLAYER_WIDTH) / 2;
			int sprY = lv3CharY + CROUCH_Y_OFF;
			int pX1 = sprX;
			int pX2 = sprX + CROUCH_W;
			int pY1 = sprY;
			int pY2 = sprY + CROUCH_H;

			bool oX = !(pX2 < eX1 || pX1 > eX2);
			bool oY = !(pY2 < eY1 || pY1 > eY2);

			if (oX && oY) {
				playerLives = 12;
				barrel2State = 5;
			}
		}
		return;
	}

	if (barrel2State == 3) {
		barrel2State = 4;
		energy2FrameIndex = 0;
		energy2AnimTick = 0;
		return;
	}

	if (!playerAlive) return;

	int eX1 = (int)barrel2WorldX + (int)(BARREL2_W * BARREL2_HB_LEFT);
	int eX2 = (int)barrel2WorldX + (int)(BARREL2_W * BARREL2_HB_RIGHT);
	int eY1 = (int)barrel2WorldY + (int)(BARREL2_H * BARREL2_HB_TOP);
	int eY2 = (int)barrel2WorldY + (int)(BARREL2_H * BARREL2_HB_BOTTOM);

	bool normalPunchActive = (punchState == 2);

	if (normalPunchActive && !barrel2WasPunching) {
		int ppX1 = lv3PHB.x1;
		int ppX2 = lv3PHB.x2;
		int ppY1 = lv3PHB.y1;
		int ppY2 = lv3PHB.y2;

		bool oX = !(ppX2 < eX1 || ppX1 > eX2);
		bool oY = !(ppY2 < eY1 || ppY1 > eY2);

		if (oX && oY) {
			barrel2State++;
		}
	}
	barrel2WasPunching = normalPunchActive;
}

void updateLevelSelectHitboxes() {
	int w = SCREEN_WIDTH;
	int panelW = w / 3;

	lvlHB[0] = { panelW * 0 + panelW / 2 - 115, 80, 230, 700 };
	lvlHB[1] = { panelW * 1 + panelW / 2 - 115, 80, 230, 700 };
	lvlHB[2] = { panelW * 2 + panelW / 2 - 115, 80, 230, 700 };
}

void initBackground() {
	updateLevelSelectHitboxes();

	bg1 = iLoadImage("Images/bg1.jpg");
	bg2 = iLoadImage("Images/bg2.jpg");
	bg41 = iLoadImage("Images/bg4.1.png");
	instruction2Img = iLoadImage("Images/instruction2.png");
	instruction3Img = iLoadImage("Images/instruction3.png");
	lv3Img = iLoadImage("Images/lv3.jpg");
	bg51 = iLoadImage("Images/bag5.1/bg5.1.jpg");
	bg6 = iLoadImage("Images/bg6.png");
	bg7 = iLoadImage("Images/bg7.png");

	// Story / instruction slides
	storyImgs[0] = iLoadImage("Images/story-1.png");
	storyImgs[1] = iLoadImage("Images/story-2.png");
	storyImgs[2] = iLoadImage("Images/story-3.png");
	storyImgs[3] = iLoadImage("Images/story-4.png");
	storyImgs[4] = iLoadImage("Images/story-5.png");
	storyImgs[5] = iLoadImage("Images/story-6.png");
	storyImgs[6] = iLoadImage("Images/instruction.png");
	gameOverImg = iLoadImage("Images/gameover.png");
	signpostImg = iLoadImage("Images/dangerpost.png");
	winFlagImg = iLoadImage("Images/win.png");
	winnerImg = iLoadImage("Images/winner.png");
	hudLifeImg = iLoadImage("Images/token/life.png");
	hudPunchImg = iLoadImage("Images/token/charged punch.png");
	tokenImg = iLoadImage("Images/token/token.png");
	treasureImg = iLoadImage("Images/token/treasure.png");
	// Player images
	playerSide = iLoadImage("Images/side.png");
	playerSideLeft = iLoadImage("Images/sideleft.png");
	playerSideM = iLoadImage("Images/sidem.png");
	playerSideLeftM = iLoadImage("Images/sideleftm.png");

	punchFistImg = iLoadImage("Images/fist.png");
	punchHitImg = iLoadImage("Images/punchf.png");
	punchFistImgM = iLoadImage("Images/fistm.png");
	punchHitImgM = iLoadImage("Images/punchff.png");

	chargedPunchImg = iLoadImage("Images/chargedfront.png");
	chargeScrollImg = iLoadImage("Images/chargescroll.png");

	chargedFistImg = iLoadImage("Images/chargedfist.png");
	chargedPunchImgSeq = iLoadImage("Images/chargedpunch.png");

	chargedPunchImgM = iLoadImage("Images/chargedfrontm.png");
	chargedFistImgM = iLoadImage("Images/chargedfistm.png");
	chargedPunchImgSeqM = iLoadImage("Images/chargedpunchm.png");

	playerDeadImg = iLoadImage("Images/player_dead.png");
	kneelImg = iLoadImage("Images/kneel.png");
	crouchImg = iLoadImage("Images/crouch.png");
	crouchImgM = iLoadImage("Images/crouchm.png");

	loadButtons(); // calling

	enemy1Img = iLoadImage("Images/enemy1.png");
	enemy1DeadImg = iLoadImage("Images/enemy1_dead.png");

	enemy2LeftImg = iLoadImage("Images/enemy2_left.png");
	enemy2RightImg = iLoadImage("Images/enemy2_right.png");
	enemy2DeadImg = iLoadImage("Images/enemy2_dead.png");

	enemy3IdleImg = iLoadImage("Images/enemy3.png");
	enemy3WalkImg = iLoadImage("Images/enemy3_walk.png");
	enemy3FistImg = iLoadImage("Images/enemy3_fist.png");
	enemy3PunchImg = iLoadImage("Images/enemy3_punch.png");
	enemy3IdleImgM = iLoadImage("Images/enemy3m.png");
	enemy3WalkImgM = iLoadImage("Images/enemy3_walkm.png");
	enemy3FistImgM = iLoadImage("Images/enemy3_fistm.png");
	enemy3PunchImgM = iLoadImage("Images/enemy3_punchm.png");
	enemy3DeadImg = iLoadImage("Images/enemy3_dead.png");

	enemy4Img = iLoadImage("Images/enemy4.png");
	enemy4DeadImg = iLoadImage("Images/enemy4_dead.png");
	lv3BossDeadImg = iLoadImage("Images/lv3_bossdead.png");
	winnerLv3Img = iLoadImage("Images/winner_lv3.png");
	finalWinImg = iLoadImage("Images/finalwin.png");
	// -------- LOAD DRAGON FRAMES --------
	char dpath[64];
	for (int i = 0; i < DRAGON_FRAME_COUNT; i++) {
		// dr1.png ... dr12.png
		sprintf_s(dpath, "Images/dragon/dr%d.png", i + 1);
		dragonFrames[i] = iLoadImage(dpath);
		dragonDeadImg = iLoadImage("Images/dragon/dr_dead.png");
	}
	// Load Boss1 frames
	char bpath[64];
	for (int i = 0; i < 41; i++) {
		sprintf_s(bpath, "Images/boss1/bs%d.png", i + 1);
		boss1Frames[i] = iLoadImage(bpath);
	}
	// Load shield frames
	char spath[64];
	for (int i = 0; i < 14; i++) {
		sprintf_s(spath, "Images/inf_shield/sh%d.png", i + 1);
		shieldFrames[i] = iLoadImage(spath);
	}
	boss1DeadImg = iLoadImage("Images/boss1/boss1dead.png");
	tokenImgLv2 = iLoadImage("Images/token/token lvl 2.png");

	// ---- Load lvl2enemy1 frames (right and left) ----
	char e1path[128];
	for (int i = 0; i < LV2E1FRAMECOUNT; i++) {
		// Right-facing: en1.1.gif .. en1.9.gif
		sprintf_s(e1path, "Images/lvl2enemy1/en1.%d.png", i + 1);
		lv2e1FramesRight[i] = iLoadImage(e1path);

		// Left-facing: en1.1o.gif .. en1.9o.gif
		sprintf_s(e1path, "Images/lvl2enemy1/en1.%do.png", i + 1);
		lv2e1FramesLeft[i] = iLoadImage(e1path);
	}
	lv2e1X = (float)(bridges2[2].maxX - LV2E1WIDTH);
	lv2e1DeadY = (float)(bridges2[2].snapY + LV2E1YOFFSET);
	lv2e1DeadImg = iLoadImage("Images/deadlvl2enemy1.png");

	// ---- Load lvl2enemy2 frames (right and left) ----
	char e2path[128];

	for (int i = 0; i < LV2E2FRAMECOUNT; i++) {
		// Going LEFT: en2.1.png .. en2.49.png
		sprintf_s(e2path, "Images/lvl2enemy2/en2.%d.png", i + 1);
		lv2e2FramesLeft[i] = iLoadImage(e2path);

		// Going RIGHT: en2.1o.png .. en2.49o.png
		sprintf_s(e2path, "Images/lvl2enemy2/en2.%do.png", i + 1);
		lv2e2FramesRight[i] = iLoadImage(e2path);
	}

	// Initial position over Bridge 5 (bridges2[4])
	lv2e2X = (float)(bridges2[LV2E2BRIDGEINDEX].minX);
	lv2e2DeadY = (float)(bridges2[LV2E2BRIDGEINDEX].snapY + LV2E2YOFFSET);
	lv2e2MovingRight = true; // start moving right across bridge 5
	lv2e2FrameIndex = 0;
	lv2e2DeadImg = iLoadImage("Images/deadlvl2enemy2.png");

	// ---- Load lvl2wkenemy frames ----
	char wkpath[128];
	for (int i = 0; i < LV2WK_FRAMECOUNT; i++) {
		sprintf_s(wkpath, "Images/lvl2wkenemy/wken%d.png", i + 1);
		lv2wkFrames[i] = iLoadImage(wkpath);
	}
	// Position: left edge of bridge 3 + adjustable offsets
	lv2wkX = (float)(bridges2[2].minX + LV2WK_XOFFSET);
	lv2wkY = (float)(bridges2[2].snapY + LV2WK_YOFFSET);

	// ---- Load lvl2wkenemy2 frames ----
	char wk2path[128];
	for (int i = 0; i < LV2WK2_FRAMECOUNT; i++) {
		sprintf_s(wk2path, "Images/lvl2wkenemy2/%d.png", i + 1);
		lv2wk2Frames[i] = iLoadImage(wk2path);
	}
	// Position: right edge of bridge 3 - enemy width + adjustable offsets
	lv2wk2X = (float)(bridges2[2].maxX - LV2WK2_WIDTH + LV2WK2_XOFFSET);
	lv2wk2Y = (float)(bridges2[2].snapY + LV2WK2_YOFFSET);

	// ---- Load lvl2wkenemy3 frames ----
	char wk3path[128];
	for (int i = 0; i < LV2WK3_FRAMECOUNT; i++) {
		sprintf_s(wk3path, "Images/lvl2wkenemy/wken%d.png", i + 1);
		lv2wk3Frames[i] = iLoadImage(wk3path);
	}
	// Position: right edge of bridge 5
	lv2wk3X = (float)(bridges2[4].maxX - LV2WK3_WIDTH + LV2WK3_XOFFSET);
	lv2wk3Y = (float)(bridges2[4].snapY + LV2WK3_YOFFSET);

	// ---- Load fire frames ----
	char fpath[128];
	for (int i = 0; i < FIRE_FRAMECOUNT; i++) {
		sprintf_s(fpath, "Images/fire/f%d.png", i + 1);
		fireFrames[i] = iLoadImage(fpath);
	}
	// ---- Load Level 3 spike frames ----
	char sppath[128];
	for (int i = 0; i < SPIKE_FRAMECOUNT; i++) {
		sprintf_s(sppath, "Images/spike/sp%d.png", i + 1);
		spikeFrames[i] = iLoadImage(sppath);
	}
	spikesLv3[0] = { (float)(bridges3[3].minX + SPIKE0_X_OFF), (float)(bridges3[3].snapY + SPIKE_Y_OFF), false, false, 0 };
	spikesLv3[1] = { (float)(bridges3[3].maxX - SPIKE_W + SPIKE1_X_OFF), (float)(bridges3[3].snapY + SPIKE_Y_OFF), false, false, 0 };
	spikesLv3[2] = { (float)(bridges3[7].minX + SPIKE2_X_OFF - 10), (float)(bridges3[7].snapY + SPIKE_Y_OFF), false, false, 0 };
	spikesLv3[3] = { (float)(bridges3[7].maxX - SPIKE_W + SPIKE3_X_OFF), (float)(bridges3[7].snapY + SPIKE_Y_OFF), false, false, 0 };

	// ---- Load Level 3 bird enemy frames ----
	char lv3bpath2[128];
	for (int i = 0; i < LV3BIRD_FRAMECOUNT; i++) {
		sprintf_s(lv3bpath2, "Images/lvl3b/lvl3b%d.png", i + 1);
		lv3BirdFrames[i] = iLoadImage(lv3bpath2);
	}
	lv3BirdDeadImg = iLoadImage("Images/lvl3bdead.png");
	// Start bird at right edge of Bridge 2, flying left
	lv3BirdX = (float)((bridges3[1].minX + bridges3[1].maxX) / 2);

	// ---- Load Level 3 power-up token frames ----
	char tkpath[128];
	for (int i = 0; i < PUPTK_FRAMECOUNT; i++) {
		sprintf_s(tkpath, "Images/poweruptoken/tk%d.png", i + 1);
		puptkFrames[i] = iLoadImage(tkpath);
	}
	// Token 1: middle of spike0 and spike1 (Bridge 4, bridges3[3])
	puptkLv3[0] = {
		(spikesLv3[0].x + spikesLv3[1].x + SPIKE_W) / 2.0f - PUPTK_W / 2.0f,
		(float)(bridges3[3].snapY + PUPTK_Y_OFF),
		false
	};
	// Token 2: middle of spike2 and spike3 (Bridge 8, bridges3[7])
	puptkLv3[1] = {
		(spikesLv3[2].x + spikesLv3[3].x + SPIKE_W) / 2.0f - PUPTK_W / 2.0f,
		(float)(bridges3[7].snapY + PUPTK_Y_OFF),
		false
	};
	// ---- Load Level 3 shield frames ----
	for (int i = 0; i < 8; i++) {
		char slpath[128];
		sprintf_s(slpath, "Images/shldload/shl%d.png", i + 1);
		shieldLoadFrames[i] = iLoadImage(slpath);
	}
	for (int i = 0; i < 11; i++) {
		char sapath[128];
		sprintf_s(sapath, "Images/shld/shield%d.png", i + 1);
		shieldAuraFrames[i] = iLoadImage(sapath);
	}
	for (int i = 0; i < 11; i++) {
		char sampath[128];
		sprintf_s(sampath, "Images/shldm/shield%dm.png", i + 1);
		shieldAuraFramesM[i] = iLoadImage(sampath);
	}
	// ---- Load Level 3 charged punch power-up frames ----
	char chppath[128];
	for (int i = 0; i < 8; i++) {
		sprintf_s(chppath, "Images/chargepowerup/chp%d.gif", i + 1);
		chpFrames[i] = iLoadImage(chppath);
	}
	for (int i = 0; i < 8; i++) {
		sprintf_s(chppath, "Images/chargepowerupm/chp%dm.png", i + 1);
		chpFramesM[i] = iLoadImage(chppath);
	}
	// ---- Level 3 Boss frames ----
	char lv3bpath[128];
	for (int i = 0; i < LV3BOSS_WALK_FRAMES; i++) {
		sprintf_s(lv3bpath, "Images/boss_lv3_walk/walk%d.png", i + 1);
		lv3BossWalkFrames[i] = iLoadImage(lv3bpath);
	}
	for (int i = 0; i < LV3BOSS_JUMP_FRAMES; i++) {
		sprintf_s(lv3bpath, "Images/boss_lv3_jump/jump%d.png", i + 1);
		lv3BossJumpFrames[i] = iLoadImage(lv3bpath);
	}
	for (int i = 13; i < 64; i++) {
		sprintf_s(lv3bpath, "Images/boss_lv3_fire/firee%d.png", i + 1);
		lv3BossFireFrames[i] = iLoadImage(lv3bpath);
	}
	for (int i = 0; i < LV3BOSS_SLASH_FRAMES; i++) {
		sprintf_s(lv3bpath, "Images/boss_lv3_slash/slash%d.png", i + 1);
		lv3BossSlashFrames[i] = iLoadImage(lv3bpath);
	}
	// Boss starts at right end of bridge 10
	lv3BossX = (float)(bridges3[10].maxX - LV3BOSS_W - 60);

	// ---- Load Level 3 Enemy 1 frames ----
	char lv3e1path[128];
	for (int i = 0; i < LV3EN1_IDLE_FRAMES; i++) {
		sprintf_s(lv3e1path, "Images/lvl3en1idle/lvl3en1idle%dleft.png", i + 1);
		lv3En1IdleFrames[i] = iLoadImage(lv3e1path);
	}
	for (int i = 0; i < LV3EN1_WALK_FRAMES; i++) {
		sprintf_s(lv3e1path, "Images/lvl3en1walk/lvl3en1walk%dleft.png", i + 1);
		lv3En1WalkFrames[i] = iLoadImage(lv3e1path);
	}
	for (int i = 0; i < LV3EN1_HIT_FRAMES; i++) {
		sprintf_s(lv3e1path, "Images/lvl3en1hit/lvl3en1hit%dleft.png", i + 1);
		lv3En1HitFrames[i] = iLoadImage(lv3e1path);
	}
	for (int i = 0; i < LV3EN1_DATK_FRAMES; i++) {
		sprintf_s(lv3e1path, "Images/lvl3en1datk/lvl3en1datk%dleft.png", i + 1);
		lv3En1DatkFrames[i] = iLoadImage(lv3e1path);
	}
	lv3En1DeadImg = iLoadImage("Images/lvl3en1dead.png");

	// Start position: right edge of Bridge 2
	lv3En1X = (float)(bridges3[1].maxX - LV3EN1_W);

	// ---- Load Level 3 Enemy 2 frames ----
	char lv3e2path[128];
	for (int i = 0; i < LV3EN2_IDLE_FRAMES; i++) {
		sprintf_s(lv3e2path, "Images/lvl3en1idle/lvl3en1idle%dright.png", i + 1);
		lv3En2IdleFrames[i] = iLoadImage(lv3e2path);
	}
	for (int i = 0; i < LV3EN2_RUN_FRAMES; i++) {
		sprintf_s(lv3e2path, "Images/lvl3en1run/lvl3en1run%dright.png", i + 1);
		lv3En2RunFrames[i] = iLoadImage(lv3e2path);
	}
	for (int i = 0; i < LV3EN2_HIT_FRAMES; i++) {
		sprintf_s(lv3e2path, "Images/lvl3en1hit/lvl3en1hit%dright.png", i + 1);
		lv3En2HitFrames[i] = iLoadImage(lv3e2path);
	}
	for (int i = 0; i < LV3EN2_DATK_FRAMES; i++) {
		sprintf_s(lv3e2path, "Images/lvl3en1datk/lvl3en1datk%dright.png", i + 1);
		lv3En2DatkFrames[i] = iLoadImage(lv3e2path);
	}
	lv3En2DeadImg = iLoadImage("Images/lvl3en1deadm.png");
	// Start X is set at activation time (left edge of Bridge 2)
	lv3En2X = (float)(bridges3[1].minX);

	// ---- Load Level 3 Enemy 3 frames (reuses lvl3en1 sprite folders) ----
	char lv3e3path[128];
	for (int i = 0; i < LV3EN3_IDLE_FRAMES; i++) {
		sprintf_s(lv3e3path, "Images/lvl3en1idle/lvl3en1idle%dleft.png", i + 1);
		lv3En3IdleFrames[i] = iLoadImage(lv3e3path);
	}
	for (int i = 0; i < LV3EN3_WALK_FRAMES; i++) {
		sprintf_s(lv3e3path, "Images/lvl3en1walk/lvl3en1walk%dleft.png", i + 1);
		lv3En3WalkFrames[i] = iLoadImage(lv3e3path);
	}
	for (int i = 0; i < LV3EN3_HIT_FRAMES; i++) {
		sprintf_s(lv3e3path, "Images/lvl3en1hit/lvl3en1hit%dleft.png", i + 1);
		lv3En3HitFrames[i] = iLoadImage(lv3e3path);
	}
	for (int i = 0; i < LV3EN3_DATK_FRAMES; i++) {
		sprintf_s(lv3e3path, "Images/lvl3en1datk/lvl3en1datk%dleft.png", i + 1);
		lv3En3DatkFrames[i] = iLoadImage(lv3e3path);
	}
	lv3En3DeadImg = iLoadImage("Images/lvl3en1dead.png");

	// Start position: right edge of Bridge 6 (bridges3[5])
	lv3En3X = (float)(bridges3[5].maxX - LV3EN3_IDLE_W);

	char lv3e4path[128];
	for (int i = 0; i < LV3EN4_IDLE_FRAMES; i++) {
		sprintf_s(lv3e4path, "Images/lvl3en1idle/lvl3en1idle%dright.png", i + 1);
		lv3En4IdleFrames[i] = iLoadImage(lv3e4path);
	}
	for (int i = 0; i < LV3EN4_RUN_FRAMES; i++) {
		sprintf_s(lv3e4path, "Images/lvl3en1run/lvl3en1run%dright.png", i + 1);
		lv3En4RunFrames[i] = iLoadImage(lv3e4path);
	}
	for (int i = 0; i < LV3EN4_HIT_FRAMES; i++) {
		sprintf_s(lv3e4path, "Images/lvl3en1hit/lvl3en1hit%dright.png", i + 1);
		lv3En4HitFrames[i] = iLoadImage(lv3e4path);
	}
	for (int i = 0; i < LV3EN4_DATK_FRAMES; i++) {
		sprintf_s(lv3e4path, "Images/lvl3en1datk/lvl3en1datk%dright.png", i + 1);
		lv3En4DatkFrames[i] = iLoadImage(lv3e4path);
	}
	lv3En4DeadImg = iLoadImage("Images/lvl3en1deadm.png");
	lv3En4X = (float)(bridges3[5].minX);

	// ── Level 3 WK Enemy pair ────────────────────────────────────────────────
	char lv3wkpath[128];
	for (int i = 0; i < LV3WK_FRAMECOUNT; i++) {
		sprintf_s(lv3wkpath, "Images/lvl2wkenemy/wken%d.png", i + 1);
		lv3WkFrames[i] = iLoadImage(lv3wkpath);
	}
	lv3WkX = (float)(bridges3[5].minX + LV3WK0_X_OFF);
	lv3WkY = (float)(bridges3[5].snapY + LV3WK_Y_OFF);

	// ---- Load Level 3 Enemy 5 frames ----
	char lv3e5path[128];

	for (int i = 0; i < LV3EN5_APR_FRAMES; i++) {
		sprintf_s(lv3e5path, "Images/lvl3en5apr/lvl3en5apr%dleft.png", i + 1);
		lv3En5AprFrames[i] = iLoadImage(lv3e5path);
	}
	for (int i = 0; i < LV3EN5_IDLE_FRAMES; i++) {
		sprintf_s(lv3e5path, "Images/lvl3en5idle/lvl3en5idle%dleft.png", i + 1);
		lv3En5IdleFrames[i] = iLoadImage(lv3e5path);
	}
	for (int i = 0; i < LV3EN5_RUN_FRAMES; i++) {
		sprintf_s(lv3e5path, "Images/lvl3en5run/lvl3en5run%dleft.png", i + 1);
		lv3En5RunFrames[i] = iLoadImage(lv3e5path);
	}
	for (int i = 0; i < LV3EN5_PUNCH_FRAMES; i++) {
		sprintf_s(lv3e5path, "Images/lvl3en5punch/lvl3en5punch%dleft.png", i + 1);
		lv3En5PunchFrames[i] = iLoadImage(lv3e5path);
	}
	for (int i = 0; i < LV3EN5_HIT_FRAMES; i++) {
		sprintf_s(lv3e5path, "Images/lvl3en5hit/lvl3en5hit%dleft.png", i + 1);
		lv3En5HitFrames[i] = iLoadImage(lv3e5path);
	}
	for (int i = 0; i < LV3EN5_REVIVE_FRAMES; i++) {
		sprintf_s(lv3e5path, "Images/lvl3en5revive/lvl3en5revive%dleft.png", i + 1);
		lv3En5ReviveFrames[i] = iLoadImage(lv3e5path);
	}
	for (int i = 0; i < LV3EN5_DEAD_FRAMES; i++) {
		sprintf_s(lv3e5path, "Images/lvl3en5dead/lvl3en5dead%dleft.png", i + 1);
		lv3En5DeadFrames[i] = iLoadImage(lv3e5path);
	}

	lv3En5X = (float)(bridges3[5].maxX + LV3EN5_LAND_X_OFF);
	lv3En5Y = (float)(800);

	// ── Enemy 6 frames (right-facing sprites) ───────────────
	char lv3e6path[128];
	for (int i = 0; i < LV3EN6_APR_FRAMES; i++) {
		sprintf_s(lv3e6path, "Images/lvl3en5apr/lvl3en5apr%dright.png", i + 1);
		lv3En6AprFrames[i] = iLoadImage(lv3e6path);
	}
	for (int i = 0; i < LV3EN6_IDLE_FRAMES; i++) {
		sprintf_s(lv3e6path, "Images/lvl3en5idle/lvl3en5idle%dright.png", i + 1);
		lv3En6IdleFrames[i] = iLoadImage(lv3e6path);
	}
	for (int i = 0; i < LV3EN6_RUN_FRAMES; i++) {
		sprintf_s(lv3e6path, "Images/lvl3en5run/lvl3en5run%dright.png", i + 1);
		lv3En6RunFrames[i] = iLoadImage(lv3e6path);
	}
	for (int i = 0; i < LV3EN6_PUNCH_FRAMES; i++) {
		sprintf_s(lv3e6path, "Images/lvl3en5punch/lvl3en5punch%dright.png", i + 1);
		lv3En6PunchFrames[i] = iLoadImage(lv3e6path);
	}
	for (int i = 0; i < LV3EN6_HIT_FRAMES; i++) {
		sprintf_s(lv3e6path, "Images/lvl3en5hit/lvl3en5hit%dright.png", i + 1);
		lv3En6HitFrames[i] = iLoadImage(lv3e6path);
	}
	for (int i = 0; i < LV3EN6_REVIVE_FRAMES; i++) {
		sprintf_s(lv3e6path, "Images/lvl3en5revive/lvl3en5revive%dright.png", i + 1);
		lv3En6ReviveFrames[i] = iLoadImage(lv3e6path);
	}
	for (int i = 0; i < LV3EN6_DEAD_FRAMES; i++) {
		sprintf_s(lv3e6path, "Images/lvl3en5dead/lvl3en5dead%dright.png", i + 1);
		lv3En6DeadFrames[i] = iLoadImage(lv3e6path);
	}
	lv3En6X = (float)(bridges3[5].maxX + LV3EN6_LAND_X_OFF);
	lv3En6Y = (float)(800);

	// ---- Load Level 3 tree frames ----
	char trpath[128];
	for (int i = 0; i < TREE_FRAMECOUNT; i++) {
		sprintf_s(trpath, "Images/tree/t%d.png", i + 1);
		treeFrames[i] = iLoadImage(trpath);
	}

	// Tree positions — midpoint between each bridge pair + X offset
	int treeMidX[NUM_TREES_LV3] = {
		(bridges3[0].maxX + bridges3[1].minX) / 2,   // B1-B2
		(bridges3[1].maxX + bridges3[2].minX) / 2,   // B2-B3
		(bridges3[4].maxX + bridges3[5].minX) / 2,   // B5-B6
		(bridges3[5].maxX + bridges3[6].minX) / 2,   // B6-B7
		(bridges3[8].maxX + bridges3[9].minX) / 2,   // B9-B10
		(bridges3[9].maxX + bridges3[10].minX) / 2   // B10-B11
	};
	int treeXOff[NUM_TREES_LV3] = { TREE0_X_OFF, TREE1_X_OFF, TREE2_X_OFF,
		TREE3_X_OFF, TREE4_X_OFF, TREE5_X_OFF };
	int treeY[NUM_TREES_LV3] = { TREE0_Y, TREE1_Y, TREE2_Y,
		TREE3_Y, TREE4_Y, TREE5_Y };

	for (int i = 0; i < NUM_TREES_LV3; i++) {
		treesLv3[i] = {
			(float)(treeMidX[i] - TREE_W / 2 + treeXOff[i]),
			(float)treeY[i],
			false
		};
	}

	// ---- Load Level 3 PBird frames ----
	char pbpath[128];
	for (int i = 0; i < LV3PBIRD_FRAMECOUNT; i++) {
		sprintf_s(pbpath, "Images/pbird/pb%dm.png", i + 1);   // left→right
		lv3PBirdFramesRight[i] = iLoadImage(pbpath);

		sprintf_s(pbpath, "Images/pbird/pb%d.png", i + 1);    // right→left
		lv3PBirdFramesLeft[i] = iLoadImage(pbpath);
	}
	// Start at left edge of Bridge 3
	lv3PBirdX = (float)bridges3[2].minX;

	// ---- Load Level 3 PBird2 frames (Bridge 7) ----
	char pb2path[128];
	for (int i = 0; i < LV3PBIRD2_FRAMECOUNT; i++) {
		sprintf_s(pb2path, "Images/pbird/pb%dm.png", i + 1);
		lv3PBird2FramesRight[i] = iLoadImage(pb2path);

		sprintf_s(pb2path, "Images/pbird/pb%d.png", i + 1);
		lv3PBird2FramesLeft[i] = iLoadImage(pb2path);
	}
	lv3PBird2X = (float)bridges3[6].minX;

	// ---- Load Level 3 Barrel frames ----
	barrelFrames[0] = iLoadImage("Images/barrel/bar1.png");
	barrelFrames[1] = iLoadImage("Images/barrel/bar2.png");
	barrelFrames[2] = iLoadImage("Images/barrel/bar3.png");
	barrelFrames[3] = iLoadImage("Images/barrel/bar4.png");

	// ---- Load Level 3 Energy frames ----
	char enpath[128];
	for (int i = 0; i < ENERGY_FRAMECOUNT; i++) {
		sprintf_s(enpath, "Images/energy/b%d.png", i + 1);
		energyFrames[i] = iLoadImage(enpath);
	}

	// Barrel world position (set once, used in update and draw)
	barrelWorldX = (float)(bridges3[4].maxX + BARREL_X_OFF);
	barrelWorldY = (float)(bridges3[4].snapY + BARREL_Y_OFF);
	barrel2WorldX = (float)(bridges3[8].maxX + BARREL2_X_OFF);
	barrel2WorldY = (float)(bridges3[8].snapY + BARREL2_Y_OFF);

	// ── Level Select images ──────────────────────────────────────
	lvlSelectImg = iLoadImage("Images/lvl.png");
	lvlSelectImg1 = iLoadImage("Images/lvl1.png");
	lvlSelectImg2 = iLoadImage("Images/lvl2.png");
	lvlSelectImg3 = iLoadImage("Images/lvl3.jpg");

	initFires();
	initCollectiblesLv2();

	winnerLv2Img = iLoadImage("Images/winner_lv2.png");
	boss1X = (float)(bridges2[6].maxX - (PLAYER_WIDTH + 70) -
		50); // start at right end of bridge 7
	// Timers
	iSetTimer(16, masterTimerTick);
	addVTimer(16, updateJump);
	addVTimer(50, updateChargedTimer);
	addVTimer(220, updateDragon);
	addVTimer(60, updateEnemy1);
	addVTimer(40, updateEnemy3);
	addVTimer(40, updateChargedHitWindow);
	addVTimer(40, updateEnemy2);
	addVTimer(60, updateEnemy4);
	addVTimer(100, updateGameOverDelay);
	addVTimer(40, checkWinFlagCollision);
	addVTimer(16, updateLevel2);
	addVTimer(16, updateLv3Camera);
	addVTimer(40, updateLv2Enemy1); // movement + animation
	addVTimer(35, updateBoss1Anim);
	addVTimer(16, updateBoss1Movement);
	addVTimer(16, checkLv2Enemy1Hits); // punch & body collision
	addVTimer(40, updateLv2Enemy2);    // movement + animation
	addVTimer(16, checkLv2Enemy2Hits);
	addVTimer(16, updateLv2WkEnemy);
	addVTimer(16, updateLv2WkEnemy2);
	addVTimer(16, updateLv2WkEnemy3);
	addVTimer(16, updateKneelHold);
	addVTimer(16, updateLv2Fires);
	addVTimer(16, updateLv3Shield);
	addVTimer(16, updateLv3ChpAnim);
	addVTimer(40, updateLv3Boss);          // state-machine + animation
	addVTimer(16, checkLv3BossHits);       // collision: player ↔ boss
	addVTimer(40, checkWinFlagCollisionLv3);
	addVTimer(16, updateLv3Spikes);
	addVTimer(16, updateLv3PowerUpTokens);
	addVTimer(40, updateLv3Bird);
	addVTimer(40, updateLv3Bird2);
	addVTimer(16, updateLv3PlayerHitbox);
	addVTimer(16, updateLv3En1);
	addVTimer(16, updateLv3En2);
	addVTimer(16, updateLv3En3);
	addVTimer(16, updateLv3En4);
	addVTimer(16, updateLv3WkEnemies);
	addVTimer(16, updateLv3En5);
	addVTimer(16, updateLv3En6);
	addVTimer(16, updateLv3Trees);
	addVTimer(40, updateLv3PBird);
	addVTimer(40, updateLv3PBird2);
	addVTimer(16, updateLv3Barrel);
	addVTimer(16, updateLv3Barrel2);
	// Add 20 more freely with addVTimer(...)
	// -------- INIT ENEMY3 POSITION --------
	enemy3X = bridges[8].maxX + 50;

	// -------- START BACKGROUND MUSIC --------
	initCollectibles();
	playMusic();
	initPunchSound();
}

void respawnPlayer() {
	playerX = PLAYER_START_X_BG41;
	playerY = PLAYER_START_Y_BG41;
	cameraX = 0;

	playerAlive = true;
	gameStarted = true;

	isJumping = false;
	jumpVelocity = 0;

	punchState = 0;
	chargingPunch = false;
	chargedSequenceActive = false;

	killedByEnemy1 = false;
	killedByEnemy2 = false;
	killedByEnemy3 = false;
	killedByEnemy4 = false;
	killedByDragon = false;
	enemy2BodyCollisionCount = 0;
	enemy2WasColliding = false;
	if (killedByBoss1) {
		killedByBoss1 = false;
		lv2CharX = 100;
		lv2CharY = bridges2[0].snapY;
		lv2IsJumping = false;
		lv2JumpVelocity = 0;
		lv2JumpVelocity = 0;
		playerAlive = true;
		gameStarted = true;
		boss1HitCount = 0;
		boss1Dead = false;
		boss1Active = true;
		deathWaiting = false;
	}
}

void shuffleEnemySprites() {
	swapEnemySprites = (rand() % 2 == 0); // 50/50 chance each run
}

void lv2HitPlayer(int livesLost, bool byFire, float fireWorldX) {
	if (!playerAlive)
		return;
	if ((chargingPunch && chargeScrollDismissed) || chargedSequenceActive)
		return;

	// ── FIRE HIT: instant game over regardless of lives, no kneel ──────
	if (byFire) {
		killedByFire = true;
		fireDeathWorldX = fireWorldX;
		playerDeathY = 0;

		playerLives = 0; // drain all lives → game over
		showKneel = false;
		kneelHolding = false;
		kneelHoldCounter = 0;

		playerAlive = false;
		gameStarted = false;
		handlePlayerDeath();
		return;
	}

	// ── ENEMY / BOSS HIT: deduct life, show kneel, NO death unless lives=0 ──
	playerLives -= livesLost;
	if (playerLives < 0)
		playerLives = 0;

	if (playerLives > 0) {
		// Still alive — flash kneel sprite at current position, player stays put
		showKneel = true;
		kneelHolding = true;
		kneelHoldCounter = 0;
		// updateKneelHold() clears showKneel after KNEEL_HOLD_TICKS

	}
	else {
		// Out of lives — show dead body + trigger game over
		showKneel = false;
		kneelHolding = false;
		kneelHoldCounter = 0;

		playerAlive = false;
		gameStarted = false;
		handlePlayerDeath();
	}
}

void handlePlayerDeath() {
	if (!playerAlive && !deathWaiting && !gameOverTriggered) {
		deathWaiting = true;
		deathDelayCounter = 0;
	}
}

void updateGameOverDelay() {
	if (deathWaiting && !gameOverTriggered) {
		deathDelayCounter++;

		if (deathDelayCounter >= DEATH_DELAY_TIME) {

			// ── Level 1: original hardcoded logic ──────────────────
			if (currentBg != 10 && currentBg != 12) {
				int livesToLose = 1;
				if (killedByEnemy3 || killedByDragon)
					livesToLose = 2;
				playerLives -= livesToLose;
			}

			if (playerLives < 0)
				playerLives = 0;

			if (playerLives > 0) {
				deathWaiting = false;

				// ── Level 2 respawn ─────────────────────────────────
				if (currentBg == 10) {
					showKneel = false;
					kneelHolding = false;
					kneelHoldCounter = 0;
					lv2IsJumping = false;
					lv2JumpVelocity = 0;
					playerAlive = true;
					gameStarted = true;
					killedByFire = false;
					killedByLv2e1 = false;
					killedByLv2e2 = false;
					killedByLv2wk = false;
					killedByLv2wk2 = false;
					killedByLv2wk3 = false;
					if (killedByBoss1) {
						killedByBoss1 = false;
						lv2CharX = 100;
						lv2CharY = bridges2[0].snapY;
						boss1HitCount = 0;
						boss1Dead = false;
						boss1Active = true;
						kneelTimer = 0;
						boss1Moving = true;
						boss1X = (float)(bridges2[6].maxX - (PLAYER_WIDTH + 70) - 50);
					}
					else {
						lv2CharX = 100;
						lv2CharY = bridges2[0].snapY;
					}
				}
				// ── Level 1 respawn (unchanged) ─────────────────────
				else {
					if (killedByBoss1) {
						killedByBoss1 = false;
						lv2CharX = 100;
						lv2CharY = bridges2[0].snapY;
						showKneel = false;
						kneelHolding = false;
						kneelHoldCounter = 0;
						lv2IsJumping = false;
						lv2JumpVelocity = 0;
						playerAlive = true;
						boss1HitCount = 0;
						boss1Dead = false;
						boss1Active = true;
						showKneel = false;
						kneelTimer = 0;
						boss1Moving = true;
						boss1X = (float)(bridges2[6].maxX - (PLAYER_WIDTH + 70) - 50);
					}
					else {
						respawnPlayer();
					}
				}
			}
			else {
				gameOverTriggered = true;
				gameOverDelayCounter = 0;
			}
		}
	}

	if (gameOverTriggered && !gameOverScreen) {
		gameOverDelayCounter++;
		if (gameOverDelayCounter >= 15) {
			gameOverScreen = true;

			// remember which level we died on
			lastGameplayBg = currentBg;

			currentBg = 8; // show Game Over
		}
	}

}

/* -------------------- MAIN -------------------- */
int main() {
	srand(time(NULL));
	iInitialize(SCREEN_WIDTH, SCREEN_HEIGHT, "Menu + Game");
	initBackground();
	glutKeyboardUpFunc(keyboardUp);
	iStart();
	return 0;
}