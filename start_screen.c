#pragma once

#include <stdbool.h>

#include "common.h"
#include "raylib.h"

extern RenderTexture2D target;
bool inStartScreen = false;

void updateStartScreen()
{
	BeginTextureMode(target);
	{
		ClearBackground(BACKGROUND_COLOR);
	}
	EndTextureMode();
}

