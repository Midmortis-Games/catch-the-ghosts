#pragma once

#include <assert.h>

#include "common.h"

RenderTexture2D targetBackup;

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

		char *diedText = "You Died!";
		char *subText = "How does it feel to get ghosted?";
		Vector2 diedSize
			= MeasureTextMidmortis(diedText, FONT_SIZE_TITLE);
		int subSize = MeasureTextMidmortis(subText, FONT_SIZE_SUBTITLE).x;
		int diedX = LOCAL_WIDTH / 2 - (int)diedSize.x / 2;
		int diedY = LOCAL_HEIGHT / 2 - (int)(diedSize.y / 1.5);
		int subX = LOCAL_WIDTH / 2 - subSize / 2;
		int subY = LOCAL_HEIGHT / 2;
		DrawTextMidmortis(
			diedText,
			diedX,
			diedY,
			FONT_SIZE_TITLE,
			LIGHTGRAY
		);
		DrawTextMidmortis(
			subText,
			subX,
			subY,
			FONT_SIZE_SUBTITLE,
			LIGHTGRAY
		);
	}
	EndTextureMode();
}

void stateExitDeath(void)
{
	assert(currentState == STATE_DEATH);
}
