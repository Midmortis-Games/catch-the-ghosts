#pragma once

#include <assert.h>

#include "common.h"
#include "ghosts.c"

#define BUCKET_HITBOX_SIZE ((Vector2){ 157, 33 })
#define BUCKET_HITBOX_OFFSET ((Vector2){ -185, -118 })
#define BUCKET_HITBOX_COLOR GREEN
#define PLAYER_SPRITE_OFFSET ((Vector2){ -225, -154 })
#define PLAYER_HITBOX_SIZE ((Vector2){ 349, 231 })
#define PLAYER_HITBOX_OFFSET ((Vector2){ -173, -79 })
#define PLAYER_HITBOX_COLOR SKYBLUE
#define PLAYER_Y_POSITION 600
#define PLAYER_MAX_HEALTH 1
#define PLAYER_MIN_HEALTH 0 /* dead when reached */
#define PLAYER_DEFAULT_POSITION ((Vector2){	   \
		.x = LOCAL_WIDTH / 2.0,	   \
		.y = PLAYER_Y_POSITION,	   \
	})

struct Player {
	void (*died)(void);
	Vector2 position;
	unsigned int score;
	unsigned int health;
	Texture2D texture;
} player = {
	.health = PLAYER_MAX_HEALTH,
	.position = PLAYER_DEFAULT_POSITION,
};

Sound catchGhost;

void drawPlayer(void)
{
	DrawTexture(
		player.texture,
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
}

void caughtGhost(void)
{
	player.score++;
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
	ghostMap(ghostTouched, &ct, sizeof(struct CollisionTest));
	if (ct.result) {
		/* Can only catch one ghost per frame */
		caughtGhost();
	}
}

void updatePhysicsPlayer(void)
{
	Rectangle playerCollisionBox = getPlayerCollisionBox();
	struct CollisionTest ct = { .test = playerCollisionBox };
	ghostMap(ghostTouched, &ct, sizeof(struct CollisionTest));
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
