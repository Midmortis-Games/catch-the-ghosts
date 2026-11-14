#pragma once

#include <assert.h>
#include <stdbool.h>

#include "common.h"

#include "game_state.c"
#include "button.c"

Music titleScreenMusic;
Texture2D titleScreenTexture;

void pressStartGame(void)
{
	stateChange(STATE_GAMEPLAY);
}

void drawStartScreenUI(void)
{
	int playX = LOCAL_WIDTH / 2;
	int playY = LOCAL_HEIGHT / 2;

	drawButton((Vector2){ playX, playY }, pressStartGame);
}

void stateEnterStartScreen(void)
{
	assert(currentMusic == NULL);
	PlayMusicStream(titleScreenMusic);
	currentMusic = &titleScreenMusic;
}

void stateProcessStartScreen(void)
{
	assert(currentState == STATE_START_SCREEN);

	UpdateMusicStream(titleScreenMusic);

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
	StopMusicStream(titleScreenMusic);
}
