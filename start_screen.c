#pragma once

#include <assert.h>
#include <stdbool.h>

#include "common.h"

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
	}
	EndTextureMode();
}

void stateExitStartScreen(void)
{
	assert(currentState == STATE_START_SCREEN);
}
