#pragma once

#include <assert.h>

#include "common.h"

#include "game_state.c"
#include "button.c"

Music deathMusic;
RenderTexture2D targetBackup;

void pressRestartGame(void)
{
	stateChange(STATE_GAMEPLAY);
}

void drawDeathScreenUI(void)
{
	char *diedText = "You Died!";
	char *subText = "How does it feel to get ghosted?";
	Vector2 diedSize = MeasureTextMidmortis(diedText, FONT_SIZE_TITLE);
	Vector2 playSize = (Vector2){
		.x = buttonOnTexture.width,
		.y = buttonOnTexture.height
	};
	int subSize = MeasureTextMidmortis(subText, FONT_SIZE_SUBTITLE).x;
	int diedX = LOCAL_WIDTH / 2 - (int)diedSize.x / 2;
	int diedY = LOCAL_HEIGHT / 3 - (int)(diedSize.y / 2);
	int subX = LOCAL_WIDTH / 2 - subSize / 2;
	int subY = LOCAL_HEIGHT / 2;
	int playX = LOCAL_WIDTH / 2;
	int playY = LOCAL_HEIGHT / 2 - (int)playSize.y / 2;
	DrawTextMidmortis(diedText, diedX, diedY, FONT_SIZE_TITLE, FONT_COLOR);
	DrawTextMidmortis(subText, subX, subY, FONT_SIZE_SUBTITLE, FONT_COLOR);

	drawButton((Vector2){ playX, playY }, pressRestartGame);
}

void stateEnterDeath(void)
{
	deathMusic.looping = false;
	PlayMusicStream(deathMusic);
	currentMusic = &deathMusic;

	BeginTextureMode(targetBackup);
	{
		DrawRenderTexture(target);
	}
	EndTextureMode();
}

void stateProcessDeath(void)
{
	assert(currentState == STATE_DEATH);

	UpdateMusicStream(deathMusic);

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
	StopMusicStream(deathMusic);
}
