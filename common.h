#pragma once

#include <stdbool.h>

#include "raylib.h"

#define LOCAL_WIDTH 1000
#define LOCAL_HEIGHT 750

#ifdef NDEBUG
#define DEBUG_BUILD false
#else
#define DEBUG_BUILD true
#endif

#define ASSETS_DIR "assets"
#define DEBUG_POSITION_COLOR ORANGE
#define DEBUG_POSITION_RADIUS 20

#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]) + \
	/* Degenerate stuff (compile error if arr is not a static array type) */ \
	((int)sizeof(struct {_Static_assert(!(__builtin_types_compatible_p(typeof((arr)), typeof(&((arr))[0]))), "must be array");})))

struct CollisionBox {
	Vector2 pos;
	float width;
	float height;
};

struct CollisionTest {
	struct CollisionBox test;
	bool result;
};

extern bool showDebug;

int errorf(const char *restrict format, ...);
bool collisionBoxesOverlap(struct CollisionBox a, struct CollisionBox b);
