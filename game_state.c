#pragma once

#include <assert.h>
#include <stdlib.h>

#include "common.h"

STATE currentState = STATE_START_SCREEN;

void stateEnterStartScreen(void);
void stateProcessStartScreen(void);
void stateExitStartScreen(void);

void stateEnterGameplay(void);
void stateProcessGameplay(void);
void stateExitGameplay(void);

void stateEnterDeath(void);
void stateProcessDeath(void);
void stateExitDeath(void);

void (*const enterFunctions[])(void) = {
	[STATE_START_SCREEN] = stateEnterStartScreen,
	[STATE_GAMEPLAY] = stateEnterGameplay,
	[STATE_DEATH] = stateEnterDeath,
};

void (*const processFunctions[])(void) = {
	[STATE_START_SCREEN] = stateProcessStartScreen,
	[STATE_GAMEPLAY] = stateProcessGameplay,
	[STATE_DEATH] = stateProcessDeath,
};

void (*const exitFunctions[])(void) = {
	[STATE_START_SCREEN] = stateExitStartScreen,
	[STATE_GAMEPLAY] = stateExitGameplay,
	[STATE_DEATH] = stateExitDeath,
};

_Static_assert(
	ARRAY_SIZE(enterFunctions) == _STATE_LAST,
	"Likely missing enter functions"
);

_Static_assert(
	ARRAY_SIZE(processFunctions) == _STATE_LAST,
	"Likely missing enter functions"
);

_Static_assert(
	ARRAY_SIZE(exitFunctions) == _STATE_LAST,
	"Likely missing enter functions"
);

__attribute__((constructor))
void _stateAssertInit(void)
{
	for (int i = _STATE_FIRST + 1; i < _STATE_LAST; i++) {
		assert(enterFunctions[i] != NULL);
		assert(processFunctions[i] != NULL);
		assert(exitFunctions[i] != NULL);
	}
}

void stateChange(STATE newState)
{
	assert(newState >= STATE_START && newState < _STATE_LAST);
	assert(newState != currentState);

	exitFunctions[currentState]();
	currentState = newState;
	enterFunctions[newState]();
}

void stateUpdate(void)
{
	assert(currentState >= STATE_START && currentState < _STATE_LAST);

	processFunctions[currentState]();
}
