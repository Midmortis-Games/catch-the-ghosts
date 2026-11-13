#pragma once

#include <assert.h>

#include "common.h"
#include "ghosts.c"

#define BUCKET_HITBOX_SIZE ((Vector2){ 157, 33 })
#define BUCKET_HITBOX_OFFSET ((Vector2){ -185, -80 })
#define BUCKET_HITBOX_COLOR (CLITERAL(Color){ 0, 228, 48, 255 / 2 })
#define SPLASH_SCORE_OFFSET ((Vector2){ -158, -248 })
#define SPLASH_HIT_FRAME_DELAY 9
#define PLAYER_SPRITE_OFFSET ((Vector2){ -225, -154 })
#define PLAYER_HITBOX_SIZE ((Vector2){ 349, 231 })
#define PLAYER_HITBOX_OFFSET ((Vector2){ -173, -47 })
#define PLAYER_HITBOX_COLOR (CLITERAL(Color){ 102, 191, 255, 255 / 2 })
#define PLAYER_Y_POSITION 600
#define PLAYER_MAX_HEALTH 3
#define PLAYER_MIN_HEALTH 0 /* dead when reached */
#define PLAYER_DEFAULT_POSITION ((Vector2){	   \
		.x = LOCAL_WIDTH / 2.0,	   \
		.y = PLAYER_Y_POSITION,	   \
	})

Texture2D playerTexture3;
Texture2D playerTexture2;
Texture2D playerTexture1;
Texture2D playerTexture0;

Texture2D splashScore1Texture;
Texture2D splashScore2Texture;
Texture2D splashScore3Texture;

struct Player {
	void (*died)(void);
	Vector2 position;
	unsigned int score;
	unsigned int health;
	unsigned short scoreAnimationTick;
	bool scoring;
} player = {
	.health = PLAYER_MAX_HEALTH,
	.position = PLAYER_DEFAULT_POSITION,
};

Sound catchGhost;

void drawSplashHit(void)
{
	if (!player.scoring) {
		return;
	}

	Texture2D splashTexture = { 0 };
	if (player.scoreAnimationTick < SPLASH_HIT_FRAME_DELAY) {
		splashTexture = splashScore1Texture;
	} else if (player.scoreAnimationTick < SPLASH_HIT_FRAME_DELAY * 2) {
		splashTexture = splashScore2Texture;
	} else if (player.scoreAnimationTick < SPLASH_HIT_FRAME_DELAY * 3) {
		splashTexture = splashScore3Texture;
	} else {
		player.scoring = false;
		player.scoreAnimationTick = 0;
		return;
	}

	DrawTexture(
		splashTexture,
		player.position.x + SPLASH_SCORE_OFFSET.x,
		player.position.y + SPLASH_SCORE_OFFSET.y,
		WHITE
	);
}

void drawPlayer(void)
{
	drawSplashHit();

	Texture2D playerTexture = { 0 };
	switch (player.health) {
	default:
	case 3:
		playerTexture = playerTexture3;
		break;
	case 2:
		playerTexture = playerTexture2;
		break;
	case 1:
		playerTexture = playerTexture1;
		break;
	case 0:
		playerTexture = playerTexture0;
		break;
	}

	DrawTexture(
		playerTexture,
		player.position.x + PLAYER_SPRITE_OFFSET.x,
		player.position.y + PLAYER_SPRITE_OFFSET.y,
		WHITE
	);

	if (showDebug) {
		DrawRectangle(
			player.position.x + PLAYER_HITBOX_OFFSET.x,
			player.position.y + PLAYER_HITBOX_OFFSET.y,
			PLAYER_HITBOX_SIZE.x,
			PLAYER_HITBOX_SIZE.y,
			PLAYER_HITBOX_COLOR
		);
		DrawRectangle(
			player.position.x + BUCKET_HITBOX_OFFSET.x,
			player.position.y + BUCKET_HITBOX_OFFSET.y,
			BUCKET_HITBOX_SIZE.x,
			BUCKET_HITBOX_SIZE.y,
			BUCKET_HITBOX_COLOR
		);
		DrawCircleV(
			player.position,
			DEBUG_POSITION_RADIUS,
			DEBUG_POSITION_COLOR
		);
	}
}

void updatePlayerLogic(float delta)
{
	(void)delta;

	player.position.y = PLAYER_Y_POSITION;
	player.position.x = GetMouseX();
	player.scoreAnimationTick += player.scoring;
}

void caughtGhost(void)
{
	player.score++;
	player.scoring = true;
	PlaySound(catchGhost);
}

void hitByGhost(void)
{
	player.health--;
	if (player.health <= PLAYER_MIN_HEALTH) {
		assert(player.died != NULL);
		player.died();
	}
	PlaySound(splash);
}

Rectangle getBucketCollisionBox(void)
{
	Vector2 final = Vector2Add(player.position, BUCKET_HITBOX_OFFSET);
	return (Rectangle){
		.x = final.x,
		.y = final.y,
		.width = BUCKET_HITBOX_SIZE.x,
		.height = BUCKET_HITBOX_SIZE.y,
	};
}

Rectangle getPlayerCollisionBox(void)
{
	Vector2 final = Vector2Add(player.position, PLAYER_HITBOX_OFFSET);
	return (Rectangle){
		.x = final.x,
		.y = final.y,
		.width = PLAYER_HITBOX_SIZE.x,
		.height = PLAYER_HITBOX_SIZE.y,
	};
}

void updatePhysicsBucket(void)
{
	Rectangle bucketCollisionBox = getBucketCollisionBox();
	struct CollisionTest ct = { .test = bucketCollisionBox };
	ghostMap(ghostTryCaught, &ct, sizeof(struct CollisionTest));
	if (ct.result) {
		/* Can only catch one ghost per frame */
		caughtGhost();
	}
}

void updatePhysicsPlayer(void)
{
	Rectangle playerCollisionBox = getPlayerCollisionBox();
	struct CollisionTest ct = { .test = playerCollisionBox };
	ghostMap(ghostTryHit, &ct, sizeof(struct CollisionTest));
	if (ct.result) {
		/* Can only get hit once per frame */
		hitByGhost();
	}
}

void playerReset(void)
{
	player.position = PLAYER_DEFAULT_POSITION;
	player.health = PLAYER_MAX_HEALTH;
	player.score = 0;
}
