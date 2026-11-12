#pragma once

#include <assert.h>

#include "common.h"
#include "ghosts.c"

#define BUCKET_HITBOX_SIZE (Vector2){ 157, 33 }
#define BUCKET_HITBOX_OFFSET (Vector2){ -185, -118 }
#define BUCKET_HITBOX_COLOR GREEN
#define PLAYER_SPRITE_OFFSET (Vector2){ -225, -154 }
#define PLAYER_HITBOX_SIZE (Vector2){ 349, 231 }
#define PLAYER_HITBOX_OFFSET (Vector2){ -173, -79 }
#define PLAYER_HITBOX_COLOR SKYBLUE
#define PLAYER_Y_POSITION (600)
#define PLAYER_MAX_HEALTH 1
#define PLAYER_MIN_HEALTH 0 /* dead when reached */

struct Player {
	void (*died)(void);
	Vector2 position;
	unsigned int score;
	unsigned int health;
	Texture2D texture;
} player = {
	.health = PLAYER_MAX_HEALTH,
	.position = {
		.x = LOCAL_WIDTH / 2.0,
		.y = PLAYER_Y_POSITION,
	},
};

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
}

void hitByGhost(void)
{
	player.health--;
	if (player.health <= PLAYER_MIN_HEALTH) {
		assert(player.died != NULL);
		player.died();
	}
}

struct CollisionBox getBucketCollisionBox(void)
{
	return (struct CollisionBox){
		.pos = Vector2Add(player.position, BUCKET_HITBOX_OFFSET),
		.width = BUCKET_HITBOX_SIZE.x,
		.height = BUCKET_HITBOX_SIZE.y,
	};
}

struct CollisionBox getPlayerCollisionBox(void)
{
	return (struct CollisionBox){
		.pos = Vector2Add(player.position, PLAYER_HITBOX_OFFSET),
		.width = PLAYER_HITBOX_SIZE.x,
		.height = PLAYER_HITBOX_SIZE.y,
	};
}

void updatePhysicsBucket(void)
{
	struct CollisionBox bucket = getBucketCollisionBox();
	struct CollisionTest ct = { .test = bucket };
	ghostMap(ghostTouched, &ct, sizeof(struct CollisionTest));
	if (ct.result) {
		/* Can only catch one ghost per frame */
		caughtGhost();
	}
}

void updatePhysicsPlayer(void)
{
	struct CollisionBox player = getPlayerCollisionBox();
	struct CollisionTest ct = { .test = player };
	ghostMap(ghostTouched, &ct, sizeof(struct CollisionTest));
	if (ct.result) {
		/* Can only get hit once per frame */
		hitByGhost();
	}
}
