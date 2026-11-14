#pragma once

#include <assert.h>
#include <stdio.h>

#include "common.h"

#include "game_state.c"
#include "player.c"
#include "ghosts.c"

#define FLOOR_HITBOX_SIZE ((Vector2){ LOCAL_WIDTH, 10 })
#define FLOOR_HITBOX_POSITION ((Vector2){ 0, LOCAL_HEIGHT })

Music gameplayMusic; /* Loops */
Music gameplayStartMusic; /* Doesn't loop */
Texture2D backgroundTexture;

void drawDebugGameplayUI(void)
{
	char fps[32] = { 0 };
	snprintf(fps, ARRAY_SIZE(fps), "%d FPS", GetFPS());
	DrawTextMidmortis(
		"Press D to show debug collisions",
		0,
		0,
		FONT_SIZE_DEBUG,
		FONT_COLOR
	);
	DrawTextMidmortis(fps, 0, FONT_SIZE_DEBUG, FONT_SIZE_DEBUG, FONT_COLOR);
}

void gameplayDrawUI(void)
{
	static int x = -1;

	if (DEBUG_BUILD) {
		drawDebugGameplayUI();
	}

	char scoreText[32] = { 0 };
	snprintf(scoreText, ARRAY_SIZE(scoreText), "Score: %u", player.score);
	int textSize = MeasureTextMidmortis(scoreText, FONT_SIZE_SCORE).x;
	if (x < 0) {
		x = LOCAL_WIDTH / 2 - textSize / 2;
		assert(x >= 0);
	}
	int y = FONT_SIZE_SCORE + 10;
	DrawTextMidmortis(scoreText, x, y, FONT_SIZE_SCORE, FONT_COLOR);
}

void gameplayDraw(void)
{
	DrawTexture(backgroundTexture, 0, 0, WHITE);

	drawGhosts();
	drawPlayer();

	gameplayDrawUI();
}

void gameplayDied(void)
{
	assert(currentState == STATE_GAMEPLAY);

	stateChange(STATE_DEATH);
}

Rectangle getFloorCollisionBox(void)
{
	return (Rectangle){
		.x = FLOOR_HITBOX_POSITION.x,
		.y = FLOOR_HITBOX_POSITION.y,
		.width = FLOOR_HITBOX_SIZE.x,
		.height = FLOOR_HITBOX_SIZE.y
	};
}

void updatePhysicsFloor(void)
{
	Rectangle floor = getFloorCollisionBox();
	struct CollisionTest ct = { .test = floor };
	ghostMap(ghostTryHit, &ct, sizeof(struct CollisionTest));
	if (ct.result) {
		PlaySound(splash);
	}
}

void updatePhysics(float delta)
{
	(void)delta;

	updatePhysicsBucket();
	updatePhysicsPlayer();
	updatePhysicsFloor();
}

void updateGameplayLogic(void)
{
	float delta = GetFrameTime();
	assert(isfinite(delta));

	if (delta > MAX_DELTA) {
		delta = MAX_DELTA;
	}

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
	timerReset(&ghostRespawnTimer);
	playerReset();
	player.died = gameplayDied;

	gameplayStartMusic.looping = false;
	gameplayMusic.looping = true;

	PlayMusicStream(gameplayStartMusic);
	currentMusic = &gameplayStartMusic;

	ghostPoolFreeAll();
}

void updateGameplayMusic(void)
{
	UpdateMusicStream(*currentMusic);

	if (currentMusic != &gameplayStartMusic) {
		return;
	}

	if (!IsMusicStreamPlaying(gameplayStartMusic)) {
		StopMusicStream(gameplayStartMusic);
		PlayMusicStream(gameplayMusic);
		currentMusic = &gameplayMusic;
	}
}

void stateProcessGameplay(void)
{
	assert(currentState == STATE_GAMEPLAY);

	updateGameplayLogic();
	updateGameplayMusic();

	BeginTextureMode(target);
	{
		gameplayDraw();
	}
	EndTextureMode();
}

void stateExitGameplay(void)
{
	assert(currentState == STATE_GAMEPLAY);

	StopMusicStream(*currentMusic);
}
