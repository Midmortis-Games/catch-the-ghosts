#pragma once

#include <assert.h>
#include <stdbool.h>

#include "common.h"

void drawStartScreenUI(void)
{
	char *titleText = "Catch the Ghosts!";
	char *playText = "[PLAY]";
	Vector2 titleSize = MeasureTextMidmortis(titleText, FONT_SIZE_TITLE);
	int playSize = MeasureTextMidmortis(playText, FONT_SIZE_SUBTITLE).x;
	int titleX = LOCAL_WIDTH / 2 - (int)titleSize.x / 2;
	int titleY = LOCAL_HEIGHT / 2 - (int)(titleSize.y / 1.5);
	int playX = LOCAL_WIDTH / 2 - playSize / 2;
	int playY = LOCAL_HEIGHT / 2;
	DrawTextMidmortis(titleText, titleX, titleY, FONT_SIZE_TITLE, LIGHTGRAY);
	DrawTextMidmortis(playText, playX, playY, FONT_SIZE_SUBTITLE, LIGHTGRAY);
}

void stateEnterStartScreen(void)
{
	currentState = STATE_START_SCREEN;
}

void stateProcessStartScreen(void)
{
	assert(currentState == STATE_START_SCREEN);

	BeginTextureMode(target);
	{
		ClearBackground(BACKGROUND_COLOR);
		drawStartScreenUI();
	}
	EndTextureMode();
}

void stateExitStartScreen(void)
{
	assert(currentState == STATE_START_SCREEN);
}
