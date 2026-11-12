#pragma once

#include <stdbool.h>

#include "raylib.h"
#include "raymath.h"
#include "rlgl.h"

#define LOCAL_WIDTH 1000
#define LOCAL_HEIGHT 750
#define BACKGROUND_COLOR (CLITERAL(Color){ 128, 128, 128, 255 })

#define FONT_SIZE_DEBUG 48
#define FONT_SIZE_SCORE 96
#define FONT_SIZE_TITLE 192
#define FONT_SIZE_SUBTITLE 96
#define FONT_SPACING (-1)

#define MAX_DELTA 0.15

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

typedef enum STATE {
	_STATE_FIRST = 0, /* Not a valid state, MUST be first */
	STATE_START_SCREEN,
	STATE_GAMEPLAY,
	STATE_DEATH,
	_STATE_LAST, /* Not a valid state, MUST be last */
} STATE;

struct CollisionTest {
	Rectangle test;
	bool result;
};

extern bool showDebug;
extern STATE currentState;
extern RenderTexture2D target;

int errorf(const char *restrict format, ...);
Vector2 MeasureTextMidmortis(const char *text, float fontSize);
void DrawTextMidmortis(
	const char *text,
	int posX,
	int posY,
	int fontSize,
	Color color
);
void DrawRenderTexture(RenderTexture2D tex);
