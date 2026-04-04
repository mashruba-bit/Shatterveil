#ifndef HUD_H
#define HUD_H

#include "globals.h"
#include "iGraphics.h"
#include <cmath>
#include <cstdio>
#include <cstring>
#include <windows.h>

// External globals used in this header
extern int currentBg;
extern bool playerWonLv3, playerWon, playerWonLv2;
extern int playerLives;
extern int lv3CoinCount;
extern int chargedUseCountLv3, chargedUseCount;
extern int coinFrames[], coinAnimIndex;
extern bool chargingPunch;
extern int chargedTimeCounter;
extern int tokenCount, treasureCount, tokenCountLv2;
extern int tokenImg, treasureImg, tokenImgLv2;

// HUD variables
int hudLifeImg, hudPunchImg;

void drawHUDLv3() {
	if (currentBg != 12 || playerWonLv3)
		return;

	// --- Animation Timing & Pulse ---
	unsigned int t = GetTickCount();
	float pulse = 1.0f + 0.05f * cosf(t * 0.005f);
	float fastPulse = 1.0f + 0.12f * cosf(t * 0.012f);
	
	// --- Interaction "Pops" ---
	static int lastLC = -1, lastTC = -1, lastPC = -1;
	static int lifePop = 0, tokenPop = 0, punchPop = 0;
	if (lastLC == -1) { lastLC = playerLives; lastTC = lv3CoinCount; lastPC = chargedUseCountLv3; }
	if (playerLives != lastLC) { lifePop = 14; lastLC = playerLives; }
	if (lv3CoinCount != lastTC) { tokenPop = 14; lastTC = lv3CoinCount; }
	if (chargedUseCountLv3 != lastPC) { punchPop = 14; lastPC = chargedUseCountLv3; }
	if (lifePop > 0) lifePop--; if (tokenPop > 0) tokenPop--; if (punchPop > 0) punchPop--;

	int hudW = 300, hudH = 26, margin = 45, iconSize = 48;
	int startX = margin + iconSize - 10; 
	int startY = SCREEN_HEIGHT - 35 - hudH;

	// --- Helper to draw the Futuristic Gothic Blade Shape ---
	auto drawBladeBar = [&](int x, int y, int w, int h, float ratio, int r, int g, int b, bool isDanger) {
		// 1. Draw Frame (Sharp Blade Shadow)
		double fx[] = { (double)x - 14, (double)x, (double)x + w, (double)x + w + 18, (double)x + w, (double)x };
		double fy[] = { (double)y + h / 2.0, (double)y + h, (double)y + h, (double)y + h / 2.0, (double)y, (double)y };
		iSetColor(15, 15, 15); iFilledPolygon(fx, fy, 6);
		iSetColor(70, 70, 80); iPolygon(fx, fy, 6); // Metallic edge

		// 2. Draw Fill Progress
		if (ratio > 0) {
			int curW = (int)(w * ratio);
			if (isDanger && (t / 150) % 2 == 0) iSetColor(255, 60, 60); else iSetColor(r, g, b);
			
			// Fill shape follows the blade path
			double fillX[] = { (double)x - (ratio > 0.05 ? 10 : 0), (double)x, (double)x + curW, (double)x + curW + (ratio > 0.95 ? 14 : 5), (double)x + curW, (double)x };
			double fillY[] = { (double)y + h / 2.0, (double)y + h, (double)y + h, (double)y + h / 2.0, (double)y, (double)y };
			iFilledPolygon(fillX, fillY, 6);

			// Angular Highlight (Glass Reflection)
			iSetColor(r + 35, g + 35, b + 35);
			double glossY[] = { (double)y + h/2.0, (double)y + h, (double)y + h, (double)y + h/2.0, (double)y + h/2.0, (double)y + h/2.0 };
			iFilledPolygon(fillX, glossY, 4);

			// Moving Shimmer
			int shimX = x + (t / 4 % (w + 400)) - 200;
			if (shimX >= x - 10 && shimX + 30 <= x + curW + 10) {
				iSetColor(r + 65, g + 65, b + 65);
				iFilledRectangle(shimX, y, 25, h);
			}
		}
	};

	// 1. LIFE BAR
	drawBladeBar(startX, startY, hudW, hudH, (float)playerLives / 12.0f, 170, 0, 0, playerLives <= 3);
	float hpPulse = (playerLives <= 3) ? fastPulse : pulse;
	int curHSize = (int)(iconSize * hpPulse * (1.0f + 0.18f * (lifePop / 14.0f)));
	iShowImage(startX - iconSize - 8 - (curHSize-iconSize)/2, startY + (hudH-curHSize)/2, curHSize, curHSize, hudLifeImg);

	// 2. TOKEN BAR (Right Aligned)
	int tokenCountVal = lv3CoinCount, maxTokens = 45;
	int tStartX = SCREEN_WIDTH - margin - hudW - 10;
	drawBladeBar(tStartX, startY, hudW, hudH, (float)tokenCountVal / maxTokens, 210, 170, 0, false);
	int curTSize = (int)(iconSize * pulse * (1.0f + 0.25f * (tokenPop / 14.0f)));
	iShowImage(tStartX - iconSize - 12 - (curTSize-iconSize)/2, startY + (hudH-curTSize)/2, curTSize, curTSize, coinFrames[coinAnimIndex]);
	char tokenStr[20]; sprintf_s(tokenStr, "%d/%d", tokenCountVal, maxTokens);
	iSetColor(255, 255, 255); iText(tStartX + (hudW - (int)strlen(tokenStr) * 10) / 2, startY + (hudH - 18) / 2, tokenStr, GLUT_BITMAP_HELVETICA_18);

	// 3. CHARGED PUNCH BAR (Center)
	int remPunches = 4 - chargedUseCountLv3;
	int pStartX = (SCREEN_WIDTH - hudW) / 2, pStartY = startY - 25 - hudH;
	int bShade = 130 + ((remPunches >= 4) ? (int)(45 * sinf(t * 0.012f)) : 0);
	drawBladeBar(pStartX, pStartY, hudW, hudH, (float)remPunches / 6.0f, 0, 70, bShade, false);
	int curPSize = (int)(iconSize * pulse * (1.0f + 0.25f * (punchPop / 14.0f)));
	iShowImage(pStartX - iconSize - 12 - (curPSize-iconSize)/2, pStartY + (hudH-curPSize)/2, curPSize, curPSize, hudPunchImg);
	char punchStr[20]; sprintf_s(punchStr, "%d/%d", remPunches, 6);
	iSetColor(255, 255, 255); iText(pStartX + (hudW - (int)strlen(punchStr) * 10) / 2, pStartY + (hudH - 18) / 2, punchStr, GLUT_BITMAP_HELVETICA_18);

	// 4. TIMER BAR
	if (chargingPunch) {
		int timerH = 10, timerY = pStartY - 6 - timerH;
		drawBladeBar(pStartX, timerY, hudW, timerH, (float)(CHARGED_TIME_LIMIT - chargedTimeCounter) / CHARGED_TIME_LIMIT, 0, 200, 200, false);
	}
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

#endif
