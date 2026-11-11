#include <assert.h>
#include <stdio.h>
#include <time.h>

#include "raylib.h"
#include "common.h"

#include "start_screen.c"
#include "ghosts.c"
#include "timer.c"

#include <emscripten/emscripten.h>
#include <emscripten/html5.h>

#ifndef __EMSCRIPTEN__
#error "This game is only designed to run in the browser."
#endif

#define PLAYER_TEXTURE_PATH ASSETS_DIR "/player.png"
#define SPLASH_TEXTURE_PATH ASSETS_DIR "/splash.png"
#define FONT_PATH ASSETS_DIR "/LakkiReddy-Regular.ttf"

#define CANVAS_HTML_ID "#game"

#define FONT_SIZE_DEBUG 48
#define FONT_SIZE_SCORE 96
#define FONT_SIZE_TITLE 192
#define FONT_SIZE_SUBTITLE 96
#define FONT_SPACING -1

#define BUCKET_HITBOX_SIZE (Vector2){ 157, 33 }
#define BUCKET_HITBOX_OFFSET (Vector2){ -185, -118 }
#define BUCKET_HITBOX_COLOR GREEN
#define PLAYER_SPRITE_OFFSET (Vector2){ -225, -154 }
#define PLAYER_HITBOX_SIZE (Vector2){ 349, 231 }
#define PLAYER_HITBOX_OFFSET (Vector2){ -173, -79 }
#define PLAYER_HITBOX_COLOR SKYBLUE
#define PLAYER_Y_POSITION (600)
#define FLOOR_HITBOX_SIZE (Vector2){ LOCAL_WIDTH, 10 }
#define FLOOR_HITBOX_POSITION (Vector2){ 0, LOCAL_HEIGHT }
#define MAX_PLAYER_HEALTH 5
#define MIN_PLAYER_HEALTH 0 /* dead when reached */

struct Player {
	Vector2 position;
} player;

bool showDebug = false;

RenderTexture2D target;
Texture2D playerTexture;
extern Texture2D ghostTexture;
extern bool inStartScreen;
Texture2D splashTexture;
Font font;
unsigned int score;
unsigned int health = MAX_PLAYER_HEALTH;

void update(void);

void loadAssets(void)
{
	playerTexture = LoadTexture(PLAYER_TEXTURE_PATH);
	ghostTexture = LoadTexture(GHOST_TEXTURE_PATH);
	splashTexture = LoadTexture(SPLASH_TEXTURE_PATH);
	font = LoadFontEx(FONT_PATH, FONT_SIZE_TITLE, NULL, 0);

	assert(IsTextureValid(playerTexture));
	assert(IsTextureValid(ghostTexture));
	assert(IsTextureValid(splashTexture));
	assert(IsFontValid(font));

	SetTextureFilter(font.texture, TEXTURE_FILTER_BILINEAR);
}

void createRenderTexture(void)
{
	target = LoadRenderTexture(LOCAL_WIDTH, LOCAL_HEIGHT);
	assert(IsRenderTextureValid(target));
	SetTextureFilter(target.texture, TEXTURE_FILTER_BILINEAR);
}

int main(void)
{
	SetRandomSeed((unsigned int)time(NULL));

	InitWindow(LOCAL_WIDTH, LOCAL_HEIGHT, "Midmortis catch the ghosts!");
	assert(IsWindowReady());

	InitAudioDevice();
	assert(IsAudioDeviceReady());

	loadAssets();
	createRenderTexture();

	SetTargetFPS(60);
	emscripten_set_main_loop(update, 0, true);
}

void updatePlayerLogic(float delta)
{
	(void)delta;

	player.position.y = PLAYER_Y_POSITION;
	player.position.x = GetMouseX();
}

void caughtGhost(void)
{
	score++;
}

void died(void)
{
}

void hitByGhost(void)
{
	health--;
	if (health <= MIN_PLAYER_HEALTH) {
		died();
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

void resizeWindow(int width, int height)
{
	SetWindowSize(width, height);

	double cssWidth = 0, cssHeight = 0;
	emscripten_get_element_css_size(CANVAS_HTML_ID, &cssWidth, &cssHeight);
	SetMouseScale(
		(float)(LOCAL_WIDTH / cssWidth),
		(float)(LOCAL_HEIGHT / cssHeight)
	);
}

void showViewport(void)
{
	int width = 0;
	int height = 0;
	emscripten_get_canvas_element_size(CANVAS_HTML_ID, &width, &height);

	if (width != GetScreenWidth() || height != GetScreenHeight()) {
		resizeWindow(width, height);
	}

	Rectangle source = {
		0,
		0,
		(float)target.texture.width,
		-(float)target.texture.height
	};

	Rectangle destination = {
		0,
		0,
		(float)width,
		-(float)height
	};

	DrawTexturePro(
		target.texture,
		source,
		destination,
		(Vector2){ 0, 0 },
		0.0f,
		WHITE
	);
}

void drawPlayer(void)
{
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

Vector2 MeasureTextMidmortis(const char *text, float fontSize)
{
	return MeasureTextEx(font, text, fontSize, FONT_SPACING);
}

void DrawTextMidmortis(
	const char *text,
	int posX,
	int posY,
	int fontSize,
	Color color
) {
	DrawTextEx(
		font,
		text,
		(Vector2){ posX, posY },
		fontSize,
		FONT_SPACING,
		color
	);
}

void drawDebugUI(void) {
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
	snprintf(scoreText, ARRAY_SIZE(scoreText), "Score: %u", score);
	int textSize = MeasureTextMidmortis(scoreText, FONT_SIZE_SCORE).x;
	int x = LOCAL_WIDTH / 2 - textSize / 2;
	int y = FONT_SIZE_SCORE + 10;
	DrawTextMidmortis(scoreText, x, y, FONT_SIZE_SCORE, LIGHTGRAY);
}

void drawViewport(void)
{
	ClearBackground(BACKGROUND_COLOR);

	drawGhosts();
	drawPlayer();

	drawUI();
}

void updateGame()
{
	updateLogic();

	BeginTextureMode(target);
	{
		drawViewport();
	}
	EndTextureMode();
}

void updateDeathScreen(void)
{
	static bool drawn = false;

	if (drawn) {
		return;
	}

	BeginTextureMode(target);
	{
		char *diedText = "You Died!";
		char *subText = "How does it feel to get ghosted?";
		Vector2 diedSize = MeasureTextMidmortis(diedText, FONT_SIZE_TITLE);
		int subSize = MeasureTextMidmortis(subText, FONT_SIZE_SUBTITLE).x;
		int diedX = LOCAL_WIDTH / 2 - (int)diedSize.x / 2;
		int diedY = LOCAL_HEIGHT / 2 - (int)(diedSize.y / 1.5);
		int subX = LOCAL_WIDTH / 2 - subSize / 2;
		int subY = LOCAL_HEIGHT / 2;
		DrawTextMidmortis(diedText, diedX, diedY, FONT_SIZE_TITLE, LIGHTGRAY);
		DrawTextMidmortis(subText, subX, subY, FONT_SIZE_SUBTITLE, LIGHTGRAY);
	}
	EndTextureMode();

	drawn = true;
}

void update(void)
{
	static int lmao = 0;
	if (inStartScreen) {
		updateStartScreen();
	} else if (health > MIN_PLAYER_HEALTH && lmao < 2) {
		updateGame();
		lmao++;
	} else {
		updateDeathScreen();
	}

	BeginDrawing();
	{
		showViewport();
	}
	EndDrawing();
}

int errorf(const char *restrict format, ...)
{
	va_list args;
	va_start(args, format);

	int ret = vfprintf(stderr, format, args);

	va_end(args);
	return ret;
}

bool collisionBoxesOverlap(struct CollisionBox a, struct CollisionBox b)
{
	return a.pos.x < b.pos.x + b.width
		&& a.pos.x + a.width  > b.pos.x
		&& a.pos.y < b.pos.y + b.height
		&& a.pos.y + a.height > b.pos.y;
}
