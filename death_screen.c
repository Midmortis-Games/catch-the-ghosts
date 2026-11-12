#pragma once

#include <assert.h>

#include "common.h"

#include "game_state.c"

RenderTexture2D targetBackup;

void pressRestartGame(void)
{
	stateExitDeath();
	stateEnterGameplay();
}

void drawDeathScreenUI(void)
{
	char *diedText = "You Died!";
	char *subText = "How does it feel to get ghosted?";
	char *playText = "[TRY AGAIN?]";
	Vector2 diedSize = MeasureTextMidmortis(diedText, FONT_SIZE_TITLE);
	Vector2 playSize = MeasureTextMidmortis(playText, FONT_SIZE_SUBTITLE);
	int subSize = MeasureTextMidmortis(subText, FONT_SIZE_SUBTITLE).x;
	int diedX = LOCAL_WIDTH / 2 - (int)diedSize.x / 2;
	int diedY = LOCAL_HEIGHT / 2 - (int)(diedSize.y / 1.5);
	int subX = LOCAL_WIDTH / 2 - subSize / 2;
	int subY = LOCAL_HEIGHT / 2;
	int playX = LOCAL_WIDTH / 2 - (int)playSize.x / 2;
	int playY = LOCAL_HEIGHT / 2 + (int)playSize.y;
	DrawTextMidmortis(diedText, diedX, diedY, FONT_SIZE_TITLE, LIGHTGRAY);
	DrawTextMidmortis(subText, subX, subY, FONT_SIZE_SUBTITLE, LIGHTGRAY);

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
		playColor = GRAY;
		SetMouseCursor(MOUSE_CURSOR_POINTING_HAND);

		if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
			pressRestartGame();
			return;
		}
	} else {
		playColor = LIGHTGRAY;
		SetMouseCursor(MOUSE_CURSOR_DEFAULT);
	}

	DrawTextMidmortis(playText, playX, playY, FONT_SIZE_SUBTITLE, playColor);
}

void stateEnterDeath(void)
{
	currentState = STATE_DEATH;

	BeginTextureMode(targetBackup);
	{
		DrawRenderTexture(target);
	}
	EndTextureMode();
}

void stateProcessDeath(void)
{
	assert(currentState == STATE_DEATH);

	BeginTextureMode(target);
	{
		DrawRenderTexture(targetBackup);
		drawDeathScreenUI();
	}
	EndTextureMode();
}

void stateExitDeath(void)
{
	assert(currentState == STATE_DEATH);

	SetMouseCursor(MOUSE_CURSOR_DEFAULT);
}
