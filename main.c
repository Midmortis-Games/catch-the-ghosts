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
#include "button.c"

#include <emscripten/emscripten.h>
#include <emscripten/html5.h>

#ifndef __EMSCRIPTEN__
#error "This game is only designed to run in the browser."
#endif

#define PLAYER_0_TEXTURE_PATH ASSETS_DIR "/player_0.png"
#define PLAYER_1_TEXTURE_PATH ASSETS_DIR "/player_1.png"
#define PLAYER_2_TEXTURE_PATH ASSETS_DIR "/player_2.png"
#define PLAYER_3_TEXTURE_PATH ASSETS_DIR "/player_3.png"
#define GHOST_1_TEXTURE_PATH ASSETS_DIR "/ghost_1.png"
#define GHOST_2_TEXTURE_PATH ASSETS_DIR "/ghost_2.png"
#define SPLASH_HIT_1_TEXTURE_PATH ASSETS_DIR "/splash_hit_1.png"
#define SPLASH_HIT_2_TEXTURE_PATH ASSETS_DIR "/splash_hit_2.png"
#define SPLASH_HIT_3_TEXTURE_PATH ASSETS_DIR "/splash_hit_3.png"
#define SPLASH_SCORE_1_TEXTURE_PATH ASSETS_DIR "/splash_score_1.png"
#define SPLASH_SCORE_2_TEXTURE_PATH ASSETS_DIR "/splash_score_2.png"
#define SPLASH_SCORE_3_TEXTURE_PATH ASSETS_DIR "/splash_score_3.png"
#define BUTTON_ON_TEXTURE_PATH ASSETS_DIR "/button_on.png"
#define BUTTON_OFF_TEXTURE_PATH ASSETS_DIR "/button_off.png"
#define TITLE_SCREEN_TEXTURE_PATH ASSETS_DIR "/title_screen.png"
#define BACKGROUND_TEXTURE_PATH ASSETS_DIR "/background.png"
#define FONT_PATH ASSETS_DIR "/LakkiReddy-Regular.ttf"
#define TITLE_SCREEN_MUSIC_PATH ASSETS_DIR "/music/title_music.ogg"
#define GAMEPLAY_START_MUSIC_PATH ASSETS_DIR "/music/gameplay_start.ogg"
#define GAMEPLAY_MUSIC_PATH ASSETS_DIR "/music/gameplay_music.ogg"
#define DEATH_MUSIC_PATH ASSETS_DIR "/music/death_music.ogg"
#define SOUND_CATCH_GHOST_PATH ASSETS_DIR "/catch_ghost.ogg"
#define SOUND_SPLASH_PATH ASSETS_DIR "/splash.ogg"

#define CANVAS_HTML_ID "#game"

bool showDebug = false;

Music *currentMusic;
RenderTexture2D target;
Font font;

void update(void);

void loadAssets(void)
{
	playerTexture0 = LoadTexture(PLAYER_0_TEXTURE_PATH);
	playerTexture1 = LoadTexture(PLAYER_1_TEXTURE_PATH);
	playerTexture2 = LoadTexture(PLAYER_2_TEXTURE_PATH);
	playerTexture3 = LoadTexture(PLAYER_3_TEXTURE_PATH);
	ghost1Texture = LoadTexture(GHOST_1_TEXTURE_PATH);
	ghost2Texture = LoadTexture(GHOST_2_TEXTURE_PATH);
	splashHit1Texture = LoadTexture(SPLASH_HIT_1_TEXTURE_PATH);
	splashHit2Texture = LoadTexture(SPLASH_HIT_2_TEXTURE_PATH);
	splashHit3Texture = LoadTexture(SPLASH_HIT_3_TEXTURE_PATH);
	splashScore1Texture = LoadTexture(SPLASH_SCORE_1_TEXTURE_PATH);
	splashScore2Texture = LoadTexture(SPLASH_SCORE_2_TEXTURE_PATH);
	splashScore3Texture = LoadTexture(SPLASH_SCORE_3_TEXTURE_PATH);
	buttonOnTexture = LoadTexture(BUTTON_ON_TEXTURE_PATH);
	buttonOffTexture = LoadTexture(BUTTON_OFF_TEXTURE_PATH);
	titleScreenTexture = LoadTexture(TITLE_SCREEN_TEXTURE_PATH);
	backgroundTexture = LoadTexture(BACKGROUND_TEXTURE_PATH);
	font = LoadFontEx(FONT_PATH, FONT_SIZE_TITLE, NULL, 0);
	titleScreenMusic = LoadMusicStream(TITLE_SCREEN_MUSIC_PATH);
	gameplayMusic = LoadMusicStream(GAMEPLAY_MUSIC_PATH);
	gameplayStartMusic = LoadMusicStream(GAMEPLAY_START_MUSIC_PATH);
	deathMusic = LoadMusicStream(DEATH_MUSIC_PATH);
	catchGhost = LoadSound(SOUND_CATCH_GHOST_PATH);
	splash = LoadSound(SOUND_SPLASH_PATH);

	assert(IsTextureValid(playerTexture0));
	assert(IsTextureValid(playerTexture1));
	assert(IsTextureValid(playerTexture2));
	assert(IsTextureValid(playerTexture3));
	assert(IsTextureValid(ghost1Texture));
	assert(IsTextureValid(ghost2Texture));
	assert(IsTextureValid(splashHit1Texture));
	assert(IsTextureValid(splashHit2Texture));
	assert(IsTextureValid(splashHit3Texture));
	assert(IsTextureValid(splashScore1Texture));
	assert(IsTextureValid(splashScore2Texture));
	assert(IsTextureValid(splashScore3Texture));
	assert(IsTextureValid(buttonOnTexture));
	assert(IsTextureValid(buttonOffTexture));
	assert(IsTextureValid(backgroundTexture));
	assert(IsTextureValid(titleScreenTexture));
	assert(IsFontValid(font));
	assert(IsMusicValid(titleScreenMusic));
	assert(IsMusicValid(gameplayMusic));
	assert(IsMusicValid(gameplayStartMusic));
	assert(IsMusicValid(deathMusic));
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
		/* Because we use targets drawn on the viewport, the alpha is
		 * blended twice. So, disabling alpha blending here*/
		BeginBlendMode(BLEND_ALPHA_PREMULTIPLY);
		{
			showViewport();
		}
		EndBlendMode();
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
