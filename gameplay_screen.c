#pragma once

#include <assert.h>
#include <stdio.h>

#include "common.h"

#include "game_state.c"
#include "player.c"
#include "ghosts.c"

#define FLOOR_HITBOX_SIZE (Vector2){ LOCAL_WIDTH, 10 }
#define FLOOR_HITBOX_POSITION (Vector2){ 0, LOCAL_HEIGHT }

void drawDebugUI(void)
{
	char fps[32] = { 0 };
	snprintf(fps, ARRAY_SIZE(fps), "%d FPS", GetFPS());
	DrawTextMidmortis(
		"Press D to show debug collisions",
		0,
		0,
		FONT_SIZE_DEBUG,
		LIGHTGRAY
	);
	DrawTextMidmortis(fps, 0, FONT_SIZE_DEBUG, FONT_SIZE_DEBUG, LIGHTGRAY);
}

void drawUI(void)
{
	if (DEBUG_BUILD) {
		drawDebugUI();
	}

	char scoreText[32] = { 0 };
	snprintf(scoreText, ARRAY_SIZE(scoreText), "Score: %u", player.score);
	int textSize = MeasureTextMidmortis(scoreText, FONT_SIZE_SCORE).x;
	int x = LOCAL_WIDTH / 2 - textSize / 2;
	int y = FONT_SIZE_SCORE + 10;
	DrawTextMidmortis(scoreText, x, y, FONT_SIZE_SCORE, LIGHTGRAY);
}

void drawGameplay(void)
{
	ClearBackground(BACKGROUND_COLOR);

	drawGhosts();
	drawPlayer();

	drawUI();
}

void gameplayDied(void)
{
	assert(currentState == STATE_GAMEPLAY);
	stateExitGameplay();
	stateEnterDeath();
}

struct CollisionBox getFloorCollisionBox(void)
{
	return (struct CollisionBox){
		.pos = FLOOR_HITBOX_POSITION,
		.width = FLOOR_HITBOX_SIZE.x,
		.height = FLOOR_HITBOX_SIZE.y
	};
}

void updatePhysicsFloor(void)
{
	struct CollisionBox floor = getFloorCollisionBox();
	struct CollisionTest ct = { .test = floor };
	/* Just kill the ghosts */
	ghostMap(ghostTouched, &ct, sizeof(struct CollisionTest));
}

void updatePhysics(float delta)
{
	(void)delta;

	updatePhysicsBucket();
	updatePhysicsPlayer();
	updatePhysicsFloor();
}

void updateLogic(void)
{
	float delta = GetFrameTime();
	assert(isfinite(delta));

	if (DEBUG_BUILD && IsKeyPressed(KEY_D)) {
		showDebug = !showDebug;
	}

	if (delta == 0.0f) {
		return;
	}

	updatePlayerLogic(delta);
	updateGhostLogic(delta);
	updatePhysics(delta);
}

void stateEnterGameplay(void)
{
	currentState = STATE_GAMEPLAY;
	player.died = gameplayDied;
}

void stateProcessGameplay(void)
{
	assert(currentState == STATE_GAMEPLAY);

	updateLogic();

	BeginTextureMode(target);
	{
		drawGameplay();
	}
	EndTextureMode();
}

void stateExitGameplay(void)
{
	assert(currentState == STATE_GAMEPLAY);
}
