#ifndef TOKENSP1_H
#define TOKENSP1_H

#include "globals.h"
#include "iGraphics.h"

// External globals used in this header
extern Bridge bridges[];
extern Bridge bridges2[];
extern int playerX, playerY;
extern int lv2CharX, lv2CharY;
extern int currentBg;
extern bool playerAlive;

// Level 1 and 2 token variables
Collectible collectibles[15];
int numCollectibles = 15;
Collectible collectiblesLv2[6];
int numCollectiblesLv2 = 6;
int tokenCount = 0;
int treasureCount = 0;
int tokenCountLv2 = 0;
int tokenImg, treasureImg, tokenImgLv2;

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

#endif
