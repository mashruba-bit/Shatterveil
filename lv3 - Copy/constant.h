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
// ── Level 3 Coins ──────────────────────────────────────────────
#define COIN_FRAMECOUNT   6
#define COIN_W            70     // display width  — tweak freely
#define COIN_H            60     // display height — tweak freely
#define COIN_Y_OFF        190     // height above ground/gap Y — tweak freely
#define COIN_SPACING      40     // horizontal spacing between coins — tweak freely
#define COIN_TEXT_DURATION 50    // ticks the "+1" text stays visible

#define NUM_COINS_LV3     45     // 6 gaps × 5 coins each
#define LVL_SELECT_DRAW_DEBUG  false// ← set false to hide debug boxes
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
#define LV3_BG_WIDTH  5100
#define LV3_BG_HEIGHT 800
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
// ── Boss Lv3 display size — tweak freely ────────────────────
#define LV3BOSS_W   310
#define LV3BOSS_H   440
#define LV3BOSS_YOFF  -10      // vertical offset above bridge snap-Y
#define LV3BOSS_LIVES_TO_KILL 5   // display lives (20 hits / 4 per life)
// ── Boss Lv3 fire-hazard hitbox (fraction of sprite) ─────────
// The "fire" region is the horizontal band the player must shield against.
// Tweak these 0-1 ratios to match the actual flame pixels in the sprite.
#define LV3BOSS_FIRE_HB_LEFT   -0.25f
#define LV3BOSS_FIRE_HB_RIGHT  0.50f
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
#define CHP_W  150   // width of overlay — tweak freely
#define CHP_H  160   // height of overlay — tweak freely
#define CHP_X_OFF  -30  // horizontal offset from player — tweak freely
#define CHP_Y_OFF  75  // vertical offset from player Y — tweak freely
#define CHP_THROW_DIST  200  // total distance thrown — tweak freely

#define CROUCH_W    290    // ← tweak freely
#define CROUCH_H    290    // ← tweak freely
#define CROUCH_Y_OFF  -40  // ← negative = move DOWN toward bridge

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

#define NUM_SPIKES_LV3 4
// ── Level 3 Power-up Tokens ──────────────────────────────────────
#define PUPTK_FRAMECOUNT  8
#define PUPTK_W  90     // display width  — tweak freely
#define PUPTK_H  120     // display height — tweak freely
#define PUPTK_Y_OFF  50 // height above bridge snapY — tweak freely
#define PUPTK_TEXT_DURATION 60  // ~1 second at 16ms ticks
#define NUM_PUPTK_LV3 2
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

// ── Dead body fade-out (Bridge 6) ─────────────────────────────────────────
#define LV3EN3_FADE_DURATION  150    // ← ticks before En3 body disappears — tweak freely
#define LV3EN4_FADE_DURATION  150    // ← ticks before En4 body disappears — tweak freely


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

#define KNEEL_HOLD_TICKS 35 // ← tweak: how long kneel stays visible

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

/* ─── Dead body position tweaks for Level 2 (adjust freely) ─── */
#define LV2_DEAD_BRIDGE_W (PLAYER_WIDTH + 40)
#define LV2_DEAD_BRIDGE_H (PLAYER_HEIGHT + 40)
#define LV2_DEAD_BRIDGE_YOFFSET -90 // ← shift up/down when lying on bridge
#define LV2_DEAD_BRIDGE_XOFFSET 0   // ← shift left/right

#define LV2_DEAD_FIRE_W (PLAYER_WIDTH + 40)
#define LV2_DEAD_FIRE_H (PLAYER_HEIGHT + 40)
#define LV2_DEAD_FIRE_YOFFSET -60 // ← shift up/down when lying on ground
#define LV2_DEAD_FIRE_XOFFSET 0   // ← shift left/right

// ── Bridge 5 coins (ON bridge5, before barrel) — 4 coins ─────
#define B5_COIN_START_X_OFF  -270  // ← tweak: start X from bridge5 RIGHT edge (more negative = further left / away from barrel)
#define B5_COIN_Y_OFF         60    // ← tweak: height above bridge snapY
#define B5_COIN_GAP           40    // ← tweak: spacing between coins

//bridge 9 coins
#define B9_COIN_START_X_OFF  -270   // ← tweak: start X from bridge9 RIGHT edge
#define B9_COIN_Y_OFF         60    // ← tweak: height above bridge snapY
#define B9_COIN_GAP           40    // ← tweak: spacing between coins

//bridge 10 coin
#define B10_COIN_X_OFF   80   // ← tweak: offset from bridge LEFT edge
#define B10_COIN_Y_OFF    70   // ← tweak: height above bridge snapY
#define B10_COIN_GAP      40   // ← tweak: spacing between coins

#endif