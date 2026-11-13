#pragma once

#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "raylib.h"
#include "raymath.h"

#include "timer.c"

#define GHOST_POOL_SIZE 16
#define GHOST_HITBOX_SIZE ((Vector2){ 64, 59 })
#define GHOST_HITBOX_OFFSET ((Vector2){ -31, -70 })
#define GHOST_HITBOX_COLOR (CLITERAL(Color){ 200, 122, 255, 255 / 2 })
#define GHOST_SPRITE_OFFSET ((Vector2){ -42, -172 })
#define GHOST_RESPAWN_TIME 3
#define GHOST_SPAWN_POSITION_THRESHOLD 50
#define GHOST_SPAWN_Y_POSITION (-10)
#define GHOST_MIN_SPEED 100
#define GHOST_MAX_SPEED 300
#define GHOST_DIRECTION ((Vector2){ 0, 1 })
#define GHOST_FRAME_DELAY 14
#define SPLASH_HIT_FRAME_DELAY 9

_Static_assert(
	LOCAL_WIDTH > GHOST_SPAWN_POSITION_THRESHOLD * 4 + 1,
	"No place to spawn ghosts, make GHOST_SPAWN_POSITION_THRESHOLD smaller"
);

struct Ghost {
	Vector2 position;
	Vector2 direction;
	float speed;
	unsigned short animationTick;
	bool dying;
};

int ghostFrameCount;
Texture2D ghost1Texture;
Texture2D ghost2Texture;
Texture2D splashHit1Texture;
Texture2D splashHit2Texture;
Texture2D splashHit3Texture;
struct Ghost ghostPool[GHOST_POOL_SIZE];
struct Ghost *ghostFreeList[GHOST_POOL_SIZE];
bool ghostUsedList[GHOST_POOL_SIZE];
int ghostFreeCount;
Sound splash;

struct Timer ghostRespawnTimer;

void ghostPoolFree(struct Ghost *ghost);
struct Ghost *ghostPoolAlloc(void);

void ghostMap(
	void (*callback)(struct Ghost *ghost, void *data, size_t size),
	void *data,
	size_t size
) {
	for (size_t i = 0; i < ARRAY_SIZE(ghostUsedList); i++) {
		if (ghostUsedList[i]) {
			callback(&ghostPool[i], data, size);
		}
	}
}

Rectangle ghostGetCollisionBox(struct Ghost *ghost)
{
	return (Rectangle){
		.x = ghost->position.x + GHOST_HITBOX_OFFSET.x,
		.y = ghost->position.y + GHOST_HITBOX_OFFSET.y,
		.width = GHOST_HITBOX_SIZE.x,
		.height = GHOST_HITBOX_SIZE.y
	};
}

void ghostKill(struct Ghost *ghost) {
	assert(ghost != NULL);

	if (ghost == NULL) {
		errorf("Null exception, skipping killing ghost");
		return;
	}

	ghost->dying = true;
	ghost->animationTick = 0;
}

bool ghostTryTouched(struct Ghost *ghost, void *data, size_t size)
{
	assert(ghost != NULL);
	assert(data != NULL);
	assert(size == sizeof(struct CollisionTest));

	if (ghost->dying) {
		return false;
	}

	if (ghost == NULL || data == NULL) {
		errorf("Null exception, skipping ghost collision");
		return false;
	}
	if (size != sizeof(struct CollisionTest)) {
		errorf(
			"Expected payload size %zu: got %zu",
			sizeof(struct CollisionTest),
			size
		);
		return false;
	}

	struct CollisionTest *ct = (struct CollisionTest *)data;
	Rectangle gc = ghostGetCollisionBox(ghost);

	if (CheckCollisionRecs(gc, ct->test)) {
		ct->result = true;
		return true;
	}

	return false;
}

void ghostTryCaught(struct Ghost *ghost, void *data, size_t size)
{
	if (ghostTryTouched(ghost, data, size)) {
		ghostPoolFree(ghost);
	}
}

void ghostTryHit(struct Ghost *ghost, void *data, size_t size)
{
	if (ghostTryTouched(ghost, data, size)) {
		ghostKill(ghost);
	}
}

void ghostDraw(struct Ghost *ghost, void *data, size_t size)
{
	(void)data;
	(void)size;

	assert(IsWindowReady());
	assert(ghost != NULL);

	if (!IsWindowReady()) {
		errorf("Window not ready, not drawing ghost");
		return;
	}

	if (ghost == NULL) {
		errorf("Ghost is null, ignoring");
		return;
	}

	Texture2D ghostTexture = { 0 };
	if (!ghost->dying) {
		int frame = ghost->animationTick % (GHOST_FRAME_DELAY * 2)
			< GHOST_FRAME_DELAY;
		ghostTexture = frame == 0 ? ghost1Texture : ghost2Texture;
	} else {
		if (ghost->animationTick < SPLASH_HIT_FRAME_DELAY) {
			ghostTexture = splashHit1Texture;
		} else if (ghost->animationTick < SPLASH_HIT_FRAME_DELAY * 2) {
			ghostTexture = splashHit2Texture;
		} else if (ghost->animationTick < SPLASH_HIT_FRAME_DELAY * 3) {
			ghostTexture = splashHit3Texture;
		} else {
			ghostPoolFree(ghost);
			return;
		}
	}

	DrawTexture(
		ghostTexture,
		ghost->position.x + GHOST_SPRITE_OFFSET.x,
		ghost->position.y + GHOST_SPRITE_OFFSET.y,
		WHITE
	);

	if (showDebug) {
		Rectangle b = ghostGetCollisionBox(ghost);
		DrawRectangle(
			b.x,
			b.y,
			b.width,
			b.height,
			GHOST_HITBOX_COLOR
		);
		DrawCircleV(
			ghost->position,
			DEBUG_POSITION_RADIUS,
			DEBUG_POSITION_COLOR
		);
	}
}

void drawGhosts(void)
{
	ghostMap(ghostDraw, NULL, 0);
}

int getGhostSpawnXPosition(void)
{
	static int previousSpawnPosition = -1;
	const int threshold = GHOST_SPAWN_POSITION_THRESHOLD;

	int rand = GetRandomValue(
		threshold,
		LOCAL_WIDTH - threshold
	);

	if (previousSpawnPosition < 0) {
		previousSpawnPosition = rand;
		return rand;
	}

	if (abs(rand - previousSpawnPosition) > threshold) {
		previousSpawnPosition = rand;
		return rand;
	}

	int tryRight = GetRandomValue(0, RAND_MAX) % 2 == 0;

	if ((tryRight && rand + threshold > LOCAL_WIDTH - threshold)
			|| (!tryRight && rand - threshold < threshold)) {
		tryRight = !tryRight;
	}

	if (tryRight) {
		rand = GetRandomValue(rand + threshold, LOCAL_WIDTH - threshold);
	} else {
		rand = GetRandomValue(threshold, rand - threshold);
	}

	previousSpawnPosition = rand;
	return rand;
}

struct Ghost *ghostPoolAlloc(void)
{
	assert(ghostFreeCount > 0);
	ghostFreeCount--;
	struct Ghost *ghost = ghostFreeList[ghostFreeCount];
	ghostUsedList[ghost - ghostPool] = true;
	memset(ghost, 0, sizeof(struct Ghost));
	return ghost;
}

void ghostPoolFree(struct Ghost *ghost)
{
	assert(ghost != NULL);
	assert(ghostFreeCount < GHOST_POOL_SIZE);
	assert(ghost >= &ghostPool[0]
		&& ghost <= &ghostPool[GHOST_POOL_SIZE - 1]);

	size_t idx = ghost - ghostPool;
	assert(ghostUsedList[idx]); /* Double free */

	/* Double free, no-op in release */
	if (!ghostUsedList[idx]) {
		return;
	}

	ghostUsedList[idx] = false;
	ghostFreeList[ghostFreeCount] = ghost;
	ghostFreeCount++;
}

void spawnGhost(void)
{
	if (ghostFreeCount <= 0) {
		errorf("Warning: no free ghost slot to use");
		return;
	}

	struct Ghost *ghost = ghostPoolAlloc();
	ghost->speed = GetRandomValue(GHOST_MIN_SPEED, GHOST_MAX_SPEED);
	ghost->direction = GHOST_DIRECTION;
	ghost->position = (Vector2) {
		getGhostSpawnXPosition(),
		GHOST_SPAWN_Y_POSITION
	};
}

void ghostPoolInit(void)
{
	for (int i = 0; i < GHOST_POOL_SIZE; i++) {
		ghostFreeList[i] = &ghostPool[i];
	}
	ghostFreeCount = GHOST_POOL_SIZE;
}

__attribute__((constructor))
void ghostLogicInit(void)
{
	ghostPoolInit();
	ghostRespawnTimer = timerCreate(GHOST_RESPAWN_TIME, spawnGhost, true);
}

void ghostMove(struct Ghost *ghost, void *data, size_t size)
{
	assert(size == sizeof(float));

	if (size != sizeof(float)) {
		errorf("Expected payload size %zu: got %zu", sizeof(float), size);
		return;
	}

	float delta = *(float*)data;

	if (!ghost->dying) {
		ghost->position = Vector2Add(
			ghost->position,
			Vector2Scale(ghost->direction, ghost->speed * delta)
			);
	}
	ghost->animationTick++;
}

void updateGhostLogic(float delta)
{
	timerProcess(&ghostRespawnTimer, delta);
	ghostMap(ghostMove, &delta, sizeof(float));
}

void ghostPoolFreeAllInternal(struct Ghost *ghost, void *data, size_t size)
{
	(void)data;
	(void)size;

	assert(ghost != NULL);

	ghostPoolFree(ghost);
}

void ghostPoolFreeAll(void)
{
	ghostMap(ghostPoolFreeAllInternal, NULL, 0);
}
