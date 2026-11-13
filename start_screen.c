#pragma once

#include <assert.h>
#include <stdbool.h>

#include "common.h"

#include "game_state.c"

Texture2D titleScreenTexture;

void pressStartGame(void)
{
	stateChange(STATE_GAMEPLAY);
}

void drawStartScreenUI(void)
{
	/* Text */
	char *titleText = "Catch the Ghosts!";
	char *playText = "[PLAY]";
	Vector2 titleSize = MeasureTextMidmortis(titleText, FONT_SIZE_TITLE);
	Vector2 playSize = MeasureTextMidmortis(playText, FONT_SIZE_SUBTITLE);
	int titleX = LOCAL_WIDTH / 2 - (int)titleSize.x / 2;
	int titleY = LOCAL_HEIGHT / 2 - (int)(titleSize.y / 1.5);
	int playX = LOCAL_WIDTH / 2 - (int)playSize.x / 2;
	int playY = LOCAL_HEIGHT / 2;
	DrawTextMidmortis(titleText, titleX, titleY, FONT_SIZE_TITLE, LIGHTGRAY);

	/* Button behavior */
	Rectangle playButton = {
		.x = playX,
		.y = playY,
		.width = playSize.x,
		.height = playSize.y,
	};
	Color playColor = { 0 };
        Vector2 position = GetMousePosition();
        bool isHovering = CheckCollisionPointRec(position, playButton);
	if (isHovering) {
		playColor = DARKGRAY;
		SetMouseCursor(MOUSE_CURSOR_POINTING_HAND);

		if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
			pressStartGame();
			return;
		}
	} else {
		playColor = LIGHTGRAY;
		SetMouseCursor(MOUSE_CURSOR_DEFAULT);
	}

	DrawTextMidmortis(playText, playX, playY, FONT_SIZE_SUBTITLE, playColor);
}

void stateEnterStartScreen(void) {}

void stateProcessStartScreen(void)
{
	assert(currentState == STATE_START_SCREEN);

	BeginTextureMode(target);
	{
		DrawTexture(titleScreenTexture, 0, 0, WHITE);
		drawStartScreenUI();
	}
	EndTextureMode();
}

void stateExitStartScreen(void)
{
	assert(currentState == STATE_START_SCREEN);

	SetMouseCursor(MOUSE_CURSOR_DEFAULT);
}
