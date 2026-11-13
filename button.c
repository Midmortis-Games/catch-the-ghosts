#pragma once

#include "common.h"

Texture2D buttonOnTexture;
Texture2D buttonOffTexture;

void drawButton(Vector2 centerPosition, void (*callback)(void))
{
	float x = centerPosition.x - buttonOnTexture.width / 2.0;
	float y = centerPosition.y - buttonOnTexture.height / 2.0;

	Rectangle button = {
		.x = x,
		.y = y,
		.width = buttonOnTexture.width,
		.height = buttonOnTexture.height,
	};

	Vector2 mousePosition = GetMousePosition();
	bool isHovering = CheckCollisionPointRec(mousePosition, button);
	Texture2D texture = { 0 };
	if (isHovering) {
		texture = buttonOnTexture;
		SetMouseCursor(MOUSE_CURSOR_POINTING_HAND);

		if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
			callback();
			return;
		}
	} else {
		texture = buttonOffTexture;
		SetMouseCursor(MOUSE_CURSOR_DEFAULT);
	}

	DrawTexture(texture, x, y, WHITE);
}
