#include <assert.h>
#include <stdio.h>
#include <time.h>

#include "raylib.h"
#include "common.h"

#include "start_screen.c"
#include "death_screen.c"
#include "gameplay_screen.c"
#include "player.c"
#include "ghosts.c"
#include "timer.c"
#include "game_state.c"

#include <emscripten/emscripten.h>
#include <emscripten/html5.h>

#ifndef __EMSCRIPTEN__
#error "This game is only designed to run in the browser."
#endif

#define PLAYER_TEXTURE_PATH ASSETS_DIR "/player.png"
#define SPLASH_TEXTURE_PATH ASSETS_DIR "/splash.png"
#define FONT_PATH ASSETS_DIR "/LakkiReddy-Regular.ttf"
#define MUSIC_PATH ASSETS_DIR "/music.ogg"
#define SOUND_CATCH_GHOST_PATH ASSETS_DIR "/catch_ghost.ogg"
#define SOUND_SPLASH_PATH ASSETS_DIR "/splash.ogg"

#define CANVAS_HTML_ID "#game"

bool showDebug = false;

RenderTexture2D target;
Texture2D splashTexture;
Font font;

void update(void);

void loadAssets(void)
{
	player.texture = LoadTexture(PLAYER_TEXTURE_PATH);
	ghostTexture = LoadTexture(GHOST_TEXTURE_PATH);
	splashTexture = LoadTexture(SPLASH_TEXTURE_PATH);
	font = LoadFontEx(FONT_PATH, FONT_SIZE_TITLE, NULL, 0);
	music = LoadMusicStream(MUSIC_PATH);
	catchGhost = LoadSound(SOUND_CATCH_GHOST_PATH);
	splash = LoadSound(SOUND_SPLASH_PATH);

	assert(IsTextureValid(player.texture));
	assert(IsTextureValid(ghostTexture));
	assert(IsTextureValid(splashTexture));
	assert(IsFontValid(font));
	assert(IsMusicValid(music));
	assert(IsSoundValid(catchGhost));
	assert(IsSoundValid(splash));

	SetTextureFilter(font.texture, TEXTURE_FILTER_BILINEAR);
}

void createRenderTexture(void)
{
	target = LoadRenderTexture(LOCAL_WIDTH, LOCAL_HEIGHT);
	assert(IsRenderTextureValid(target));
	SetTextureFilter(target.texture, TEXTURE_FILTER_BILINEAR);

	targetBackup = LoadRenderTexture(LOCAL_WIDTH, LOCAL_HEIGHT);
	assert(IsRenderTextureValid(targetBackup));
	SetTextureFilter(targetBackup.texture, TEXTURE_FILTER_BILINEAR);
}

EM_BOOL OnBlur(int type, const EmscriptenFocusEvent *e, void *ud)
{
	(void)type;
	(void)e;
	(void)ud;

	emscripten_pause_main_loop();
	return EM_TRUE;
}

EM_BOOL OnFocus(int type, const EmscriptenFocusEvent *e, void *ud)
{
	(void)type;
	(void)e;
	(void)ud;

	emscripten_resume_main_loop();
	return EM_TRUE;
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

	const char *event = EMSCRIPTEN_EVENT_TARGET_WINDOW;
	emscripten_set_blur_callback(event, NULL, 1, OnBlur);
	emscripten_set_focus_callback(event, NULL, 1, OnFocus);

	emscripten_set_main_loop(update, 0, true);
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

void update(void)
{
	stateUpdate();

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

void DrawRenderTexture(RenderTexture2D tex)
{
	ClearBackground(BACKGROUND_COLOR);

	DrawTextureRec(
		tex.texture,
		(Rectangle){
			0,
			0,
			(float)tex.texture.width,
			-(float)tex.texture.height
		},
		(Vector2){ 0, 0 },
		WHITE
	);
}
